#include "eval.hpp"
#include <fstream>

constexpr int INF = 1000000;

float val_table_me[16][16];
float val_table_op[16][16];

void eval_init() {
  std::ifstream ifs("table");
  for (int i = 0; i < 16; ++i) {
    for (int j = 0; j <= i; ++j) {
      ifs >> val_table_me[i][j];
    }
  }
  for (int i = 0; i < 16; ++i) {
    for (int j = 0; j <= i; ++j) {
      ifs >> val_table_op[i][j];
    }
  }
}

bool is_stable(const line::Line &line) {
  bool ok = true;
  for (int i = 0; i < 5; ++i) {
    if (line[i] == 0) ok = false;
  }
  for (int i = 0; i < 4; ++i) {
    if (line[i] == line[i+1]) ok = false;
  }
  return ok;
}

int stability(const Board &bd) {
  int r = 0, c = 0;
  int mx = 0;
  for (int i = 0; i < 5; ++i) {
    for (int j = 0; j < 5; ++j) {
      if (bd.at(i, j) > mx) {
        mx = bd.at(i, j);
        r = i;
        c = j;
      }
    }
  }
  bool is_er = r == 0 || r == 4;
  bool is_ec = c == 0 || c == 4;
  int score = 0;
  if (is_er && is_ec) {
    score += 1 << (mx+1);
  } else if (is_er || is_ec) {
    score += 1 << mx;
  }
  if (r == 0) {
    if (is_stable(bd.row(0))) {
      score += 1 << (mx+1);
    }
  }
  if (r == 4) {
    if (is_stable(bd.row(4))) {
      score += 1 << (mx+1);
    }
  }
  if (c == 0) {
    if (is_stable(bd.col(0))) {
      score += 1 << (mx+1);
    }
  }
  if (c == 4) {
    if (is_stable(bd.col(4))) {
      score += 1 << (mx+1);
    }
  }
  return score;
}

int eval_impl(const Board &bd) {
  if (bd.is_gameover()) return -INF;
  int sum = 0;
  for (int i = 0; i < 5; ++i) {
    for (int j = 0; j < 4; ++j) {
      int a = bd.at(i, j);
      int b = bd.at(i, j+1);
      int x = std::max(a, b);
      int y = std::min(a, b);
      sum += val_table_me[x][y];
    }
  }
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 5; ++j) {
      int a = bd.at(i, j);
      int b = bd.at(i+1, j);
      int x = std::max(a, b);
      int y = std::min(a, b);
      sum += val_table_me[x][y];
    }
  }
  return sum;
}

int eval_impl_op(const Board &bd) {
  if (bd.is_gameover()) return -INF;
  int sum = 0;
  for (int i = 0; i < 5; ++i) {
    for (int j = 0; j < 4; ++j) {
      int a = bd.at(i, j);
      int b = bd.at(i, j+1);
      int x = std::max(a, b);
      int y = std::min(a, b);
      sum += val_table_op[x][y];
    }
  }
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 5; ++j) {
      int a = bd.at(i, j);
      int b = bd.at(i+1, j);
      int x = std::max(a, b);
      int y = std::min(a, b);
      sum += val_table_op[x][y];
    }
  }
  return sum;
}

int eval_diff_impl(const Board &bd) {
  int sum = 0;
  for (int i = 0; i < 5; ++i) {
    for (int j = 0; j < 4; ++j) {
      int a = bd.at(i, j);
      int b = bd.at(i, j+1);
      int x = std::max(a, b);
      int y = std::min(a, b);
      sum -= (1 << x) - (1 << y);
    }
  }
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 5; ++j) {
      int a = bd.at(i, j);
      int b = bd.at(i+1, j);
      int x = std::max(a, b);
      int y = std::min(a, b);
      sum -= (1 << x) - (1 << y);
    }
  }
  return sum;
}

int eval(const GameState &gs) {
  return eval_impl(gs.get_me()) + eval_impl_op(gs.get_op());
}

int eval_diff(const GameState &gs) {
  if (gs.get_me().is_gameover()) return -INF;
  if (gs.get_op().is_gameover()) return INF;
  return eval_diff_impl(gs.get_me()) - eval_diff_impl(gs.get_op());
}
