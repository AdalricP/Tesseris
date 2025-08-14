#ifndef RENDERER_H
#define RENDERER_H

#include <SDL.h>
#include "vector.h"
#include "matrix.h"

void renderer_draw_line(SDL_Renderer* renderer, Vec2 start, Vec2 end);
void renderer_draw_cube(SDL_Renderer* renderer, Mat4 view_proj, Vec3* vertices, int (*edges)[2], int edge_count);
Vec2 renderer_project_3d_to_2d(Vec3 point_3d, Mat4 view_proj);

#endif 