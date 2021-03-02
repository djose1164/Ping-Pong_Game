#include <stdio.h>
#include <stdlib.h>

#define MAX_X 60
#define MAX_Y 30

int print_world(char world[][MAX_Y])
{
  for (size_t i = 0; i <= MAX_X; i++)
  {
    for (size_t j = 0; j <= MAX_Y; j++)
    {
      printf("%c", world[i][j]);
    }

    putchar('\n');
  }

  return EXIT_SUCCESS;
}

int fill_world(char world[][MAX_Y])
{
  for (size_t i = 0; i <= MAX_X; i++)
  {

    for (size_t j = 0; j <= MAX_Y; j++)
    {

      *(*(world + i) + j) = '0';
    }
  }

  return EXIT_SUCCESS;
}

int life_cicle(char world[][MAX_Y])
{
  fill_world(world);
  
  print_world(world);

  return EXIT_SUCCESS;
}

int main(int argc, char const *argv[])
{
  char world[MAX_X][MAX_Y];

  life_cicle(world);

  return EXIT_SUCCESS;
}