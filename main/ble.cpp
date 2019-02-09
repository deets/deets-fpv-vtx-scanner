#include "ble.h"
#include "vtx_scanner_ble.h"
#include "btstack.h"

#include <esp_log.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


namespace {

app_state_t* app_state = 0;
int next_notification = 0;

#define CURRENT_CHANNEL_VALUE_HANDLE ATT_CHARACTERISTIC_93E9A984_40CD_4B57_A31A_3D3857D80A09_01_VALUE_HANDLE
#define CURRENT_CHANNEL_CLIENT_CONFIGURATION_HANDLE ATT_CHARACTERISTIC_93E9A984_40CD_4B57_A31A_3D3857D80A09_01_CLIENT_CONFIGURATION_HANDLE

#define LAST_RSSI_VALUE_HANDLE ATT_CHARACTERISTIC_FBD47252_6210_4692_A247_1DB3007CF668_01_VALUE_HANDLE
#define LAST_RSSI_CLIENT_CONFIGURATION_HANDLE ATT_CHARACTERISTIC_FBD47252_6210_4692_A247_1DB3007CF668_01_CLIENT_CONFIGURATION_HANDLE

int  le_notification_enabled;
btstack_packet_callback_registration_t hci_event_callback_registration;
hci_con_handle_t con_handle;


void packet_handler (uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size);
uint16_t att_read_callback(hci_con_handle_t con_handle, uint16_t att_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size);
int att_write_callback(hci_con_handle_t con_handle, uint16_t att_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size);

const uint8_t adv_data[] = {
    // Flags general discoverable, BR/EDR not supported
    0x02, BLUETOOTH_DATA_TYPE_FLAGS, 0x06,
    // Name
    0x0a, BLUETOOTH_DATA_TYPE_COMPLETE_LOCAL_NAME, 'V', 'T', 'X', ' ', 'G', 'i', 'z', 'm', 'o',
    // Incomplete List of 16-bit Service Class UUIDs -- FF10 - only valid for testing!
    0x03, BLUETOOTH_DATA_TYPE_INCOMPLETE_LIST_OF_16_BIT_SERVICE_CLASS_UUIDS, 0x10, 0xff,
};
const uint8_t adv_data_len = sizeof(adv_data);


void packet_handler (uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size){
    UNUSED(channel);
    UNUSED(size);

    switch (packet_type) {
        case HCI_EVENT_PACKET:
            switch (hci_event_packet_get_type(packet)) {
                case HCI_EVENT_DISCONNECTION_COMPLETE:
                    le_notification_enabled = 0;
                    break;
                case ATT_EVENT_CAN_SEND_NOW:
                  if(next_notification & NOTIFY_CURRENT_CHANNEL)
                  {
                    att_server_notify(
                      con_handle,
                      CURRENT_CHANNEL_VALUE_HANDLE,
                      (uint8_t*)&app_state->scanner_state.channels.cursor_pos, sizeof(int)

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
                  next_notification = 0;
                  break;
            }
            break;
    }
}

uint16_t att_read_callback(hci_con_handle_t connection_handle, uint16_t att_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size){
    UNUSED(connection_handle);

    if (att_handle == CURRENT_CHANNEL_VALUE_HANDLE){
        return att_read_callback_handle_blob((uint8_t*)&app_state->current_channel, buffer_size, offset, buffer, buffer_size);
    }
    if (att_handle == LAST_RSSI_VALUE_HANDLE){
        return att_read_callback_handle_blob((uint8_t*)&app_state->last_read_channel, buffer_size, offset, buffer, buffer_size);
    }
    return 0;
}

int att_write_callback(hci_con_handle_t connection_handle, uint16_t att_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size){
    UNUSED(transaction_mode);
    UNUSED(offset);
    UNUSED(buffer_size);

    switch(att_handle)
    {
    case CURRENT_CHANNEL_CLIENT_CONFIGURATION_HANDLE:
    case LAST_RSSI_CLIENT_CONFIGURATION_HANDLE:
      le_notification_enabled |= little_endian_read_16(buffer, 0) == GATT_CLIENT_CHARACTERISTICS_CONFIGURATION_NOTIFICATION;
      con_handle = connection_handle;
      break;
    case CURRENT_CHANNEL_VALUE_HANDLE:
      ESP_LOGI("ble", "Write current channel");
      assert(buffer_size == sizeof(int));
      app_state->scanner_state.channels.cursor_pos = *(int*)buffer;
      break;
    }
    return 0;
}

} // end ns anonymous


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
