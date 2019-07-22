#include "ble.hh"
#include "vtx_scanner_ble.h"

#include <btstack.h>

#include <esp_log.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <cassert>

namespace {

#define CURRENT_CHANNEL_VALUE_HANDLE ATT_CHARACTERISTIC_93E9A984_40CD_4B57_A31A_3D3857D80A09_01_VALUE_HANDLE
#define CURRENT_CHANNEL_CLIENT_CONFIGURATION_HANDLE ATT_CHARACTERISTIC_93E9A984_40CD_4B57_A31A_3D3857D80A09_01_CLIENT_CONFIGURATION_HANDLE

#define LAST_RSSI_VALUE_HANDLE ATT_CHARACTERISTIC_FBD47252_6210_4692_A247_1DB3007CF668_01_VALUE_HANDLE
#define LAST_RSSI_CLIENT_CONFIGURATION_HANDLE ATT_CHARACTERISTIC_FBD47252_6210_4692_A247_1DB3007CF668_01_CLIENT_CONFIGURATION_HANDLE

#define CURRENT_MODE_VALUE_HANDLE ATT_CHARACTERISTIC_903BB704_5ADC_48D3_B0D4_0628BDB35250_01_VALUE_HANDLE
#define CURRENT_MODE_CLIENT_CONFIGURATION_HANDLE ATT_CHARACTERISTIC_903BB704_5ADC_48D3_B0D4_0628BDB35250_01_CLIENT_CONFIGURATION_HANDLE

#define MAX_RSSI_VALUE_HANDLE ATT_CHARACTERISTIC_538D01D2_662F_4C0E_A808_1F23F159DF1A_01_VALUE_HANDLE
#define MAX_RSSI_CLIENT_CONFIGURATION_HANDLE ATT_CHARACTERISTIC_538D01D2_662F_4C0E_A808_1F23F159DF1A_01_CLIENT_CONFIGURATION_HANDLE

#define LAPTIME_RSSI_VALUE_HANDLE ATT_CHARACTERISTIC_135BFFE1_E787_4A27_9402_D76493424B53_01_VALUE_HANDLE
#define LAPTIME_RSSI_CLIENT_CONFIGURATION_HANDLE ATT_CHARACTERISTIC_135BFFE1_E787_4A27_9402_D76493424B53_01_CLIENT_CONFIGURATION_HANDLE

#define MESSAGE_OVERHEAD 3 // this is from the streamer example.

app_state_t* app_state = 0;
int next_notification = 0;
std::function<void(int)> change_current_mode_callback = 0;
int  le_notification_enabled;
int laptime_max_data_size;
std::vector<uint8_t> laptimer_data;
decltype(app_state->laptime_buffer_pos) last_read_pos;
btstack_packet_callback_registration_t hci_event_callback_registration;
hci_con_handle_t con_handle;


void packet_handler (uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size);
uint16_t att_read_callback(hci_con_handle_t con_handle, uint16_t att_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size);
int att_write_callback(hci_con_handle_t con_handle, uint16_t att_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size);

const uint8_t adv_data[] = {
    // Flags general discoverable, BR/EDR not supported
    0x02, BLUETOOTH_DATA_TYPE_FLAGS, 0x06,
    // Name
    0x09, BLUETOOTH_DATA_TYPE_COMPLETE_LOCAL_NAME, 'V', 'T', 'X', 'G', 'i', 'z', 'm', 'o',
    0x11, BLUETOOTH_DATA_TYPE_COMPLETE_LIST_OF_128_BIT_SERVICE_CLASS_UUIDS, 0x01, 0xdd, 0xfd, 0x1e, 0x30, 0x09, 0xb6, 0xa8, 0x03, 0x41, 0xbd, 0x4d, 0xb4, 0x29, 0x7e, 0xd2
};
const uint8_t adv_data_len = sizeof(adv_data);

size_t transfer_laptimer_data()
{
  const auto buffer_size = app_state->laptime_buffer.size();
  auto current_pos = app_state->laptime_buffer_pos;
  auto p = laptimer_data.data();
  const auto to_transfer = std::min(
    laptime_max_data_size,
    (int)(((current_pos - last_read_pos) + buffer_size) % buffer_size)
    );
  auto start = current_pos - to_transfer;
  // from current_pos to the end of the buffer
  if(start > current_pos)
  {
    std::copy(
      app_state->laptime_buffer.begin() + start,
      app_state->laptime_buffer.end(),
      p
      );
    p += app_state->laptime_buffer.end() - (app_state->laptime_buffer.begin() + start);
    start = 0;
  }
  std::copy(
    app_state->laptime_buffer.begin() + start,
    app_state->laptime_buffer.begin() + current_pos,
    p
    );
  last_read_pos = current_pos;
  return to_transfer;
}

void packet_handler (uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size){
    UNUSED(channel);
    UNUSED(size);

    switch (packet_type) {
        case HCI_EVENT_PACKET:
            switch (hci_event_packet_get_type(packet)) {
                case ATT_EVENT_MTU_EXCHANGE_COMPLETE:
                  laptime_max_data_size = att_event_mtu_exchange_complete_get_MTU(packet) - MESSAGE_OVERHEAD;
                  ESP_LOGI("ble", "laptime data size: %i", laptime_max_data_size);
                  laptimer_data.resize(laptime_max_data_size, 0);
                  break;
                case HCI_EVENT_DISCONNECTION_COMPLETE:
                    le_notification_enabled = 0;
                    laptime_max_data_size = -1;
                    break;
                case ATT_EVENT_CAN_SEND_NOW:
                  if(next_notification & NOTIFY_CURRENT_CHANNEL)
                  {
                    att_server_notify(
                      con_handle,
                      CURRENT_CHANNEL_VALUE_HANDLE,
                      (uint8_t*)&app_state->selected_channel, sizeof(uint32_t)

                      );
                  }
                  if(next_notification & NOTIFY_LAST_RSSI)
                  {
                    att_server_notify(
                      con_handle,
                      LAST_RSSI_VALUE_HANDLE,
                      (uint8_t*)&app_state->last_read_channel, sizeof(uint32_t)
                      );
                  }
                  if(next_notification & NOTIFY_CURRENT_MODE)
                  {
                    att_server_notify(
                      con_handle,
                      CURRENT_MODE_VALUE_HANDLE,
                      (uint8_t*)&app_state->current_mode, sizeof(int)
                      );
                  }
                  if(next_notification & NOTIFY_MAX_RSSI)
                  {
                    att_server_notify(
                      con_handle,
                      MAX_RSSI_VALUE_HANDLE,
                      (uint8_t*)&app_state->max_rssi_reading, sizeof(uint16_t)
                      );
                  }
                  next_notification = 0;
                  break;
            }
            break;
    }
}

uint16_t att_read_callback(hci_con_handle_t connection_handle, uint16_t att_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size){
    UNUSED(connection_handle);

    // if buffer is NULL, we need to deliver the size of the data
    if(!buffer)
    {
      switch(att_handle)
      {
      case CURRENT_CHANNEL_VALUE_HANDLE:
        return sizeof(app_state->selected_channel);
      case LAST_RSSI_VALUE_HANDLE:
        // this is a special case, we bundle the
        // two value last_read_channel and last_rssi_reading
        // together!
        return sizeof(app_state->last_read_channel) * 2;
      case CURRENT_MODE_VALUE_HANDLE:
        return sizeof(app_state->current_mode);
      case MAX_RSSI_VALUE_HANDLE:
        return sizeof(app_state->max_rssi_reading);
      case LAPTIME_RSSI_VALUE_HANDLE:
        return transfer_laptimer_data();
      }
    }

    if (att_handle == CURRENT_CHANNEL_VALUE_HANDLE) {
        return att_read_callback_handle_blob((uint8_t*)&app_state->selected_channel, buffer_size, offset, buffer, buffer_size);
    }
    if (att_handle == LAST_RSSI_VALUE_HANDLE) {
        return att_read_callback_handle_blob((uint8_t*)&app_state->last_read_channel, buffer_size, offset, buffer, buffer_size);
    }
    if (att_handle == CURRENT_MODE_VALUE_HANDLE) {
        return att_read_callback_handle_blob((uint8_t*)&app_state->current_mode, buffer_size, offset, buffer, buffer_size);
    }
    if (att_handle == MAX_RSSI_VALUE_HANDLE) {
        return att_read_callback_handle_blob((uint8_t*)&app_state->max_rssi_reading, buffer_size, offset, buffer, buffer_size);
    }
    if (att_handle == LAPTIME_RSSI_VALUE_HANDLE) {
      return att_read_callback_handle_blob((uint8_t*)laptimer_data.data(), buffer_size, offset, buffer, buffer_size);
    }
    return 0;
}

int att_write_callback(hci_con_handle_t connection_handle, uint16_t att_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size){
    UNUSED(transaction_mode);
    UNUSED(offset);
    UNUSED(buffer_size);

    switch(att_handle)
    {
      // Any chracteristic establishs the connection
    case CURRENT_CHANNEL_CLIENT_CONFIGURATION_HANDLE:
    case LAST_RSSI_CLIENT_CONFIGURATION_HANDLE:
    case CURRENT_MODE_CLIENT_CONFIGURATION_HANDLE:
    case MAX_RSSI_CLIENT_CONFIGURATION_HANDLE:
    case LAPTIME_RSSI_CLIENT_CONFIGURATION_HANDLE:
      le_notification_enabled |= little_endian_read_16(buffer, 0) == GATT_CLIENT_CHARACTERISTICS_CONFIGURATION_NOTIFICATION;
      con_handle = connection_handle;
      break;
    case CURRENT_CHANNEL_VALUE_HANDLE:
      ESP_LOGD("ble", "Write current channel");
      assert(buffer_size == sizeof(int));
      app_state->selected_channel = *(uint32_t*)buffer;
      break;
    case CURRENT_MODE_VALUE_HANDLE:
      ESP_LOGD("ble", "Write current mode");
      assert(buffer_size == sizeof(int));
      if(change_current_mode_callback)
      {
        change_current_mode_callback(*(int*)buffer);
      }
      break;
    }
    return 0;
}

} // end ns anonymous


