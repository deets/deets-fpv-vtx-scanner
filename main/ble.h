// Copyright: 2019, Diez B. Roggisch, Berlin, all rights reserved

#include "appstate.h"

#ifdef __cplusplus
extern "C"  {
#endif

typedef enum {
  NOTIFY_CURRENT_CHANNEL=1 << 0,
  NOTIFY_LAST_RSSI=1 << 1,
} characeristic_notify_t;

void ble_init(app_state_t* app_state);
void ble_update(characeristic_notify_t);

#ifdef __cplusplus
}
#endif
