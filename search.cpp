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
  if (depth == 0) return 0;
  int count = next_states_count(gs);
  if (count == 0) return eval(gs);
  std::uniform_int_distribution<int> dis(0, count-1);
  return -0.98 * playout(play_index(gs, dis(mt)).first, depth-1, mt);
}

int alpha_beta_op(const GameState &gs, int alpha, const int beta,
    const int depth, std::mt19937 &mt, const int width);

int alpha_beta_me(const GameState &gs, int alpha, const int beta,
    const int depth, std::mt19937 &mt, const int width) {
  if (depth == 0) return eval(gs.get_me());
  std::vector<int> counts = next_states_count_vec(gs);
  int next_counts = 0;
  for (int i = 0; i < 4; ++i) {
    if (counts[i] == 0) continue;
    std::uniform_int_distribution<int> dis(0, counts[i] - 1);
    int index = next_counts + dis(mt);
    auto [next, play] = play_index(gs, index);
    alpha = std::max(alpha, -alpha_beta_op(next, -beta, -alpha, depth-1, mt, width));
    if (alpha >= beta) return alpha;
    next_counts += counts[i];
  }
  return alpha;
}

int alpha_beta_op(const GameState &gs, int alpha, const int beta,
    const int depth, std::mt19937 &mt, const int width) {
  int n = next_states_count(gs);
  if (n == 0) return -1000000;
  std::uniform_int_distribution<int> dis(0, n-1);
  for (int i = 0; i < width; ++i) {
    int index = dis(mt);
    auto [next, play] = play_index(gs, index);
    alpha = std::max(alpha, -alpha_beta_me(next, -beta, -alpha, depth-1, mt, width));
    if (alpha >= beta) return alpha;
  }
  return alpha;
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
  constexpr int threads = 5;
  for (int i = 0; i < threads; ++i) {
    vmt.emplace_back(mt());
  }
  for (int i = 0; i < threads; ++i) {
    vt.emplace_back([&](const int i) {
      std::uniform_int_distribution<int> dis(0, n-1);
      while (timer.elapsed().wall < 80'000'000) {
        int index = dis(vmt[i]);
        int val;
        m.lock();
        if (auto next = nexts[index]) {
          m.unlock();
          val = -playout(next->first, 80, vmt[i]);
        } else {
          m.unlock();
          auto next_ = play_index(gs, index);
          val = -playout(next_.first, 80, vmt[i]);
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
  const std::vector<int> counts = next_states_count_vec(gs);
  std::vector<int> value(4, -1000000);
  int width = 16;
  for (int depth = 1; timer.elapsed().wall < 5'000'000 && depth < 15; depth += 2) {
    std::cerr << depth << std::endl;
    const int alpha = -1000000;
    const int beta = 1000000;
    int next_counts = 0;
    int mx = -1000000;
    for (int i = 0; i < 4; ++i) {
      if (counts[i] == 0) continue;
      std::uniform_int_distribution<int> dis(0, counts[i] - 1);
      int index = next_counts + dis(mt);
      auto [next, play] = play_index(gs, index);
      value[i] = -alpha_beta_op(next, -beta, -alpha, depth, mt, width);
      mx = std::max(mx, value[i]);
      next_counts += counts[i];
    }
    if (abs(mx) == 1000000) {
      width += 4;
      depth -= 2;
      continue;
    }
  }
  for (auto && t : vt) t.join();
  std::cerr << loop_count << std::endl;
  Play p;
  double mx = -100000000;
  for (int i = 0; i < n; ++i) {
    if (count[i] == 0) continue;
    int s;
    if (score[i] > -500000) {
      s = score[i] + 10.0 * value[static_cast<int>(nexts[i]->second.dir)];
    } else {
      s = score[i];
    }
    if (s > mx) {
      mx = s;
      p = nexts[i]->second;
    }
  }
  return std::make_pair((int)mx, p);
}
