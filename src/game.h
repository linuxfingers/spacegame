#ifndef GAME_H
#define GAME_H

typedef struct{       //16-bit minimum unsigned int 2d vector
  uint_fast8_t x;
  uint_fast8_t y;
} vec2ui;

typedef struct{       //16-bit minimum signed int 2d vector
  uint_fast8_t x;
  uint_fast8_t y;
} vec2i;

typedef struct{       //int rect
  vec2i offset;
  vec2i bounds;

  uint_fast16_t top() { return offset.y; }
  uint_fast16_t bot() { return offset.y + bounds.y; }
  uint_fast16_t left() { return offset.x; }
  uint_fast16_t right() { return offset.x + bounds.x; }
  uint_fast16_t width() { return bounds.x; }
  uint_fast16_t height() { return bounds.y; }

  bool contains(vec2i a) { return (a.x >= offset.x && a.x < right()) &&
                                  (a.y >= offset.y && a.y < bot()); }
} rect;

struct enemy{
  vec2i pos;
};

struct star {
  vec2i pos;
};

int init();
void run();
void close();

#endif
