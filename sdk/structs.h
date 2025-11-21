#pragma once
#include "../includes.h"

void draw_cornered_box(int x, int y, int w, int h, const ImColor color, int thickness)
{
    auto draw = ImGui::GetForegroundDrawList();

    int outline = thickness + 2;
    ImColor black = ImColor(0, 0, 0, 255);

    draw->AddLine(ImVec2(x, y), ImVec2(x, y + (h / 3)), black, outline);
    draw->AddLine(ImVec2(x, y), ImVec2(x + (w / 3), y), black, outline);
    draw->AddLine(ImVec2(x + w - (w / 3), y), ImVec2(x + w, y), black, outline);
    draw->AddLine(ImVec2(x + w, y), ImVec2(x + w, y + (h / 3)), black, outline);
    draw->AddLine(ImVec2(x, y + h - (h / 3)), ImVec2(x, y + h), black, outline);
    draw->AddLine(ImVec2(x, y + h), ImVec2(x + (w / 3), y + h), black, outline);
    draw->AddLine(ImVec2(x + w - (w / 3), y + h), ImVec2(x + w, y + h), black, outline);
    draw->AddLine(ImVec2(x + w, y + h - (h / 3)), ImVec2(x + w, y + h), black, outline);

    draw->AddLine(ImVec2(x, y), ImVec2(x, y + (h / 3)), color, thickness);
    draw->AddLine(ImVec2(x, y), ImVec2(x + (w / 3), y), color, thickness);
    draw->AddLine(ImVec2(x + w - (w / 3), y), ImVec2(x + w, y), color, thickness);
    draw->AddLine(ImVec2(x + w, y), ImVec2(x + w, y + (h / 3)), color, thickness);
    draw->AddLine(ImVec2(x, y + h - (h / 3)), ImVec2(x, y + h), color, thickness);
    draw->AddLine(ImVec2(x, y + h), ImVec2(x + (w / 3), y + h), color, thickness);
    draw->AddLine(ImVec2(x + w - (w / 3), y + h), ImVec2(x + w, y + h), color, thickness);
    draw->AddLine(ImVec2(x + w, y + h - (h / 3)), ImVec2(x + w, y + h), color, thickness);
}

void Box(int X, int Y, int W, int H, const ImColor color, int thickness)
{
    int outline = thickness + 2;
    ImColor black = ImColor(0, 0, 0, 255);

    ImGui::GetForegroundDrawList()->AddLine(ImVec2{ (float)X, (float)Y }, ImVec2{ (float)(X + W), (float)Y }, black, outline);
    ImGui::GetForegroundDrawList()->AddLine(ImVec2{ (float)(X + W), (float)Y }, ImVec2{ (float)(X + W), (float)(Y + H) }, black, outline);
    ImGui::GetForegroundDrawList()->AddLine(ImVec2{ (float)X, (float)(Y + H) }, ImVec2{ (float)(X + W), (float)(Y + H) }, black, outline);
    ImGui::GetForegroundDrawList()->AddLine(ImVec2{ (float)X, (float)Y }, ImVec2{ (float)X, (float)(Y + H) }, black, outline);

    ImGui::GetForegroundDrawList()->AddLine(ImVec2{ (float)X, (float)Y }, ImVec2{ (float)(X + W), (float)Y }, color, thickness);
    ImGui::GetForegroundDrawList()->AddLine(ImVec2{ (float)(X + W), (float)Y }, ImVec2{ (float)(X + W), (float)(Y + H) }, color, thickness);
    ImGui::GetForegroundDrawList()->AddLine(ImVec2{ (float)X, (float)(Y + H) }, ImVec2{ (float)(X + W), (float)(Y + H) }, color, thickness);
    ImGui::GetForegroundDrawList()->AddLine(ImVec2{ (float)X, (float)Y }, ImVec2{ (float)X, (float)(Y + H) }, color, thickness);
}

