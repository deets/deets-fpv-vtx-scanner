#include "storage.hh"

#include <esp_log.h>

#define DEETS_FVP_SCANNER_NAMESPACE "de-fpv-scan"

#define K_SELECTED_CHANNEL "sel-chan"

Storage::Storage(app_state_t& app_state)
  : _app_state(app_state)
{
  esp_err_t err = nvs_flash_init();
  if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    // NVS partition was truncated and needs to be erased
    // Retry nvs_flash_init
    ESP_ERROR_CHECK(nvs_flash_erase());
    err = nvs_flash_init();
  }
  ESP_ERROR_CHECK(err);
  err = nvs_open(DEETS_FVP_SCANNER_NAMESPACE, NVS_READWRITE, &_nvs_handle);
  ESP_ERROR_CHECK(err);

  // restore restores the real
  // app_state, so *after* that we need to copy
  restore();
  _persisted_app_state = app_state;
}


void Storage::store()
{
  esp_err_t err;
  bool commit = false;
  if(_app_state.selected_channel != _persisted_app_state.selected_channel)
  {
    ESP_LOGE("storage", "Storing selected_channel");
    err = nvs_set_u32(_nvs_handle, K_SELECTED_CHANNEL, _app_state.selected_channel);
    commit = (err == ESP_OK || err == ESP_ERR_NVS_NOT_FOUND);
    if(!commit)
    {
      ESP_LOGE("storage", "Failed storing selected_channel");
    }
  }
  if(commit)
  {
    err = nvs_commit(_nvs_handle);
  }
  _persisted_app_state = _app_state;
}


void Storage::restore()
{
  ESP_LOGD("storage", "Storage::restore");

  // we can ignore the error because
  // it won't touch the passed field.
  auto err = nvs_get_u32(_nvs_handle, K_SELECTED_CHANNEL, &_app_state.selected_channel);
  switch (err) {
  case ESP_OK:
    break;
  case ESP_ERR_NVS_NOT_FOUND:
    break;
  default :
    ESP_LOGE("storage", "Error (%s) reading!", esp_err_to_name(err));
  }
}
