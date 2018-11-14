#include <iostream>
#include <random>
#include "gamestate.hpp"
#include "search.hpp"
#include "utils.hpp"
#include "eval.hpp"

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

int main(int argc, char **argv) {
  line::init();
  init();
  eval_init();
  std::random_device rd;
  std::mt19937 mt(rd());
  if (argc > 1) {
    int n;
    std::cin >> n;
    for (int i = 0; i < n; ++i) {
      Board me = input_bd();
      Board op = input_bd();
      GameState gs(me, op);
      auto [score, play] = search(gs, mt, 100'000'000, false);
      std::cout << to_string(gs);
      std::cout << score << std::endl;
    }
    return 0;
  }
  int turn;
  std::cin >> turn;
  std::uniform_int_distribution<int> dis(1, 5);
  std::cout << dis(mt) << ' ' << dis(mt) << std::endl;
  std::string i2c = "LURD";
  while (true) {
    int turn, timeleft, score_me, score_op;
    std::cin >> turn >> timeleft >> score_me >> score_op;
    if (std::cin.eof()) break;
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
      int64_t tl = timeleft * 1000000ll / (1000 - turn);
      auto [score, play] = search(gs, mt, tl);
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
