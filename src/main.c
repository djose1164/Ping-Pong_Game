/************************************************************/
/*           Programacion para mecatronicos                 */
/*  Nombre: Jose S. Daniel Victoriano Bello                 */
/*  Matricula: 2020-10646                                   */
/*  Seccion: Miercoles                                      */
/*  Practica: Ping Pong Game                                */
/*  Fecha: 6/3/2021                                         */
/************************************************************/

/**
 * @file main.c
 * @author Jose S. Daniel (djose1164@gmail.com)
 * @brief Keep on mid: this proyect isn't finished yet and has many errors! I'll be thankful if you have 
 * any advice to me.
 * @version 0.2
 * @date 2021-03-06
 * 
 * @copyright Copyright (c) 2021
 * 
 */

/**
 * TODO: Make all struct pointer const.
 * TODO: Add OOP style.
 * TODO: All On(n^2) to O(n).
 * TODO: Reduce global scape.
 */

// Necessary libraries. DON'T DELETE IT!
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#ifdef __linux__
#include <unistdio.h>
#include <sys/ioctl.h>
#include <termios.h>
#elif _WIN32
#include <windows.h>
#include <conio.h>
#endif // __linux__

// Size for matrix.
#define MAX_X 25
#define MAX_Y 25

// Forms to display.
#define BALL '*'
#define BACKGROUND ' '
#define RACKET 'x'

// For sleep function.
#define _BSD_SOURCE

// Global variables.
bool first_time = true;
bool init_movement = true;
bool keep_direction, keep_direction_AI = false;
unsigned short move_down = 3;
short temp = -1;
unsigned short which_one;

enum Crash
{
  CRASH_LEFT_LIMIT,
  CRASH_RIGHT_LIMIT,
  CRASH_PLAYER,
  CRASH_AI,
  CRASH_UPPER,
  CRASH_BOTTOM
};

// Get the middle point.
#define MIDDLE(x) (int)(x / 2)

struct Player
{
  unsigned short width;
  unsigned short height;
  unsigned short x, y;
  unsigned short goal_counter;
};
struct Ball
{
  unsigned short x, y;
};

#ifdef __linux__
static struct termios old, current;

/* Initialize new terminal i/o settings */
void initTermios(int echo)
{
  tcgetattr(0, &old);         /* grab old terminal i/o settings */
  current = old;              /* make new settings same as old settings */
  current.c_lflag &= ~ICANON; /* disable buffered i/o */
  if (echo)
    current.c_lflag |= ECHO; /* set echo mode */

  else
    current.c_lflag &= ~ECHO; /* set no echo mode */

  tcsetattr(0, TCSANOW, &current); /* use these new terminal i/o settings now */
}

/* Restore old terminal i/o settings */
void resetTermios(void)
{
  tcsetattr(0, TCSANOW, &old);
}

/* Read 1 character - echo defines echo mode */
char getch_(int echo)
{
  char ch;
  initTermios(echo);
  ch = getchar();
  resetTermios();
  return ch;
}

/* Read 1 character without echo */
char getch(void)
{
  return getch_(0);
}

/* Read 1 character with echo */
char getche(void)
{
  return getch_(1);
}

bool kbhit()
{
  struct termios term;
  tcgetattr(0, &term);

  struct termios term2 = term;
  term2.c_lflag &= ~ICANON;
  tcsetattr(0, TCSANOW, &term2);

  int byteswaiting;
  ioctl(0, FIONREAD, &byteswaiting);

  tcsetattr(0, TCSANOW, &term);

  return byteswaiting > 0;
}
#endif // __linux__

/**
 * @brief Restart the game whether the player or AI has obtained a point.
 * 
 * @param player A const pointer to the player.
 * @param ball A const pointer to the ball.
 * @param world A const pointer to the matrix(world).
 */
void restart(struct Ball *const ball)
{
  // Set the necessary variables to its default value. Necessary for the init movement.
  temp = -1;
  first_time = true;
  init_movement = true;
  keep_direction = keep_direction_AI = false;

  // Set the ball in the middle.
  ball->x = MIDDLE(MAX_X);
  ball->y = MIDDLE(MAX_Y);

// Stop the game for 2 seconds.
#if defined(__linux__)

  sleep(2);

#else

  Sleep(2);

#endif // __linux__
}

