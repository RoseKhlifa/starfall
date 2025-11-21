#pragma once
#include "offsets.h"
#include <d3d9types.h>
#define M_PI (float)3.1415926535

class fvector {
public:
    fvector() : x(0.0), y(0.0), z(0.0) {}
    fvector(double _x, double _y, double _z) : x(_x), y(_y), z(_z) {}
    ~fvector() {}

    double x;
    double y;
    double z;

    inline double Dot(const fvector& v) const {
        return x * v.x + y * v.y + z * v.z;
    }

    inline double distance(const fvector& v) const {
        return std::sqrtf(std::powf(v.x - x, 2.0) +
            std::powf(v.y - y, 2.0) +
            std::powf(v.z - z, 2.0));
    }

    fvector operator+(const fvector& v) const {
        return fvector(x + v.x, y + v.y, z + v.z);
    }

    fvector operator-(const fvector& v) const {
        return fvector(x - v.x, y - v.y, z - v.z);
    }

    fvector operator/(double flDiv) const {
        return fvector(x / flDiv, y / flDiv, z / flDiv);
    }

    fvector operator*(double Scale) const {
        return fvector(x * Scale, y * Scale, z * Scale);
    }

    inline double Length() const {
        return sqrtf((x * x) + (y * y) + (z * z));
    }

    bool operator==(const fvector& other) const {
        return x == other.x && y == other.y && z == other.z;
    }

    fvector& operator-=(const fvector& v) {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
    }
};

struct FVector2D {
public:
    double x;
    double y;

    inline FVector2D() : x(0.0), y(0.0) {}
    inline FVector2D(double _x, double _y) : x(_x), y(_y) {}

    inline double Distance(const FVector2D& v) const {
        return sqrtf(((v.x - x) * (v.x - x)) +
            ((v.y - y) * (v.y - y)));
    }

    inline FVector2D operator+(const FVector2D& v) const {
        return FVector2D(x + v.x, y + v.y);
    }

    inline FVector2D operator-(const FVector2D& v) const {
        return FVector2D(x - v.x, y - v.y);
    }
};

struct fquat {
    double x;
    double y;
    double z;
    double w;
};

struct ftransform {
    fquat rot;
    fvector translation;
    char pad[4];
    fvector scale;
    char pad1[4];

    D3DMATRIX ToMatrixWithScale() const {
        D3DMATRIX m;
        m._41 = translation.x;
        m._42 = translation.y;
        m._43 = translation.z;

        double x2 = rot.x + rot.x;
        double y2 = rot.y + rot.y;
        double z2 = rot.z + rot.z;

        double xx2 = rot.x * x2;
        double yy2 = rot.y * y2;
        double zz2 = rot.z * z2;

        m._11 = (1.0 - (yy2 + zz2)) * scale.x;
        m._22 = (1.0 - (xx2 + zz2)) * scale.y;
        m._33 = (1.0 - (xx2 + yy2)) * scale.z;

        double yz2 = rot.y * z2;
        double wx2 = rot.w * x2;
        m._32 = (yz2 - wx2) * scale.z;
        m._23 = (yz2 + wx2) * scale.y;

        double xy2 = rot.x * y2;
        double wz2 = rot.w * z2;
        m._21 = (xy2 - wz2) * scale.y;
        m._12 = (xy2 + wz2) * scale.x;

        double xz2 = rot.x * z2;
        double wy2 = rot.w * y2;
        m._31 = (xz2 + wy2) * scale.z;
        m._13 = (xz2 - wy2) * scale.x;

        m._14 = 0.0;
        m._24 = 0.0;
        m._34 = 0.0;
        m._44 = 1.0;

        return m;
    }
};

inline D3DMATRIX matrix(fvector rot, fvector origin = fvector(0.0, 0.0, 0.0)) {
    auto radPitch = (rot.x * M_PI / 180.0);
    auto radYaw = (rot.y * M_PI / 180.0);
    auto radRoll = (rot.z * M_PI / 180.0);

    auto sp = sinf(radPitch);
    auto cp = cosf(radPitch);
    auto sy = sinf(radYaw);
    auto cy = cosf(radYaw);
    auto sr = sinf(radRoll);
    auto cr = cosf(radRoll);

    D3DMATRIX matrix;
    matrix.m[0][0] = cp * cy;
    matrix.m[0][1] = cp * sy;
    matrix.m[0][2] = sp;
    matrix.m[0][3] = 0.f;

    matrix.m[1][0] = sr * sp * cy - cr * sy;
    matrix.m[1][1] = sr * sp * sy + cr * cy;
    matrix.m[1][2] = -sr * cp;
    matrix.m[1][3] = 0.f;

    matrix.m[2][0] = -(cr * sp * cy + sr * sy);
    matrix.m[2][1] = cy * sr - cr * sp * sy;
    matrix.m[2][2] = cr * cp;
    matrix.m[2][3] = 0.f;

    matrix.m[3][0] = 0.0f;
    matrix.m[3][1] = 0.0f;
    matrix.m[3][2] = 0.0f;
    matrix.m[3][3] = 1.f;

    return matrix;
}

