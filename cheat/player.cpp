#include "player.h"
#include "../sdk/structs.h"
#include <km/kernel.h>
#include <mutex>


struct FLinearColor {
	float R; // 0x00(0x04)
	float G; // 0x04(0x04)
	float B; // 0x08(0x04)
	float A; // 0x0c(0x04)
};


enum class EAresOutlineMode : UINT8 {
	None = 0,
	Outline = 1,
	Block = 2,
	Enemy = 3,
	AlwaysOutline = 4,
	AlwaysEnemy = 5,
	EAresOutlineMode_MAX = 6
};


namespace valhalla::players
{
	std::vector<valhalla::sdk::player> Pplayer;
	std::mutex cacheMutex;
#define Ccache valhalla::cache::ccache
#define cache *valhalla::cache::ccache

	void player_loop()
	{
		std::vector<valhalla::sdk::player> tc;
		tc.reserve(128);
		while (true)
		{
			tc.clear();
			tc.reserve((cache).actor_count);
			for (int x = 0; x < (cache).actor_count; x++)
			{
				uintptr_t actor = km::RPM<uintptr_t>((cache).actor_array + (x * 0x8));
				if (!actor) continue;

				INT32 unique_id = km::RPM<INT32>(actor + 0x38);
				if (unique_id != 18743553) continue;

				bool ally = km::RPM<bool>(actor + 0x0F29);
				if (ally) continue;

				uintptr_t damage_handler = km::RPM<uintptr_t>(actor + offsets::damage_handler);
				if (!damage_handler) continue;

				float health = km::RPM<float>(damage_handler + offsets::health);
				if (health <= 0) continue;

				uintptr_t mesh = km::RPM<uintptr_t>(actor + offsets::mesh);
				if (!mesh) continue;

				uintptr_t bone_array = km::RPM<uintptr_t>(mesh + offsets::bone_array);
				if (!bone_array)
					bone_array = km::RPM<uintptr_t>(mesh + offsets::bone_array_cache);
				if (!bone_array) continue;

				uintptr_t bone_count = km::RPM<int>(mesh + 0x740 + 0x8);

				tc.emplace_back(actor, mesh, bone_count, damage_handler, health, bone_array);
			}
			{
				std::lock_guard<std::mutex> lock(cacheMutex);
				Pplayer.swap(tc);
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}

	valhalla::sdk::camera* camera = new valhalla::sdk::camera();
	bool flicked = false;

	void hack_loop()
	{
		ImColor skelcolor = ImColor(settings::skeletoncolor[0], settings::skeletoncolor[1], settings::skeletoncolor[2]);
		ImColor iskelcolor = ImColor(settings::iskeletoncolor[0], settings::iskeletoncolor[1], settings::iskeletoncolor[2]);
		ImColor espcolor = ImColor(settings::espcolor[0], settings::espcolor[1], settings::espcolor[2]);
		ImColor iespcolor = ImColor(settings::iespcolor[0], settings::iespcolor[1], settings::iespcolor[2]);

		valhalla::sdk::player best = {};
		float distanc = FLT_MAX;

		std::vector<valhalla::sdk::player> snapshot;
		{
			std::lock_guard<std::mutex> lock(cacheMutex);
			snapshot = Pplayer;
		}
		for (auto& player : snapshot)
		{

			player.bone_pos.bone_head = player.get_bone(player.bone_ids.head);
			player.bone_pos.bone_bottom = player.get_bone(player.bone_ids.bottom);

			FVector2D head = camera->w2s(cache, player.bone_pos.bone_head);
			FVector2D bottom = camera->w2s(cache, player.bone_pos.bone_bottom);
			float box_height = abs(head.y - bottom.y);
			float box_width = box_height * 0.50f;
			double distance = player.get_dist(cache);

			if (player.bone_pos.bone_head == fvector(0, 0, 0)) continue;
			if (player.IsVisible() && distance > settings::espvisdistance) continue;
			if (!player.IsVisible() && distance > settings::espinvisdistance) continue;

			if (settings::distance)
			{
				ImU32 color = ImColor(122, 122, 122);
				ImDrawList* draw = ImGui::GetForegroundDrawList();
				float diamond_size = 4.0f;
				ImVec2 diamond_center(head.x, head.y - 10);

				ImVec2 top = ImVec2(diamond_center.x, diamond_center.y - diamond_size);
				ImVec2 right = ImVec2(diamond_center.x + diamond_size, diamond_center.y);
				ImVec2 bottom = ImVec2(diamond_center.x, diamond_center.y + diamond_size);
				ImVec2 left = ImVec2(diamond_center.x - diamond_size, diamond_center.y);

				draw->AddLine(top, right, color, 1.0f);
				draw->AddLine(right, bottom, color, 1.0f);
				draw->AddLine(bottom, left, color, 1.0f);
				draw->AddLine(left, top, color, 1.0f);

				std::string gender;

				if (player.bone_count == 103)
				{
					gender = "npc";
				}
				else if (player.bone_count == 104)
				{
					gender = "boy";
				}
				else if (player.bone_count == 101)
				{
					gender = "girl";
				}

				// draw distance text next to diamond
				std::string text = std::to_string((int)distance) + "m" + " | " + gender;
				ImVec2 text_pos(diamond_center.x + diamond_size + 2.0f,
					diamond_center.y - ImGui::CalcTextSize(text.c_str()).y / 2.0f);
				draw->AddText(text_pos, color, text.c_str());
			}

			if (settings::oofarrows)
			{
				float screen_w = ImGui::GetIO().DisplaySize.x;
				float screen_h = ImGui::GetIO().DisplaySize.y;
				ImVec2 center(screen_w / 2, screen_h / 2);

				// player head position in screen space
				FVector2D head2D = camera->w2s(cache, player.bone_pos.bone_head);

				// off-screen check
				bool offscreen = head2D.x < 0 || head2D.x > screen_w || head2D.y < 0 || head2D.y > screen_h;

				if (offscreen)
				{
					float dx = head2D.x - center.x;
					float dy = head2D.y - center.y;
					float angle = atan2f(dy, dx);

					// arrow parameters (small & near center)
					float arrow_distance = 50.0f; // pixels from screen center
					float arrow_size = 10.0f;     // tiny arrow tip

					// arrow tip position
					ImVec2 tip(
						center.x + cosf(angle) * arrow_distance,
						center.y + sinf(angle) * arrow_distance
					);

					// triangle points
					ImVec2 left(
						tip.x + cosf(angle + 3.14159f * 0.75f) * arrow_size,
						tip.y + sinf(angle + 3.14159f * 0.75f) * arrow_size
					);
					ImVec2 right(
						tip.x + cosf(angle - 3.14159f * 0.75f) * arrow_size,
						tip.y + sinf(angle - 3.14159f * 0.75f) * arrow_size
					);

					// draw arrow
					ImU32 color = player.IsVisible() ? espcolor : iespcolor;

					ImGui::GetForegroundDrawList()->AddTriangleFilled(tip, left, right, color);
				}
			}

			if (settings::box)
			{
				if (player.IsVisible())
				{
					draw_cornered_box(head.x - (box_width / 2), head.y, box_width, box_height + 4.f, espcolor, 1);
				}
				else
				{
					draw_cornered_box(head.x - (box_width / 2), head.y, box_width, box_height + 4.f, iespcolor, 1);
				}
			}

			if (settings::sealesp)
			{
				float seal_size = box_height * 0.3f; 
				ImVec2 seal_min(
					head.x - seal_size / 2.0f,      
					head.y - seal_size - 4.0f       
				);
				ImVec2 seal_max(
					head.x + seal_size / 2.0f,      
					head.y - 4.0f
				);

				ImGui::GetForegroundDrawList()->AddImage(yoseal, seal_min, seal_max, ImVec2(0, 0), ImVec2(1, 1));
			}

			if (settings::tdbox)
			{
				fvector head = player.bone_pos.bone_head;

				head.z += 20;

				if (player.IsVisible())
				{
					draw3dbox(player.bone_pos.bone_bottom, head, espcolor, 38.0f, *camera, cache);
				}
				else
				{
					draw3dbox(player.bone_pos.bone_bottom, head, espcolor, 38.0f, *camera, cache);
				}
			}

			if (settings::fullbox)
			{
				if (player.IsVisible())
				{
					Box(head.x - (box_width / 2), head.y, box_width, box_height + 4.f, espcolor, 1);
				}
				else
				{
					Box(head.x - (box_width / 2), head.y, box_width, box_height + 4.f, iespcolor, 1);
				}
			}

			if (settings::healthbar)
			{
				float box_h = bottom.y - head.y;
				float box_w = box_h / 2.5f;
				float box_x = head.x - box_w / 2.0f;
				float box_y = head.y;
				float healthbar_offset = 6.0f;
				float healthbar_x = box_x - healthbar_offset;

				DrawHealthBar(player, healthbar_x, box_y, box_w, box_h);
			}

			if (settings::headdot)
			{
				if (player.IsVisible())
				{

					ImGui::GetForegroundDrawList()->AddCircle(ImVec2(head.x, head.y),
						2.0f,
						espcolor,
						12,
						1.5f);
				}
				else
				{
					ImGui::GetForegroundDrawList()->AddCircle(ImVec2(head.x, head.y),
						2.0f,
						iespcolor,
						12,
						1.5f);
				}
			}

			if (settings::chinahat)
			{
				RiceHat(player.bone_pos.bone_head, cache, *camera, 30.f, 20.f, 16, 1.f);
			}

			if (settings::chams)
			{

			}

			if (settings::blackhole)
				black_hole_3d(player.bone_pos.bone_head, 15.f, *camera, cache);


			if (settings::skeleton)
			{
				player.set_bones();
				player.bone_pos.bone_neck = player.get_bone(player.bone_ids.neck);
				player.bone_pos.bone_chest = player.get_bone(player.bone_ids.chest);
				player.bone_pos.bone_pelvis = player.get_bone(player.bone_ids.pelvis);

				player.bone_pos.bone_rshoulder = player.get_bone(player.bone_ids.rshoulder);
				player.bone_pos.bone_relbow = player.get_bone(player.bone_ids.relbow);
				player.bone_pos.bone_rhand = player.get_bone(player.bone_ids.rhand);

				player.bone_pos.bone_lshoulder = player.get_bone(player.bone_ids.lshoulder);
				player.bone_pos.bone_lelbow = player.get_bone(player.bone_ids.lelbow);
				player.bone_pos.bone_lhand = player.get_bone(player.bone_ids.lhand);

				player.bone_pos.bone_rthigh = player.get_bone(player.bone_ids.rthigh);
				player.bone_pos.bone_rknee = player.get_bone(player.bone_ids.rknee);
				player.bone_pos.bone_rfoot = player.get_bone(player.bone_ids.rfoot);

				player.bone_pos.bone_lthigh = player.get_bone(player.bone_ids.lthigh);
				player.bone_pos.bone_lknee = player.get_bone(player.bone_ids.lknee);
				player.bone_pos.bone_lfoot = player.get_bone(player.bone_ids.lfoot);

				FVector2D neck = camera->w2s(cache, player.bone_pos.bone_neck);
				FVector2D chest = camera->w2s(cache, player.bone_pos.bone_chest);
				FVector2D pelvis = camera->w2s(cache, player.bone_pos.bone_pelvis);

				FVector2D rshoulder = camera->w2s(cache, player.bone_pos.bone_rshoulder);
				FVector2D relbow = camera->w2s(cache, player.bone_pos.bone_relbow);
				FVector2D rhand = camera->w2s(cache, player.bone_pos.bone_rhand);

				FVector2D lshoulder = camera->w2s(cache, player.bone_pos.bone_lshoulder);
				FVector2D lelbow = camera->w2s(cache, player.bone_pos.bone_lelbow);
				FVector2D lhand = camera->w2s(cache, player.bone_pos.bone_lhand);

				FVector2D rthigh = camera->w2s(cache, player.bone_pos.bone_rthigh);
				FVector2D rknee = camera->w2s(cache, player.bone_pos.bone_rknee);
				FVector2D rfoot = camera->w2s(cache, player.bone_pos.bone_rfoot);

				FVector2D lthigh = camera->w2s(cache, player.bone_pos.bone_lthigh);
				FVector2D lknee = camera->w2s(cache, player.bone_pos.bone_lknee);
				FVector2D lfoot = camera->w2s(cache, player.bone_pos.bone_lfoot);

				if (player.IsVisible())
				{
					draw_line(neck, chest, skelcolor);
					draw_line(chest, pelvis, skelcolor);

					draw_line(chest, rshoulder, skelcolor);
					draw_line(rshoulder, relbow, skelcolor);
					draw_line(relbow, rhand, skelcolor);

					draw_line(chest, lshoulder, skelcolor);
					draw_line(lshoulder, lelbow, skelcolor);
					draw_line(lelbow, lhand, skelcolor);

					draw_line(pelvis, rthigh, skelcolor);
					draw_line(rthigh, rknee, skelcolor);
					draw_line(rknee, rfoot, skelcolor);

					draw_line(pelvis, lthigh, skelcolor);
					draw_line(lthigh, lknee, skelcolor);
					draw_line(lknee, lfoot, skelcolor);
				}
				else
				{
					draw_line(neck, chest, iskelcolor);
					draw_line(chest, pelvis, iskelcolor);

					draw_line(chest, rshoulder, iskelcolor);
					draw_line(rshoulder, relbow, iskelcolor);
					draw_line(relbow, rhand, iskelcolor);

					draw_line(chest, lshoulder, iskelcolor);
					draw_line(lshoulder, lelbow, iskelcolor);
					draw_line(lelbow, lhand, iskelcolor);

					draw_line(pelvis, rthigh, iskelcolor);
					draw_line(rthigh, rknee, iskelcolor);
					draw_line(rknee, rfoot, iskelcolor);

					draw_line(pelvis, lthigh, iskelcolor);
					draw_line(lthigh, lknee, iskelcolor);
					draw_line(lknee, lfoot, iskelcolor);
				}
			}
			double dx = head.x - Width / 2;
			double dy = head.y - Height / 2;
			float dist = sqrtf(static_cast<float>(dx * dx + dy * dy));
			if (dist <= settings::fov && dist < distanc)
			{
				distanc = dist;
				best = player;
			}
		}
		if (settings::enabled && !settings::menu && distanc != FLT_MAX)
		{
			FVector2D head2d = camera->w2s(cache, best.bone_pos.bone_head);
			if (GetAsyncKeyState(settings::key) & 0x8000)
			{
				if (settings::silent && !flicked)
				{
					if (settings::vischeck)
						if (!best.IsVisible())
							return;
					Silent(head2d);
					flicked = true;
				}
				else if (!settings::silent)
				{
					if (settings::vischeck)
						if (!best.IsVisible())
							return;
					aimbot(head2d.x, head2d.y);
				}
			}
		}
		else
		{
			flicked = false;
		}
	}
}