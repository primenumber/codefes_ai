#include "search.hpp"
#include <algorithm>
#include <iostream>
#include <boost/timer/timer.hpp>
#include "eval.hpp"

int alpha_beta(const GameState &gs,
    int alpha, const int beta, const int depth) {
  if (depth == 0) return eval(gs);
  if (gs.is_gameover()) return eval(gs);
  for (const auto &[next, play] : next_states(gs)) {
    alpha = std::max(alpha, -alpha_beta(next, -beta, -alpha, depth-1));
    if (alpha >= beta) return alpha;
  }
  return alpha;
}

std::pair<int, Play> search(const GameState &gs) {
  int depth = 0;
  boost::timer::cpu_timer timer;
  while (true) {
    int alpha = -10000000;
    const int beta = 10000000;
    Play p;
    auto nexts = next_states(gs);
    std::sort(std::begin(nexts), std::end(nexts),
        [] (const auto &l, const auto &r) {
          return eval(l.first) < eval(r.first);
        });
    for (const auto &[next, play] : nexts) {
      int val = -alpha_beta(next, alpha, beta, depth);
      if (val > alpha) {
        alpha = val;
        p = play;
      }
    }
    if (timer.elapsed().wall > 300'000) {
      std::cerr << timer.elapsed().wall << std::endl;
      return std::make_pair(alpha, p);
    }
    ++depth;
  }
}
