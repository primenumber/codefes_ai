#include "board.hpp"

Board::Board(const int r, const int c) {
  for (int i = 0; i < 5; ++i) {
    for (int j = 0; j < 5; ++j) {
      data[i][j] = 0;
    }
  }
  data[r][c] = 1;
}

line::Line Board::row(const int r) const {
  line::Line res;
  std::copy(data[r], data[r] + 5, std::begin(res));
  return res;
}

line::Line Board::col(const int c) const {
  line::Line res;
  for (int i = 0; i < 5; ++i) {
    res[i] = data[i][c];
  }
  return res;
}

uint8_t Board::at(const int r, const int c) const {
  return data[r][c];
}

uint8_t &Board::at(const int r, const int c) {
  return data[r][c];
}

bool Board::is_gameover() const {
  for (int i = 0; i < 4; ++i) {
    if (is_movable(*this, static_cast<Dir>(i))) {
      return false;
    }
  }
  return true;
}

std::tuple<Board, int, int> move(const Board &bd, Dir dir) {
  int score = 0;
  int merge = 0;
  Board res;
  switch (dir) {
    case Dir::L:
      for (int i = 0; i < 5; ++i) {
        line::Line l = bd.row(i);
        line::Line n;
        int s, m;
        std::tie(n, s, m) = line::move(l);
        for (int j = 0; j < 5; ++j) {
          res.at(i, j) = n[j];
        }
        score += s;
        merge += m;
      }
      break;
    case Dir::U:
      for (int i = 0; i < 5; ++i) {
        line::Line l = bd.col(i);
        line::Line n;
        int s, m;
        std::tie(n, s, m) = line::move(l);
        for (int j = 0; j < 5; ++j) {
          res.at(j, i) = n[j];
        }
        score += s;
        merge += m;
      }
      break;
    case Dir::R:
      for (int i = 0; i < 5; ++i) {
        line::Line l = line::reverse(bd.row(i));
        line::Line n;
        int s, m;
        std::tie(n, s, m) = line::move(l);
        for (int j = 0; j < 5; ++j) {
          res.at(i, 4-j) = n[j];
        }
        score += s;
        merge += m;
      }
      break;
    case Dir::D:
      for (int i = 0; i < 5; ++i) {
        line::Line l = line::reverse(bd.col(i));
        line::Line n;
        int s, m;
        std::tie(n, s, m) = line::move(l);
        for (int j = 0; j < 5; ++j) {
          res.at(4-j, i) = n[j];
        }
        score += s;
        merge += m;
      }
      break;
  }
  return std::make_tuple(res, score, merge);
}

bool is_movable(const Board &bd, Dir dir) {
  switch (dir) {
    case Dir::L:
      for (int i = 0; i < 5; ++i) {
        line::Line l = bd.row(i);
        if (line::is_movable(l))
          return true;
      }
      break;
    case Dir::U:
      for (int i = 0; i < 5; ++i) {
        line::Line l = bd.col(i);
        if (line::is_movable(l))
          return true;
      }
      break;
    case Dir::R:
      for (int i = 0; i < 5; ++i) {
        line::Line l = line::reverse(bd.row(i));
        if (line::is_movable(l))
          return true;
      }
      break;
    case Dir::D:
      for (int i = 0; i < 5; ++i) {
        line::Line l = line::reverse(bd.col(i));
        if (line::is_movable(l))
          return true;
      }
      break;
  }
  return false;
}

std::vector<std::tuple<Board, int, int, Dir>> next_boards(const Board &bd) {
  std::vector<std::tuple<Board, int, int, Dir>> res;
  for (int i = 0; i < 4; ++i) {
    if (is_movable(bd, static_cast<Dir>(i))) {
      auto [next, score, merge] = move(bd, static_cast<Dir>(i));
      res.emplace_back(next, score, merge, static_cast<Dir>(i));
    }
  }
  return res;
}

std::string to_string_hex(int n) {
  std::string res;
  if (n < 10) {
    res += (char)(n + '0');
  } else {
    res += (char)(n - 10 + 'A');
  }
  return res;
}

std::string to_string(const Board &bd) {
  std::string res;
  for (int i = 0; i < 5; ++i) {
    for (int j = 0; j < 5; ++j) {
      if (bd.at(i, j)) {
        res += to_string_hex(bd.at(i, j));
      } else {
        res += ".";
      }
    }
    res += "\n";
  }
  return res;
}
