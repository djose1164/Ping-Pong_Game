/**
 * @file main.c
 * @author Jose S. Daniel (djose1164@gmail.com)
 * @brief Keep on mid: this proyect isn't finished yet and has many errors! I'll be thankful if you have 
 * any advice to me.
 * @version 0.1
 * @date 2021-03-06
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistdio.h>
#include <termios.h>
#include <sys/ioctl.h>

#define MAX_X 30
#define MAX_Y 30
#define BALL '*'
#define BACKGROUND ' '
#define _BSD_SOURCE

// Global variables.
bool first_time = true;
bool init_movement = true;
unsigned short move_down = 3;
short temp;

enum Crash
{
  CRASH_LEFT_LIMIT,
  CRASH_RIGHT_LIMIT,
  CRASH_PLAYER,
  CRASH_AI,
  CRASH_UPPER,
  CRASH_BOTTOM,
};

// Get the middle point.
#define MIDDLE(x) (int)(x / 2)

struct Player
{
  unsigned short width;
  unsigned short height;
  unsigned short x, y;
};
struct Ball
{
  unsigned short x, y;
};

static struct termios old, current;

/* Initialize new terminal i/o settings */
void initTermios(int echo)
{
  tcgetattr(0, &old);         /* grab old terminal i/o settings */
  current = old;              /* make new settings same as old settings */
  current.c_lflag &= ~ICANON; /* disable buffered i/o */
  if (echo)
  {
    current.c_lflag |= ECHO; /* set echo mode */
  }
  else
  {
    current.c_lflag &= ~ECHO; /* set no echo mode */
  }
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

void refresh(struct Player *player, struct Player *AI, struct Ball *ball, char world[][MAX_Y])
{
  for (size_t i = player->x; i < (player->height + player->x); i++)
  {
    for (size_t j = player->y; j < (player->width + player->y); j++)
    {
      *(*(world + i) + j) = '*';
    }
  }

  for (size_t i = AI->x; i < (AI->height + AI->x); i++)
  {
    for (size_t j = AI->y; j < (AI->width + AI->y); j++)
    {
      *(*(world + i) + j) = '*';
    }
  }

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
 * @return int 
 */
int verification(char world[][MAX_Y], struct Player *player, struct Player *AI, struct Ball *ball)
{
  int crash_detection();

  int _temp = crash_detection(world, player, AI, ball);

  if (_temp != -2 && _temp != -1)
    temp = crash_detection(world, player, AI, ball);

  unsigned int goal_counter = 0; // For the counter that will be show.

  /* -*-*-*-*-*-*The logic part*-*-*-*-*-*- */

  // The init movement.
  if (_temp == -1 && init_movement)
  {
    //ball->x -= 1;
    ball->y -= 1;
  }

  // The crash was detected in the player's side.
  if (temp == CRASH_UPPER)
  {
    // From upper litmit; move to AI's side.
    ball->x += 1;
    ball->y += 1;
  }
  /*
    else if ((temp == CRASH_UPPER) && (ball->y > MIDDLE(MAX_Y)))
    {
      // From upper litmit; move to player's side.
      ball->x += 1;
      ball->y -= 1;
    }
  */
  if (temp == CRASH_BOTTOM)
  {
    // From bottom litmit; move to AI's side.
    ball->x -= 1;
    ball->y -= 1;
  }
  /*
    else if ((temp == CRASH_BOTTOM) && (ball->y > MIDDLE(MAX_Y)))
    {
      // From bottom litmit; move to player's side.
      ball->x -= 1;
      ball->y -= 1;
    }
  */
  if (goal_counter == 10)
  {
    printf("The goal counter has reached the limit.\n");
    exit(EXIT_SUCCESS);
  }

  if (temp == CRASH_PLAYER)
  {
    ball->x--;
    ball->y++;
  }

  if (temp == CRASH_AI)
  {
    ball->x++;
    ball->y--;
  }
  /*
    else if (temp == CRASH_AI && ball->x > MIDDLE(AI->height))
    {
      ball->x--;
      ball->y++;
    }
  */
  /* AI movement. */
  if (move_down == 3)
    AI->x--;

  if (AI->x == 0)
    move_down = 1;

  else if (AI->x == (MAX_X - AI->height))
    move_down = 0;

  if (move_down == 1)
    AI->x++;
  else if (move_down == 0)
    AI->x--;

  return EXIT_SUCCESS;
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
  case 0:
    return CRASH_UPPER; // Crash detection with the word's upper limit. The ball have to rebound to
                        // the button limit.
    break;

  case MAX_X - 1: // Crash detection with the word's bottom limit. The ball have to rebound to
                  // the upper limit.
    return CRASH_BOTTOM;
    break;
  }

  // Detection for y abxis.
  if (ball->y == 0)
    return CRASH_LEFT_LIMIT; // Crash detection with the player's side. Goal for AI.

  else if (ball->y == (MAX_Y - 1)) // Crash detection with the AI's side. Goal for player.
    return CRASH_RIGHT_LIMIT;

  else if (ball->y == player->y + 1)
  {
    init_movement = false;
    return CRASH_PLAYER;
  }
  else if ((AI->y + 1) == ball->y)
  {
    init_movement = false;
    return CRASH_AI;
  }
  else
    return -1;

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
      if (player->x != 0)
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
    system("clear");

    player_goto(player);

    verification(world, player, AI, ball);

    refresh(player, AI, ball, world);

    draw(world, player, AI, ball);

    delete (player, AI, world, ball);

    sleep(1);
  }
  return EXIT_SUCCESS;
}

/**
 * @brief The @MAX_X and @MAX_Y must be set with enough space to see the item
 * in the middle of the screen.
 * 
 * I need to see for solutions to the problem.
 * 
 * @param player The player's struct.
 * @param world The 2D array of the world.
 */
void print_players(struct Player player[], char world[][MAX_Y])
{
  for (size_t k = 0; k < 2; k++)
  {
    for (size_t i = player[k].x; i < (player[k].height + player[k].x); i++)
    {
      for (size_t j = player[k].y; j < (player[k].width + player[k].y); j++)
      {
        *(*(world + i) + j) = '*';
      }
    }
  }
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

  AI->height = (int)(MAX_Y - (MAX_Y * 0.2));
  player->height = 5;
  AI->width = player->width = 2;
  player->x = MIDDLE(MAX_Y) - MIDDLE(player->height);
  player->y = 1;

  AI->x = MIDDLE(MAX_Y) - MIDDLE(AI->height);
  AI->y = MAX_X - 3;

  struct Player temp[] = {*player, *AI};

  print_players(temp, world);

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
  struct Ball ball = {MIDDLE(MAX_X), MIDDLE(MAX_Y)};

  // Put the ball in the middle of the world.
  world[MIDDLE(MAX_X)][MIDDLE(MAX_Y)] = BALL;

  if (!life_cycle(world, &player, &AI, &ball))
    return EXIT_FAILURE;

  return EXIT_SUCCESS;
}