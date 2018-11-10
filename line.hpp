#pragma once
#include <array>
#include <tuple>

namespace line {

void init();

using Line = std::array<uint8_t, 5>;

Line reverse(const Line &line);
std::tuple<Line, int, int> move(const Line &line);
bool is_movable(const Line &line);

} // namespace line
