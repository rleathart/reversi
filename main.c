#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* An even number that defines the number of row/columns in the grid. */
#define GRID_SIZE 8

int grid[GRID_SIZE][GRID_SIZE];

bool Player1Turn = true;
int PlayerColour;

typedef struct GridInfo GridInfo;

struct GridInfo
{
  int Position[GRID_SIZE * GRID_SIZE][2];
  void (*print)(GridInfo *FlipInfo);
};

void GridInfo_print(GridInfo *self)
{
  int GridInfoPositionSize = sizeof self->Position / sizeof self->Position[0];
  for (int i = 0; i < GridInfoPositionSize; ++i)
  {
    printf("{%d, %d},",self->Position[i][0], self->Position[i][1]);
  }
  printf("\n");
}

GridInfo New_GridInfo(void)
{
  GridInfo self;
  int GridInfoPositionSize = sizeof self.Position / sizeof self.Position[0];
  for (int i = 0; i < GridInfoPositionSize; ++i)
  {
    self.Position[i][0] = -1;
    self.Position[i][1] = -1;
  }
  self.print = GridInfo_print;
  return self;
}

GridInfo GetFlips(int i, int j)
{
  int Directions[8][2] = {{-1, -1}, {-1, 1}, {1, -1}, {1, 1},
                          {-1, 0},  {1, 0},  {0, -1}, {0, 1}};

  int Starti = i;
  int Startj = j;

  int StartColour = grid[i][j] ? grid[i][j] : PlayerColour;

  GridInfo FlipInfo = New_GridInfo();

  int FlipCounter = 0;
  for (int n = 0; n < sizeof Directions / sizeof Directions[0]; ++n)
  {
    i = Starti;
    j = Startj;
    int *Direction = Directions[n];

    int ProgressCheck[GRID_SIZE * GRID_SIZE] = {0};
    bool validMove = true;
    while (true)
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
      FlipInfo.Position[FlipCounter][0] = i;
      FlipInfo.Position[FlipCounter][1] = j;

      ProgressCheck[FlipCounter] = 1;

      ++FlipCounter;
    }

    /* If this direction isn't valid, reset all the positions that we staged
     * to be flipped. */
    if (!validMove)
      for (int i = 0; i < sizeof ProgressCheck / sizeof ProgressCheck[0]; ++i)
        if (ProgressCheck[i] == 1)
        {
          FlipInfo.Position[i][0] = -1;
          FlipInfo.Position[i][1] = -1;
        }
  }
  return FlipInfo;
}

void DoFlips(GridInfo FlipInfo)
{
  int FlipInfoPositionSize =
      sizeof FlipInfo.Position / sizeof FlipInfo.Position[0];

  for (int i = 0; i < FlipInfoPositionSize; ++i)
  {
    int row = FlipInfo.Position[i][0];
    int col = FlipInfo.Position[i][1];
    if (row != -1 && col != -1)
      grid[row][col] = -grid[row][col];
  }
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

bool isValidMove(int Move[], GridInfo FlipInfo)
{
  /* This tile is occupied already. */
  if (grid[Move[0]][Move[1]] != 0)
    return false;

  int FlipInfoPositionSize =
      sizeof FlipInfo.Position / sizeof FlipInfo.Position[0];

  for (int i = 0; i < FlipInfoPositionSize; ++i)
  {
    int row = FlipInfo.Position[i][0];
    int col = FlipInfo.Position[i][1];

    /* If this move results in at least one flip. */
    if (row != -1 && col != -1)
      return true;
  }
  return false;
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
  return Move;
}

void DoPlayerMove(int Move[], GridInfo FlipInfo)
{
  grid[Move[0]][Move[1]] = Player1Turn ? -1 : 1;
  DoFlips(FlipInfo);
  Player1Turn = !Player1Turn;
}

bool CanPlay(GridInfo FlipInfo)
{
  /* For every tile on the grid, see if putting a piece there results in a
   * valid move. If no tiles result in valid moves, the game is over. */
  for (int i = 0; i < GRID_SIZE; ++i)
    for (int j = 0; j < GRID_SIZE; ++j)
    {
      int Move[] = {i, j};
      if (grid[i][j] != 0)
        continue;
      FlipInfo = GetFlips(i, j);
      if (isValidMove(Move, FlipInfo))
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
  /* Set the seed for rand() */
  srand(time(0));

  GridInfo FlipInfo = New_GridInfo();

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
  bool canPlay = true;
  do
  {
    ++TurnCount;
    printf("Turn %d. %s to play\n", TurnCount, Player1Turn ? "W" : "B");
    PlayerColour = Player1Turn ? -1 : 1;
    canPlay = CanPlay(FlipInfo);
    if (!canPlay)
      break;
    PrintGrid(grid);
    int *Move;
    /* int Move[2]; */
    bool validMove = true;
    do
    {
      Move = GetPlayerMove();
      /* Move[0] = randrange(0, 7); */
      /* Move[1] = randrange(0, 7); */
      FlipInfo = GetFlips(Move[0], Move[1]);
      validMove = isValidMove(Move, FlipInfo);
      if (!validMove)
        printf("Invalid move!\n");
    } while (!validMove);

    DoPlayerMove(Move, FlipInfo);
  } while (canPlay);

  PrintGrid(grid);
  int Player1Score = GetScore(-1);
  int Player2Score = GetScore(1);

  if (Player1Score != Player2Score)
    printf("Player %d wins!\n", Player1Score > Player2Score ? 1 : 2);
  else
    printf("Draw!\n");

  return 0;
}
