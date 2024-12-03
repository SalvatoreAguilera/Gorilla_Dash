#ifndef PLATFORMS_H
#define PLATFORMS_H

#include <X11/Xlib.h>
#include <functional>
#include <vector>
#include <unordered_set>
#include "Sprite.hpp"

void render_score();
void render_end_screen();
extern std::function<void(std::vector<std::vector<int>>&, int&, int&, int, int)> getCoords;
extern std::function<int(int, int)> randNum;
extern std::function<void(std::vector<std::vector<int>>&, Sprite&)> moveRight;
extern std::function<void(std::unordered_set<int>&, std::vector<std::vector<int>>&, std::unordered_map<int, int>&)> outOfBounds;
extern std::function<void(const std::vector<int>&, std::unordered_map<int, int>&)> mapDelete;
extern std::function<void(const std::vector<std::pair<int, int>>&, std::unordered_map<int, int>&)> mapInsert;

#endif
