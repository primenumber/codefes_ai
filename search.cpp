#include "search.hpp"
#include <algorithm>
#include <iostream>
#include <optional>
#include <boost/timer/timer.hpp>
#include "eval.hpp"

int playout(const GameState &gs, const int depth, std::mt19937 &mt) {
  if (depth == 0) return eval(gs);
  int count = next_states_count(gs);
  if (count == 0) return eval(gs);
  std::uniform_int_distribution<int> dis(0, count-1);
  return -playout(play_index(gs, dis(mt)).first, depth-1, mt);
}

std::pair<int, Play> search(const GameState &gs, std::mt19937 &mt) {
  int n = next_states_count(gs);
  std::vector<std::optional<std::pair<GameState, Play>>> nexts(n);
  if (n < 10000) {
    auto nexts_noopt = next_states(gs);
    for (int i = 0; i < n; ++i) {
      nexts[i] = nexts_noopt[i];
    }
  }
  std::uniform_int_distribution<int> dis(0, n-1);
  std::vector<double> score(n, 0.0);
  std::vector<int> count(n, 0);
  boost::timer::cpu_timer timer;
  int loop_count = 0;
  while (timer.elapsed().wall < 80'000'000) {
    int index = dis(mt);
    int val;
    if (auto next = nexts[index]) {
      val = -playout(next->first, 20, mt);
    } else {
      auto next_ = play_index(gs, index);
      val = -playout(next_.first, 20, mt);
      nexts[index] = next_;
    }
    score[index] = (score[index] * count[index] + val) / (count[index]+1);
    ++count[index];
    ++loop_count;
  }
  std::cerr << loop_count << std::endl;
  Play p;
  double mx = -10000000;
  for (int i = 0; i < n; ++i) {
    if (count[i] == 0) continue;
    if (score[i] > mx) {
      mx = score[i];
      p = nexts[i]->second;
    }
  }
  return std::make_pair((int)mx, p);
}
