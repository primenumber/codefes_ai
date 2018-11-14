#pragma once
#include <random>
#include "gamestate.hpp"

std::pair<int, Play> search(const GameState &gs, std::mt19937 &mt,
    const int tl, const bool limit = true);
