#pragma once
#include <array>
#include <vector>
#include <string>
#include "line.hpp"

class Board {
 public:
  Board() = default;
  Board(const int r, const int c);
  Board(const Board &) = default;
  Board(Board &&) = default;
  Board &operator=(const Board &) = default;
  Board &operator=(Board &&) = default;
  line::Line row(const int r) const;
  line::Line col(const int c) const;
  uint8_t at(const int r, const int c) const;
  uint8_t &at(const int r, const int c);
  bool is_gameover() const;
 private:
  uint8_t data[5][5];
};

enum class Dir {
  L,
  U,
  R,
  D
};

std::tuple<Board, int, int> move(const Board &bd, Dir dir);
bool is_movable(const Board &bd, Dir dir);

std::vector<std::tuple<Board, int, int, Dir>> next_boards(const Board &bd); 

std::string to_string(const Board &bd);
