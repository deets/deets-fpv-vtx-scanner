#include "splash-screen.hh"

#include "deets_fpv_logo_bw_left.h"
#include "deets_fpv_logo_bw_right.h"
#include "font.h"
#include "pc_senior.h"

#define FONT &pc_senior

SplashScreen::SplashScreen(app_state_t& app_state, app_mode_t follow_mode)
  : Mode(app_state)
  , _follow_mode(follow_mode)
{
}


void SplashScreen::setup()
{
  periodic(pdMS_TO_TICKS(1000 / 60));
  _x = 200;
}


app_mode_t SplashScreen::update(ssd1306_display_t* display)
{
  if(_x > 24)
  {
    --_x;
  }
  ssd1306_blit(display, &deets_fpv_logo_bw_left, _x, 32);
  ssd1306_blit(display, &deets_fpv_logo_bw_right, _x, 32);

  if(total_elapsed_ms() > 1800)
  {
    font_render(
      display,
      FONT,
      "deets FPV",
      128 - font_text_width(FONT, "deets FPV"),
      10
      );
  }

  if(total_elapsed_ms() > 2000)
  {
    font_render(
      display,
      FONT,
      "scanner",
      128 - font_text_width(FONT, "scanner"),
      64 - 8 - 10
      );
  }
  if(total_elapsed_ms() > 3000)
  {
    return _follow_mode;
  }
  return SPLASH_SCREEN;
}


void SplashScreen::teardown()
{
  periodic(0);
}
