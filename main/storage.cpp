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

template<typename T>
struct NVSLoadStore
{
  esp_err_t store(const T&);
  esp_err_t restore(T*);
};

template<>
struct NVSLoadStore<ts_t>
{
  esp_err_t store(nvs_handle nvs_handle, const char* name, const ts_t& value)
  {
    return nvs_set_i64(nvs_handle, name, value);
  }

  esp_err_t restore(nvs_handle nvs_handle, const char* name, ts_t* value)
  {
    return nvs_get_i64(nvs_handle, name, value);
  }

};

template<>
struct NVSLoadStore<uint32_t>
{
  esp_err_t store(nvs_handle nvs_handle, const char* name, const uint32_t& value)
  {
    return nvs_set_u32(nvs_handle, name, value);
  }

  esp_err_t restore(nvs_handle nvs_handle, const char* name, uint32_t* value)
  {
    return nvs_get_u32(nvs_handle, name, value);
  }

};

template<>
struct NVSLoadStore<int>
{
  esp_err_t store(nvs_handle nvs_handle, const char* name, const int& value)
  {
    return nvs_set_i32(nvs_handle, name, value);
  }

  esp_err_t restore(nvs_handle nvs_handle, const char* name, int* value)
  {
    return nvs_get_i32(nvs_handle, name, value);
  }

};

template<>
struct NVSLoadStore<bool>
{
  esp_err_t store(nvs_handle nvs_handle, const char* name, const bool& value)
  {
    return nvs_set_u8(nvs_handle, name, value);
  }

  esp_err_t restore(nvs_handle nvs_handle, const char* name, bool* value)
  {
    uint8_t v;
    const auto res = nvs_get_u8(nvs_handle, name, &v);
    *value = (bool)v;
    return res;
  }

};

} // end ns anonymous

#define CHECK_AND_STORE(attributename) \
  if(_app_state.attributename != _persisted_app_state.attributename) \
  { \
    ESP_LOGE("storage", "Storing "#attributename); \
    err = NVSLoadStore<decltype(_app_state.attributename)>{}.store( \
      _nvs_handle, hash(#attributename).c_str(), _app_state.attributename); \
    commit &= (err == ESP_OK || err == ESP_ERR_NVS_NOT_FOUND); \
  } \


#define RESTORE(attributename) \
  err = NVSLoadStore<decltype(_app_state.attributename)>{}.restore(_nvs_handle, hash(#attributename).c_str(), &_app_state.attributename); \
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
  CHECK_AND_STORE(peak_detection_config.trigger_threshold_percent);
  CHECK_AND_STORE(peak_detection_config.laptime);
  CHECK_AND_STORE(use_buzzer);
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
  RESTORE(peak_detection_config.trigger_threshold_percent);
  RESTORE(peak_detection_config.laptime);
  RESTORE(use_buzzer);
}
