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

#define LAPTIME_EVENT_VALUE_HANDLE ATT_CHARACTERISTIC_C112478C_9801_481D_8A79_854D23FD9DF2_01_VALUE_HANDLE
#define LAPTIME_EVENT_CONFIGURATION_HANDLE ATT_CHARACTERISTIC_C112478C_9801_481D_8A79_854D23FD9DF2_01_CLIENT_CONFIGURATION_HANDLE

#define FLYBY_VALUE_HANDLE ATT_CHARACTERISTIC_6F1AD544_8B26_46DF_8A7B_314BCB7D1AC5_01_VALUE_HANDLE
#define FLYBY_CONFIGURATION_HANDLE ATT_CHARACTERISTIC_6F1AD544_8B26_46DF_8A7B_314BCB7D1AC5_01_CLIENT_CONFIGURATION_HANDLE

#define TIMESTAMP_EVENT_VALUE_HANDLE ATT_CHARACTERISTIC_438351FA_60D1_424F_A08A_90EA69BE91D5_01_VALUE_HANDLE

#define MESSAGE_OVERHEAD 3 // this is from the streamer example.


const uint8_t adv_data[] = {
    // Flags general discoverable, BR/EDR not supported
    0x02, BLUETOOTH_DATA_TYPE_FLAGS, 0x06,
    // Name
    0x09, BLUETOOTH_DATA_TYPE_COMPLETE_LOCAL_NAME, 'V', 'T', 'X', 'G', 'i', 'z', 'm', 'o',
    0x11, BLUETOOTH_DATA_TYPE_COMPLETE_LIST_OF_128_BIT_SERVICE_CLASS_UUIDS, 0x01, 0xdd, 0xfd, 0x1e, 0x30, 0x09, 0xb6, 0xa8, 0x03, 0x41, 0xbd, 0x4d, 0xb4, 0x29, 0x7e, 0xd2
};
const uint8_t adv_data_len = sizeof(adv_data);

struct laptime_wire_t {
  uint8_t data[sizeof(laptime_t::count) + sizeof(laptime_t::time)]; // uint16_t + int64_t

  laptime_wire_t(laptime_t t)
  {
    uint8_t* p = reinterpret_cast<uint8_t*>(this);
    p = std::copy(
      reinterpret_cast<uint8_t*>(&t.count),
      reinterpret_cast<uint8_t*>(&t.count) + sizeof(decltype(t.count)),
      p);
    std::copy(
      reinterpret_cast<uint8_t*>(&t.time),
      reinterpret_cast<uint8_t*>(&t.time) + sizeof(decltype(t.time)),
      p);
  }

};
} // end ns anonymous

BLE* BLE::s_instance = nullptr;


size_t BLE::transfer_laptimer_data()
{
  const ssize_t buffer_size = _app_state.laptime_buffer.size();
  ssize_t current_pos = _app_state.laptime_buffer_pos;
  auto p = _laptimer_data.data();

  const auto to_transfer = std::min(
    _laptime_max_data_size,
    (ssize_t)(((current_pos + buffer_size) - _last_read_pos) % buffer_size)
    );

  auto start = (current_pos + buffer_size - to_transfer) % buffer_size;
  // from current_pos to the end of the buffer
  if(start > current_pos)
  {
    p = std::copy(
      _app_state.laptime_buffer.begin() + start,
      _app_state.laptime_buffer.end(),
      p
      );
    start = 0;
  }
  std::copy(
    _app_state.laptime_buffer.begin() + start,
    _app_state.laptime_buffer.begin() + current_pos,
    p
    );
  _last_read_pos = current_pos;
  return to_transfer;
}


