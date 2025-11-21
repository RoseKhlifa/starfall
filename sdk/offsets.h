#pragma once
#include <includes.h>
namespace offsets
{
    // Core Engine Offsets
    inline std::ptrdiff_t uworld_pointer = 0xC0;
    inline std::ptrdiff_t game_instance = 0x01D8;
    inline std::ptrdiff_t gamestate = 0x178;
    inline std::ptrdiff_t persistent_level = 0x0038;
    inline std::ptrdiff_t localplayers_array = 0x0040;
    inline std::ptrdiff_t localplayer = 0x0040;
    inline std::ptrdiff_t player_controller = 0x0038;
    inline std::ptrdiff_t player_camera_manager = 0x0520;
    inline std::ptrdiff_t camera_private = 0x17B0;
    inline std::ptrdiff_t camera_manager = 0x540;
    inline std::ptrdiff_t control_rotation = 0x04E0;
    inline std::ptrdiff_t fname_pool = 0xB654040; // ULevel
    inline std::ptrdiff_t outline_mode = 0x0358; // UWorld
    inline std::ptrdiff_t outline_component = 0x0D80; // AActor

    // Component/Attachment
    inline std::ptrdiff_t attach_children = 0x1A0; // USceneComponent
    inline std::ptrdiff_t attach_children_count = 0x1A8; // USceneComponent

    // Player/Pawn Related
    inline std::ptrdiff_t acknowledge_pawn = 0x0510;
    inline std::ptrdiff_t apawn = 0x0510;
    inline std::ptrdiff_t player_state = 0x3D8;
    inline std::ptrdiff_t player_state_array = 0x478; // GameState
    inline std::ptrdiff_t spawned_character = 0x0A18; // AShooterCharacter
    inline std::ptrdiff_t root_component = 0x0288;
    inline std::ptrdiff_t root_position = 0x0170;

    // Health/Damage
    inline std::ptrdiff_t damage_handler = 0xC68;
    inline std::ptrdiff_t health = 0x01E0;
    inline std::ptrdiff_t shieldtype = 0x118;
    inline std::ptrdiff_t shieldlife = 0x124;
    inline std::ptrdiff_t shieldmaxlife = 0x128;
    inline std::ptrdiff_t damagesections = 0x1c8;

    // Mesh/Rendering
    inline std::ptrdiff_t mesh = 0x04E8;
    inline std::ptrdiff_t component_to_world = 0x2D0;
    inline std::ptrdiff_t BoundsScale = 0x0474; // ASkeletalMeshComponent
    inline std::ptrdiff_t bone_array = 0x730;
    inline std::ptrdiff_t bone_array_cache = bone_array + 0x10;
    inline std::ptrdiff_t bone_count = 0x740;
    inline std::ptrdiff_t LocalBounds = 0x0730;
    inline std::ptrdiff_t last_submit_time = 0x478;
    inline std::ptrdiff_t last_render_time = 0x47C;
    inline std::ptrdiff_t relative_location = 0x0170; // FTransform
    inline std::ptrdiff_t relative_rotation = 0x170; // FTransform

    // Team Related
    inline std::ptrdiff_t team_component = 0x0680;
    inline std::ptrdiff_t team_id = 0xE8;
    inline std::ptrdiff_t was_ally = 0x0F29;

    // Minimap/UI
    inline std::ptrdiff_t CharacterPortraitMinimapComponent = 0x1210;
    inline std::ptrdiff_t ShooterCharacterMinimapComponent = 0x1218;

    // Visibility
    inline std::ptrdiff_t VisibilityComponent = 0x538;
    inline std::ptrdiff_t CharactersWithVisibility = 0x118;
    inline std::ptrdiff_t bIsVisible = 0x521;
    inline std::ptrdiff_t bLocalObserver = 0x550;
    inline std::ptrdiff_t dormant = 0x100;

    // Actor Management
    inline std::ptrdiff_t actor_array = 0xA0;
    inline std::ptrdiff_t actors_count = 0xA8;
    inline std::ptrdiff_t actor_id = 0x18;
    inline std::ptrdiff_t unique_id = 0x38;
}
