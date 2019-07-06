#include "appstate.hh"
#include "display.hh"
#include "ble.hh"
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
#include <memory>


// RX5808
#define RTC_ADC_CHANNEL ADC1_CHANNEL_6 // GPIO 34
#define RTC_CS 14
#define RTC_MOSI 13
#define RTC_CLK 12

#define BUZZER_PIN GPIO_NUM_21

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

  Display display;

  rtc6715_t rtc;
  rtc6715_setup(
    &rtc,
    RTC_ADC_CHANNEL,
    RTC_CS,
    RTC_CLK,
    RTC_MOSI
    );

  buzzer_setup(BUZZER_PIN);
  buzzer_buzz(100, 3);

  Storage storage(app_state);

  ModeManager modes(
    app_state,
    SPLASH_SCREEN,
    std::unique_ptr<Mode>(new SplashScreen(app_state, SCANNER))
    );
  modes.add_mode(
    SCANNER,
    std::unique_ptr<Mode>(new Scanner(app_state, rtc))
    );
  modes.add_mode(
    LAPTIMER,
    std::unique_ptr<Mode>(new LapTimer(app_state, rtc))
    );

  ble_set_mode_change_callback(
    [&modes](int mode) {
      modes.change_active_mode(mode);
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
      ESP_LOGI("main", "BUTTON_RIGHT");
      modes.input(input_t::RIGHT_BUTTON);
    }
    if(status_bits & LEFT_PIN_ISR_FLAG)
    {
      ESP_LOGI("main", "BUTTON_LEFT");
      modes.input(input_t::LEFT_BUTTON);
    }
    if(status_bits & UP_PIN_ISR_FLAG)
    {
      ESP_LOGI("main", "BUTTON_UP");
      modes.input(input_t::MODE_BUTTON);
    }
    if(status_bits & DOWN_PIN_ISR_FLAG)
    {
      ESP_LOGI("main", "BUTTON_DOWN");
      modes.input(input_t::SETTINGS_BUTTON);
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

    display.clear();
    modes.update(display);
    display.update();
    storage.store();
  }
}


extern "C" void btstack_main();
void btstack_main()
{
  ble_init(&app_state);
  app_state.laptime_buffer.resize(8192, 0);
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
