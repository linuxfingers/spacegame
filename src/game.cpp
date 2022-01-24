#include <unistd.h>
#include <ncurses.h>

#include <cstdint>
#include <string>
#include <vector>

#include "game.h"
#include "ObjectField.h"

//globals

WINDOW* main_wnd;
WINDOW* game_wnd;

rect game_area;
rect screen_area;

vec2ui cur_size;

ObjectField asteroids;
ObjectField stars;

struct {
  vec2i pos;
  rect bounds;
  char disp_char;
  int energy;
} player;

int init(){
  srand(time(0));

  //CURSES!
  main_wnd = initscr();
  cbreak();
  noecho();
  clear();
  refresh();

  curs_set(0);
  start_color();
  screen_area = { { 0, 0}, {80, 24} };
  int infopanel_height = 4;
  game_wnd = newwin(  screen_area.height() - infopanel_height - 2,   //defining bounds of the two windows
                      screen_area.width() - 2,
                      screen_area.top() + 1,
                      screen_area.left() + 1 );
  main_wnd = newwin(screen_area.height(), screen_area.width(), 0, 0);

  game_area = { {0, 0}, { screen_area.width() - 2, screen_area.height() - infopanel_height - 4 } };

  init_pair(1, COLOR_WHITE, COLOR_BLACK);
  init_pair(2, COLOR_GREEN, COLOR_BLACK);
  init_pair(3, COLOR_YELLOW, COLOR_BLACK);
  init_pair(4, COLOR_RED, COLOR_BLACK);
  init_pair(5, COLOR_BLUE, COLOR_BLACK);

  //enable fn keys
  keypad(main_wnd, true);
  keypad(game_wnd, true);

  //disable input blocking
  nodelay(main_wnd, true);
  nodelay(game_wnd, true);

  if(!has_colors()){
    endwin();
    printf("ERROR: Sorry. Your terminal does not support color.\n");
    exit(1);
  }

  return 0;
}



  /*
  wnd = initscr();
  cbreak();
  noecho();
  clear();
  refresh();

  keypad(wnd, true);  //keyboard functions
  nodelay(wnd, true);
  curs_set(0);        //hide cursor
  if(!has_colors()){
    endwin();
    printf("ERROR: Sorry. Your terminal does not support color.\n");
    exit(1);
  }

  start_color();
  attron(A_BOLD);
  box(wnd, 0, 0);
  attroff(A_BOLD);

  init_pair(1, COLOR_WHITE, COLOR_BLACK);
  wbkgd(wnd, COLOR_PAIR(1));

  return 0;
  */

void run(){

  int tick;

  player.disp_char = 'o';                       //initialize character
  player.pos = {10, 5};

  stars.setBounds(game_area);                   //star bounds
  asteroids.setBounds(game_area);

  int in_char = 0;
  bool exit_requested = false;
  bool game_over = false;

  //window frame
  wattron(main_wnd, A_BOLD);
  box(main_wnd, 0, 0);
  wattroff(main_wnd, A_BOLD);

  //line between infobox and game
  wmove(main_wnd, game_area.bot() + 3, 1);
  whline(main_wnd, '_', screen_area.width() - 2);

  //initial draw
  wrefresh(main_wnd);
  wrefresh(game_wnd);

  tick = 0;

  while(1){
    //clears game windows
    werase(game_wnd);
    //reads inputs & lowecases all letters
    in_char = wgetch(main_wnd);
    in_char = tolower(in_char);

    switch(in_char){
      case 'q':
        exit_requested = true;
        break;
      case KEY_UP:
      case 'w':
      case 'i':
        if(player.pos.y > game_area.top())
          player.pos.y -= 1;
        break;
      case KEY_DOWN:
      case 's':
      case 'k':
        if(player.pos.y < game_area.bot() + 1)
          player.pos.y += 1;
        break;
      case KEY_LEFT:
      case 'a':
      case 'j':
        if(player.pos.x > game_area.left() + 1)
          player.pos.x -= 1;
        break;
      case KEY_RIGHT:
      case 'd':
      case 'l':
        if(player.pos.x < game_area.right() - 2)
          player.pos.x += 1;
        break;
      default:
        break;
      }

    //stars & asteroids update
    if(tick % 7 == 0)
      stars.update();
    if(tick > 100 && tick % 20 == 0)
      asteroids.update();

    //player boundaries
    player.bounds = { { player.pos.x - 1, player.pos.y }, { 3, 1 } };

    for(size_t i = 0; i < asteroids.getData().size(); i++) {
        if(player.bounds.contains(asteroids.getData().at(i).getPos())) {
            asteroids.erase(i);
        }
    }


    for(auto s: stars.getData())
      mvwaddch(game_wnd, s.getPos().y, s.getPos().x, '.');

    for(auto a: asteroids.getData()){
      wattron(game_wnd, A_BOLD);
      mvwaddch(game_wnd, a.getPos().y, a.getPos().x, '*');
      wattroff(game_wnd, A_BOLD);
    }



    //player ship
    wattron(game_wnd, A_ALTCHARSET);
    mvwaddch(game_wnd, player.pos.y, player.pos.x - 1, ACS_LARROW);
    mvwaddch(game_wnd, player.pos.y, player.pos.x + 1, ACS_RARROW);

    if((tick % 10) / 3){
      wattron(game_wnd, COLOR_PAIR(tick % 2 ? 3 : 4));
      mvwaddch(game_wnd, player.pos.y + 1, player.pos.x, ACS_UARROW);
      wattroff(game_wnd, COLOR_PAIR(tick % 2 ? 3 : 4));
    }

    wattroff(game_wnd, A_ALTCHARSET);

    wrefresh(main_wnd);
    wrefresh(game_wnd);

    if(exit_requested || game_over) break;

    tick++;

    usleep(10000);
  };
}
/*
  uint_fast16_t maxx, maxy;    //initialize rect
  player.disp_char = 'o';
  player.pos = {10, 5};

  getmaxyx(wnd, maxy, maxx);    //get window boundaries
  rect game_area = { {0, 0} , {maxx, maxy} };   //init rect with 0 offset
  stars.setBounds(game_area);   //star bounds

  int in_char;
  bool exit_requested = false;

  while(1){
    in_char = wgetch(wnd);
    mvaddch(player.pos.y, player.pos.x, ' ');

    for(auto s : stars.getData()){
      mvaddch(s.getPos().y, s.getPos().x, ' ');
    }

    stars.update();

    switch(in_char){
      case 'q':
        exit_requested = true;
        break;
      case KEY_UP:
      case 'w':
        player.pos.y -= 1;
        break;
      case KEY_DOWN:
      case 's':
        player.pos.y += 1;
        break;
      case KEY_LEFT:
      case 'a':
        player.pos.x -= 1;
        break;
      case KEY_RIGHT:
      case 'd':
        player.pos.x += 1;
        break;
      default:
        break;
      }

    mvaddch(player.pos.y, player.pos.x, player.disp_char);

    for(auto s : stars.getData()){
      mvaddch(s.getPos().y, s.getPos().x, '*');
    }

    if(exit_requested) break;
    usleep(10000);
    refresh();
    }

  endwin();
*/
