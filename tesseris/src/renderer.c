#include "renderer.h"

void renderer_draw_line(SDL_Renderer* renderer, Vec2 start, Vec2 end) {
    SDL_RenderDrawLine(renderer, (int)start.x, (int)start.y, (int)end.x, (int)end.y);
}

Vec2 renderer_project_3d_to_2d(Vec3 point_3d, Mat4 view_proj) {
    Vec4 point_4d = {point_3d.x, point_3d.y, point_3d.z, 1.0f};
    Vec4 transformed = mat4_mul_vec4(view_proj, point_4d);
    if (transformed.w != 0.0f) {
        transformed.x /= transformed.w;
        transformed.y /= transformed.w;
    }
    Vec2 screen_pos = {
        (transformed.x + 1.0f) * 800 / 2.0f,
        (1.0f - transformed.y) * 600 / 2.0f
    };
    return screen_pos;
}

void renderer_draw_cube(SDL_Renderer* renderer, Mat4 view_proj, Vec3* vertices, int (*edges)[2], int edge_count) {
    for (int i = 0; i < edge_count; i++) {
        Vec3 start_3d = vertices[edges[i][0]];
        Vec3 end_3d = vertices[edges[i][1]];
        Vec2 start_2d = renderer_project_3d_to_2d(start_3d, view_proj);
        Vec2 end_2d = renderer_project_3d_to_2d(end_3d, view_proj);
        renderer_draw_line(renderer, start_2d, end_2d);
    }
} 