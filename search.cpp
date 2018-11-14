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

std::pair<int, Play> search_many(const GameState &gs, std::mt19937 &mt,
    const int tl, const bool limit) {
  std::vector<int> counts = next_states_count_vec(gs);
  std::vector<int> dirs;
  for (int i = 0; i < 4; ++i) {
    if (counts[i] == 0) continue;
    dirs.push_back(i);
  }
  std::vector<int> count(std::size(dirs), 0);
  std::vector<double> score(std::size(dirs), 0);
  std::atomic_int loop_count(0);
  std::vector<std::mt19937> vmt;
  boost::timer::cpu_timer timer;
  std::vector<std::thread> vt;
  std::mutex m;
  int threads = limit ? 6 : std::thread::hardware_concurrency();
  int depth = limit ? 40 : 1000;
  for (int i = 0; i < threads; ++i) {
    vmt.emplace_back(mt());
  }
  for (int i = 0; i < threads; ++i) {
    vt.emplace_back([&](const int i) {
      std::uniform_int_distribution<int> dis_dir(0, std::size(dirs)-1);
      std::vector<std::uniform_int_distribution<int>> vdis;
      int next_count = 0;
      for (int i = 0; i < std::size(dirs); ++i) {
        vdis.emplace_back(next_count, next_count + counts[dirs[i]] - 1);
        next_count += counts[i];
      }
      while (timer.elapsed().wall < 0.8 * tl) {
        int dir = dis_dir(vmt[i]);
        int index = vdis[dir](vmt[i]);
        int val = -playout(play_index(gs, index).first, depth, vmt[i]);
        m.lock();
        score[dir] = (score[dir] * count[dir] + val) / (count[dir]+1);
        ++count[dir];
        m.unlock();
        ++loop_count;
      }
    }, i);
  }
  for (auto && t : vt) t.join();
  std::cerr << loop_count << std::endl;
  double mx = -10000000;
  std::vector<std::uniform_int_distribution<int>> vdis;
  int next_count = 0;
  for (int dir : dirs) {
    vdis.emplace_back(next_count, next_count + counts[dir] - 1);
    next_count += counts[dir];
  }
  int d;
  for (int i = 0; i < std::size(dirs); ++i) {
    if (count[i] == 0) continue;
    if (score[i] > mx) {
      mx = score[i];
      d = i;
    }
  }
  Play p = play_index(gs, vdis[d](mt)).second;
  std::cerr << mx << std::endl;
  return std::make_pair((int)mx, p);
}

std::pair<int, Play> search(const GameState &gs, std::mt19937 &mt,
    const int tl, const bool limit) {
  int n = next_states_count(gs);
  if (n >= 100) {
    return search_many(gs, mt, tl, limit);
  }
  std::vector<std::pair<GameState, Play>> nexts = next_states(gs);
  std::vector<double> score(n, 0.0);
  std::vector<int> count(n, 0);
  std::atomic_int loop_count(0);
  std::vector<std::mt19937> vmt;
  boost::timer::cpu_timer timer;
  std::vector<std::thread> vt;
  std::mutex m;
  int threads = limit ? 6 : std::thread::hardware_concurrency();
  int depth = limit ? 40 : 1000;
  for (int i = 0; i < threads; ++i) {
    vmt.emplace_back(mt());
  }
  for (int i = 0; i < threads; ++i) {
    vt.emplace_back([&](const int i) {
      std::uniform_int_distribution<int> dis(0, n-1);
      while (timer.elapsed().wall < 0.8 * tl) {
        int index = dis(vmt[i]);
        int val = -playout(nexts[index].first, depth, vmt[i]);
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
      p = nexts[i].second;
    }
  }
  std::cerr << mx << std::endl;
  return std::make_pair((int)mx, p);
}
