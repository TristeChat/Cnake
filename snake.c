#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define DIMY 12
#define DIMX DIMY*2
#define MAX_LEN (DIMX-1)*(DIMY-1)

/**
 * States that the game can be in 
*/
enum State {
  GOOD,
  GAME_OVER,
  EATEN
};

/**
 * Functions
*/
int game(WINDOW* game_window);
void mouse(WINDOW* game_window, int* rx, int* ry);
enum State mov_snake(WINDOW* game_window, int snake[MAX_LEN][2], int len, int dir);
enum State mov_head(WINDOW* game_window, int* bufferx, int* buffery, int dir);
void grow_snake(WINDOW* game_window, int snake[MAX_LEN][2], int* len);

/**
 * Main initializes the ncurses window 
*/
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

/**
 * Runs the actual game
 *
 * @param game_window ncurses window buffer to write to  
 *
 * @return 0
*/

int game(WINDOW* game_window)
{
  keypad(game_window, TRUE);

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
  enum State state = GOOD;

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
        else if (state == EATEN) {
          mouse(game_window, &rx, &ry);
          grow_snake(game_window, snake, &len);
        }
    } 

    napms(300);
  }
  exit_loop: ;
  
  move(DIMY + 1, 0);
  clrtoeol();
  refresh();
  printw("Game over! Press any key to exit.");
  getch();
  return 0;
}

/**
 * Moves the snake and updates it's array
 *
 * @param game_window   ncurses window buffer to write to  
 * @param snake         positions of the snake's body
 * @param len           length of the snake
 * @param dir           direction it's moving in
 *
 * @return              state of the game
*/
enum State mov_snake(WINDOW* game_window, int snake[MAX_LEN][2], int len, int dir) 
{
  // I think moving works but EATEN isn't being used idk why
  // -_-
  int bufferx;
  int buffery;
  int prevx;
  int prevy;

  char body = '%';
  char next;
  enum State ret_val = GOOD;

  for (int i = 0; i <= len-1 ; i++) {
    bufferx = snake[i][0];
    buffery = snake[i][1];
    mvwaddch(game_window, buffery, bufferx, ' ');

    switch (i) {
      case 0:
        prevx = bufferx;
        prevy = buffery;
        ret_val = mov_head(game_window, &bufferx, &buffery, dir);
        if (ret_val == GAME_OVER) {
          return ret_val;
        }
        break;
      default:
        if (ret_val == EATEN && i == len) {
          mvwaddch(game_window, buffery, bufferx, body); 
          snake[++len][0] = bufferx;
          snake[len][1] = buffery;
        }

        bufferx = prevx;
        buffery = prevy;
        prevx = snake[i][0];
        prevy = snake[i][1];
    }

    snake[i][0] = bufferx;
    snake[i][1] = buffery; 
    mvwaddch(game_window, buffery, bufferx, body);
  }

  wrefresh(game_window);
  return ret_val;
}

/**
 * Computes where to move the snake's head 
 *
 * @param game_window       ncurses window buffer to write to  
 * @param bufferx buffery   pointers to the x and y coordinates of the head
 * @param dir               direction in which the head is moving
 *
 * @return snake's state
*/
enum State mov_head(WINDOW* game_window, int* bufferx, int* buffery, int dir)
{
  char next;
  switch (dir) {
      case KEY_UP:
        --*buffery;
        if (*buffery <= 0) {
          return GAME_OVER;
        }
        break;
      case KEY_DOWN:
        ++*buffery;
        if(*buffery >= DIMY-1) {
          return GAME_OVER;
        }
        break;
      case KEY_RIGHT:
        ++*bufferx;
        if (*bufferx >= DIMX-1) {
          return GAME_OVER;
        }
        break;
      default:
        --*bufferx;
        if (*bufferx <= 0) {
          return GAME_OVER;
        }
    }
  // wtf is happening 
  mvwscanw(game_window, *buffery, *bufferx, "%c", &next);
  if (next == '@') {
    return EATEN;
  }

  return GOOD;
}

/**
 * Grows the snake and updates the position array
 *
 * @param game_window   ncurses window buffer to write to  
 * @param snake array   of the snake's body's coordinates
 * @param len           pointer to the length of the snake 
*/
void grow_snake(WINDOW* game_window, int snake[MAX_LEN][2], int* len)
{
  ++*len;
  int prevx = snake[*len-1][0];
  int prevy = snake[*len-1][1];

  snake[*len][0] = prevx;
  snake[*len][1] = prevy;
  mvwaddch(game_window, prevy, prevx, '*');
  wrefresh(game_window);
  return;
}

/**
 * Updates the position of the mouse
 *    
 * @param game_window   ncurses window buffer to write to  
 * @param rx ry         pointers to the current coordinates of the mouse  
*/
void mouse(WINDOW* game_window, int* rx, int* ry)
{
  mvwaddch(game_window, *ry, *rx, ' ');
  srand(time(NULL));
  *rx = rand() % (DIMX-2) + 1;
  *ry = rand() % (DIMY-2) + 1;
  mvwaddch(game_window, *ry, *rx, '@');
  wrefresh(game_window);
}
