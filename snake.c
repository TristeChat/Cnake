#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define DIMY 12
#define DIMX DIMY*2
#define MAX_LEN (DIMX-1)*(DIMY-1)
#define GAME_OVER 1

int game(WINDOW* game_window);
void mouse(WINDOW* game_window, int* rx, int* ry);
int mov_snake(WINDOW* game_window, int snake[MAX_LEN][2], int len, int dir);

int main(void)
{
  //if (has_colors() == FALSE) {
  //  printf("Terminal doesn't support color.\n");
  //  return 1;
  //}

  WINDOW* game_window; 

  initscr();
  raw();
  keypad(stdscr, TRUE);
  noecho();
  curs_set(FALSE);

  game_window = newwin(DIMY, DIMX, 0, 0);
  box(game_window, 0, 0);
  wrefresh(game_window);
  wborder(game_window, '|', '|', '-', '-', '+', '+', '+', '+');

  refresh();
  int exit_stat = 0;
  do {
    exit_stat = game(game_window);
  } while (exit_stat); 

  wborder(game_window, ' ', ' ', ' ',' ',' ',' ',' ',' ');
  wrefresh(game_window);
  delwin(game_window);
  
  endwin();
  return 0;
}

int game(WINDOW* game_window)
{
  int ch;
  int startx = DIMX/2;
  int starty = DIMY/2;
  int snake[MAX_LEN][2] = {{startx, starty}};

  nodelay(game_window, TRUE);

  mvwaddch(game_window, starty, startx, '%');
  wrefresh(game_window);

  mvprintw(DIMY + 1, 0, "Press any key to play.");
  refresh();
  getch();
  move(DIMY + 1, 0);
  clrtoeol();
  refresh();
  mvprintw(DIMY + 1, 0, "Press q(Q) to exit.");
  refresh();

  int rx;
  int ry;
  mouse(game_window, &rx, &ry);
  int dir = KEY_UP;
  int len = 1;
  int state = 0;

  int f = 0;

  while (1) {
    ch = wgetch(game_window);

    switch (ch) {
      case 'q':
      case 'Q':
        goto exit_loop;
        break;
      case 'm':
        mouse(game_window, &rx, &ry);
        break;
      case KEY_UP:
      case KEY_DOWN:
      case KEY_LEFT:
      case KEY_RIGHT:
        dir = ch;
      case ERR:
      default:
        state = mov_snake(game_window, snake, len, dir);
        if (state == GAME_OVER) {
          goto exit_loop;
        }
    }

    mvprintw(f++, 200, "Direction: %d, Snake: %p", dir, snake);
    refresh();

    napms(1000);
  }
  exit_loop: ;
  
  move(DIMY + 1, 0);
  clrtoeol();
  refresh();
  printw("Game over! Press any key to exit.");
  getch();
  return 0;
}

int mov_snake(WINDOW* game_window, int snake[MAX_LEN][2], int len, int dir) 
{
  // the array stuff is all wrong
  // code the part that grows the snake
  // button part too
  // -_-
  int bufferx;
  int buffery;
  char body;

  for (int i = 0; i < len ; i++) {
    bufferx = snake[i][0];
    buffery = snake[i][1];
    body = (i == 0) ? '%' : '*';

    mvwaddch(game_window, buffery, bufferx, ' ');
    switch (dir) {
      case KEY_UP:
        --buffery;
        if (buffery < 0) {
          return GAME_OVER;
        }
        break;
      case KEY_DOWN:
        ++buffery;
        if(buffery > DIMY) {
          return GAME_OVER;
        }
        break;
      case KEY_RIGHT:
        ++bufferx;
        if (bufferx > DIMX) {
          return GAME_OVER;
        }
        break;
      default:
        --bufferx;
        if (bufferx < 0) {
          return GAME_OVER;
        }
    }
    snake[i][0] = bufferx;
    snake[i][1] = buffery;
    mvwaddch(game_window, buffery, bufferx, body);
  }

  wrefresh(game_window);
  return 0;
}

void mouse(WINDOW* game_window, int* rx, int* ry)
{
  mvwaddch(game_window, *ry, *rx, ' ');
  srand(time(NULL));
  *rx = rand() % (DIMX-2) + 1;
  *ry = rand() % (DIMY-2) + 1;
  mvwaddch(game_window, *ry, *rx, '@');
  wrefresh(game_window);
}
