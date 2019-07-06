#include "splash-screen.hh"

#include <u8g2.h>


namespace {

#define FONT  u8g2_font_helvB08_tf

#include "deets-fpv-logo-bw.xbm"

sprite_t logo = {
  deets_fpv_logo_bw_bits,
  48, 54,
  48/2, 54/2
};


} // end anon ns

SplashScreen::SplashScreen(app_state_t& app_state, app_mode_t follow_mode)
  : Mode(app_state)
  , _follow_mode(follow_mode)
  , _logo_arrived(nullptr)
  , _early_exit(false)
{
}


void SplashScreen::setup()
{
  periodic(pdMS_TO_TICKS(1000 / 60));
  _x = 200;
  if(_logo_arrived)
  {
    delete _logo_arrived;
  }
  _logo_arrived = nullptr;
  _early_exit = false;
}


void SplashScreen::input(input_t)
{
  _early_exit = true;
}

app_mode_t SplashScreen::update(Display& display)
{
  if(_early_exit)
  {
    return _follow_mode;
  }

  if(_x > 24)
  {
    --_x;
  }
  if(!_logo_arrived && _x == 24)
  {
    _logo_arrived = new int(now());
  }

  display.blit(logo, _x, 32);

  if(_logo_arrived)
  {
    display.font_render(
      FONT,
      "deets FPV",
      128 - display.font_text_width(FONT, "deets FPV"),
      10
      );
  }

  if(_logo_arrived && now() - *_logo_arrived > 500)
  {
    display.font_render(
      FONT,
      "scanner",
      128 - display.font_text_width(FONT, "scanner"),
      64 - 8 - 10
      );
  }
  if(_logo_arrived && now() - *_logo_arrived > 1500)
  {
    return _follow_mode;
  }
  return SPLASH_SCREEN;
}


void SplashScreen::teardown()
{
  periodic(0);
}
