// Copyright: 2019, Diez B. Roggisch, Berlin, all rights reserved

#include "appstate.hh"
#include "laptime_tracker.hh"

#include <btstack.h>

#include <functional>

enum characeristic_notify_t
{
  NOTIFY_CURRENT_CHANNEL=1 << 0,
  NOTIFY_LAST_RSSI=1 << 1,
  NOTIFY_CURRENT_MODE=1 << 2,
  NOTIFY_MAX_RSSI=1 << 3,
  NOTIFY_LAPTIME_RSSI_VALUE=1 << 4,
  NOTIFY_NEW_LAPTIME=1 << 5
};

class BLE {

public:

  BLE(app_state_t& app_state, LapTimeTracker& lap_time_tracker);
  void set_mode_change_callback(std::function<void(int)>);
  void notify(characeristic_notify_t);

private:
  BLE(const BLE&)=delete;

  static BLE* s_instance;

  static uint16_t s_att_read_callback(hci_con_handle_t connection_handle, uint16_t att_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size);
  static int s_att_write_callback(hci_con_handle_t connection_handle, uint16_t att_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size);
  static void s_packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size);

  uint16_t att_read_callback(hci_con_handle_t connection_handle, uint16_t att_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size);
  int att_write_callback(hci_con_handle_t connection_handle, uint16_t att_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size);
  void packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size);

  size_t transfer_laptimer_data();

  app_state_t& _app_state;
  LapTimeTracker& _lap_time_tracker;

  std::function<void(int)> _change_current_mode_callback;
  int  _le_notification_enabled;
  int _next_notification;

  ssize_t _laptime_max_data_size;
  std::vector<uint8_t> _laptimer_data;
  ssize_t _last_read_pos;

  btstack_packet_callback_registration_t _hci_event_callback_registration;
  hci_con_handle_t _con_handle;

  friend void ble_notify(characeristic_notify_t);
  friend void ble_set_mode_change_callback(std::function<void(int)>);
};


void ble_notify(characeristic_notify_t);
void ble_set_mode_change_callback(std::function<void(int)>);
