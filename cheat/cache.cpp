#include "cache.h"
#include <km/kernel.h>
#include <km/utils.h>


namespace valhalla::cache
{
	uintptr_t uworld;
	void getworld()
	{
		auto pml4_base = utils::find_pml4_base();
		if (pml4_base == 0)
		{
			printf("[-] Failed to find pml4 base!\n");
		}

		uintptr_t UWOLRD_1 = km::RPM<uintptr_t>(pml4_base + 0x1C0);

		uintptr_t uworld_offset = 0;

		for (int i = 0x0; i < 0x500; i += 0x8)
		{
			uintptr_t address_to_read = pml4_base + i;

			auto potential_uworld = km::RPM<uintptr_t>(address_to_read);

			if (potential_uworld != 0)
			{
				uintptr_t level_address = potential_uworld + offsets::persistent_level;


				auto level = km::RPM<uintptr_t>(level_address);
				if (level != 0)
				{
					uintptr_t owning_world_address = level + offsets::uworld_pointer;


					auto owning_world = km::RPM<uintptr_t>(owning_world_address);



					if (owning_world == potential_uworld)
					{
						uworld = potential_uworld;


						uworld_offset = i;
						break;
					}
				}
			}
		}
	}

	bool thread_running = false;

	void start_uworld()
	{
		auto last_update = std::chrono::steady_clock::now();
		int iteration_count = 0;

		while (true)
		{
			auto now = std::chrono::steady_clock::now();
			auto time_t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

			auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - last_update).count();

			if (!uworld || elapsed > 15) {
				getworld();
				last_update = now;
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		}
	}

	void cache()
	{
		  
		while (true)
		{
				if (!thread_running)
				{
					std::thread(start_uworld).detach();
					thread_running = true;
				}

			ccache->game_instance = km::RPM<uintptr_t>(uworld + offsets::game_instance);
			ccache->game_state = km::RPM<uintptr_t>(uworld + offsets::gamestate);
			ccache->level = km::RPM<uintptr_t>(uworld + offsets::persistent_level);

			ccache->local_player_array = km::RPM<uintptr_t>(ccache->game_instance + offsets::localplayers_array);
			ccache->local_player = km::RPM<uintptr_t>(ccache->local_player_array);

			ccache->player_controller = km::RPM<uintptr_t>(ccache->local_player + offsets::player_controller);
			ccache->local_pawn = km::RPM<uintptr_t>(ccache->player_controller + offsets::acknowledge_pawn);

			ccache->player_state = km::RPM<uintptr_t>(ccache->local_pawn + offsets::player_state);
			ccache->localmesh = km::RPM<uintptr_t>(ccache->player_state + offsets::mesh);
			ccache->root_comp = km::RPM<uintptr_t>(ccache->local_pawn + offsets::root_component);
			ccache->relative = km::RPM <fvector>(ccache->root_comp + offsets::relative_location);

			ccache->camera_manager = km::RPM<uintptr_t>(ccache->player_controller + offsets::player_camera_manager);
			ccam->get_view(*ccache);

			ccache->actor_array = km::RPM<uintptr_t>(ccache->level + offsets::actor_array);
			ccache->actor_count = km::RPM<int>(ccache->level + offsets::actors_count);

			std::this_thread::sleep_for(std::chrono::milliseconds(5));
		}
	}

}