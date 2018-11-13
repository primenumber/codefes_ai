#include "eval.hpp"
#include <cmath>
#include <fstream>

constexpr int INF = 1000000;

double val_table[16];
double offset;

void init_eval() {
  std::ifstream ifs("table");
  for (int i = 0; i < 16; ++i) {
    ifs >> val_table[i];
  }
  ifs >> offset;
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

double sigmoid(const double x) {
  return 1.0 / (1 + std::exp(x));
}

double eval(const Board &bd) {
  if (bd.is_gameover()) return -INF;
  int sum = 0;
  for (int i = 0; i < 5; ++i) {
    for (int j = 0; j < 4; ++j) {
      int a = bd.at(i, j);
      int b = bd.at(i, j+1);
      int x = std::max(a, b);
      int n = std::min(a, b);
      sum += val_table[x - n] * (1 << x);
    }
  }
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 5; ++j) {
      int a = bd.at(i, j);
      int b = bd.at(i+1, j);
      int x = std::max(a, b);
      int n = std::min(a, b);
      sum += val_table[x - n] * (1 << x);
    }
  }
  sum += offset;
  return sigmoid(sum) * INF;
}

int eval(const GameState &gs) {
  return eval(gs.get_me()) - eval(gs.get_op());
}