void BLE::packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size)
{
    UNUSED(channel);
    UNUSED(size);

    switch (packet_type) {
        case HCI_EVENT_PACKET:
            switch (hci_event_packet_get_type(packet)) {
                case ATT_EVENT_MTU_EXCHANGE_COMPLETE:
                  _laptime_max_data_size = att_event_mtu_exchange_complete_get_MTU(packet) - MESSAGE_OVERHEAD;
                  ESP_LOGI("ble", "laptime data size: %i", _laptime_max_data_size);
                  _laptimer_data.resize(_laptime_max_data_size, 0);
                  break;
                case HCI_EVENT_DISCONNECTION_COMPLETE:
                    _le_notification_enabled = 0;
                    _laptime_max_data_size = -1;
                    break;
                case ATT_EVENT_CAN_SEND_NOW:
                  if(_next_notification & NOTIFY_CURRENT_CHANNEL)
                  {
                    att_server_notify(
                      _con_handle,
                      CURRENT_CHANNEL_VALUE_HANDLE,
                      (uint8_t*)&_app_state.selected_channel, sizeof(uint32_t)

                      );
                  }
                  if(_next_notification & NOTIFY_LAST_RSSI)
                  {
                    att_server_notify(
                      _con_handle,
                      LAST_RSSI_VALUE_HANDLE,
                      (uint8_t*)&_app_state.last_read_channel, sizeof(uint32_t)
                      );
                  }
                  if(_next_notification & NOTIFY_CURRENT_MODE)
                  {
                    att_server_notify(
                      _con_handle,
                      CURRENT_MODE_VALUE_HANDLE,
                      (uint8_t*)&_app_state.current_mode, sizeof(int)
                      );
                  }
                  if(_next_notification & NOTIFY_MAX_RSSI)
                  {
                    att_server_notify(
                      _con_handle,
                      MAX_RSSI_VALUE_HANDLE,
                      (uint8_t*)&_app_state.max_rssi_reading, sizeof(uint16_t)
                      );
                  }
                  if(_next_notification & NOTIFY_FLYBY)
                  {
                    const auto last_flyby = _lap_time_tracker.last_flyby();
                    att_server_notify(
                      _con_handle,
                      FLYBY_VALUE_HANDLE,
                      (uint8_t*)&last_flyby, sizeof(ts_t)
                      );
                  }
                  if(_next_notification & NOTIFY_NEW_LAPTIME)
                  {
                    const auto last_laptime = laptime_wire_t(_lap_time_tracker.laptime_at(0));
                    att_server_notify(
                      _con_handle,
                      LAPTIME_EVENT_VALUE_HANDLE,
                      // this is copied, so no worries about last_laptime falling out of scope
                      (uint8_t*)&last_laptime, sizeof(laptime_wire_t)
                      );
                  }
                  _next_notification = 0;
                  break;
            }
            break;
    }
}


uint16_t BLE::att_read_callback(hci_con_handle_t connection_handle, uint16_t att_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size){
    UNUSED(connection_handle);

    // if buffer is NULL, we need to deliver the size of the data
    if(!buffer)
    {
      switch(att_handle)
      {
      case CURRENT_CHANNEL_VALUE_HANDLE:
        return sizeof(_app_state.selected_channel);
      case LAST_RSSI_VALUE_HANDLE:
        // this is a special case, we bundle the
        // two value last_read_channel and last_rssi_reading
        // together!
        return sizeof(_app_state.last_read_channel) * 2;
      case CURRENT_MODE_VALUE_HANDLE:
        return sizeof(_app_state.current_mode);
      case MAX_RSSI_VALUE_HANDLE:
        return sizeof(_app_state.max_rssi_reading);
      case LAPTIME_RSSI_VALUE_HANDLE:
        return transfer_laptimer_data();
      case LAPTIME_EVENT_VALUE_HANDLE:
        return sizeof(laptime_wire_t);
      case TIMESTAMP_EVENT_VALUE_HANDLE:
        return sizeof(ts_t);
      case FLYBY_VALUE_HANDLE:
        return sizeof(ts_t);
      }
    }

    if (att_handle == CURRENT_CHANNEL_VALUE_HANDLE) {
        return att_read_callback_handle_blob((uint8_t*)&_app_state.selected_channel, buffer_size, offset, buffer, buffer_size);
    }
    if (att_handle == LAST_RSSI_VALUE_HANDLE) {
        return att_read_callback_handle_blob((uint8_t*)&_app_state.last_read_channel, buffer_size, offset, buffer, buffer_size);
    }
    if (att_handle == CURRENT_MODE_VALUE_HANDLE) {
        return att_read_callback_handle_blob((uint8_t*)&_app_state.current_mode, buffer_size, offset, buffer, buffer_size);
    }
    if (att_handle == MAX_RSSI_VALUE_HANDLE) {
        return att_read_callback_handle_blob((uint8_t*)&_app_state.max_rssi_reading, buffer_size, offset, buffer, buffer_size);
    }
    if (att_handle == LAPTIME_RSSI_VALUE_HANDLE) {
      return att_read_callback_handle_blob((uint8_t*)_laptimer_data.data(), buffer_size, offset, buffer, buffer_size);
    }
    if (att_handle == LAPTIME_EVENT_VALUE_HANDLE) {
      const auto last_laptime = laptime_wire_t(_lap_time_tracker.laptime_at(0));
      return att_read_callback_handle_blob((uint8_t*)&last_laptime, buffer_size, offset, buffer, buffer_size);
    }
    if (att_handle == TIMESTAMP_EVENT_VALUE_HANDLE) {
      const auto now = esp_timer_get_time();
      return att_read_callback_handle_blob((uint8_t*)&now, buffer_size, offset, buffer, buffer_size);
    }
    if (att_handle == FLYBY_VALUE_HANDLE) {
      const auto flyby = _lap_time_tracker.last_flyby();
      return att_read_callback_handle_blob((uint8_t*)&flyby, buffer_size, offset, buffer, buffer_size);
    }
    return 0;
}

