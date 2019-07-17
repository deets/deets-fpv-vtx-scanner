// Copyright: 2018, Diez B. Roggisch, Berlin, all rights reserved
#include "starscroller.h"

#include "esp_system.h"

static int rand_range(int start, int end)
{
  int r = esp_random();
  r = r % (end - start);
  r += start;
  return r;
}

static void starscroller_place_star(starscroller_handle_t* stars, star_t* star)
{
  star->x = rand_range(-1024, 1024);
  star->y = rand_range(-1024, 1024);
  star->z = rand_range(0, stars->distance) + stars->distance;
}


starscroller_handle_t* starscroller_create(
  int count,
  int distance,
  int speed,
  int factor,
  int viewport_width,
  int viewport_height
  )
{
  starscroller_handle_t* res = malloc(sizeof(starscroller_handle_t));
  res->starcount = count;
  res->distance = distance;
  res->speed = speed;
  res->factor = factor;
  res->viewport_width = viewport_width;
  res->viewport_height = viewport_height;

  res->stars = malloc(sizeof(star_t) * count);
  res->projections = malloc(sizeof(coordinate_t) * count);
  for(int i=0; i < count; ++i)
  {
    starscroller_place_star(res, &res->stars[i]);
  }
  return res;
}

void starscroller_step(starscroller_handle_t* stars)
{
  for(int i=0; i < stars->starcount; ++i)
  {
    stars->stars[i].z -= stars->speed;
    if(stars->stars[i].z <= 0)
    {
      starscroller_place_star(stars, &stars->stars[i]);
    }
    stars->projections[i].x = stars->stars[i].x * stars->factor / stars->stars[i].z + (stars->viewport_width / 2);
    stars->projections[i].y = stars->stars[i].y * stars->factor / stars->stars[i].z + (stars->viewport_height / 2);
  }
}
