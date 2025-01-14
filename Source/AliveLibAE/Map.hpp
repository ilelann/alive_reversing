#pragma once

#include "../AliveLibCommon/FunctionFwd.hpp"
#include "FixedPoint.hpp"
#include "Psx.hpp"
#include "ResourceManager.hpp"
#include "Path.hpp"

void Map_ForceLink();

EXPORT void CC static_map_init_4802D0();

struct Map_PathsArray final
{
    u8** field_0_pPathRecs[30];
};
ALIVE_ASSERT_SIZEOF(Map_PathsArray, 120);

class BaseAliveGameObject;
class Camera;
struct PathData;
struct Path_TLV;

enum class LevelIds : s16;

enum class CameraSwapEffects : s16
{
    eInstantChange_0 = 0,
    eLeftToRight_1 = 1,     // Left to right
    eRightToLeft_2 = 2,     // Right to left
    eTopToBottom_3 = 3,     // Top to bottom
    eBottomToTop_4 = 4,     // Bottom to top
    ePlay1FMV_5 = 5,        // Play single fmv
    eVerticalSplit_6 = 6,   // Screen splits from the middle and moves out up/down
    eHorizontalSplit_7 = 7, // Screen splits from the middle and moves out left/right
    eBoxOut_8 = 8,          // A rect "grows" out from the centre of the screen
    ePlay2FMVs_9 = 9,       // Play 2 fmvs
    ePlay3FMVs_10 = 10,     // Play 3 fmvs - apparently just taking an array of fmvs is too simple ?
    eUnknown_11 = 11        // Unknown, has special handing in the map object
};

struct CameraName final
{
    char_type name[8];
};
ALIVE_ASSERT_SIZEOF(CameraName, 8);

enum class CameraPos : s16
{
    eCamInvalid_m1 = -1,
    eCamCurrent_0 = 0,
    eCamTop_1 = 1,
    eCamBottom_2 = 2,
    eCamLeft_3 = 3,
    eCamRight_4 = 4,
    eCamNone_5 = 5, // Not "in" the camera
};

// TODO: Never used in AE path data?
struct Path_Change final : public Path_TLV
{
    LevelIds field_10_level;
    s16 field_12_path;
    s16 field_14_camera;
    s16 field_16_movie;
    s16 field_18_wipe;
    Scale_short field_1A_scale;
};
ALIVE_ASSERT_SIZEOF_ALWAYS(Path_Change, 0x1C);

enum class LoadMode : s16;

class Map final
{
public:
    enum class MapDirections : s16
    {
        eMapLeft_0 = 0,
        eMapRight_1 = 1,
        eMapTop_2 = 2,
        eMapBottom_3 = 3,
    };

    LevelIds field_0_current_level;
    s16 field_2_current_path;
    s16 field_4_current_camera;

    enum class CamChangeStates : s16
    {
        eInactive_0 = 0,
        eSliceCam_1 = 1,
        eInstantChange_2 = 2
    };
    CamChangeStates field_6_state;
    s16 field_8_force_load;

    LevelIds field_A_level;
    s16 field_C_path;
    s16 field_E_camera;
    CameraSwapEffects field_10_screen_change_effect;
    u16 field_12_fmv_base_id;

    MapDirections field_14_direction;
    s16 field_16_padding;

    BaseAliveGameObject* field_18_pAliveObj;

    CameraSwapEffects field_1C;
    s16 field_1E_door;
    s16 field_20;
    s16 field_22_overlayID;

    FP_Point field_24_camera_offset;

    Camera* field_2C_camera_array[5];
    Camera* field_40_stru_5[5];
    Map_PathsArray field_54_path_res_array;

    s16 field_CC_unused;
    s16 field_CE_free_all_anim_and_palts;

    s16 field_D0_cam_x_idx;
    s16 field_D2_cam_y_idx;

    const PathData* field_D4_ptr;
    u8* field_D8_restore_quick_save;


    EXPORT void ScreenChange_480B80();
    EXPORT void RemoveObjectsWithPurpleLight_480740(s16 a2);
    EXPORT void Handle_PathTransition_481610();
    EXPORT void Init_4803F0(LevelIds level, s16 path, s16 camera, CameraSwapEffects screenChangeEffect, s16 fmvBaseId, s16 forceChange);
    EXPORT void Shutdown_4804E0();
    EXPORT void Reset_4805D0();
    EXPORT void GoTo_Camera_481890();

    EXPORT void Get_map_size_480640(PSX_Point* pPoint);
    EXPORT void GetCurrentCamCoords_480680(PSX_Point* pPoint);
    EXPORT void Get_Abe_Spawn_Pos_4806D0(PSX_Point* pPoint);
    EXPORT s16 GetOverlayId_480710();
    EXPORT void Create_FG1s_480F10();
    EXPORT CameraPos Rect_Location_Relative_To_Active_Camera_480FE0(PSX_RECT* pRect);
    EXPORT s16 SetActiveCam_480D30(LevelIds level, s16 path, s16 cam, CameraSwapEffects screenChangeEffect, s16 fmvBaseId, s16 forceChange);
    EXPORT static BaseGameObject* CC FMV_Camera_Change_482650(u8** ppBits, Map* pMap, LevelIds lvlId);
    EXPORT Camera* Create_Camera_4829E0(s16 xpos, s16 ypos, s32 a4);
    EXPORT static void CCSTD Load_Path_Items_482C10(Camera* pCamera, LoadMode loadMode);

    EXPORT static void CC LoadResource_4DBE00(const char_type* pFileName, s32 type, s32 resourceId, LoadMode loadMode, s16 bDontLoad = 0);
    EXPORT static void CC LoadResourcesFromList_4DBE70(const char_type* pFileName, ResourceManager::ResourcesToLoadList* pList, LoadMode loadMode, s16 bDontLoad = 0);

    EXPORT s16 SetActiveCameraDelayed_4814A0(MapDirections direction, BaseAliveGameObject* pObj, s16 kMinus1);

    // Type safe wrappers as s32 level is bigger than the enum type size
    s16 Is_Point_In_Current_Camera_4810D0(LevelIds level, s32 path, FP xpos, FP ypos, s16 width)
    {
        return Is_Point_In_Current_Camera_4810D0(static_cast<s32>(level), path, xpos, ypos, width);
    }

    CameraPos GetDirection_4811A0(LevelIds level, s32 path, FP xpos, FP ypos)
    {
        return GetDirection_4811A0(static_cast<s32>(level), path, xpos, ypos);
    }

    EXPORT s16 Get_Camera_World_Rect_481410(CameraPos camIdx, PSX_RECT* pRect);

private:
    EXPORT s16 Is_Point_In_Current_Camera_4810D0(s32 level, s32 path, FP xpos, FP ypos, s16 width);
    EXPORT CameraPos GetDirection_4811A0(s32 level, s32 path, FP xpos, FP ypos);

    Camera* GetCamera(CameraPos pos);

    void CreateScreenTransistionForTLV(Path_TLV* pTlv);
    void ScreenChange_Common();
};
ALIVE_ASSERT_SIZEOF(Map, 0xDC);

ALIVE_VAR_EXTERN(Map, gMap_5C3030);
ALIVE_VAR_EXTERN(s16, sMap_bDoPurpleLightEffect_5C311C);
extern const CameraSwapEffects kPathChangeEffectToInternalScreenChangeEffect_55D55C[10];
