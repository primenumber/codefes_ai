#include "gamestate.hpp"
#include "utils.hpp"
#include <exception>
#include <iostream>

bool GameState::is_gameover() const {
  return me.is_gameover() || op.is_gameover();
}

std::vector<std::pair<GameState, Play>> next_states(const GameState &gs) {
  std::vector<std::pair<int, int>> empties;
  for (int i = 0; i < 5; ++i) {
    for (int j = 0; j < 5; ++j) {
      if (gs.get_op().at(i, j) == 0) {
        empties.emplace_back(i, j);
      }
    }
  }
  auto vt = next_boards(gs.get_me());
  std::vector<std::pair<GameState, Play>> res;
  for (const auto [bd, score, merge, dir] : vt) {
    for (int i = 0; i <= merge; ++i) {
      const int num = 1 << (i+1);
      const int count = 1 << (merge-i);
      const int n = std::size(empties);
      if (count > n) continue;
      int x, y;
      for (int T = (1 << count) - 1; T < (1 << n);
          x = T & -T, y = T + x, T = (((T & ~y) / x) >> 1) | y) {
        Board nx = gs.get_op();
        std::vector<std::pair<int, int>> vp;
        for (int j = 0; j < n; ++j) {
          if ((T >> j) & 1) {
            auto [r, c] = empties[j];
            nx.at(r, c) = i+1;
            vp.emplace_back(r, c);
          }
        }
        res.emplace_back(
            GameState(nx, bd),
            Play{dir, count, i+1, vp});
      }
    }
  }
  return res;
}

int next_states_count(const GameState &gs) {
  int empties_count = 0;
  for (int i = 0; i < 5; ++i) {
    for (int j = 0; j < 5; ++j) {
      if (gs.get_op().at(i, j) == 0) {
        ++empties_count;
      }
    }
  }
  auto vt = next_boards(gs.get_me());
  int res = 0;
  for (const auto [bd, score, merge, dir] : vt) {
    for (int i = 0; i <= merge; ++i) {
      const int count = 1 << (merge-i);
      const int n = empties_count;
      if (count > n) continue;
      res += combination(n, count);
    }
  }
  return res;
}

std::pair<GameState, Play> play_index(const GameState &gs, const int index) {
  std::vector<std::pair<int, int>> empties;
  for (int i = 0; i < 5; ++i) {
    for (int j = 0; j < 5; ++j) {
      if (gs.get_op().at(i, j) == 0) {
        empties.emplace_back(i, j);
      }
    }
  }
  auto vt = next_boards(gs.get_me());
  int next_count = 0;
  for (const auto [bd, score, merge, dir] : vt) {
    for (int i = 0; i <= merge; ++i) {
      const int num = 1 << (i+1);
      const int count = 1 << (merge-i);
      const int n = std::size(empties);
      if (count > n) continue;
      if (index >= next_count + combination(n, count)) {
        next_count += combination(n, count);
        continue;
      }
      int x, y;
      for (int T = (1 << count) - 1; T < (1 << n);
          x = T & -T, y = T + x, T = (((T & ~y) / x) >> 1) | y) {
        if (index == next_count) {
          Board nx = gs.get_op();
          std::vector<std::pair<int, int>> vp;
          for (int j = 0; j < n; ++j) {
            if ((T >> j) & 1) {
              auto [r, c] = empties[j];
              nx.at(r, c) = i+1;
              vp.emplace_back(r, c);
            }
          }
          return std::make_pair(
            GameState(nx, bd),
            Play{dir, count, i+1, vp});
        } else {
          ++next_count;
        }
      }
    }
  }
  throw std::out_of_range("Out of range");
}

std::string to_string(const GameState &gs) {
  std::string res;
  res += to_string(gs.get_me());
  res += "#####\n";
  res += to_string(gs.get_op());
  return res;
}