/**
 * @brief The function that makes possible the movement inner the world(matrix).
 * *After the ball, the player or AI has passed for [x, y] position this function will take that position,
 * *and fill in with background.
 * 
 * TODO: reactor to improve speed to O(n).
 * 
 * @param player A pointer to the player's struct.
 * @param AI  A pointer to the AI's struct.
 * @param world The matrix as world.
 * @param ball A pointer to the ball's struct.
 */
void delete (struct Player *player, struct Player *AI, char world[][MAX_Y], struct Ball *ball)
{
  for (size_t i = player->x; i < (player->height + player->x); i++)
  {
    for (size_t j = player->y; j < (player->width + player->y); j++)
    {
      *(*(world + i) + j) = BACKGROUND;
    }
  }

  for (size_t i = AI->x; i <= (AI->height + AI->x); i++)
  {
    for (size_t j = AI->y; j < (AI->width + AI->y); j++)
    {
      *(*(world + i) + j) = BACKGROUND;
    }
  }

  for (size_t i = ball->x; i < ball->x + 1; i++)
  {
    for (size_t j = ball->y; j < ball->y + 1; j++)
    {
      world[i][j] = BACKGROUND;
    }
  }

  if (!first_time)
    world[MIDDLE(MAX_X)][MIDDLE(MAX_Y)] = BACKGROUND;
}
/**
 * @brief Set the new values after verification.
 * TODO: Reactor to improve speed to O(n).
 * 
 * @param player A pointer to the player's struct.
 * @param AI A pointer to the AI's struct.
 * @param ball A pointer to the ball's struct.
 * @param world Matrix as world. 
 */
void refresh(struct Player *player, struct Player *AI, struct Ball *ball, char world[][MAX_Y])
{
  // Move the player to his new position.
  for (size_t i = player->x; i < (player->height + player->x); i++)
  {
    for (size_t j = player->y; j < (player->width + player->y); j++)
    {
      *(*(world + i) + j) = RACKET;
    }
  }

  // Move the AI to its new position.
  for (size_t i = AI->x; i < (AI->height + AI->x); i++)
  {
    for (size_t j = AI->y; j < (AI->width + AI->y); j++)
    {
      *(*(world + i) + j) = RACKET;
    }
  }

  // Move the ball to its new position.
  for (size_t i = ball->x; i < (ball->x + 1); i++)
  {
    for (size_t j = ball->y; j < (ball->y + 1); j++)
    {
      *(*(world + i) + j) = BALL;
    }
  }
}

int draw(char world[][MAX_Y], struct Player *player, struct Player *AI, struct Ball *ball)
{
  for (size_t i = 0; i < MAX_X; i++)
  {
    for (size_t j = 0; j < MAX_Y; j++)
    {
      printf("%c", *(*(world + i) + j));
    }
    putchar('\n');
  }
  first_time = false;

  return EXIT_SUCCESS;
}

/**
 * @brief Here's allocate the all logic of the game. The movement and all is controlled here.
 * 
 * @param world Depends the trayectory, must give the values abxis.
 * @param player The width, height and x, y abxis are inside this struct.
 * @param AI Like player.
 * @param ball The ball.
 */
