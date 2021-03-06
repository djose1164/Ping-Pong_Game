#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistdio.h>

#define MAX_X 25
#define MAX_Y 25
#define BALL '*'

enum Crash
{
  CRASH_LEFT_LIMIT = 1,
  CRASH_RIGHT_LIMIT = 2,
  CRASH_PLAYER = 3,
  CRASH_AI = 4,
  CRASH_UPPER = 5,
  CRASH_BOTTOM = 6
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

void swap(char world[][MAX_Y])
{
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

  for (size_t i = ball->x; i < ball->x + 1; i++)
  {
    for (size_t j = ball->y; j < ball->y + 1; j++)
    {
      world[i][j] = BALL;
      world[i - 1][j - 1] = ' ';
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
  int temp = crash_detection(world, player, AI, ball);

  unsigned int goal_counter = 0; // For the counter that will be show.

  /* -*-*-*-*-*-*The logic part*-*-*-*-*-*- */

  // The start movement.
  if (temp == -1)
  {
    ball->x -= 1;
    ball->y -= 1;
  }

  // The crash was detected in the player's side.
  if ((temp == CRASH_UPPER) && (ball->y <= MIDDLE(MAX_Y)))
  {
    // From upper litmit; move to AI's side.
    ball->x += 1;
    ball->y += 1;
  }
  else
  {
    // From upper litmit; move to player's side.
    ball->x += 1;
    ball->y -= 1;
  }

  if ((temp == CRASH_BOTTOM) && (ball->y <= MIDDLE(MAX_Y)))
  {
    // From bottom litmit; move to AI's side.
    ball->x -= 1;
    ball->y += 1;
  }
  else
  {
    // From bottom litmit; move to player's side.
    ball->x -= 1;
    ball->y -= 1;
  }

  if (goal_counter == 10)
  {
    printf("The goal counter has reached the limit.\n");
    exit(EXIT_SUCCESS);
  }

  switch (ball->y)
  {
  case CRASH_LEFT_LIMIT: // Goal for AI.
    goal_counter += 1;
    break;

  case CRASH_RIGHT_LIMIT: // Goal for player.
    goal_counter += 1;
    break;
  }

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
  switch (ball->y)
  {
  case 0: // Crash detection with the player's side. Goal for AI.
    return CRASH_LEFT_LIMIT;
    break;

  case MAX_Y - 1: // Crash detection with the AI's side. Goal for player.
    return CRASH_RIGHT_LIMIT;
    break;
  }

  return -1;
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
    verification(world, player, AI, ball);
    draw(world, player, AI, ball);
    refresh(player, AI, ball, world);
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

      *(*(world + i) + j) = ' ';
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

  // Put the ball in the middle of the screen.
  world[ball->x][ball->y] = BALL;

  AI->height = player->height = 5;
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

  if (!life_cycle(world, &player, &AI, &ball))
    return EXIT_FAILURE;

  return EXIT_SUCCESS;
}