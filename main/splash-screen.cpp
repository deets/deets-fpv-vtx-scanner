#include "splash-screen.hh"

#include "deets_fpv_logo_bw_left.h"
#include "deets_fpv_logo_bw_right.h"


void SplashScreen::setup()
{
  periodic(pdMS_TO_TICKS(1000 / 60));
  _x = 200;
}


void SplashScreen::update(ssd1306_display_t* display)
{
  if(_x > 24)
  {
    --_x;
  }
  ssd1306_blit(display, &deets_fpv_logo_bw_left, _x, 32);
  ssd1306_blit(display, &deets_fpv_logo_bw_right, _x, 32);
}


void SplashScreen::teardown()
{
  periodic(0);
}
