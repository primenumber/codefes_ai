#include "utils.hpp"
#include <algorithm>

int comb[26][26];

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
}

int combination(const int n, const int k) {
  return comb[n][k];
}
