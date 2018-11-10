#include "search.hpp"
#include <algorithm>
#include <iostream>
#include "eval.hpp"

std::pair<int, Play> search(const GameState &gs) {
  int score = -10000000;
  Play p;
  auto nexts = next_states(gs);
  for (const auto &[next, play] : nexts) {
    int val = -eval(next);
    if (val > score) {
      score = val;
      p = play;
    }
  }
  return std::make_pair(score, p);
}
