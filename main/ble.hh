// Copyright: 2019, Diez B. Roggisch, Berlin, all rights reserved

#include "appstate.hh"

#include <functional>

enum characeristic_notify_t
{
  NOTIFY_CURRENT_CHANNEL=1 << 0,
  NOTIFY_LAST_RSSI=1 << 1,
  NOTIFY_CURRENT_MODE=1 << 2,
  NOTIFY_MAX_RSSI=1 << 3,
  NOTIFY_LAPTIME_RSSI_VALUE=1 << 4
};

void ble_init(app_state_t* app_state);
void ble_set_mode_change_callback(std::function<void(int)>);
void ble_update(characeristic_notify_t);
