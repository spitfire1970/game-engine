#include "Game/Game.h"

int main(int argc, char *argv[]) {
  Game game;
  if (game.Initialize()) {
    return EXIT_FAILURE;
  };
  game.Run();
  game.Destroy();
  return 0;
}
