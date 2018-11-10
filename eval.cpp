#include "eval.hpp"

constexpr int INF = 1000000;

int eval_impl(const Board &bd) {
  if (bd.is_gameover()) return -INF;
  int sum = 0;
  for (int i = 0; i < 5; ++i) {
    for (int j = 0; j < 4; ++j) {
      //if (bd.at(i, j) && bd.at(i, j+1)) {
        sum += abs((1 << bd.at(i, j)) - (1 << bd.at(i, j+1)));
      //}
    }
  }
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 5; ++j) {
      //if (bd.at(i, j) && bd.at(i+1, j)) {
        sum += abs((1 << bd.at(i, j)) - (1 << bd.at(i+1, j)));
      //}
    }
  }
  return -sum;
}

int eval(const GameState &gs) {
  return eval_impl(gs.get_me()) - eval_impl(gs.get_op());
}
