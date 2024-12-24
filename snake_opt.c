#include <ncurses.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

// Screen constants
#define DIMY 12
#define DIMX 12*2
#define MAX_LEN (DIMX-1) * (DIMY-1)

// Snake states
#define ALIVE 0
#define DEAD 1 
#define EATEN 2 

// Game states
#define GAME_OVER 0 
#define REPLAY 1

typedef struct Snake {
  uint8_t snake_state;
  uint16_t len;
  uint16_t snake_arr[MAX_LEN][2];
  int direction;
} Snake;

uint8_t game(WINDOW* game_window);
void mouse(WINDOW* game_window, Snake* snake, uint16_t* rx, uint16_t* ry);
uint8_t mov_snake(WINDOW* game_window, Snake* snake) ;
uint8_t mov_head(WINDOW* game_window, uint16_t* bufferx, uint16_t* buffery, int dir);
void grow_snake(WINDOW* game_window, Snake* snake);

/**
 * Main initializes the ncurses window 
*/
int main(int argc, char *argv[])
{
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
  uint8_t exit_state = GAME_OVER;
  do {
    exit_state = game(game_window);
  } while (exit_state); 

  wborder(game_window, ' ', ' ', ' ',' ',' ',' ',' ',' ');
  wrefresh(game_window);
  delwin(game_window);
  
  endwin();
  return EXIT_SUCCESS;
}

/**
 * Runs the actual game
 *
 * @param game_window ncurses window buffer to write to  
 *
 * @return 0
*/
uint8_t game(WINDOW* game_window) 
{
  keypad(game_window, true);
  uint8_t end_state = GAME_OVER;
  int ch;
  uint16_t startx = DIMX/2;
  uint16_t starty = DIMY/2;
  Snake snake = {
    //UP, // direction
    ALIVE, // snake_state
    1, //length
    {{startx, starty}}, //snake_arr
    KEY_UP // direction
  };

  nodelay(game_window, TRUE);

  mvwaddch(game_window, starty, startx, '%');
  wrefresh(game_window);
  
  move(DIMY + 1, 0);
  clrtoeol();
  printw("Press any key to play.");
  refresh();
  getch();
  move(DIMY + 1, 0);
  clrtoeol();
  refresh();
  printw("Press q(Q) to exit.");
  refresh();

  uint16_t rx;
  uint16_t ry;
  mouse(game_window, &snake, &rx, &ry);

  while (1) {
    ch = wgetch(game_window);

    switch (ch) {
      case 'q':
      case 'Q':
        goto exit_loop;
        break;
      case 'm':
        mouse(game_window, &snake, &rx, &ry);
        break;
      case KEY_UP:
      case KEY_DOWN:
      case KEY_LEFT:
      case KEY_RIGHT:
        snake.direction = ch;
      case ERR: 
      default:
        snake.snake_state = mov_snake(game_window, &snake);
        switch (snake.snake_state) {
          case ALIVE: break;
          case DEAD: goto exit_loop; break;
          case EATEN: 
            mouse(game_window, &snake, &rx, &ry);
            grow_snake(game_window, &snake);
        }
    } 

    napms(300);
  }
  exit_loop: ;
  
  move(DIMY + 1, 0);
  clrtoeol();
  printw("Game over! Press r(R) to replay or press any other key to exit.");
  refresh();

  nodelay(game_window, FALSE);
  ch = wgetch(game_window);
  switch (ch) {
    case 'r':
    case 'R':
      end_state = REPLAY;
  }
  return end_state;
}

/**
 * Updates the position of the mouse
 *    
 * @param game_window   ncurses window buffer to write to  
 * @param snake         snake object with related information
 * @param rx ry         pointers to the current coordinates of the mouse  
*/
void mouse(WINDOW* game_window, Snake* snake, uint16_t* rx, uint16_t* ry)
{
  mvwaddch(game_window, *ry, *rx, ' ');
  srand(time(NULL));
  *rx = rand() % (DIMX-2) + 1;
  *ry = rand() % (DIMY-2) + 1;
  for (uint16_t i = 0; i < snake->len; i++) {
    if (snake->snake_arr[i][0] == *rx || snake->snake_arr[i][1] == *ry) {
      mouse(game_window, snake, rx, ry);
      break;
    } 
  }
  mvwaddch(game_window, *ry, *rx, '@');
  wrefresh(game_window);
}

/**
 * Moves the snake and updates it's array
 *
 * @param game_window   ncurses window buffer to write to  
 * @param snake         snake object with related information
 * @return              state of the game
*/
uint8_t mov_snake(WINDOW* game_window, Snake* snake) 
{
  // I think moving works but EATEN isn't being used idk why
  // -_-
  uint16_t bufferx;
  uint16_t buffery;
  uint16_t prevx;
  uint16_t prevy;

  char body = '%';
  char next;
  uint8_t ret_val = ALIVE;

  for (uint16_t i = 0; i <= snake->len-1 ; i++) {
    bufferx = snake->snake_arr[i][0];
    buffery = snake->snake_arr[i][1];
    mvwaddch(game_window, buffery, bufferx, ' ');

    switch (i) {
      case 0:
        prevx = bufferx;
        prevy = buffery;
        ret_val = mov_head(game_window, &bufferx, &buffery, snake->direction);
        if (ret_val == DEAD) {
          return ret_val;
        }
        break;
      default:
        if (ret_val == EATEN && i == snake->len) {
          mvwaddch(game_window, buffery, bufferx, body); 
          snake->snake_arr[++snake->len][0] = bufferx;
          snake->snake_arr[snake->len][1] = buffery;
        }

        bufferx = prevx;
        buffery = prevy;
        prevx = snake->snake_arr[i][0];
        prevy = snake->snake_arr[i][1];
    }

    snake->snake_arr[i][0] = bufferx;
    snake->snake_arr[i][1] = buffery; 
    mvwscanw(game_window, buffery, bufferx, "%c", &next);
    if (next == body) {
      return DEAD;
    }
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
uint8_t mov_head(WINDOW* game_window, uint16_t* bufferx, uint16_t* buffery, int dir)
{
  char next;
  switch (dir) {
      case KEY_UP:
        --*buffery;
        if (*buffery <= 0) {
          return DEAD;
        }
        break;
      case KEY_DOWN:
        ++*buffery;
        if(*buffery >= DIMY-1) {
          return DEAD;
        }
        break;
      case KEY_RIGHT:
        ++*bufferx;
        if (*bufferx >= DIMX-1) {
          return DEAD;
        }
        break;
      default:
        --*bufferx;
        if (*bufferx <= 0) {
          return DEAD;
        }
    }
  // wtf is happening 
  mvwscanw(game_window, *buffery, *bufferx, "%c", &next);
  if (next == '@') {
    return EATEN;
  }

  return ALIVE;
}

/**
 * Grows the snake and updates the position array
 *
 * @param game_window   ncurses window buffer to write to  
 * @param snake         snake object storing realted information
*/
void grow_snake(WINDOW* game_window, Snake* snake)
{
  ++snake->len;
  uint16_t prevx = snake->snake_arr[snake->len-1][0];
  uint16_t prevy = snake->snake_arr[snake->len-1][1];

  snake->snake_arr[snake->len][0] = prevx;
  snake->snake_arr[snake->len][1] = prevy;
  mvwaddch(game_window, prevy, prevx, '*');
  wrefresh(game_window);
  return;
}