inline D3DMATRIX MatrixMultiplication(D3DMATRIX pM1, D3DMATRIX pM2)
{
    D3DMATRIX pOut;
    pOut._11 = pM1._11 * pM2._11 + pM1._12 * pM2._21 + pM1._13 * pM2._31 + pM1._14 * pM2._41;
    pOut._12 = pM1._11 * pM2._12 + pM1._12 * pM2._22 + pM1._13 * pM2._32 + pM1._14 * pM2._42;
    pOut._13 = pM1._11 * pM2._13 + pM1._12 * pM2._23 + pM1._13 * pM2._33 + pM1._14 * pM2._43;
    pOut._14 = pM1._11 * pM2._14 + pM1._12 * pM2._24 + pM1._13 * pM2._34 + pM1._14 * pM2._44;
    pOut._21 = pM1._21 * pM2._11 + pM1._22 * pM2._21 + pM1._23 * pM2._31 + pM1._24 * pM2._41;
    pOut._22 = pM1._21 * pM2._12 + pM1._22 * pM2._22 + pM1._23 * pM2._32 + pM1._24 * pM2._42;
    pOut._23 = pM1._21 * pM2._13 + pM1._22 * pM2._23 + pM1._23 * pM2._33 + pM1._24 * pM2._43;
    pOut._24 = pM1._21 * pM2._14 + pM1._22 * pM2._24 + pM1._23 * pM2._34 + pM1._24 * pM2._44;
    pOut._31 = pM1._31 * pM2._11 + pM1._32 * pM2._21 + pM1._33 * pM2._31 + pM1._34 * pM2._41;
    pOut._32 = pM1._31 * pM2._12 + pM1._32 * pM2._22 + pM1._33 * pM2._32 + pM1._34 * pM2._42;
    pOut._33 = pM1._31 * pM2._13 + pM1._32 * pM2._23 + pM1._33 * pM2._33 + pM1._34 * pM2._43;
    pOut._34 = pM1._31 * pM2._14 + pM1._32 * pM2._24 + pM1._33 * pM2._34 + pM1._34 * pM2._44;
    pOut._41 = pM1._41 * pM2._11 + pM1._42 * pM2._21 + pM1._43 * pM2._31 + pM1._44 * pM2._41;
    pOut._42 = pM1._41 * pM2._12 + pM1._42 * pM2._22 + pM1._43 * pM2._32 + pM1._44 * pM2._42;
    pOut._43 = pM1._41 * pM2._13 + pM1._42 * pM2._23 + pM1._43 * pM2._33 + pM1._44 * pM2._43;
    pOut._44 = pM1._41 * pM2._14 + pM1._42 * pM2._24 + pM1._43 * pM2._34 + pM1._44 * pM2._44;

    return pOut;
}

namespace valhalla::sdk
{
    class sdk
    {
    public:

        struct FMinimalViewInfo {
            struct fvector Location {};
            struct fvector Rotation {};
            float FOV;
        };
    };

    class cache : public sdk
    {
    public:
        uintptr_t game_instance;
        uintptr_t game_state;
        uintptr_t level;

        uintptr_t local_player_array;
        uintptr_t local_player;

        uintptr_t player_controller;
        uintptr_t local_pawn;

        uintptr_t player_state;
        uintptr_t localmesh;
        uintptr_t root_comp;
        fvector relative;
        uintptr_t team_component;
        double local_team_id;

        uintptr_t camera_manager;
        FMinimalViewInfo view;

        uintptr_t actor_array;
        int actor_count;
    };

    class player : public sdk
    {
    public:
        uintptr_t actor_count{};
        uintptr_t actor{};
        uintptr_t player_state{};
        uintptr_t damage_handler{};
        float health{};
        uintptr_t mesh{};
        int bone_count{};
        uintptr_t bone_array{};
        int team_id{};

        player() = default;

        player(uintptr_t a, uintptr_t m, int bc, uintptr_t dh, float h, uintptr_t ba, int t = 0)
            : actor(a), mesh(m), bone_count(bc), damage_handler(dh), health(h), bone_array(ba), team_id(t) {
        }

        struct bones {
            int head = 8,
                neck,
                chest,
                pelvis,
                rshoulder,
                relbow,
                rhand,
                rthigh,
                rknee,
                rfoot,
                lshoulder,
                lelbow,
                lhand,
                lthigh,
                lknee,
                lfoot,
                bottom;
        } bone_ids;

        struct bonesPos
        {
            fvector bone_head,
                bone_neck,
                bone_chest,
                bone_pelvis,
                bone_rshoulder,
                bone_relbow,
                bone_rhand,
                bone_rthigh,
                bone_rknee,
                bone_rfoot,
                bone_lshoulder,
                bone_lelbow,
                bone_lhand,
                bone_lthigh,
                bone_lknee,
                bone_lfoot,
                bone_bottom;
        } bone_pos;

        struct gender
        {
            int NPC = 103,
                Male = 104,
                Female = 101;
        };