inline void DrawLine(const FVector2D& p1, const FVector2D& p2, ImU32 color, float thickness = 1.5f)
{
    ImGui::GetBackgroundDrawList()->AddLine(
        ImVec2(p1.x, p1.y),
        ImVec2(p2.x, p2.y),
        color,
        thickness
    );
}

inline void DrawHealthBar(const valhalla::sdk::player& player, float box_x, float box_y, float box_w, float box_h)
{
    if (player.health <= 0.0f || player.health > 100.0f)
        return;

    float healthPerc = player.health / 100.0f;


    float barWidth = 4.0f;                   
    float barHeight = box_h * healthPerc;    
    float x = box_x - (barWidth + 2.0f);    
    float y = box_y + (box_h - barHeight);  

    // background (black)
    ImGui::GetBackgroundDrawList()->AddRectFilled(
        ImVec2(x, box_y),
        ImVec2(x + barWidth, box_y + box_h),
        IM_COL32(0, 0, 0, 200)
    );


    ImU32 healthColor;
    if (healthPerc > 0.6f)
        healthColor = IM_COL32(0, 255, 0, 255);   
    else if (healthPerc > 0.3f)
        healthColor = IM_COL32(255, 255, 0, 255);
    else
        healthColor = IM_COL32(255, 0, 0, 255);   

    // filled health
    ImGui::GetBackgroundDrawList()->AddRectFilled(
        ImVec2(x, y),
        ImVec2(x + barWidth, y + barHeight),
        healthColor
    );
}

void draw_line(const FVector2D& start, const FVector2D& end, ImU32 color = IM_COL32(255, 0, 0, 255), float thickness = 1.0f)
{

    ImDrawList* draw_list = ImGui::GetBackgroundDrawList();
    draw_list->AddLine(
        ImVec2(start.x, start.y),
        ImVec2(end.x, end.y),
        color,
        thickness
    );
}

void aimbot(double targetX, double targetY)
{
    FVector2D head2d = { targetX, targetY };
    FVector2D target{};
    if (head2d.x != 0)
    {
        if (head2d.x > Width /2)
        {
            target.x = -(Width /2 - head2d.x);
            target.x /= settings::smoothing;
            if (target.x + Width /2 > Width /2 * 2) target.x = 0;
        }
        if (head2d.x < Width /2)
        {
            target.x = head2d.x - Width /2;
            target.x /= settings::smoothing;
            if (target.x + Width /2 < 0) target.x = 0;
        }
    }
    if (head2d.y != 0)
    {
        if (head2d.y > Height /2)
        {
            target.y = -(Height /2 - head2d.y);
            target.y /= settings::smoothing;
            if (target.y + Height /2 > Height /2 * 2) target.y = 0;
        }
        if (head2d.y < Height /2)
        {
            target.y = head2d.y - Height /2;
            target.y /= settings::smoothing;
            if (target.y + Height /2 < 0) target.y = 0;
        }
    }

    km::mouse_mouse(static_cast<int>(target.x), static_cast<int>(target.y), 0);
}

void Silent(FVector2D targetPos)
{
    FVector2D screenCenter = { Width / 2.0f, Height / 2.0f };
    FVector2D relativePos = { targetPos.x - screenCenter.x, targetPos.y - screenCenter.y };

    // Move fully to the head
    km::mouse_mouse(relativePos.x * 2.20003, relativePos.y * 2.20003, 0);

    // Click
    km::mouse_mouse(0, 0, 0x0001); 
    std::this_thread::sleep_for(std::chrono::milliseconds(20)); 
    km::mouse_mouse(0, 0, 0x0002); 

    // Move back to center
    km::mouse_mouse(-relativePos.x, -relativePos.y, 0);
}


