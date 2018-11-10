#include "utils.hpp"
#include <algorithm>
#include <vector>

int comb[26][26];
std::vector<std::vector<std::vector<uint32_t>>> table;

void init() {
  for (int i = 0; i <= 25; ++i) {
    for (int j = 0; j <= i; ++j) {
      int k = std::min(j, i - j);
      int64_t res = 1;
      for (int l = 1; l <= k; ++l) {
        res *= i - l + 1;
        res /= l;
      }
      comb[i][j] = res;
    }
  }
  table.resize(25);
  for (int i = 1; i < 25; ++i) {
    table[i].resize(i+1);
    for (int j = 1; j <= i; ++j) {
      table[i][j].resize(comb[i][j]);
      int x, y;
      int index = 0;
      for (int T = (1 << j) - 1; T < (1 << i);
          x = T & -T, y = T + x, T = (((T & ~y) / x) >> 1) | y) {
        table[i][j][index] = T;
        ++index;
      }
    }
  }
}

int combination(const int n, const int k) {
  return comb[n][k];
}

uint32_t get_combination(const int n, const int k, const int index) {
  return table[n][k][index];
}
