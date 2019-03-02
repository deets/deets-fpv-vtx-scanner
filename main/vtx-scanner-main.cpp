#include "appstate.h"
#include "ssd1306.h"
#include "ble.h"
#include "splash-screen.hh"
#include "scanner.hh"
#include "storage.hh"
#include "laptimer.hh"
#include "buzzer.hh"
#include "io-buttons.hh"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include <driver/adc.h>
#include <esp_intr_alloc.h>
#include <esp_timer.h>
#include <esp_log.h>


#define PIN_NUM_MISO 25
#define PIN_NUM_MOSI 23
#define PIN_NUM_CLK  19
#define PIN_NUM_CS   22
#define PIN_NUM_DC   16 // TODO: needs to become 15 for WROVER-2 module!
#define PIN_NUM_RST  18

#define RTC_CS 14
#define RTC_CLK 13
#define RTC_MOSI 12
#define RTC_ADC_CHANNEL ADC1_CHANNEL_6 // GPIO 34

#define STAR_SPEED 50
#define STAR_FACTOR 1024
#define STAR_DISTANCE 8192
#define STAR_COUNT 300

#define DISPLAY_TASK_STACK_SIZE 8192

static app_state_t app_state;

typedef struct {
  TaskHandle_t display_task_handle;
  StaticTask_t display_task_buffer;
  StackType_t  display_task_stack[DISPLAY_TASK_STACK_SIZE];
} task_state_t;

task_state_t task_state;

#define __BTSTACK_FILE__ "vtx-scanner-main.c"

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


void display_task(void*)
{
  iobuttons_setup(xTaskGetCurrentTaskHandle());

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

  rtc6715_t rtc;
  rtc6715_setup(
    &rtc,
    RTC_ADC_CHANNEL,
    RTC_CS,
    RTC_CLK,
    RTC_MOSI
    );

  buzzer_setup();
  buzzer_buzz(100, 3);

  Storage storage(app_state);

  SplashScreen* splash_screen = new SplashScreen(app_state, SCANNER);
  Scanner* scanner = new Scanner(app_state, rtc);
  LapTimer* lap_timer = new LapTimer(app_state, rtc);
  Mode* active_mode = splash_screen;
  app_mode_t next_mode = app_state.current_mode = SPLASH_SCREEN;

  active_mode->setup();
  ble_update(NOTIFY_CURRENT_MODE);

  ble_set_mode_change_callback(
    [&active_mode, splash_screen, scanner, lap_timer](int mode) {
      app_mode_t next_mode;
      switch(mode)
      {
      case SPLASH_SCREEN:
      case SCANNER:
      case LAPTIMER:
        next_mode = (app_mode_t)mode;
        break;
      default:
        return;
      }
      if(next_mode != app_state.current_mode)
      {
        active_mode->teardown();
        switch(next_mode)
        {
        case SPLASH_SCREEN:
          active_mode = splash_screen;
          break;
        case SCANNER:
          active_mode = scanner;
          break;
        case LAPTIMER:
          active_mode = lap_timer;
          break;
        }
        active_mode->setup();
        app_state.current_mode = next_mode;
        ble_update(NOTIFY_CURRENT_MODE);
      }
    });

  // TODO: REMOVE, hard-coded for laptimer experiments
  app_state.selected_channel = 27;
  app_state.trigger_arm_threshold = 1750;
  app_state.trigger_disarm_threshold = 1500;
  app_state.trigger_max_latency = 100000;
  app_state.trigger_cooldown = 3000000;
  app_state.min_rssi_reading = 8129; // bigger than anything we can ever read

  uint16_t max_rssi = 0;
  while(1)
  {
    uint32_t status_bits = wait_for_notification();
    if(status_bits & RIGHT_PIN_ISR_FLAG)
    {
      active_mode->input(input_t::RIGHT_BUTTON);
      iobuttons_info();
    }
    if(status_bits & LEFT_PIN_ISR_FLAG)
    {
      active_mode->input(input_t::LEFT_BUTTON);
      iobuttons_info();
    }
    if(status_bits & READER_TASK_WAKEUP_FLAG)
    {
      ble_update(NOTIFY_LAST_RSSI);
    }

    // This is a bit ugly, but for the time
    // being I notify here because the display
    // loop will always update, and the
    // laptimer acquires data from a different
    // core, so marshaling and managing max
    // rssi reading not only becomes more cumbersome,
    // it also would crash the BTStack
    if(max_rssi < app_state.max_rssi_reading)
    {
      max_rssi = app_state.max_rssi_reading;
      ESP_LOGI("main", "max rssi changed: %i", app_state.max_rssi_reading);
      ble_update(NOTIFY_MAX_RSSI);
    }

    ssd1306_clear(&display);
    next_mode = active_mode->update(&display);
    if(next_mode != app_state.current_mode)
    {
      active_mode->teardown();
      switch(next_mode)
      {
      case SPLASH_SCREEN:
        active_mode = splash_screen;
        break;
      case SCANNER:
        active_mode = scanner;
        break;
      case LAPTIMER:
        active_mode = lap_timer;
        break;
      }
      active_mode->setup();
      app_state.current_mode = next_mode;
      ble_update(NOTIFY_CURRENT_MODE);
    }
    ssd1306_update(&display);
    storage.store();
  }
}


extern "C" void btstack_main();
void btstack_main()
{
  ble_init(&app_state);
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
