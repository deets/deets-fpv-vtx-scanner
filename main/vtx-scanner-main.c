#include "appstate.h"
#include "ssd1306.h"
#include "rtc6715.h"
#include "ble.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "soc/gpio_struct.h"
#include "driver/gpio.h"
#include <driver/adc.h>
#include <esp_intr_alloc.h>
#include <esp_timer.h>



#define PIN_NUM_MISO 25
#define PIN_NUM_MOSI 23
#define PIN_NUM_CLK  19
#define PIN_NUM_CS   22
#define PIN_NUM_DC   16
#define PIN_NUM_RST  18

#define RTC_CS 14
#define RTC_CLK 13
#define RTC_MOSI 12

#define STAR_SPEED 50
#define STAR_FACTOR 1024
#define STAR_DISTANCE 8192
#define STAR_COUNT 300

#define READER_TASK_STACK_SIZE 2000
#define DISPLAY_TASK_STACK_SIZE 2000
#define READS_PER_SECOND 20
#define READER_TASK_WAKEUP_FLAG (1 << 0)
#define RIGHT_PIN_ISR_FLAG (1 << 1)
#define LEFT_PIN_ISR_FLAG (1 << 2)


static app_state_t app_state;

typedef struct {
  TaskHandle_t display_task_handle;
  StaticTask_t display_task_buffer;
  StackType_t  display_task_stack[DISPLAY_TASK_STACK_SIZE];

  TaskHandle_t reader_task_handle;
  StaticTask_t reader_task_buffer;
  StackType_t  reader_task_stack[READER_TASK_STACK_SIZE];
} task_state_t;

task_state_t task_state;

uint32_t isr_count = 0;

#define __BTSTACK_FILE__ "vtx-scanner-main.c"

void reader_task()
{
  TickType_t last_wake_time;
  const TickType_t frequency =  pdMS_TO_TICKS(1000 / READS_PER_SECOND);
  last_wake_time = xTaskGetTickCount ();

  rtc6715_t rtc;
  rtc6715_setup(
    &rtc,
    ADC1_CHANNEL_6,
    RTC_CS,
    RTC_CLK,
    RTC_MOSI
    );

  for( ;; )
  {
    vTaskDelayUntil( &last_wake_time, frequency );
    channel_display_update_channel(
      app_state.current_channel,
      rtc6715_read_rssi(&rtc),
      &app_state.scanner_state.channels
      );
    xTaskNotify(
      task_state.display_task_handle,
      READER_TASK_WAKEUP_FLAG,
      eSetBits
      );
    // switch channel to the next one after reading
    // so we get the maximum of stabilisation time.
    app_state.current_channel = (app_state.current_channel + 1) % CHANNEL_NUM;
    rtc6715_select_channel(&rtc, app_state.current_channel);
  }
}

static void copy_legal_channel_info(vtx_info_t* vtx, channel_display_t* channels, int has_ham)
{
  for(size_t i=0; i < CHANNEL_NUM; ++i)
  {
    channels->channels[i].legal = vtx->channel_legal[i] == LEGAL ||
       (vtx->channel_legal[i] == HAM && has_ham);
  }
}


void init_channels()
{
  app_state.scanner_state.selected_vtx = &tbs_unify_info;
  app_state.scanner_state.selected_goggle = &aomway_commander_v1_info;
  app_state.scanner_state.has_ham = 0;
  channel_display_init(&app_state.scanner_state.channels);
  copy_legal_channel_info(
    app_state.scanner_state.selected_vtx,
    &app_state.scanner_state.channels,
    app_state.scanner_state.has_ham
    );

  app_state.current_channel = 0;
  task_state.reader_task_handle = xTaskCreateStatic(
                  reader_task,       // Function that implements the task.
                  "RTC",          // Text name for the task.
                  READER_TASK_STACK_SIZE,      // Stack size in bytes, not words.
                  0,
                  tskIDLE_PRIORITY,// Priority at which the task is created.
                  task_state.reader_task_stack,          // Array to use as the task's stack.
                  &task_state.reader_task_buffer // Variable to hold the task's data structure.
    );
}

#define DEBOUNCE (20 * 1000)

static void IRAM_ATTR gpio_isr_handler(void* arg)
{
  BaseType_t higher_prio_has_woken;
  static int64_t last = 0;
  isr_count++;
  int pin = (int)arg;
  int bit = 0;
  int64_t ts = esp_timer_get_time();
  if(last + DEBOUNCE > ts)
  {
    return;
  }
  last = ts;

  switch(pin)
  {
  case GPIO_NUM_0:
    bit = RIGHT_PIN_ISR_FLAG;
    break;
  case GPIO_NUM_17:
    bit = LEFT_PIN_ISR_FLAG;
    break;
  }

  xTaskNotifyFromISR(
    task_state.display_task_handle,
    bit,
    eSetBits,
    &higher_prio_has_woken
    );
  if(higher_prio_has_woken)
  {
    portYIELD_FROM_ISR();
  }
}


uint32_t wait_for_notification()
{
  uint32_t status_bits;
  xTaskNotifyWait(
    pdFALSE,
    ULONG_MAX,
    &status_bits,
    portMAX_DELAY
    );
  return status_bits;
}


void display_task()
{
  gpio_config_t io_conf;
  //interrupt of rising edge
  io_conf.intr_type = GPIO_PIN_INTR_NEGEDGE;
  //bit mask of the pins, use GPIO4/5 here
  io_conf.pin_bit_mask = (1ULL<< GPIO_NUM_0) | (1ULL<< GPIO_NUM_17);
  //set as input mode
  io_conf.mode = GPIO_MODE_INPUT;
  //enable pull-up mode
  io_conf.pull_up_en = 1;
  gpio_config(&io_conf);

  // install global GPIO ISR handler
  gpio_install_isr_service(0);
  // install individual interrupts
  gpio_isr_handler_add(GPIO_NUM_0, gpio_isr_handler, (void*)0);
  gpio_isr_handler_add(GPIO_NUM_17, gpio_isr_handler, (void*)17);

  ssd1306_display_t display;
  ssd1306_init_static(
    &display,
    PIN_NUM_CS,
    PIN_NUM_CLK,
    PIN_NUM_MOSI,
    PIN_NUM_MISO,
    PIN_NUM_DC,
    PIN_NUM_RST
    );

  init_channels();
  while(1)
  {
    uint32_t status_bits = wait_for_notification();
    if(status_bits & RIGHT_PIN_ISR_FLAG)
    {
      channel_display_step_cursor(&app_state.scanner_state.channels, 1);
    }
    if(status_bits & LEFT_PIN_ISR_FLAG)
    {
      channel_display_step_cursor(&app_state.scanner_state.channels, -1);
    }

    ssd1306_clear(&display);
    channel_display_draw(&display, &app_state.scanner_state.channels);
    vtx_display_draw(&display, app_state.scanner_state.selected_vtx, app_state.scanner_state.channels.cursor_pos);
    goggle_display_draw(&display, app_state.scanner_state.selected_goggle, app_state.scanner_state.channels.cursor_pos);
    ssd1306_update(&display);
  }
}
void btstack_main()
{
  ble_init();
  task_state.display_task_handle = xTaskCreateStatic(
    display_task,       // Function that implements the task.
    "DSP",          // Text name for the task.
    DISPLAY_TASK_STACK_SIZE,      // Stack size in bytes, not words.
    0,
    tskIDLE_PRIORITY,// Priority at which the task is created.
    task_state.display_task_stack,          // Array to use as the task's stack.
    &task_state.display_task_buffer // Variable to hold the task's data structure.
    );

}
