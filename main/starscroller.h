// Copyright: 2018, Diez B. Roggisch, Berlin, all rights reserved

typedef struct {
  int x, y, z;
} star_t;

typedef struct {
  int x, y;
} coordinate_t;

typedef struct {
  int starcount;
  int distance;
  int speed;
  int factor;
  int viewport_width;
  int viewport_height;
  star_t* stars;
  coordinate_t* projections;
} starscroller_handle_t;

starscroller_handle_t* starscroller_create(
  int count,
  int distance,
  int speed,
  int factor,
  int viewport_width,
  int viewport_height
  );

void starscroller_step(starscroller_handle_t* stars);
