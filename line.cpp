#include "line.hpp"
#include <iostream>
#include <vector>

namespace line {

constexpr int pack_bit_len = 4;

std::vector<uint32_t> move_table;
std::vector<int> score_table;
std::vector<int> merge_table;
std::vector<bool> is_movable_table;

uint32_t encode(const Line &line) {
  uint32_t index = 0;
  for (int i = 0; i < 5; ++i) {
    index |= line[i] << (i * pack_bit_len);
  }
  return index;
}

Line decode(const uint32_t index) {
  Line res;
  for (int i = 0; i < 5; ++i) {
    res[i] = (index >> (i*pack_bit_len)) & ((1 << pack_bit_len) - 1);
  }
  return res;
}

std::tuple<Line, int, int, bool> move_impl(const Line &line) {
  int score = 0;
  int merge = 0;
  bool movable = false;
  Line res = line;
  for (int i = 0; i < 5; ++i) {
    int j;
    for (j = i; j < 5; ++j) {
      if (res[j]) {
        res[i] = res[j];
        if (i != j) {
          res[j] = 0;
          movable = true;
        }
        break;
      }
    }
    if (res[i] == 0) continue;
    for (++j; j < 5; ++j) {
      if (res[i] == res[j]) {
        ++res[i];
        res[j] = 0;
        score += 1 << res[i];
        ++merge;
        movable = true;
        break;
      } else if (res[j]) {
        break;
      }
    }
  }
  return std::make_tuple(res, score, merge, movable);
}

void init() {
  size_t len = 1 << 5*pack_bit_len;
  move_table.resize(len);
  score_table.resize(len);
  merge_table.resize(len);
  is_movable_table.resize(len);
  for (uint32_t i = 0; i < len; ++i) {
    Line line;
    int score, merge;
    bool movable;
    std::tie(line, score, merge, movable) = move_impl(decode(i));
    move_table[i] = encode(line);
    score_table[i] = score;
    merge_table[i] = merge;
    is_movable_table[i] = movable;
  }
}

Line reverse(const Line &line) {
  Line res;
  for (int i = 0; i < 5; ++i) {
    res[i] = line[4-i];
  }
  return res;
}

std::tuple<Line, int, int> move(const Line &line) {
  uint32_t index = encode(line);
  return std::make_tuple(
      decode(move_table[index]),
      score_table[index],
      merge_table[index]);
}

bool is_movable(const Line &line) {
  return is_movable_table[encode(line)];
}

} // namespace line
