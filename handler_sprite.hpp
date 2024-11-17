#ifndef HANDLER_SPRITE_HPP
#define HANDLER_SPRITE_HPP
#include "Sprite.hpp"
void handle_running(bool& running, int& direction, bool& idle, Sprite& sprite_run, std::vector<int>& character_coords, std::vector<std::vector<int>>& block_coords);
void handle_jumping(bool& jump, bool& idle, Sprite& sprite_jump, std::vector<int>& character_coords,
std::vector<std::vector<int>>& block_coords, int& direction);
void handle_idle(bool& idle, Sprite& sprite_idle, std::vector<int>& character_coords);
void tile_block(Sprite& sprite_block, std::vector<std::vector<int>>& block_coords);
void init_character(std::vector<int>& character_coords, Sprite& sprite, Sprite& sprite_block);
void handle_gravity(std::vector<int>& character_coords, std::vector<std::vector<int>>& block_coords, bool& gravity, bool& jump);
bool collision_sprite(std::vector<int>& c, int& moveX, int& moveY, std::vector<std::vector<int>>& block_coords);

#endif