void ble_set_mode_change_callback(std::function<void(int)> cmcb)
{
  change_current_mode_callback = cmcb;
}

void ble_init(app_state_t* app_state_p)
{
  app_state = app_state_p;
  l2cap_init();

  // setup le device db
  le_device_db_init();

  // setup SM: Display only
  sm_init();

  // setup ATT server
  att_server_init(profile_data, att_read_callback, att_write_callback);

  // setup advertisements
  uint16_t adv_int_min = 0x0030;
  uint16_t adv_int_max = 0x0030;
  uint8_t adv_type = 0;
  bd_addr_t null_addr;
  memset(null_addr, 0, 6);
  gap_advertisements_set_params(adv_int_min, adv_int_max, adv_type, 0, null_addr, 0x07, 0x00);
  gap_advertisements_set_data(adv_data_len, (uint8_t*) adv_data);
  gap_advertisements_enable(1);

  // register for HCI events
  hci_event_callback_registration.callback = &packet_handler;
  hci_add_event_handler(&hci_event_callback_registration);

  // register for ATT event
  att_server_register_packet_handler(packet_handler);

  hci_power_control(HCI_POWER_ON);
}


void ble_update(characeristic_notify_t notify_for)
{
  if (le_notification_enabled)
  {
    next_notification |= notify_for;
    att_server_request_can_send_now_event(con_handle);
  }
}
