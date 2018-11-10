#include "eval.hpp"

constexpr int INF = 1000000;

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
      //if (bd.at(i, j) && bd.at(i, j+1)) {
        sum += abs((1 << bd.at(i, j)) - (1 << bd.at(i, j+1)));
      //}
    }
  }
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 5; ++j) {
      //if (bd.at(i, j) && bd.at(i+1, j)) {
        sum += abs((1 << bd.at(i, j)) - (1 << bd.at(i+1, j)));
      //}
    }
  }
  sum += stability(bd);
  return -sum;
}

int eval(const GameState &gs) {
  return eval_impl(gs.get_me()) - eval_impl(gs.get_op());
}
