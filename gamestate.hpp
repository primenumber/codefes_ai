#pragma once
#include "board.hpp"

class GameState {
 public:
  GameState(int rm, int cm, int ro, int co)
    : me(rm, cm), op(ro, co) {}
  GameState(const Board &me, const Board &op)
    : me(me), op(op) {}
  GameState(const GameState &) = default;
  GameState(GameState &&) = default;
  GameState &operator=(const GameState &) = default;
  GameState &operator=(GameState &&) = default;
  const Board &get_me() const { return me; }
  const Board &get_op() const { return op; }
  bool is_gameover() const;
 private:
  Board me, op;
};

struct Play {
  Dir dir;
  int m, v;
  std::vector<std::pair<int, int>> vp;
};

std::vector<std::pair<GameState, Play>> next_states(const GameState &gs);
int next_states_count(const GameState &gs);
std::pair<GameState, Play> play_index(const GameState &gs, const int index);

std::string to_string(const GameState &gs);
