#include "laptimer.hh"


LapTimer::LapTimer(app_state_t& app_state, rtc6715_t& rtc)
  : Mode(app_state)
  , _rtc(rtc)
{

}

void LapTimer::setup()
{
  periodic(pdMS_TO_TICKS(1000 / 60));
}


app_mode_t LapTimer::update(ssd1306_display_t* display)
{
  for(int i=0; i < 128; ++i)
  {
    ssd1306_draw_pixel(display, i, i % 3);
  }
  return LAPTIMER;
}


void LapTimer::teardown()
{
  periodic(0);
}
