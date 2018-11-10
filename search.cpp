#include "search.hpp"
#include <algorithm>
#include <iostream>
#include "eval.hpp"

std::pair<int, Play> search(const GameState &gs, std::mt19937 &mt) {
  int score = -10000000;
  Play p;
  auto nexts = next_states(gs);
  int count = 0;
  for (const auto &[next, play] : nexts) {
    int val = -eval(next);
    if (val > score) {
      score = val;
      p = play;
      count = 1;
    } else if (val == score) {
      ++count;
      std::uniform_int_distribution<int> dis(0, count-1);
      if (dis(mt) == 0) {
        p = play;
      }
    }
  }
  return std::make_pair(score, p);
}
