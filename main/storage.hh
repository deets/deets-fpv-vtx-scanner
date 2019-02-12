// Copyright: 2019, Diez B. Roggisch, Berlin, all rights reserved
#pragma once

#include "appstate.h"

#include "nvs_flash.h"
#include "nvs.h"

class Storage
{
public:
  Storage(app_state_t&);

  void store();
private:
  void restore();

  app_state_t& _app_state;
  app_state_t _persisted_app_state;

  nvs_handle _nvs_handle;
};
