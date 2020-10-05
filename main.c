#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define GRID_SIZE 8

char *grid[GRID_SIZE][GRID_SIZE];
bool Player1Turn = true;
bool PlayerColour;

void PrintGrid(char *grid[GRID_SIZE][GRID_SIZE])
{
  for (int i = 0; i < GRID_SIZE; ++i)
  {
    for (int j = 0; j < GRID_SIZE; ++j)
    {
      char *DisplayString;
      if (grid[i][j])
        DisplayString = grid[i][j];
      else
        DisplayString = " ";

      if (j == GRID_SIZE - 1)
        printf("[%s]\n", DisplayString);
      else
        printf("[%s] ", DisplayString);
    }
  }
}

bool CheckLine(int i, int j)
{
  if (!grid[i][j]) // If there's an empty square in this line
    return false;
  return true;
}

bool ScanLine(int Position[], char *Direction)
{
  int i = Position[0];
  int j = Position[1];
  char *UpLeft = "upleft";
  char *UpRight = "upright";
  char *DownLeft = "downleft";
  char *DownRight = "downright";
  char *Up = "up";
  char *Down = "down";
  char *Left = "left";
  char *Right = "right";

  if (Direction == Left)
  {
    while (j >= 0)
    {
      if (!CheckLine(i, j--))
        return false;
    }
  }
  if (Direction == Right)
  {
    while (j < GRID_SIZE)
    {
      CheckLine(i, j++);
    }
  }
  return true;
}

int *GetPlayerMove(void)
{
  static int Move[2];
  printf("Move vertical 0-7: ");
  scanf("%d", &Move[0]);
  printf("Move horizontal 0-7: ");
  scanf("%d", &Move[1]);

  if (!grid[Move[0]][Move[1]])
    // TODO: Check if move is actually valid.
    return Move;
  else
  {
    printf("Grid location %d,%d is already occupied!\n", Move[0], Move[1]);
    GetPlayerMove();
  }
  return Move;
}

bool CheckForSurround(int x, int y)
{
  /* TODO: Need to fix this to check in all straight line directions and
   * swap all pieces of there are two anchoring pieces of the opposite
   * colour. */
  bool isSurrounded = false;

  char *OpponentColour = "";
  char *GridColour = grid[x][y];
  char *White = "W";
  char *Black = "B";

  if (GridColour == White)
    OpponentColour = "B";
  else if (GridColour == Black)
    OpponentColour = "W";

  // Left-right
  if (grid[x - 1][y] == OpponentColour && grid[x + 1][y] == OpponentColour)
    isSurrounded = true;
  // Diagonal
  if (grid[x - 1][y - 1] == OpponentColour && grid[x + 1][y + 1] == OpponentColour)
    isSurrounded = true;
  if (grid[x + 1][y - 1] == OpponentColour && grid[x - 1][y + 1] == OpponentColour)
    isSurrounded = true;
  // Vertical
  if (grid[x][y - 1] == OpponentColour && grid[x][y + 1] == OpponentColour)
    isSurrounded = true;

  if (isSurrounded)
  {
    grid[x][y] = OpponentColour;
  }

  return isSurrounded;
}

void DoPlayerMove(int Move[])
{
  if (Player1Turn)
  {
    grid[Move[0]][Move[1]] = "W";
  }
  else
  {
    grid[Move[0]][Move[1]] = "B";
  }

  for (int i = 0; i < GRID_SIZE; ++i)
  {
    for (int j = 0; j < GRID_SIZE; ++j)
    {
      CheckForSurround(i, j);
    }
  }

  Player1Turn = !Player1Turn;
}

int main(int argc, char *argv[])
{
  grid[3][3] = "W";
  grid[4][4] = "W";
  grid[3][4] = "B";
  grid[4][3] = "B";

  while (true)
  {
    PrintGrid(grid);
    int *Move = GetPlayerMove();
    DoPlayerMove(Move);
    printf("%s\n", ScanLine(Move, "left") ? "true" : "false");
  }

  /* For any grid pos [x][y] the surrounding squares are
   * [x-1][y-1] [x][y-1] [x+1][y-1]
   * [x-1][y]   [x][y]   [x+1][y]
   * [x-1][y+1] [x][y+1] [x+1][y+1]
   */

  return 0;
}