int BLE::att_write_callback(hci_con_handle_t connection_handle, uint16_t att_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size){
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
    case FLYBY_CONFIGURATION_HANDLE:
      _le_notification_enabled |= little_endian_read_16(buffer, 0) == GATT_CLIENT_CHARACTERISTICS_CONFIGURATION_NOTIFICATION;
      _con_handle = connection_handle;
      break;
    case CURRENT_CHANNEL_VALUE_HANDLE:
      ESP_LOGD("ble", "Write current channel");
      assert(buffer_size == sizeof(int));
      _app_state.selected_channel = *(uint32_t*)buffer;
      break;
    case CURRENT_MODE_VALUE_HANDLE:
      ESP_LOGD("ble", "Write current mode");
      assert(buffer_size == sizeof(int));
      if(_change_current_mode_callback)
      {
        _change_current_mode_callback(*(int*)buffer);
      }
      break;
    }
    return 0;
}


void BLE::set_mode_change_callback(std::function<void(int)> cmcb)
{
  _change_current_mode_callback = cmcb;
}


BLE::BLE(app_state_t& app_state, LapTimeTracker& lap_time_tracker)
  : _app_state(app_state)
  , _lap_time_tracker(lap_time_tracker)
  , _change_current_mode_callback(0)
  , _le_notification_enabled(0)
  , _next_notification(0)
  , _last_read_pos(0)
{
  assert(!s_instance);
  s_instance = this;

  l2cap_init();
  // setup le device db
  le_device_db_init();
  // setup SM: Display only
  sm_init();
  // setup ATT server, profile_data is from vtx_scanner_ble.h
  att_server_init(profile_data, &BLE::s_att_read_callback, &BLE::s_att_write_callback);

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
  _hci_event_callback_registration.callback = &BLE::s_packet_handler;
  hci_add_event_handler(&_hci_event_callback_registration);

  // register for ATT event
  att_server_register_packet_handler(&BLE::s_packet_handler);

  hci_power_control(HCI_POWER_ON);
}


void BLE::notify(characeristic_notify_t notify_for)
{
  if (_le_notification_enabled)
  {
    _next_notification |= notify_for;
    att_server_request_can_send_now_event(_con_handle);
  }
}


uint16_t BLE::s_att_read_callback(hci_con_handle_t connection_handle, uint16_t att_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size)
{
  assert(s_instance);
  return BLE::s_instance->att_read_callback(connection_handle, att_handle, offset, buffer, buffer_size);
}


int BLE::s_att_write_callback(hci_con_handle_t connection_handle, uint16_t att_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size)
{
  assert(s_instance);
  return BLE::s_instance->att_write_callback(connection_handle, att_handle, transaction_mode, offset, buffer, buffer_size);
}


void BLE::s_packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size)
{
  assert(s_instance);
  BLE::s_instance->packet_handler(packet_type, channel, packet, size);
}

// TODO: remove
void ble_notify(characeristic_notify_t n)
{
  BLE::s_instance->notify(n);
}


// TODO: remove
void ble_set_mode_change_callback(std::function<void(int)> f)
{
  BLE::s_instance->set_mode_change_callback(f);
}