void RiceHat(const fvector& head, valhalla::sdk::cache& cache, valhalla::sdk::camera& cam, float radius, float height, int segments, float tick) {
    fvector tip = { head.x, head.y, head.z + height };

    std::vector<ImVec2> basePoints2D;

    for (int i = 0; i < segments; ++i) {
        float angle = (2 * M_PI / segments) * i;
        float x = cosf(angle) * radius;
        float y = sinf(angle) * radius;

        fvector basePoint3D = { head.x + x, head.y + y, head.z + 5 };
        FVector2D screenBase = cam.w2s(cache, basePoint3D);
        basePoints2D.push_back(ImVec2(screenBase.x, screenBase.y));
    }

    FVector2D screenTip = cam.w2s(cache, tip);
    ImVec2 tip2D = ImVec2(screenTip.x, screenTip.y);

    auto draw_list = ImGui::GetForegroundDrawList();

    ImU32 cyan = ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f));  
    ImU32 black = ImGui::GetColorU32(ImVec4(0, 0, 0, 1.0f));          
    float outlineThickness = tick + 2.0f;
    for (int i = 0; i < segments; ++i) {
        draw_list->AddLine(tip2D, basePoints2D[i], black, outlineThickness);
        draw_list->AddLine(basePoints2D[i], basePoints2D[(i + 1) % segments], black, outlineThickness);
    }
    for (int i = 0; i < segments; ++i) {
        draw_list->AddLine(tip2D, basePoints2D[i], cyan, tick);
        draw_list->AddLine(basePoints2D[i], basePoints2D[(i + 1) % segments], cyan, tick);
    }
}

ImVec2 FTITWOD(FVector2D a) 
{
    auto b = ImVec2{ static_cast<float>(a.x),  static_cast<float>(a.y) };
    return b;
}


void draw3dbox( fvector base, fvector top_reach,  ImColor col, float wide, valhalla::sdk::camera& c, valhalla::sdk::cache& cache)
{
    fvector t = fvector(base.x + wide, base.y + wide, base.z);
    fvector tt = fvector(base.x + wide, base.y - wide, base.z);
    fvector ttt = fvector(base.x - wide, base.y + wide, base.z);
    fvector tttt = fvector(base.x - wide, base.y - wide, base.z);


    fvector t1 = fvector(top_reach.x + wide, top_reach.y + wide, top_reach.z);
    fvector t2 = fvector(top_reach.x + wide, top_reach.y - wide, top_reach.z);
    fvector t3 = fvector(top_reach.x - wide, top_reach.y + wide, top_reach.z);
    fvector t4 = fvector(top_reach.x - wide, top_reach.y - wide, top_reach.z);


    FVector2D bottom_rect_1 = c.w2s(cache, t);
    FVector2D bottom_rect_2 = c.w2s(cache, tt);
    FVector2D  bottom_rect_3 = c.w2s(cache, ttt);
    FVector2D bottom_rect_4 = c.w2s(cache, tttt);


    FVector2D top_rect_1 = c.w2s(cache, t1);
    FVector2D top_rect_2 = c.w2s(cache, t2);
    FVector2D  top_rect_3 = c.w2s(cache, t3);
    FVector2D top_rect_4 = c.w2s(cache, t4);


    ImGui::GetForegroundDrawList()->AddLine(ImVec2(bottom_rect_1.x, bottom_rect_1.y), ImVec2(bottom_rect_2.x, bottom_rect_2.y), col, 1.5f);
    ImGui::GetForegroundDrawList()->AddLine(ImVec2(bottom_rect_2.x, bottom_rect_2.y), ImVec2(bottom_rect_4.x, bottom_rect_4.y), col, 1.5f);
    ImGui::GetForegroundDrawList()->AddLine(ImVec2(bottom_rect_4.x, bottom_rect_4.y), ImVec2(bottom_rect_3.x, bottom_rect_3.y), col, 1.5f);
    ImGui::GetForegroundDrawList()->AddLine(ImVec2(bottom_rect_3.x, bottom_rect_3.y), ImVec2(bottom_rect_1.x, bottom_rect_1.y), col, 1.5f);

    ImGui::GetForegroundDrawList()->AddLine(ImVec2(top_rect_1.x, top_rect_1.y), ImVec2(top_rect_2.x, top_rect_2.y), col, 1.5f);
    ImGui::GetForegroundDrawList()->AddLine(ImVec2(top_rect_2.x, top_rect_2.y), ImVec2(top_rect_4.x, top_rect_4.y), col, 1.5f);
    ImGui::GetForegroundDrawList()->AddLine(ImVec2(top_rect_4.x, top_rect_4.y), ImVec2(top_rect_3.x, top_rect_3.y), col, 1.5f);
    ImGui::GetForegroundDrawList()->AddLine(ImVec2(top_rect_3.x, top_rect_3.y), ImVec2(top_rect_1.x, top_rect_1.y), col, 1.5f);

    //render connection lines
    ImGui::GetForegroundDrawList()->AddLine(ImVec2(bottom_rect_1.x, bottom_rect_1.y), ImVec2(top_rect_1.x, top_rect_1.y), col, 1.5f);
    ImGui::GetForegroundDrawList()->AddLine(ImVec2(bottom_rect_2.x, bottom_rect_2.y), ImVec2(top_rect_2.x, top_rect_2.y), col, 1.5f);
    ImGui::GetForegroundDrawList()->AddLine(ImVec2(bottom_rect_3.x, bottom_rect_3.y), ImVec2(top_rect_3.x, top_rect_3.y), col, 1.5f);
    ImGui::GetForegroundDrawList()->AddLine(ImVec2(bottom_rect_4.x, bottom_rect_4.y), ImVec2(top_rect_4.x, top_rect_4.y), col, 1.5f);
}