        fvector get_bone(int index) const {
            ftransform comp_to_world = km::RPM<ftransform>(mesh + offsets::component_to_world);
            ftransform bone = km::RPM<ftransform>(bone_array + (index * 0x60));
            D3DMATRIX Matrix = MatrixMultiplication(bone.ToMatrixWithScale(), comp_to_world.ToMatrixWithScale());
            return fvector(Matrix._41, Matrix._42, Matrix._43);
        }

        bool IsVisible()
        {
            if (!mesh)
                return false;

            try {
                double fLastSubmitTime = km::RPM<double>(mesh + offsets::last_submit_time);
                double fLastRenderTimeOnScreen = km::RPM<double>(mesh + offsets::last_render_time);

                if (isnan(fLastSubmitTime) || isnan(fLastRenderTimeOnScreen) ||
                    isinf(fLastSubmitTime) || isinf(fLastRenderTimeOnScreen))
                    return false;

                const double fVisionTick = 0.06f;
                bool bVisible = fLastRenderTimeOnScreen + fVisionTick >= fLastSubmitTime;
                return bVisible;
            }
            catch (...) {

                return false;
            }
        }

        int get_gender() {
            gender g;
            if (bone_count == g.NPC)    return g.NPC;
            if (bone_count == g.Male)   return g.Male;
            if (bone_count == g.Female) return g.Female;
            return -1;
        }

        double get_dist(valhalla::sdk::cache& c)
        {
            return c.relative.distance(bone_pos.bone_bottom) / 100;
        }

        void set_bones()
        {
            gender g;
            int type = get_gender();
            if (type == -1) return;

            bone_ids.chest = 6;
            bone_ids.pelvis = 3;

            if (type == g.NPC) {
                bone_ids.neck = 9;
                bone_ids.lshoulder = 33;
                bone_ids.lelbow = 30;
                bone_ids.lhand = 32;
                bone_ids.rshoulder = 58;
                bone_ids.relbow = 55;
                bone_ids.rhand = 57;
                bone_ids.lthigh = 63;
                bone_ids.lknee = 65;
                bone_ids.lfoot = 69;
                bone_ids.rthigh = 77;
                bone_ids.rknee = 79;
                bone_ids.rfoot = 83;
            }
            else if (type == g.Male) {
                bone_ids.neck = 21;
                bone_ids.lshoulder = 23;
                bone_ids.lelbow = 24;
                bone_ids.lhand = 25;
                bone_ids.rshoulder = 49;
                bone_ids.relbow = 50;
                bone_ids.rhand = 51;
                bone_ids.lthigh = 77;
                bone_ids.lknee = 78;
                bone_ids.lfoot = 80;
                bone_ids.rthigh = 84;
                bone_ids.rknee = 85;
                bone_ids.rfoot = 87;
            }
            else if (type == g.Female) {
                bone_ids.neck = 21;
                bone_ids.lshoulder = 23;
                bone_ids.lelbow = 24;
                bone_ids.lhand = 25;
                bone_ids.rshoulder = 49;
                bone_ids.relbow = 50;
                bone_ids.rhand = 51;
                bone_ids.lthigh = 75;
                bone_ids.lknee = 76;
                bone_ids.lfoot = 78;
                bone_ids.rthigh = 82;
                bone_ids.rknee = 83;
                bone_ids.rfoot = 85;
            }
        }
    };

    class camera : public sdk
    {
    public:

        void get_view(cache& cache)
        {
            cache.view = km::RPM<FMinimalViewInfo>(cache.camera_manager + offsets::camera_private + 0x10);
        }

        FVector2D w2s(cache& cache, fvector world_location)
        {
            fvector CameraLocation = cache.view.Location;
            fvector CameraRotation = cache.view.Rotation;

            D3DMATRIX rotation_matrix = matrix(CameraRotation);
            auto& axis_x = rotation_matrix.m[0];
            auto& axis_y = rotation_matrix.m[1];
            auto& axis_z = rotation_matrix.m[2];

            fvector delta = world_location - CameraLocation;

            auto transformed_x = delta.Dot(fvector(axis_y[0], axis_y[1], axis_y[2]));
            auto transformed_y = delta.Dot(fvector(axis_z[0], axis_z[1], axis_z[2]));
            auto transformed_z = max(delta.Dot(fvector(axis_x[0], axis_x[1], axis_x[2])), 1.0f);

            float FovAngle = cache.view.FOV;

            float fov_rad = FovAngle * M_PI / 180.0f;
            float fov_tan = tanf(fov_rad / 2.0f);

            int screen_width_center = GetSystemMetrics(0) / 2;
            int screen_height_center = GetSystemMetrics(1) / 2;

            float screen_x = screen_width_center + (transformed_x / fov_tan) * screen_width_center / transformed_z;
            float screen_y = screen_height_center - (transformed_y / fov_tan) * screen_width_center / transformed_z;

            return FVector2D(screen_x, screen_y);
        }
    };
}

#include "../cheat/player.h"