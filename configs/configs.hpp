#pragma once
#include <includes.h>
#include <fstream>
#include <filesystem>
#include <configs/json.hpp>
#include <shlobj.h>
using json = nlohmann::json;

/*
## all my settings
namespace settings
{
	inline bool menu = false;
	inline bool teamcheck = false;

	inline bool box = false;
	inline bool fullbox = false;
	inline bool skeleton = false;
	inline bool distance = false;
	inline bool healthbar = false;
	inline bool chinahat = false;
	inline bool chams = true;
	inline bool oofarrows = false;
	inline bool tdbox = false;
	inline bool blackhole = false;
	inline bool sealesp = false;
	inline bool headdot = false;

	inline bool enabled = false;
	inline bool silent = false;
	inline bool vischeck = false;
	inline bool showfov = false;
	inline float fov = 25;
	inline float smoothing = 1.f;
	inline int key = 0x06;

	inline float espcolor[4];
	inline float iespcolor[4];
	inline float skeletoncolor[4];
	inline float iskeletoncolor[4];
	inline float espinvisdistance = 90.f;
	inline float espvisdistance = 90.f;
}

*/

namespace config
{
	inline std::string app_folder;
	inline std::string config_dir;
	inline std::string extension = ".holy";

	inline std::string get_appdata_path()
	{
		char path[MAX_PATH];
		if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, path)))
		{
			return std::string(path);
		}
		return ".";
	}

	inline void init(const std::string& app_name = "starfall")
	{
		app_folder = get_appdata_path() + "\\" + app_name;
		config_dir = app_folder + "\\configs";

		if (!std::filesystem::exists(app_folder))
			std::filesystem::create_directory(app_folder);

		if (!std::filesystem::exists(config_dir))
			std::filesystem::create_directory(config_dir);
	}

	inline void open_folder()
	{
		ShellExecuteA(NULL, "open", config_dir.c_str(), NULL, NULL, SW_SHOWDEFAULT);
	}

	inline void save(const std::string& name)
	{
		init();
		json j;

		j["menu"] = settings::menu;
		j["teamcheck"] = settings::teamcheck;
		j["box"] = settings::box;
		j["fullbox"] = settings::fullbox;
		j["skeleton"] = settings::skeleton;
		j["distance"] = settings::distance;
		j["healthbar"] = settings::healthbar;
		j["chinahat"] = settings::chinahat;
		j["chams"] = settings::chams;
		j["oofarrows"] = settings::oofarrows;
		j["tdbox"] = settings::tdbox;
		j["blackhole"] = settings::blackhole;
		j["sealesp"] = settings::sealesp;
		j["headdot"] = settings::headdot;

		j["enabled"] = settings::enabled;
		j["silent"] = settings::silent;
		j["vischeck"] = settings::vischeck;
		j["showfov"] = settings::showfov;
		j["fov"] = settings::fov;
		j["smoothing"] = settings::smoothing;
		j["key"] = settings::key;

		j["espcolor"] = { settings::espcolor[0], settings::espcolor[1], settings::espcolor[2], settings::espcolor[3] };
		j["iespcolor"] = { settings::iespcolor[0], settings::iespcolor[1], settings::iespcolor[2], settings::iespcolor[3] };
		j["skeletoncolor"] = { settings::skeletoncolor[0], settings::skeletoncolor[1], settings::skeletoncolor[2], settings::skeletoncolor[3] };
		j["iskeletoncolor"] = { settings::iskeletoncolor[0], settings::iskeletoncolor[1], settings::iskeletoncolor[2], settings::iskeletoncolor[3] };

		j["espinvisdistance"] = settings::espinvisdistance;
		j["espvisdistance"] = settings::espvisdistance;

		std::ofstream file(config_dir + "\\" + name + extension);
		file << j.dump(4);
		file.close();
	}

	inline void load(const std::string& name)
	{
		init();
		std::ifstream file(config_dir + "\\" + name + extension);
		if (!file.is_open()) return;

		json j;
		file >> j;
		file.close();

		settings::menu = j.value("menu", false);
		settings::teamcheck = j.value("teamcheck", false);
		settings::box = j.value("box", false);
		settings::fullbox = j.value("fullbox", false);
		settings::skeleton = j.value("skeleton", false);
		settings::distance = j.value("distance", false);
		settings::healthbar = j.value("healthbar", false);
		settings::chinahat = j.value("chinahat", false);
		settings::chams = j.value("chams", true);
		settings::oofarrows = j.value("oofarrows", false);
		settings::tdbox = j.value("tdbox", false);
		settings::blackhole = j.value("blackhole", false);
		settings::sealesp = j.value("sealesp", false);
		settings::headdot = j.value("headdot", false);

		settings::enabled = j.value("enabled", false);
		settings::silent = j.value("silent", false);
		settings::vischeck = j.value("vischeck", false);
		settings::showfov = j.value("showfov", false);
		settings::fov = j.value("fov", 25.f);
		settings::smoothing = j.value("smoothing", 1.f);
		settings::key = j.value("key", 0x06);

		auto loadColor = [&](const std::string& key, float* color) {
			if (j.contains(key) && j[key].is_array() && j[key].size() == 4)
				for (int i = 0; i < 4; ++i)
					color[i] = j[key][i];
			};

		loadColor("espcolor", settings::espcolor);
		loadColor("iespcolor", settings::iespcolor);
		loadColor("skeletoncolor", settings::skeletoncolor);
		loadColor("iskeletoncolor", settings::iskeletoncolor);

		settings::espinvisdistance = j.value("espinvisdistance", 90.f);
		settings::espvisdistance = j.value("espvisdistance", 90.f);
	}

	inline std::string selected_config;
	inline std::vector<std::string> config_list;
	
	inline void refresh_list()
	{
		config_list.clear();
		init();

		for (auto& entry : std::filesystem::directory_iterator(config_dir))
		{
			if (entry.path().extension() == extension)
				config_list.push_back(entry.path().stem().string());
		}

		if (selected_config.empty() && !config_list.empty())
			selected_config = config_list[0];
	}


	inline void delete_config(const std::string& name)
	{
		std::filesystem::remove(config_dir + "\\" + name + extension);
		refresh_list();
	}

	inline void render_menu()
	{
		ImGui::Text("Config System");
		ImGui::Separator();

		if (ImGui::Button("Refresh List"))
			refresh_list();

		ImGui::SameLine();

		if (ImGui::Button("Save Current"))
		{
			if (!selected_config.empty())
				save(selected_config);
		}

		ImGui::SameLine();

		if (ImGui::Button("Load Selected"))
		{
			if (!selected_config.empty())
				load(selected_config);
		}

		ImGui::SameLine();

		if (ImGui::Button("Open Config Folder"))
		{
			config::open_folder();
		}

		ImGui::Separator();
		ImGui::Text("Available Configs:");

		ImGui::BeginChild("##configlist", ImVec2(0, 150), true);
		for (auto& cfg : config_list)
		{
			bool is_selected = (cfg == selected_config);

			if (ImGui::Selectable(cfg.c_str(), is_selected))
				selected_config = cfg;

			if (ImGui::BeginPopupContextItem())
			{
				if (ImGui::MenuItem("Delete"))
					delete_config(cfg);
				ImGui::EndPopup();
			}
		}
		ImGui::EndChild();

		static char new_cfg_name[64] = "";
		ImGui::InputText("Config Name  ", new_cfg_name, IM_ARRAYSIZE(new_cfg_name));

		if (ImGui::Button("Create New Config"))
		{
			if (strlen(new_cfg_name) > 0)
			{
				save(new_cfg_name);
				strcpy_s(new_cfg_name, "");
				refresh_list();
			}
		}
	}
}