struct bh
{
    fvector pos;
    double radius;
    double r_s; 
};

void black_hole_3d(fvector pos, float radius, valhalla::sdk::camera& c, valhalla::sdk::cache& cache)
{
    auto draw_list = ImGui::GetForegroundDrawList();
    const int num_segments = 128;

    // --- Event Horizon (3D Sphere Core) ---
    FVector2D center = c.w2s(cache, pos);
    if (center.x == 0 && center.y == 0)
        return;

    const int sphere_layers = 40;
    for (int i = 0; i < sphere_layers; i++)
    {
        float t = (float)i / (float)(sphere_layers - 1);

        // Sphere curvature along Z
        float z_curve = cosf(t * IM_PI);
        float layer_radius = radius * (0.9f + 0.1f * z_curve);

        // Subtle dark shading gradient
        int brightness = (int)(20 + 40 * (1.0f - fabsf(z_curve)));
        int alpha = (int)(200 - t * 140);

        ImU32 color = IM_COL32(brightness, brightness, brightness, alpha);

        // Slight vertical offset for fake curvature
        float y_offset = (t - 0.5f) * radius * 0.4f;

        draw_list->AddCircleFilled(
            ImVec2(center.x, center.y + y_offset),
            layer_radius,
            color,
            64
        );
    }

    // --- Accretion Disk / Halo ---
    float disk_inner = radius * 1.05f;
    float disk_outer = radius * 1.35f;
    int glow_layers = 12;
    float tilt = 0.5f;

    for (int layer = 0; layer < glow_layers; layer++)
    {
        float t = (float)layer / (float)(glow_layers - 1);
        float r = disk_inner + t * (disk_outer - disk_inner);

        ImU32 col;
        if (t < 0.3f)
            col = IM_COL32(255, 255, 255, 200 - t * 80);
        else if (t < 0.6f)
            col = IM_COL32(255, 220, 120, 180 - t * 60);
        else
            col = IM_COL32(200, 100, 255, 150 - t * 50);

        std::vector<ImVec2> ring_points;
        ring_points.reserve(num_segments);

        for (int i = 0; i < num_segments; i++)
        {
            float theta = (2.0f * IM_PI * i) / num_segments;

            fvector world_point = fvector(
                pos.x + cosf(theta) * r,
                pos.y + sinf(theta) * r,
                pos.z + sinf(theta) * radius * tilt
            );

            FVector2D screen = c.w2s(cache, world_point);
            if (screen.x != 0 && screen.y != 0)
                ring_points.emplace_back(screen.x, screen.y);
        }

        if (ring_points.size() > 2)
        {
            draw_list->AddPolyline(
                ring_points.data(),
                ring_points.size(),
                col,
                true,
                2.0f
            );
        }
    }
}