void verification(char world[][MAX_Y], struct Player *player, struct Player *AI, struct Ball *ball)
{
  int crash_detection();

  int _temp = crash_detection(world, player, AI, ball);

  if (_temp != -2)
    temp = crash_detection(world, player, AI, ball);

  // *-*-*-*-*-*-*-*-Verification for counter.*-*-*-*-*-*-*-*-
  switch (temp)
  {
  case CRASH_LEFT_LIMIT:
    AI->goal_counter++;
    world[0][MIDDLE(MAX_Y) + 1] = '0' + AI->goal_counter;
    //printf("Point to AI");
    restart(ball);

  case CRASH_RIGHT_LIMIT:
    player->goal_counter++;
    world[0][MIDDLE(MAX_Y) - 1] = '0' + player->goal_counter;
    //printf("Point to player");
    restart(ball);
  }

  if (player->goal_counter == 2 || AI->goal_counter == 2)
  {
    printf("The goal counter has reached the limit.\n");
    if (AI->goal_counter == 2)
      printf("Oh men! The AI has defeated you.\n");
    else
      printf("It was very easy, wait untill the next update.\n");
    exit(EXIT_SUCCESS);
  }

  // The init movement.
  static unsigned short random = 10;
  srand(time(NULL));

  if (_temp == -2 && ball->x <= MAX_X /* && ball->y != AI->y) */ && init_movement)
  {
    if (random == 10)
      //random = rand() % 6;
      random = 1;

    switch (random)
    {
    case 0:
      ball->y--;
      break;

    case 1:
      ball->y++;
      break;

    case 2:
      if ((ball->x % ball->y) == 0)
        ball->y++;
      ball->x++;
      break;

    case 3:
      ball->y++;
      ball->x -= 1;
      break;
    case 4:
      ball->y -= 1;
      ball->x--;
      break;

    case 5:
      if ((ball->x % ball->y) == 0)
        ball->y--;
      ball->x++;
      break;

    default:
      break;
    }
  }

  // The crash was detected in the player's side.
  if (temp == CRASH_UPPER)
  {
    // From upper litmit; move to AI's side.
    ball->x++;
    ball->y++;
  }
  /*
    else if ((temp == CRASH_UPPER) && (ball->y > MIDDLE(MAX_Y)))
    {
      // From upper litmit; move to player's side.
      ball->x += 1;
      ball->y -= 1;
    }
  */
  if ((temp == CRASH_BOTTOM ) && (which_one == CRASH_AI))
  {
    // From bottom litmit; move to AI's side.
    ball->x--;
    ball->y--;
  }
  else if ((temp == CRASH_BOTTOM ) && (which_one == CRASH_PLAYER))
  {
    ball->x--;
    ball->y++;
  }
  
  /*
    else if ((temp == CRASH_BOTTOM) && (ball->y > MIDDLE(MAX_Y)))
    {
      // From bottom litmit; move to player's side.
      ball->x -= 1;
      ball->y -= 1;
    }
  */

  if (temp == CRASH_PLAYER)
  {
    if (ball->x >= MIDDLE(MAX_X) && !keep_direction)
    {
      ball->x--;
      if ((ball->x % 2) == 0)
        ball->y++;
    }
    else
    {
      keep_direction = true;

      ball->x++;
      if ((ball->x % 2) == 0)
        ball->y++;
    }
  }

  if (temp == CRASH_AI)
  {
    if (ball->x >= MIDDLE(MAX_X) && !keep_direction_AI)
    {
      ball->x--;
      if ((ball->x % 2) == 0)
        ball->y--;
    }
    else
    {
      keep_direction_AI = true;
      ball->x++;
      if ((ball->x % 2) == 0)
        ball->y--;
    }
  }

  /* AI movement. */
  if (move_down == 3)
    AI->x--;

  if (AI->x == 1)
    move_down = 1;

  else if (AI->x == (MAX_X - AI->height))
    move_down = 0;

  if (move_down == 1)
    AI->x++;
  else if (move_down == 0)
    AI->x--;
}

/**
   * @brief <b>The crash detection.</b>
   * If the ball crash with the abxis y = 0 and y = MAX_Y - 1. There'll be a goal
   * for the oponent. To know who is the oponent, if the crash was in y = 0 so it was the AI;
   * if it was in y = MAX_Y so it was the player.
   * 
   */
int crash_detection(char world[][MAX_Y], struct Player *player, struct Player *AI, struct Ball *ball)
{

  // Detection for x abxis.
  switch (ball->x)
  {
  case 1:
    return CRASH_UPPER;

  case MAX_X - 1:
    return CRASH_BOTTOM;
  }

  // Detection for y abxis.
  switch (ball->y)
  {
  case 0:
    return CRASH_LEFT_LIMIT;

  case MAX_Y - 1:
    return CRASH_RIGHT_LIMIT;
  }

  if (ball->y == (player->y + 1) && (ball->x >= player->x && ball->x <= (player->x + player->height))) // Crash with player's racker.
  {
    init_movement = false;
    which_one = CRASH_PLAYER;
    return CRASH_PLAYER;
  }
  else if ((AI->y) == ball->y && (ball->x >= AI->x && ball->x <= (AI->x + AI->height))) // Crash with AI's racket.
  {
    init_movement = false;
    which_one = CRASH_AI;
    return CRASH_AI;
  }
  /*
  if (!init_movement)
  {
    fprintf(stderr, "Bug found on crash dectetion. Actual value of temp: %d", temp);
    exit(EXIT_FAILURE);
  }
*/
  return -2;
}

