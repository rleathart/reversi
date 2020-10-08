#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/* An even number that defines the number of row/columns in the grid. */
#define GRID_SIZE 8

static int grid[GRID_SIZE][GRID_SIZE] = {0};

bool Player1Turn = true;
int PlayerColour;

bool ScanLine(int i, int j, bool doFlip)
{
  /* Check outwards from the grid position.
   * Store the position of all tiles after the starting position in some array
   * toFlip[].
   * If we encounter a blank tile before a tile of the same colour, illegal
   * move.
   * Otherwise, keep adding to toFlip until we reach a tile of the same
   * colour.
   * Then loop over toFlip[] inverting the tile colour.
   */

  int Directions[8][2] = {{-1, -1}, {-1, 1}, {1, -1}, {1, 1},
                          {-1, 0},  {1, 0},  {0, -1}, {0, 1}};

  int Starti = i;
  int Startj = j;

  int StartColour = grid[i][j] ? grid[i][j] : PlayerColour;

  bool didFlip = false;
  for (int n = 0; n < 8; ++n)
  {
    i = Starti;
    j = Startj;
    int *Direction = Directions[n];

    /* Need to initialise toFlip values to {-1,-1} otherwise grid[0][0] will
     * always be flipped. */
    int toFlip[GRID_SIZE][2];
    for (int i = 0; i < GRID_SIZE; ++i)
    {
      toFlip[i][0] = -1;
      toFlip[i][1] = -1;
    }

    int FlipCounter = 0;
    bool validMove = true;
    while (i >= 0 && i < GRID_SIZE && j >= 0 && j < GRID_SIZE)
    {
      i += Direction[0];
      j += Direction[1];

      /* If we're out of bounds after iterating, move on to the next
       * direction. */
      if (!(i >= 0 && i < GRID_SIZE && j >= 0 && j < GRID_SIZE))
      {
        validMove = false;
        break;
      }

      /* If we encounter a piece that is the same colour as that of the tile
       * we started on, we have checked all we need to for this direction. */
      if (grid[i][j] == StartColour)
        break;

      /* If we encounter a blank tile before a tile of the same colour, this
       * cannot be a valid direction so move on to the next one. */
      if (grid[i][j] == 0)
      {
        validMove = false;
        break;
      }

      /* Store the tiles that we have traversed in toFlip so we can flip them
       * later if we have to. */
      toFlip[FlipCounter][0] = i;
      toFlip[FlipCounter][1] = j;

      ++FlipCounter;
    }

    if (!validMove)
      continue;

    for (int i = 0; i < 8; ++i)
    {
      /* If there isn't at least one tile to flip then check the next
       * direction. */
      if (toFlip[i][0] != -1 && toFlip[i][1] != -1)
        break;
      goto NEXT_DIRECTION;
    }

    /* We want to actually flip the pieces and not just check if they need to
     * be flipped. */
    if (doFlip)
    {
      for (int loop = 0; loop < GRID_SIZE; ++loop)
      {
        int row = toFlip[loop][0];
        int col = toFlip[loop][1];

        if (row >= 0 && col >= 0 && row < GRID_SIZE && col < GRID_SIZE)
          grid[row][col] = -grid[row][col];
      }
    }
    didFlip = true;
  NEXT_DIRECTION:
    continue;
  }
  return didFlip;
}

void PrintGrid(int grid[GRID_SIZE][GRID_SIZE])
{
  for (int i = 0; i < GRID_SIZE; ++i)
    for (int j = 0; j < GRID_SIZE; ++j)
    {
      char *DisplayString, *TileSep;
      DisplayString = grid[i][j] == -1 ? "W" : grid[i][j] == 1 ? "B" : " ";
      /* Print a space between tiles and a newline at the end of a row. */
      TileSep = j == GRID_SIZE - 1 ? "\n" : " ";

      printf("[%s]%s", DisplayString, TileSep);
    }
}

bool isValidMove(int Move[])
{
  /* This tile is occupied already. */
  if (grid[Move[0]][Move[1]] != 0)
    return false;
  /* Check if this move will result in any pieces being flipped. */
  return ScanLine(Move[0], Move[1], false);
}

int *GetPlayerMove(void)
{
  static int Move[2];
  printf("[%s] Move vertical 0-%d: ", Player1Turn ? "W" : "B", GRID_SIZE - 1);
  scanf("%d", &Move[0]);
  printf("[%s] Move horizontal 0-%d: ", Player1Turn ? "W" : "B", GRID_SIZE - 1);
  scanf("%d", &Move[1]);

  if (grid[Move[0]][Move[1]])
  {
    printf("Grid location %d,%d is already occupied!\n", Move[0], Move[1]);
    return GetPlayerMove();
  }
  else if (!isValidMove(Move))
  {
    printf("Invalid move!\n");
    return GetPlayerMove();
  }
  return Move;
}

void DoPlayerMove(int Move[])
{
  grid[Move[0]][Move[1]] = Player1Turn ? -1 : 1;
  ScanLine(Move[0], Move[1], true);
  Player1Turn = !Player1Turn;
}

bool CanPlay(void)
{
  /* For every tile on the grid, see if putting a piece there results in a
   * valid move. If no tiles result in valid moves, the game is over. */
  for (int i = 0; i < GRID_SIZE; ++i)
    for (int j = 0; j < GRID_SIZE; ++j)
    {
      int Move[] = {i, j};
      if (isValidMove(Move))
        return true;
    }
  return false;
}

int GetScore(int Player)
{
  /* Count number of pieces on the grid for a given player.
   * Player 1: -1
   * Player 2:  1 */
  int Score = 0;
  for (int i = 0; i < GRID_SIZE; ++i)
    for (int j = 0; j < GRID_SIZE; ++j)
      if (grid[i][j] == Player)
        ++Score;

  return Score;
}

int randrange(int lower, int upper)
{
  /* Return a random int between lower and upper inclusive. */
  return (rand() % (upper - lower + 1)) + lower;
}

int main(int argc, char *argv[])
{
  /* Initialise all grid tiles to blank. */
  for (int i = 0; i < GRID_SIZE; ++i)
    for (int j = 0; j < GRID_SIZE; ++j)
      grid[i][j] = 0;
  /* Initialise starting grid positions. */
  grid[GRID_SIZE / 2 - 1][GRID_SIZE / 2 - 1] = 1;
  grid[GRID_SIZE / 2][GRID_SIZE / 2] = 1;
  grid[GRID_SIZE / 2 - 1][GRID_SIZE / 2] = -1;
  grid[GRID_SIZE / 2][GRID_SIZE / 2 - 1] = -1;

  int TurnCount = 0;
  while (CanPlay())
  {
    ++TurnCount;
    printf("Turn %d\n", TurnCount);
    PlayerColour = Player1Turn ? -1 : 1;
    PrintGrid(grid);
    int *Move = GetPlayerMove();
    /* int Move[2] = {randrange(0, 7), randrange(0, 7)}; */
    /* printf("\n"); */
    DoPlayerMove(Move);
  }

  int Player1Score = GetScore(-1);
  int Player2Score = GetScore(1);

  if (Player1Score != Player2Score)
    printf("Player %d wins!\n", Player1Score > Player2Score ? 1 : 2);
  else
    printf("Draw!\n");

  return 0;
}
