#include <iostream>
#include "gamestate.hpp"
#include "search.hpp"

Board input_bd() {
  Board res;
  for (int i = 0; i < 5; ++i) {
    for (int j = 0; j < 5; ++j) {
      int v;
      std::cin>>v;
      res.at(i, j) = v;
    }
  }
  return res;
}

int main() {
  line::init();
  int turn;
  std::cin >> turn;
  std::cout << 1 << ' ' << 1 << std::endl;
  std::string i2c = "LURD";
  while (true) {
    Board me = input_bd();
    Board op = input_bd();
    GameState gs(me, op);
    if (gs.is_gameover()) {
      int r = 0, c = 0;
      for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
          if (gs.get_me().at(i, j) == 0) {
            r = i;
            c = j;
          }
        }
      }
      std::cout << "L 1 1 " << (r+1) << ' ' << (c+1) << std::endl;
    } else {
      auto [score, play] = search(gs);
      std::cout << i2c[static_cast<int>(play.dir)];
      std::cout << ' ' << play.m << ' ' << play.v;
      for (const auto &[r, c] : play.vp) {
        std::cout << ' ' << (r+1) << ' ' << (c+1);
      }
      std::cout << std::endl;
    }
  }
  return 0;
}