void player_goto(struct Player *player)
{
  char key_pressed;
  if (kbhit())
  {
    key_pressed = getch();

    if ((key_pressed == 'W') || (key_pressed == 'w'))
    {
      // Player crash with upper litmit; stop.
      if (player->x != 1)
        player->x--; // Move to up.
    }
    else if ((key_pressed == 'S') || (key_pressed == 's'))
    {
      if (player->x != (MAX_X - player->height))
        player->x++; // Move to down.
    }
    else if ((key_pressed == 'q') || (key_pressed == 'Q'))
      exit(0); // Close the game.
  }
}

/**
 * @brief An inifite loop for the game; where the game is held.
 * 
 * @param player A pointer to the player's struct.
 * @param AI A pointer to the AI's struct.
 * @param world The bidimensional world.
 * @param ball A pointer to the ball's struct.
 * @return int 
 */
int gameloop(struct Player *player, struct Player *AI, char world[][MAX_Y], struct Ball *ball)
{
  for (;;)
  {
#ifdef __linux__
    system("clear");
#elif _WIN32
    sytem("cls");
#endif // __linux__

    player_goto(player);

    verification(world, player, AI, ball);

    refresh(player, AI, ball, world);

    draw(world, player, AI, ball);

    delete (player, AI, world, ball);

#ifdef __linux__
    //sleep(1);
    usleep(90000);
#elif _WIN32
    Sleep(1);
#endif // __linux__
  }

  return EXIT_SUCCESS;
}

/**
 * @brief Fill in the bidemensional word with blank spaces.
 * 
 * @param world The array pointer.
 * @return int 0 for success.
 */
int fill_world(char world[][MAX_Y])
{
  for (size_t i = 0; i <= MAX_X; i++)
  {
    for (size_t j = 0; j <= MAX_Y; j++)
    {
      *(*(world + i) + j) = BACKGROUND;
    }
  }
  return EXIT_SUCCESS;
}

/**
 * @brief The life cycle of the game. Here evaluates when the game has to end and the callings to
 * the other functions.
 * 
 * @param world The array where are contained th bidimensional world.
 * @return int 0 for success.
 */
int life_cycle(char world[][MAX_Y], struct Player *player, struct Player *AI, struct Ball *ball)
{
  fill_world(world);

  // Show counter on screen.
  world[0][MIDDLE(MAX_Y) - 1] = '0';
  world[0][MIDDLE(MAX_Y)] = ':';
  world[0][MIDDLE(MAX_Y) + 1] = '0';

  // Set init values.
  AI->height = (int)(MAX_Y - (MAX_Y * 0.6));
  player->height = 5;
  AI->width = player->width = 2;
  player->x = MIDDLE(MAX_Y) - MIDDLE(player->height);
  player->y = 1;

  AI->x = MIDDLE(MAX_Y) - MIDDLE(AI->height);
  AI->y = MAX_X - 3;

  AI->goal_counter = 0;
  player->goal_counter = 0;

  if (!gameloop(player, AI, world, ball))
  {
    fprintf(stderr, "There was an error on gameloop.\n");
    exit(EXIT_FAILURE);
  }

  return EXIT_SUCCESS;
}

int main(int argc, char const *argv[])
{
  char world[MAX_X][MAX_Y];

  struct Player player;
  struct Player AI;
  struct Ball ball = {MIDDLE(MAX_X),
                      MIDDLE(MAX_Y)};

  // Put the ball in the middle of the world.
  world[MIDDLE(MAX_X)][MIDDLE(MAX_Y)] = BALL;

  if (!life_cycle(world, &player, &AI, &ball))
    {
      fprintf(stderr, "An annoying bug has been found on life cycle.\nGobal variables...\nfirst_time: %d\ninit_movement: %d\nkeep_direction: %d\nkeep_direction_AI\nmov_down: %d\ntemp: %d\n",
       first_time, init_movement, keep_direction, keep_direction_AI, move_down);
    return EXIT_FAILURE;
    }
  return EXIT_SUCCESS;
}