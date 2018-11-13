#include "search.hpp"
#include <algorithm>
#include <atomic>
#include <iostream>
#include <mutex>
#include <optional>
#include <thread>
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
  std::vector<double> score(n, 0.0);
  std::vector<int> count(n, 0);
  std::atomic_int loop_count(0);
  std::vector<std::mt19937> vmt;
  boost::timer::cpu_timer timer;
  std::vector<std::thread> vt;
  std::mutex m;
  for (int i = 0; i < 6; ++i) {
    vmt.emplace_back(mt());
  }
  for (int i = 0; i < 6; ++i) {
    vt.emplace_back([&](const int i) {
      std::uniform_int_distribution<int> dis(0, n-1);
      while (timer.elapsed().wall < 80'000'000) {
        int index = dis(vmt[i]);
        int val;
        m.lock();
        if (auto next = nexts[index]) {
          m.unlock();
          val = -playout(next->first, 20, vmt[i]);
        } else {
          m.unlock();
          auto next_ = play_index(gs, index);
          val = -playout(next_.first, 20, vmt[i]);
          m.lock();
          nexts[index] = next_;
          m.unlock();
        }
        m.lock();
        score[index] = (score[index] * count[index] + val) / (count[index]+1);
        ++count[index];
        m.unlock();
        ++loop_count;
      }
    }, i);
  }
  for (auto && t : vt) t.join();
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
