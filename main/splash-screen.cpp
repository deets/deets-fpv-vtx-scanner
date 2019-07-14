#include "splash-screen.hh"
#include "font.h"

#include <u8g2.h>


namespace {

const auto LOGO_SPEED = 2;
const auto LOGO_POS = 24;

#include "deets-fpv-logo-bw.xbm"

sprite_t logo = {
  deets_fpv_logo_bw_bits,
  48, 54,
  48/2, 54/2
};

int now()
{
  return xTaskGetTickCount() * 1000 / configTICK_RATE_HZ;
}

} // end anon ns

SplashScreen::SplashScreen(app_state_t& app_state, app_mode_t follow_mode)
  : Mode(app_state)
  , _follow_mode(follow_mode)
  , _logo_arrived(nullptr)
  , _early_exit(false)
{
}


void SplashScreen::setup_impl()
{
  _x = 200;
  if(_logo_arrived)
  {
    delete _logo_arrived;
  }
  _logo_arrived = nullptr;
  _early_exit = false;
  periodic(16);
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

  if(_x > LOGO_POS)
  {
    _x -= LOGO_SPEED;
  }
  if(!_logo_arrived && _x == LOGO_POS)
  {
    _logo_arrived = new int(now());
  }

  display.blit(logo, _x, 32);

  if(_logo_arrived)
  {
    display.font_render(
      NORMAL,
      "deets FPV",
      128 - display.font_text_width(NORMAL, "deets FPV"),
      10
      );
  }

  if(_logo_arrived && now() - *_logo_arrived > 500)
  {
    display.font_render(
      NORMAL,
      "scanner",
      128 - display.font_text_width(NORMAL, "scanner"),
      64 - 8 - 10
      );
  }
  if(_logo_arrived && now() - *_logo_arrived > 1500)
  {
    return _follow_mode;
  }
  return SPLASH_SCREEN;
}
