#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef enum
{
  false,
  true
} bool;

enum
{
  ExitSuccess
};

/* An even number that defines the number of row/columns in the grid. */
#define GRID_SIZE 8

int PlayerColour;

typedef struct GridInfo GridInfo;

struct GridInfo
{
  int Position[GRID_SIZE * GRID_SIZE][2];
  void (*print)(GridInfo* FlipInfo);
};

typedef struct GameState_t GameState_t;

struct GameState_t
{
  bool Player1Turn;
  int Grid[GRID_SIZE][GRID_SIZE];
  int TurnCount;
};

GameState_t GameState = {.Player1Turn = true};

void GridInfo_print(GridInfo* self)
{
  int GridInfoPositionSize = sizeof self->Position / sizeof self->Position[0];
  for (int i = 0; i < GridInfoPositionSize; ++i)
  {
    printf("{%d, %d},", self->Position[i][0], self->Position[i][1]);
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

  PlayerColour = GameState.Player1Turn ? -1 : 1;
  int StartColour = GameState.Grid[i][j] ? GameState.Grid[i][j] : PlayerColour;

  GridInfo FlipInfo = New_GridInfo();

  int FlipCounter = 0;
  for (int n = 0; n < sizeof Directions / sizeof Directions[0]; ++n)
  {
    i = Starti;
    j = Startj;
    int* Direction = Directions[n];

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
      if (GameState.Grid[i][j] == StartColour)
        break;

      /* If we encounter a blank tile before a tile of the same colour, this
       * cannot be a valid direction so move on to the next one. */
      if (GameState.Grid[i][j] == 0)
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
      GameState.Grid[row][col] = -GameState.Grid[row][col];
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
  if (GameState.Grid[Move[0]][Move[1]] != 0)
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

void SaveGame()
{
  FILE* SaveGame = fopen("save.dat", "wb");
  fwrite(&GameState, sizeof GameState, 1, SaveGame);
  fclose(SaveGame);
}

int LoadGame()
{
  FILE* SaveGame = fopen("save.dat", "rb");
  if (!SaveGame)
  {
    fprintf(stderr, "Error: No save file to load!\n");
    fclose(SaveGame);
    return 1;
  }
  fread(&GameState, sizeof GameState, 1, SaveGame);
  fclose(SaveGame);
  return 0;
}

void FlushInputStream()
{
  int c;
  while ((c = getchar()) != '\n' && c != EOF)
    ;
}

int GetUserInput(char* Prompt, int BytesToRead, char* OutString)
{
  printf("%s", Prompt);
  fgets(OutString, BytesToRead, stdin);

  int StringLength = strlen(OutString);

  if (OutString[StringLength - 1] == '\n')
    OutString[StringLength - 1] = '\0';
  else
    FlushInputStream();

  return 0;
}

#define INPUT_BUFFER_SIZE 16

int* GetPlayerMove(void)
{
  static int Move[2];

  char Input[INPUT_BUFFER_SIZE] = {0};

  char* Directions[] = {"vertical", "horizontal"};
  for (int i = 0; i < sizeof Directions / sizeof Directions[0]; ++i)
  {
    printf("[%s] Move %s 0-%d: ", GameState.Player1Turn ? "W" : "B",
           Directions[i], GRID_SIZE - 1);

    GetUserInput("", INPUT_BUFFER_SIZE, Input);

    if (strlen(Input) == 1)
      switch (Input[0])
      {
      case 'l':
        if (LoadGame() == 0)
          printf("Game loaded!\n");
        PrintGrid(GameState.Grid);
        return GetPlayerMove();
      case 's':
        SaveGame();
        printf("Game saved!\n");
        return GetPlayerMove();
      case 'x':
        exit(ExitSuccess);
      }
    Move[i] = atoi(Input);
  }

  if (GameState.Grid[Move[0]][Move[1]])
  {
    printf("Grid location %d,%d is already occupied!\n", Move[0], Move[1]);
    return GetPlayerMove();
  }
  return Move;
}

void DoPlayerMove(int Move[], GridInfo FlipInfo)
{
  GameState.Grid[Move[0]][Move[1]] = GameState.Player1Turn ? -1 : 1;
  DoFlips(FlipInfo);
  GameState.Player1Turn = !GameState.Player1Turn;
}

bool CanPlay(void)
{
  /* For every tile on the grid, see if putting a piece there results in a
   * valid move. If no tiles result in valid moves, the game is over. */
  for (int i = 0; i < GRID_SIZE; ++i)
    for (int j = 0; j < GRID_SIZE; ++j)
    {
      int Move[] = {i, j};
      if (GameState.Grid[i][j] != 0)
        continue;
      GridInfo FlipInfo = GetFlips(i, j);
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
      if (GameState.Grid[i][j] == Player)
        ++Score;

  return Score;
}

int randrange(int lower, int upper)
{
  /* Return a random int between lower and upper inclusive. */
  return (rand() % (upper - lower + 1)) + lower;
}

int GameExit(void)
{
  printf("Game finished!\n");

  int Player1Score = GetScore(-1);
  int Player2Score = GetScore(1);

  PrintGrid(GameState.Grid);

  printf("Player 1 score: %d\n", Player1Score);
  printf("Player 2 score: %d\n", Player2Score);

  if (Player1Score != Player2Score)
    printf("Player %d wins!\n", Player1Score > Player2Score ? 1 : 2);
  else
    printf("Draw!\n");
  return 0;
}

void PrintWelcome()
{
  printf("------- Welcome to Reversi! -------\n\n"
         "Enter 's' or 'l' at any time to save/load your game.\n\n");
}

int main(void)
{
  /* Set the seed for rand(). Only required for testing purposes. */
  /* srand(time(0)); */

  PrintWelcome();

  GridInfo FlipInfo = New_GridInfo();

  /* Initialise all grid tiles to blank. */
  for (int i = 0; i < GRID_SIZE; ++i)
    for (int j = 0; j < GRID_SIZE; ++j)
      GameState.Grid[i][j] = 0;

  /* Initialise starting grid positions. */
  GameState.Grid[GRID_SIZE / 2 - 1][GRID_SIZE / 2 - 1] = 1;
  GameState.Grid[GRID_SIZE / 2][GRID_SIZE / 2] = 1;
  GameState.Grid[GRID_SIZE / 2 - 1][GRID_SIZE / 2] = -1;
  GameState.Grid[GRID_SIZE / 2][GRID_SIZE / 2 - 1] = -1;

  GameState.TurnCount = 0;

  /* We need a bool to keep track of whether there was a valid move available on
   * the previous turn. We should stop if nothing could be played on the
   * previous turn and nothing can be played on the current turn. */
  bool lastCouldPlay = true;
  while (true)
  {
    PlayerColour = GameState.Player1Turn ? -1 : 1;

    if (!CanPlay())
    {
      /* If the current player can't play and the previous player also
       * couldn't play, the game is over. */
      if (!lastCouldPlay)
        break;

      /* Otherwise, let the next loop iteration know we couldn't play and go
       * to the next turn */
      lastCouldPlay = false;
      GameState.Player1Turn = !GameState.Player1Turn;
      continue;
    }

    /* The current player can play, so let the next iteration know that. */
    lastCouldPlay = true;

    ++GameState.TurnCount;
    printf("Turn %d. %s to play\n", GameState.TurnCount,
           GameState.Player1Turn ? "W" : "B");

    PrintGrid(GameState.Grid);

    int* Move;
    /* int Move[2]; */
    /* Keep asking the user for a move until it is a valid one. */
    bool validMove = false;
    while (!validMove)
    {
      Move = GetPlayerMove();
      /* Move[0] = randrange(0, GRID_SIZE - 1); */
      /* Move[1] = randrange(0, GRID_SIZE - 1); */
      FlipInfo = GetFlips(Move[0], Move[1]);
      validMove = isValidMove(Move, FlipInfo);

      if (!validMove)
        printf("Invalid move!\n");
    }

    DoPlayerMove(Move, FlipInfo);
  }

  return GameExit();
}
