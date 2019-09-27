#include "storage.hh"

#include <esp_log.h>
#include <functional>
#include <sstream>

#define DEETS_FVP_SCANNER_NAMESPACE "de-fpv-scan"

#define K_SELECTED_CHANNEL "sel-chan"

namespace {
std::string hash(const char* arg)
{
  const auto h = std::hash<std::string>{}(arg);
  std::stringstream s;
  s << h;
  return s.str();
}

} // end ns anonymous

#define CHECK_AND_STORE(attributename) \
  if(_app_state.attributename != _persisted_app_state.attributename) \
  { \
    ESP_LOGE("storage", "Storing "#attributename); \
    err = nvs_set_u32(_nvs_handle, hash(#attributename).c_str(), _app_state.attributename); \
    commit &= (err == ESP_OK || err == ESP_ERR_NVS_NOT_FOUND); \
  } \


#define RESTORE(attributename) \
  err = nvs_get_u32(_nvs_handle, hash(#attributename).c_str(), (uint32_t*)&_app_state.attributename); \
  switch (err) { \
  case ESP_OK: \
    break; \
  case ESP_ERR_NVS_NOT_FOUND: \
    break; \
  default : \
    ESP_LOGE("storage", "Error (%s) reading!", esp_err_to_name(err)); \
  }

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
  bool commit = true;
  CHECK_AND_STORE(selected_channel);
  CHECK_AND_STORE(peak_detection_config.peak_size);
  if(commit)
  {
    err = nvs_commit(_nvs_handle);
  }
  _persisted_app_state = _app_state;
}


void Storage::restore()
{
  ESP_LOGD("storage", "Storage::restore");

  esp_err_t err;
  RESTORE(selected_channel);
  RESTORE(peak_detection_config.peak_size);
}
