#include "stdafx.h"
#include "DebugHelpers.hpp"
#include "stdlib.hpp"
#include <iomanip>
#include "Function.hpp"
#include "Map.hpp"
#include "PathData.hpp"
#include "PsxDisplay.hpp"
#include "ScreenManager.hpp"
#include "ResourceManager.hpp"
#include "Font.hpp"
#include "DDCheat.hpp"
#include "Resources.hpp"
#include "SwitchStates.hpp"
#include "Events.hpp"
#include "MainMenu.hpp"
#include "Sound/Midi.hpp"
#include "Sfx.hpp"
#include "Sys.hpp"
#include "Sound/Sound.hpp"
#include "RenderingTestTimData.hpp"
#include "PsxRender.hpp"
#include "LvlArchive.hpp"
#include "Movie.hpp"
#include "Text.hpp"
#include "AbilityRing.hpp"
#include "MusicController.hpp"
#include "QuikSave.hpp"
#include "PauseMenu.hpp"
#include "BaseBomb.hpp"

#include "Factory.hpp"

// Objects
#include "UXB.hpp"
#include "Abe.hpp"
#include "Slurg.hpp"
#include "Spark.hpp"
#include "Mine.hpp"
#include "FlyingSlig.hpp"
#include "Mudokon.hpp"

char_type g_devConsoleBuffer[1000];

bool sDebugEnabled_VerboseEvents = false;

Alive::Font g_DebugGlobalFont;
s32 g_DebugGlobalFontPolyIndex = 0;
s8 g_DebugGlobalFontPalette[32];
Font_Context g_DebugGlobalFontContext;
bool g_DebugGlobalFontIsInit = false;
bool g_EnabledRaycastRendering = false;
static bool g_DisableMusic = false;

std::vector<RaycastDebug> g_RaycastDebugList;

void DebugAddRaycast(RaycastDebug rc)
{
    g_RaycastDebugList.push_back(rc);
}

void InitDebugFont()
{
    if (!g_DebugGlobalFontIsInit)
    {
        g_DebugGlobalFontContext.LoadFontTypeCustom(reinterpret_cast<File_Font*>(sDebugFont), reinterpret_cast<Font_AtlasEntry*>(sDebugFontAtlas), g_DebugGlobalFontPalette);
        g_DebugGlobalFont.ctor_433590(1024, reinterpret_cast<u8*>(g_DebugGlobalFontPalette), &g_DebugGlobalFontContext);
        g_DebugGlobalFontIsInit = true;
    }
}

class ObjectDebugger final : public BaseGameObject
{
public:
    ObjectDebugger()
    {
        DisableVTableHack disableHack;

        BaseGameObject_ctor_4DBFA0(1, 1);

        field_6_flags.Set(BaseGameObject::eDrawable_Bit4);
        field_6_flags.Set(BaseGameObject::eSurviveDeathReset_Bit9);
        field_4_typeId = AETypes::eDebugHelper_1001;

        mFontContext.LoadFontTypeCustom(reinterpret_cast<File_Font*>(sDebugFont), reinterpret_cast<Font_AtlasEntry*>(sDebugFontAtlas), mFontPalette);
        mFont.ctor_433590(512, reinterpret_cast<u8*>(mFontPalette), &mFontContext);

        gObjList_drawables_5C1124->Push_Back(this);
    }

    ~ObjectDebugger()
    {
    }

    static bool Enabled;

    void Destruct()
    {
        BaseGameObject_dtor_4DBEC0();
        gObjList_drawables_5C1124->Remove_Item(this);
    }

    virtual BaseGameObject* VDestructor(s32 flags) override
    {
        Destruct();
        if (flags & 1)
        {
            ae_delete_free_495540(this);
        }
        return this;
    }

    virtual void VUpdate() override
    {
        mFontPIndex = 0;
    }

    virtual void VScreenChanged() override
    {
        // Dont kill!
    }

    static bool IsInAnimationList(AnimationBase* toFind)
    {
        for (s32 i = 0; i < gObjList_animations_5C1A24->Size(); i++)
        {
            if (gObjList_animations_5C1A24->ItemAt(i) == toFind)
            {
                return true;
            }
        }
        return false;
    }

    void DrawObjectIDs(PrimHeader** pOrderingTable)
    {
        /*struct MainMenuButton final
        {
        s16 field_0_type;
        s16 field_2_x;
        s16 field_4_y;
        s16 field_6;
        s16 field_8;
        s16 field_A;
        };

        auto list = reinterpret_cast<MainMenuButton *>(0x5610B8);

        while (list->field_0_type)
        {
        pIndex = mFont.DrawString_4337D0(pOrderingTable, "O", list->field_2_x, list->field_4_y, 0, 1, 0, 40, 255, 0, 0, pIndex, FP_FromDouble(1.0), 640, 0);
        list++;
        }*/

        for (s32 baseObjIdx = 0; baseObjIdx < gBaseGameObject_list_BB47C4->Size(); baseObjIdx++)
        {
            BaseGameObject* pBaseGameObject = gBaseGameObject_list_BB47C4->ItemAt(baseObjIdx);

            if (!pBaseGameObject)
            {
                break;
            }

            if (pBaseGameObject->field_6_flags.Get(BaseGameObject::eIsBaseAnimatedWithPhysicsObj_Bit5))
            {
                auto aliveObj = ((BaseAnimatedWithPhysicsGameObject*) pBaseGameObject);
                s16 x = FP_GetExponent(aliveObj->field_B8_xpos) - FP_GetExponent(gMap_5C3030.field_24_camera_offset.field_0_x);
                s16 y = FP_GetExponent(aliveObj->field_BC_ypos) - FP_GetExponent(gMap_5C3030.field_24_camera_offset.field_4_y);

                if (IsInAnimationList(&aliveObj->field_20_animation))
                {
                    if (aliveObj->field_20_animation.field_92_current_frame != -1)
                    {
                        FrameInfoHeader* framePtr = aliveObj->field_20_animation.Get_FrameHeader_40B730(aliveObj->field_20_animation.field_92_current_frame);
                        if (framePtr != nullptr)
                        {
                            y += static_cast<s16>(framePtr->field_8_data.offsetAndRect.mMax.y * FP_GetDouble(aliveObj->field_CC_sprite_scale));
                        }
                    }
                }

                std::string text = std::to_string(static_cast<s32>(pBaseGameObject->field_4_typeId));

                mFontPIndex = mFont.DrawString_4337D0(pOrderingTable, text.c_str(), x - (mFont.MeasureWidth_433700(text.c_str()) / 2) + 1, y + 1, TPageAbr::eBlend_0, 0, 0, Layer::eLayer_Above_FG1_39, 0, 0, 0, mFontPIndex, FP_FromDouble(1.0), 640, 0);
                mFontPIndex = mFont.DrawString_4337D0(pOrderingTable, text.c_str(), x - (mFont.MeasureWidth_433700(text.c_str()) / 2), y, TPageAbr::eBlend_0, 1, 0, Layer::eLayer_FadeFlash_40, 255, 255, 255, mFontPIndex, FP_FromDouble(1.0), 640, 0);
            }
        }
    }

    f32 Vec2Distance(f32 x1, f32 y1, f32 x2, f32 y2)
    {
        // This takes into account the fact that the height of the screen is / 2
        return static_cast<f32>(sqrt(pow(x1 - x2, 2) + pow((y1 - y2) * 2, 2)));
    }

    bool isDragging = false;
    BaseAnimatedWithPhysicsGameObject* mDragObject;

    void DrawUI(PrimHeader** /*pOrderingTable*/)
    {
        POINT mousePos = Sys_GetScreenMousePos();
        bool mouseLeftDown = Sys_IsMouseButtonDown(MouseButtons::eLeft);
        mousePos.y /= 2;

        for (s32 baseObjIdx = 0; baseObjIdx < gBaseGameObject_list_BB47C4->Size(); baseObjIdx++)
        {
            BaseGameObject* pBaseGameObject = gBaseGameObject_list_BB47C4->ItemAt(baseObjIdx);

            if (!pBaseGameObject)
            {
                break;
            }

            if (pBaseGameObject->field_6_flags.Get(BaseGameObject::eIsBaseAnimatedWithPhysicsObj_Bit5))
            {
                auto aliveObj = ((BaseAnimatedWithPhysicsGameObject*) pBaseGameObject);

                s32 x = static_cast<s32>((FP_GetExponent(aliveObj->field_B8_xpos) - FP_GetExponent(gMap_5C3030.field_24_camera_offset.field_0_x)) / 0.575);
                s32 y = static_cast<s32>((FP_GetExponent(aliveObj->field_BC_ypos) - FP_GetExponent(gMap_5C3030.field_24_camera_offset.field_4_y)));

                if (Vec2Distance(static_cast<f32>(x), static_cast<f32>(y), static_cast<f32>(mousePos.x), static_cast<f32>(mousePos.y)) < 10 && !isDragging && mouseLeftDown)
                {
                    isDragging = true;
                    mDragObject = aliveObj;
                }

                if (Vec2Distance(static_cast<f32>(x), static_cast<f32>(y), static_cast<f32>(mousePos.x), static_cast<f32>(mousePos.y)) > 100)
                {
                    continue;
                }

                if (IsInAnimationList(&aliveObj->field_20_animation))
                {
                    if (aliveObj->field_20_animation.field_92_current_frame != -1)
                    {
                        FrameInfoHeader* framePtr = aliveObj->field_20_animation.Get_FrameHeader_40B730(aliveObj->field_20_animation.field_92_current_frame);
                        if (framePtr != nullptr)
                        {
                            y += static_cast<s32>(framePtr->field_8_data.offsetAndRect.mMax.y * FP_GetDouble(aliveObj->field_CC_sprite_scale));
                        }
                    }
                }

                sFontDrawScreenSpace_5CA4B4 = 1;
                //mFontPIndex = mFont.DrawString_4337D0(pOrderingTable, "Test", x, y, 0, 0, 0, 32, 255, 255, 255, mFontPIndex, FP_FromDouble(1.0), 640, 0);
                sFontDrawScreenSpace_5CA4B4 = 0;
            }

            if (isDragging && !mouseLeftDown)
            {
                isDragging = false;
            }

            if (isDragging)
            {
                mDragObject->field_B8_xpos = FP_FromInteger(static_cast<s32>(FP_GetExponent(gMap_5C3030.field_24_camera_offset.field_0_x) + (mousePos.x * 0.575)));
                mDragObject->field_BC_ypos = FP_FromInteger(static_cast<s32>(FP_GetExponent(gMap_5C3030.field_24_camera_offset.field_4_y) + mousePos.y));
            }
        }
    }

    virtual void VRender(PrimHeader** ppOt) override
    {
        if (!Enabled)
            return;

        DrawUI(ppOt);

        DrawObjectIDs(ppOt);
    }

    s32 mFontPIndex = 0;
    Alive::Font mFont;
    s8 mFontPalette[32];
    Font_Context mFontContext;
};
bool ObjectDebugger::Enabled = false;

class DebugPathRenderer final : public BaseGameObject
{
public:
    DebugPathRenderer()
    {
        DisableVTableHack disableHack;

        BaseGameObject_ctor_4DBFA0(1, 1);
        field_6_flags.Set(BaseGameObject::eDrawable_Bit4);
        field_6_flags.Set(BaseGameObject::eSurviveDeathReset_Bit9);
        field_4_typeId = AETypes::eDebugPathRenderer_1003;

        mFontContext.LoadFontTypeCustom(reinterpret_cast<File_Font*>(sDebugFont), reinterpret_cast<Font_AtlasEntry*>(sDebugFontAtlas), mFontPalette);
        mFont.ctor_433590(128, reinterpret_cast<u8*>(mFontPalette), &mFontContext);

        gObjList_drawables_5C1124->Push_Back(this);
    }

    ~DebugPathRenderer()
    {
    }

    static bool Enabled;
    static bool GridEnabled;

    void Destruct()
    {
        BaseGameObject_dtor_4DBEC0();
        gObjList_drawables_5C1124->Remove_Item(this);
    }

    virtual BaseGameObject* VDestructor(s32 flags) override
    {
        Destruct();
        if (flags & 1)
        {
            ae_delete_free_495540(this);
        }
        return this;
    }

    virtual void VUpdate() override
    {
    }

    virtual void VScreenChanged() override
    {
        // Dont kill!
    }

    struct LineColor final
    {
        u8 r;
        u8 g;
        u8 b;
    };
    std::map<s32, LineColor> mLineColors = {
        {0, {255, 0, 0}},      // Floor
        {1, {0, 0, 255}},      // Left Wall
        {3, {255, 0, 0}},      // Ceiling
        {2, {0, 0, 255}},      // Right Wall
        {4, {100, 0, 0}},      // Background Floor
        {5, {0, 0, 100}},      // Background Left Wall
        {6, {0, 0, 100}},      // Background Right Wall
        {8, {0, 255, 0}},      // Path Group
        {9, {255, 255, 255}},  // Comment lines
        {10, {128, 128, 128}}, // Bullet blocking walls
        {17, {255, 255, 0}},   // Flying Slig Collision
        {32, {255, 255, 100}}, // Lift Platform
    };

    virtual void VRender(PrimHeader** ppOt) override
    {
        s32 pIndex = 0;

        bool gridSemiTrans = true;

        if (GridEnabled)
        {
            const s32 gridX = 25 / 2;
            const s32 gridY = 20;
            const Layer layer = Layer::eLayer_BeforeWell_22;

            for (s32 y = 0; y < 12; y++)
            {
                for (s32 x = 0; x < 15; x++)
                {
                    s8 c = ((x + y) % 2 == 0) ? 200 : 127;
                    for (s32 i = -1; i < 2; i++)
                    {
                        DEV::DebugDrawLine(ppOt, layer, (x * gridX) + (gridX / 2) + i, y * gridY, (x * gridX) + (gridX / 2) + i, (y * gridY) + (gridY / 4), 255, 255, 255, false, gridSemiTrans);
                    }

                    if (x == 0)
                    {
                        DEV::DebugDrawLine(ppOt, layer, 0, y * gridY, 640, y * gridY, 255, 255, 255, false, gridSemiTrans);
                    }

                    if (y == 0)
                    {
                        DEV::DebugDrawLine(ppOt, layer, x * gridX, 0, x * gridX, 240, 255, 255, 255, false, gridSemiTrans);
                        DEV::DebugDrawLine(ppOt, layer, (x * gridX) - 1, 0, (x * gridX) - 1, 240, 255, 255, 255, false, gridSemiTrans);
                    }

                    DEV::DebugFillRect(ppOt, layer, x * gridX, y * gridY, gridX, gridY, c, c, c, false, gridSemiTrans);
                }
            }
        }

        if (Enabled)
        {
            for (s32 i = 0; i < sCollisions_DArray_5C1128->field_C_max_count; i++)
            {
                auto l = &sCollisions_DArray_5C1128->field_0_pArray[i];
                LineColor color = {255, 0, 255};
                s32 mode = l->field_8_type;

                if (mLineColors.find(mode) != mLineColors.end())
                {
                    color = mLineColors[mode];
                }

                Layer fontLayer = Layer::eLayer_Menu_41;
                Layer layer = Layer::eLayer_FadeFlash_40;
                if (mode == 4 || mode == 5 || mode == 6)
                {
                    layer = Layer::eLayer_Well_23;
                    fontLayer = Layer::eLayer_RopeWebDrill_24;
                }
                DEV::DebugDrawLine(ppOt, layer, l->field_0_rect.x, l->field_0_rect.y, l->field_0_rect.w, l->field_0_rect.h, color.r, color.g, color.b, true, false);

                s16 id_x = l->field_0_rect.x - FP_GetExponent(gMap_5C3030.field_24_camera_offset.field_0_x);
                s16 id_y = l->field_0_rect.y - FP_GetExponent(gMap_5C3030.field_24_camera_offset.field_4_y);

                if (id_x > 0 && id_x <= 640 && id_y > 0 && id_y <= 240)
                {
                    pIndex = mFont.DrawString_4337D0(ppOt, std::to_string(mode).c_str(), id_x, id_y, TPageAbr::eBlend_0, 0, 0, fontLayer, 255, 255, 255, pIndex, FP_FromDouble(1.0), 640, 0);
                    pIndex = mFont.DrawString_4337D0(ppOt, std::to_string(mode).c_str(), id_x + 1, id_y + 1, TPageAbr::eBlend_0, 0, 0, fontLayer, 0, 0, 0, pIndex, FP_FromDouble(1.0), 640, 0);
                }
            }
        }
    }

    Alive::Font mFont;
    s8 mFontPalette[32];
    Font_Context mFontContext;
};
bool DebugPathRenderer::Enabled = false;
bool DebugPathRenderer::GridEnabled = false;

struct DebugConsoleMessage final
{
    std::string message;
    s32 time;
    f32 y;
    u8 r, g, b;
};

static std::vector<DebugConsoleMessage> sDebugConsoleMessages;

void ShowDebugConsoleMessage(std::string message, f32 duration, u8 r, u8 g, u8 b)
{
    auto lines = SplitString(message, '\n');

    for (auto& l : lines)
    {
        sDebugConsoleMessages.insert(sDebugConsoleMessages.begin(), {l, static_cast<s32>(30 * duration), 250, r, g, b});
    }
}

void ShowDebugConsoleMessage(std::string message, f32 duration)
{
    ShowDebugConsoleMessage(message, duration, 255, 255, 255);
}

struct DebugConsoleCommand final
{
    std::string command;
    s32 paramsCount;
    std::function<void(const std::vector<std::string>& args)> callback;
    std::string helpText;
};

extern std::vector<DebugConsoleCommand> sDebugConsoleCommands;

void Command_Help(const std::vector<std::string>& /*args*/)
{
    DEV_CONSOLE_MESSAGE("You can call the following: ", 6);
    for (auto c : sDebugConsoleCommands)
    {
        DEV_CONSOLE_MESSAGE(c.command + " - " + c.helpText, 6);
    }
}

void Command_Test(const std::vector<std::string>& args)
{
    DEV_CONSOLE_MESSAGE("You called this with " + std::to_string(args.size()) + " arguments", 5);
}

void Command_Die(const std::vector<std::string>& /*args*/)
{
    auto explosion = ae_new<BaseBomb>();
    if (explosion)
    {
        explosion->ctor_423E70(sControlledCharacter_5C1B8C->field_B8_xpos, sControlledCharacter_5C1B8C->field_BC_ypos, 0, sControlledCharacter_5C1B8C->field_CC_sprite_scale);
    }
}

void Command_Murder(const std::vector<std::string>& /*args*/)
{
    for (s32 baseObjIdx = 0; baseObjIdx < gBaseGameObject_list_BB47C4->Size(); baseObjIdx++)
    {
        BaseGameObject* pBaseGameObject = gBaseGameObject_list_BB47C4->ItemAt(baseObjIdx);

        if (!pBaseGameObject)
        {
            break;
        }

        if (pBaseGameObject == sActiveHero_5C1B68)
        {
            continue;
        }

        if (pBaseGameObject->field_6_flags.Get(BaseGameObject::eIsBaseAliveGameObject_Bit6))
        {
            auto aliveObj =  static_cast<BaseAliveGameObject*>(pBaseGameObject);

            auto explosion = ae_new<BaseBomb>();
            if (explosion)
            {
                explosion->ctor_423E70(aliveObj->field_B8_xpos, aliveObj->field_BC_ypos, 0, aliveObj->field_CC_sprite_scale);
            }
        }
    }
}

bool sHasTeleported = false;
void Command_HelperUpdate()
{
    if (sHasTeleported)
    {
        PSX_Point pos;
        gMap_5C3030.GetCurrentCamCoords_480680(&pos);

        sActiveHero_5C1B68->field_B8_xpos = FP_FromInteger(pos.field_0_x + 184);
        sActiveHero_5C1B68->field_BC_ypos = FP_FromInteger(pos.field_2_y + 60);
        sHasTeleported = false;
        sActiveHero_5C1B68->field_106_current_motion = eAbeMotions::Motion_3_Fall_459B60;
        sActiveHero_5C1B68->field_1AC_flags.Set(Abe::Flags_1AC::e1AC_Bit7_land_softly);
        sActiveHero_5C1B68->field_C2_lvl_number = gMap_5C3030.field_0_current_level;
        sActiveHero_5C1B68->field_C0_path_number = gMap_5C3030.field_2_current_path;
        sActiveHero_5C1B68->field_100_pCollisionLine = nullptr;
        sActiveHero_5C1B68->field_F8_LastLineYPos = sActiveHero_5C1B68->field_BC_ypos;
        sActiveHero_5C1B68->field_CC_sprite_scale = FP_FromDouble(1.0);
        sActiveHero_5C1B68->field_D6_scale = 1;
        FP rX = FP_FromInteger(0);
        FP rY = FP_FromInteger(0);
        PathLine* rUnk = nullptr;

        f32 subDevide = 368 / 10.0f;
        for (s32 i = 0; i < 10; i++)
        {
            s32 centerIndex = ((i + 5) % 10);
            FP xOffset = FP_FromDouble(pos.field_0_x + (subDevide * centerIndex));
            FP yOffset = FP_FromDouble(pos.field_2_y);
            if (sCollisions_DArray_5C1128->Raycast_417A60(xOffset, yOffset,
                                                          xOffset, yOffset + FP_FromDouble(240), &rUnk, &rX, &rY, 1))
            {
                sActiveHero_5C1B68->field_CC_sprite_scale = FP_FromDouble(1.0);
                sActiveHero_5C1B68->field_D6_scale = 1;
                sActiveHero_5C1B68->field_B8_xpos = rX;
                sActiveHero_5C1B68->field_BC_ypos = rY;
                break;
            }
            else if (sCollisions_DArray_5C1128->Raycast_417A60(xOffset, yOffset,
                                                               xOffset, yOffset + FP_FromDouble(240), &rUnk, &rX, &rY, 1 << 4))
            {
                sActiveHero_5C1B68->field_CC_sprite_scale = FP_FromDouble(0.5);
                sActiveHero_5C1B68->field_D6_scale = 0;
                sActiveHero_5C1B68->field_B8_xpos = rX;
                sActiveHero_5C1B68->field_BC_ypos = rY;
                break;
            }
        }

        sControlledCharacter_5C1B8C = sActiveHero_5C1B68;
    }
}

void Command_ToggleBool(bool* var, std::string varName)
{
    *var = !*var;
    DEV_CONSOLE_MESSAGE(varName + " is now " + std::string(((*var) ? "On" : "Off")), 6);
}

void Command_Teleport(const std::vector<std::string>& args)
{
    s16 level = 0;
    if (IsStringNumber(args[0]))
    {
        level = static_cast<s16>(std::stoi(args[0]));
    }
    else
    {
        bool found = false;
        for (s16 i = 0; i < sizeof(sPathData_559660.paths) / sizeof(PathRoot); i++)
        {
            if (!strcmpi(sPathData_559660.paths[i].field_14_lvl_name, args[0].c_str()))
            {
                level = i;
                found = true;
                break;
            }
        }

        if (!found)
        {
            DEV_CONSOLE_MESSAGE("Cannot find level specified!", 6);
        }
    }
    s16 path = static_cast<s16>(std::stoi(args[1]));
    s16 cam = static_cast<s16>(std::stoi(args[2]));
    gMap_5C3030.SetActiveCam_480D30(static_cast<LevelIds>(level), path, cam, CameraSwapEffects::ePlay1FMV_5, 0, 0);


    sHasTeleported = true;

    DEV_CONSOLE_MESSAGE("Teleported", 6);
}

void Command_Event(const std::vector<std::string>& args)
{
    s32 eventId = std::stoi(args[0]);
    if (eventId >= Event::kEventMax)
    {
        DEV_CONSOLE_MESSAGE_C("Invalid event id", 6, 127, 0, 0);
        return;
    }
    Event_Broadcast_422BC0(static_cast<Event>(eventId), sControlledCharacter_5C1B8C);
}

void Command_Menu(const std::vector<std::string>& args)
{
    s32 menuCam = std::stoi(args[0]);
    if (MainMenuController::gMainMenuController != nullptr)
    {
        MainMenuController::gMainMenuController->field_21A_target_button_index = static_cast<s16>(menuCam);
        MainMenuController::gMainMenuController->field_218_target_page_index = static_cast<s16>(MainMenuController::GetPageIndexFromCam_4D05A0(menuCam));
        MainMenuController::gMainMenuController->field_21C_camSwapEffectState = static_cast<camTransEffectState>(menuCam);
        MainMenuController::gMainMenuController->field_23C_T80.Set(MainMenuController::Flags::eBit22_GameSpeakPlaying);
    }
}

void Command_Midi1(const std::vector<std::string>& args)
{
    const u8 arg1 = static_cast<u8>(std::stoi(args[0]));

    SFX_Play_46FA90(arg1, 0);

    DEV_CONSOLE_MESSAGE("Played Midi1", 6);
}

void Command_LoadSave(const std::vector<std::string>& args)
{
    if (pPauseMenu_5C9300 == nullptr)
    {
        pPauseMenu_5C9300 = ae_new<PauseMenu>();
        pPauseMenu_5C9300->ctor_48FB80();
        pPauseMenu_5C9300->field_1C_update_delay = 0;
    }

    std::string filePath = args[0] + ".sav";

    // TODO: Isn't binary, probably bugged
    std::ifstream saveFile(filePath.c_str());

    if (!saveFile.fail())
    {
        saveFile.read(reinterpret_cast<char_type*>(&sActiveQuicksaveData_BAF7F8), sizeof(sActiveQuicksaveData_BAF7F8));
        Quicksave_LoadActive_4C9170();
        saveFile.close();
        DEV_CONSOLE_PRINTF("Loaded Save %s", filePath.c_str());
    }
    else
    {
        DEV_CONSOLE_PRINTF("Failed to load save %s", filePath.c_str());
    }
}

void Command_DDV(const std::vector<std::string>& args)
{
    SND_StopAll_4CB060();

    std::string filePath = args[0] + ".STR";
    while (!DDV_Play_493210(filePath.c_str()))
    {
        if (gAttract_5C1BA0)
        {
            break;
        }

        if (!Display_Full_Screen_Message_Blocking_465820(sPathData_559660.paths[sLevelId_dword_5CA408].field_1A_unused, MessageType::eSkipMovie_1))
        {
            break;
        }
    }
}

void Command_SetState(const std::vector<std::string>& args)
{
    if (sControlledCharacter_5C1B8C->field_4_typeId != AETypes::eAbe_69)
    {
        DEV_CONSOLE_MESSAGE_C("Setting motion not supported on this object (only allowed for abe)!", 6, 255, 0, 0);
        return;
    }

    s16 motion = static_cast<s16>(std::stoi(args[0]));
    Abe* pAbe = static_cast<Abe*>(sControlledCharacter_5C1B8C);
    auto resource = pAbe->MotionToAnimResource_44AAB0(motion);

    if (resource != nullptr)
    {
        pAbe->field_106_current_motion = motion;
        //pAbe->field_20_animation.Set_Animation_Data_409C80(sAbeFrameOffsetTable_554B18[motion], resource);
        DEV_CONSOLE_PRINTF("Set motion to %i", motion);
    }
    else
    {
        DEV_CONSOLE_PRINTF("Cannot set motion to %i! Resource NULL", motion);
    }
}

void Command_Ring(const std::vector<std::string>& args)
{
    s32 ringType = std::stoi(args[0]);

    PSX_RECT rect = {};
    sActiveHero_5C1B68->vGetBoundingRect_424FD0(&rect, 1);
    AbilityRing::Factory_482F80(
        FP_FromInteger((rect.x + rect.w) / 2),
        FP_FromInteger((rect.y + rect.h) / 2),
        static_cast<RingTypes>(ringType), sActiveHero_5C1B68->field_CC_sprite_scale);

    SFX_Play_46FBA0(SoundEffect::PossessEffect_17, 25, 2650);
}

struct DebugKeyBinds final
{
    std::string key;
    std::string command;
};

std::vector<DebugKeyBinds> gDebugKeyBinds;

void Command_Bind(const std::vector<std::string>& args)
{
    std::string key = args[0];

    std::string command;
    for (u32 i = 1; i < args.size(); i++)
    {
        command += args[i];

        if (i != args.size() - 1)
            command += " ";
    }

    gDebugKeyBinds.push_back({key, command});

    DEV_CONSOLE_PRINTF("Added bind for key %s for command: '%s'", key.c_str(), command.c_str());
}

void Command_Spawn(const std::vector<std::string>& args)
{
    std::string objName = args[0];
    TlvItemInfoUnion tlvinfo;
    tlvinfo.all = 0;
    s32 spawnX = FP_GetExponent(sControlledCharacter_5C1B8C->field_B8_xpos) + 50;
    s32 spawnY = FP_GetExponent(sControlledCharacter_5C1B8C->field_BC_ypos);

    FP hitX;
    FP hitY;
    PathLine* pLine;
    if (sCollisions_DArray_5C1128->Raycast_417A60(FP_FromInteger(spawnX), FP_FromInteger(spawnY), FP_FromInteger(spawnX), FP_FromInteger(spawnY + 1000), &pLine, &hitX, &hitY, 1))
    {
        spawnX = FP_GetExponent(hitX);
        spawnY = FP_GetExponent(hitY);
    }

    PSX_Point spawnTopLeft = {static_cast<s16>(spawnX), static_cast<s16>(spawnY - 5)};
    PSX_Point spawnBottomRight = {static_cast<s16>(spawnX + 50), static_cast<s16>(spawnY + 30)};

    TPathFunctionFn factoryFunc = nullptr;
    Path_TLV* factoryTLV = nullptr;

    Path_Mudokon mudPath = {};
    mudPath.field_10_scale = Scale_short::eFull_0;
    mudPath.field_12_state = Mud_State::eScrub_1;
    mudPath.field_16_voice_pitch = (rand() % 1500) - 600;
    mudPath.field_8_top_left = spawnTopLeft;
    mudPath.field_C_bottom_right = spawnBottomRight;

    s8 blankMemory[512];
    memset(blankMemory, 0, sizeof(blankMemory));
    Path_TLV* basicTlvPath = reinterpret_cast<Path_TLV*>(&blankMemory);
    basicTlvPath->field_8_top_left = spawnTopLeft;
    basicTlvPath->field_C_bottom_right = spawnBottomRight;

    factoryTLV = basicTlvPath;

    if (objName == "mud")
    {
        factoryTLV = &mudPath;
        factoryFunc = &Factory_Mudokon_4D8EC0;
    }
    else if (objName == "slig")
    {
        factoryFunc = &Factory_Slig_4D7BC0;
    }
    else if (objName == "nslig")
    {
        factoryFunc = &Factory_NakedSlig_4D95A0;
    }
    else if (objName == "fleech")
    {
        factoryFunc = &Factory_Fleech_4D8C30;
    }
    else if (objName == "slog")
    {
        factoryFunc = &Factory_Slog_4D8B20;
    }
    else if (objName == "elec")
    {
        factoryFunc = &Factory_ElectricWall_4DA020;
    }
    else if (objName == "paramite")
    {
        factoryFunc = &Factory_Paramite_4D9190;
    }
    else if (objName == "uxb")
    {
        factoryFunc = &Factory_UXB_4D8960;
    }
    else if (objName == "mine")
    {
        factoryFunc = &Factory_Mine_4D8890;
    }
    else if (objName == "greeter")
    {
        factoryFunc = &Factory_Greeter_4DAFE0;
    }
    else if (objName == "scrab")
    {
        factoryFunc = &Factory_Scrab_4D9200;
    }
    else if (objName == "fslig")
    {
        factoryFunc = &Factory_FlyingSlig_4D92E0;
    }
    else
    {
        DEV_CONSOLE_PRINTF("Unsupported object: %s\n", objName.c_str());
    }

    if (factoryFunc != nullptr)
    {
        factoryFunc(factoryTLV, nullptr, tlvinfo, LoadMode::LoadResource_2);
        factoryFunc(factoryTLV, nullptr, tlvinfo, LoadMode::ConstructObject_0);
    }
}

std::vector<DebugConsoleCommand> sDebugConsoleCommands = {
    {"help", -1, Command_Help, "Shows what you're looking at"},
    {"test", -1, Command_Test, "Is this thing on?"},
    {"die", -1, Command_Die, "Kills you."},
    {"murder", -1, Command_Murder, "Kill everything around you."},
    {"ddcheat", -1, [](const std::vector<std::string>& /*args*/)
     { Command_ToggleBool(&sCommandLine_DDCheatEnabled_5CA4B5, "DDCheat"); },
     "Toggle DDCheat"},
    {"object_id", -1, [](const std::vector<std::string>& /*args*/)
     { Command_ToggleBool(&ObjectDebugger::Enabled, "Object ID Debugger"); },
     "Shows object id's on screen"},
    {"no_frame_skip", -1, [](const std::vector<std::string>& /*args*/)
     { Command_ToggleBool(&sCommandLine_NoFrameSkip_5CA4D1, "No Frame Skip"); },
     "Toggle No Frame Skip"},
    {"fps", -1, [](const std::vector<std::string>& /*args*/)
     { Command_ToggleBool(&sCommandLine_ShowFps_5CA4D0, "FPS"); },
     "Toggle FPS"},
#if USE_SDL2_SOUND
    {"reverb", -1, [](const std::vector<std::string>& /*args*/)
     { Command_ToggleBool(&gReverbEnabled, "Reverb"); },
     "Toggle Reverb (New Sound Engine)"},
#endif
    {"music", -1, [](const std::vector<std::string>& /*args*/)
     { Command_ToggleBool(&g_DisableMusic, "Disable Music"); },
     "Disable In Game Music"},
    {"raycast", -1, [](const std::vector<std::string>& /*args*/)
     { Command_ToggleBool(&g_EnabledRaycastRendering, "Raycast Debug"); },
     "Toggle Raycast Debug"},
    {"verbose_events", -1, [](const std::vector<std::string>& /*args*/)
     { Command_ToggleBool(&sDebugEnabled_VerboseEvents, "Verbose Events"); },
     "Toggle Verbose Events"},
    {"open_doors", -1, [](const std::vector<std::string>& /*args*/)
     { Cheat_OpenAllDoors(); },
     "Open all doors."},
    {"teleport", 3, Command_Teleport, "Teleport to a cam. (LEVEL, PATH, CAM)"},
    {"event", 1, Command_Event, "Broadcast's an event (EVENT ID)"},
    //{ "menu", 1, Command_Menu, "Changes to given menu cam" },
    {"motion", 1, Command_SetState, "Sets currently controlled objects motion."},
    {"ddv", 1, Command_DDV, "Plays a ddv"},
    {"spawn", 1, Command_Spawn, "Spawns an object"},
    {"loadsave", 1, Command_LoadSave, "Loads a Save"},
    {"bind", -1, Command_Bind, "Binds a key to a command"},
    {"ring", 1, Command_Ring, "Emits a ring"},
    {"midi1", 1, Command_Midi1, "Play sound using midi func 1"},
    {"path_lines", -1, [](const std::vector<std::string>& /*args*/)
     { Command_ToggleBool(&DebugPathRenderer::Enabled, "Path Lines"); },
     "Renders path lines on screen"},
    {"grid", -1, [](const std::vector<std::string>& /*args*/)
     { Command_ToggleBool(&DebugPathRenderer::GridEnabled, "Grid"); },
     "Renders grid on screen"},
    {"pcopen", -1, [](const std::vector<std::string>& /*args*/)
     { Command_ToggleBool(reinterpret_cast<bool*>(&sbEnable_PCOpen_5CA4B0), "PCOpen"); },
     "Toggles PCOpen"},
};
//

class DebugConsole final : public BaseGameObject
{
public:
    DebugConsole()
    {
        DisableVTableHack disableHack;

        BaseGameObject_ctor_4DBFA0(1, 1);
        field_6_flags.Set(BaseGameObject::eDrawable_Bit4);
        field_6_flags.Set(BaseGameObject::eSurviveDeathReset_Bit9);
        field_4_typeId = AETypes::eDebugConsole_1002;

        mFontContext.LoadFontTypeCustom(reinterpret_cast<File_Font*>(sDebugFont), reinterpret_cast<Font_AtlasEntry*>(sDebugFontAtlas), mFontPalette);
        mFont.ctor_433590(4096 * 2, reinterpret_cast<u8*>(mFontPalette), &mFontContext);

        gObjList_drawables_5C1124->Push_Back(this);

        InitDebugFont();

        Map::LoadResource_4DBE00("ABEBLOW.BAN", ResourceManager::Resource_Animation, kAbeblowResID, LoadMode::ConstructObject_0, 0);
        Map::LoadResource_4DBE00("EXPLODE.BND", ResourceManager::Resource_Animation, kAbebombResID, LoadMode::ConstructObject_0, 0);
        Map::LoadResource_4DBE00("EXPLODE.BND", ResourceManager::Resource_Animation, kDebrisID00, LoadMode::ConstructObject_0, 0);
        Map::LoadResource_4DBE00("EXPLODE.BND", ResourceManager::Resource_Animation, kBgexpldResID, LoadMode::ConstructObject_0, 0);
    }

    void Destruct()
    {
        BaseGameObject_dtor_4DBEC0();
        gObjList_drawables_5C1124->Remove_Item(this);
    }

    virtual BaseGameObject* VDestructor(s32 flags) override
    {
        Destruct();
        if (flags & 1)
        {
            ae_delete_free_495540(this);
        }
        return this;
    }

    void ParseCommand(std::string command)
    {
        command = StringToLower(command);
        auto commandSplit = SplitString(command, ' ');

        if (!commandSplit.empty())
        {
            for (const auto& c : sDebugConsoleCommands)
            {
                if (commandSplit[0] == c.command)
                {
                    commandSplit.erase(commandSplit.begin());

                    if (c.paramsCount == -1 || c.paramsCount == static_cast<s32>(commandSplit.size()))
                    {
                        c.callback(commandSplit);
                    }
                    else
                    {
                        DEV_CONSOLE_MESSAGE_C("Command '" + c.command + "' was expecting " + std::to_string(c.paramsCount) + " args but got " + std::to_string(commandSplit.size()), 6, 127, 0, 0);
                    }

                    return;
                }
            }

            DEV_CONSOLE_MESSAGE_C("Unknown command '" + command + "' Type help for more info.", 6, 127, 0, 0);
        }
    }

    std::string GetAutoComplete(std::string s)
    {
        s = StringToLower(s);
        for (auto c : sDebugConsoleCommands)
        {
            if (StringStartsWith(c.command, s))
            {
                return c.command;
            }
        }

        return "";
    }

    virtual void VUpdate() override
    {
        if (g_DisableMusic)
        {
            MusicController::EnableMusic_47FE10(FALSE);
        }

        static bool hasRunAutorun = false;
        static s32 autoRunWait = 1;
        if (!hasRunAutorun && autoRunWait <= 0)
        {
            // Runs some commands on game startup
            auto autoRun = FS::ReadFile("autorun.conf");
            autoRun.push_back(0);

            if (!autoRun.empty())
            {
                std::string str = std::string(reinterpret_cast<const char_type*>(autoRun.data()));

                str.erase(std::remove(str.begin(), str.end(), '\r'), str.end());

                auto lines = SplitString(str, '\n');

                for (auto& command : lines)
                {
                    ParseCommand(command);
                }
            }

            hasRunAutorun = true;
        }

        autoRunWait--;

        char_type titleBuffer[1000];
        char_type camBuffer[32];
        Path_Format_CameraName_460FB0(
            camBuffer,
            gMap_5C3030.field_0_current_level,
            gMap_5C3030.field_2_current_path,
            gMap_5C3030.field_4_current_camera);
        sprintf(titleBuffer, "Oddworld Abe's Exoddus DEV MODE - %s", camBuffer);
        Sys_SetWindowText(Sys_GetHWnd_4F2C70(), titleBuffer);

        Command_HelperUpdate();

        const char_type key = static_cast<char_type>(Input_GetLastPressedKey_492610());
        if (Input_IsVKPressed_4EDD40(VK_OEM_3))
        {
            mCommandLineEnabled = !mCommandLineEnabled;

            if (mCommandLineEnabled)
            {
                Input_DisableInputForPauseMenuAndDebug_4EDDC0();
            }
            else
            {
                Input_EnableInput_4EDDD0();
            }
        }


#if USE_SDL2
        static Uint8 keyStatesPrev[256];
        auto keyStates = SDL_GetKeyboardState(nullptr);


        for (const auto& bind : gDebugKeyBinds)
        {
            auto scanCode = SDL_GetScancodeFromKey(SDL_GetKeyFromName(bind.key.c_str()));
            if (keyStates[scanCode] && !keyStatesPrev[scanCode])
            {
                ParseCommand(bind.command);
            }
        }

        memcpy(keyStatesPrev, keyStates, sizeof(keyStatesPrev));
#endif

        const char_type* allowedChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789 .!-+@#$%^&*()_\"'";

        if (Input_IsVKPressed_4EDD40(VK_UP) && mCommandLineEnabled)
        {
            mCommandLineInput = mLastCommand;
        }

        if (mCommandLineEnabled && key > 0)
        {
            switch (key)
            {
                case VK_BACK:
                    if (mCommandLineInput.size() > 0)
                    {
                        mCommandLineInput.pop_back();
                    }
                    break;
                case VK_RETURN:
                    if (mCommandLineInput.size() > 0)
                    {
                        ParseCommand(mCommandLineInput);
                        mLastCommand = mCommandLineInput;
                        mCommandLineInput.clear();
                    }

                    mCommandLineEnabled = false;
                    Input_EnableInput_4EDDD0();
                    Input_Reset_492660();
                    break;
                case VK_TAB:
                    if (mAutoComplete.size() > 0)
                    {
                        mCommandLineInput = mAutoComplete;
                    }
                    break;
                default:
                    if (strspn(&key, allowedChars))
                    {
                        mCommandLineInput += key;
                    }
                    break;
            }

            if (mCommandLineInput.size() > 0)
            {
                mAutoComplete = GetAutoComplete(mCommandLineInput);
            }
            else
            {
                mAutoComplete = "";
            }
        }
    }

    virtual void VScreenChanged() override
    {
        // Dont kill!
    }

    virtual void VRender(PrimHeader** ppOt) override
    {
        s32 pIndex = 0;

        s32 i = 0;
        for (std::vector<DebugConsoleMessage>::iterator it = sDebugConsoleMessages.begin();
             it != sDebugConsoleMessages.end();
             /*it++*/)
        {
            auto message = it;
            s32 targetY = 232 - (i * 9) - 9;

            message->y = static_cast<f32>(targetY);

            pIndex = mFont.DrawString_4337D0(ppOt, message->message.c_str(), 0, static_cast<s16>(message->y), TPageAbr::eBlend_0, 1, 0, Layer::eLayer_FadeFlash_40, message->r, message->g, message->b, pIndex, FP_FromDouble(1.0), 640, 0);
            pIndex = mFont.DrawString_4337D0(ppOt, message->message.c_str(), 1, static_cast<s16>(message->y) + 1, TPageAbr::eBlend_0, 1, 0, Layer::eLayer_FadeFlash_40, 0, 0, 0, pIndex, FP_FromDouble(1.0), 640, 0);

            message->time--;

            if (message->time <= 0 || i > 64)
            {
                it = sDebugConsoleMessages.erase(it);
            }
            else
            {
                i++;
                ++it;
            }
        }

        if (mCommandLineEnabled)
        {
            std::string trail = (sGnFrame_5C1B84 % 10 < 5) ? "" : "_";
            pIndex = mFont.DrawString_4337D0(ppOt, (">" + mCommandLineInput + trail).c_str(), 0, 232, TPageAbr::eBlend_0, 1, 0, Layer::eLayer_FadeFlash_40, 255, 255, 255, pIndex, FP_FromDouble(1.0), 640, 0);
            pIndex = mFont.DrawString_4337D0(ppOt, (" " + mAutoComplete).c_str(), 0, 232, TPageAbr::eBlend_0, 1, 0, Layer::eLayer_FadeFlash_40, 30, 30, 30, pIndex, FP_FromDouble(1.0), 640, 0);
        }
    }

    Alive::Font mFont;
    s8 mFontPalette[32];
    Font_Context mFontContext;

    bool mCommandLineEnabled = false;
    std::string mCommandLineInput;
    std::string mAutoComplete;
    std::string mLastCommand;
};

struct PsxTimHeader final
{
    u32 mMagic; // 0x10
    u32 mFlag;  // 0x08 4BPP, 0x09 8BPP, 0x02 16BPP
    u32 mUnknown;
    u16 mClutX;
    u16 mClutY;
    u16 mNumClutColours;
    u16 mClutCount;
};

struct PsxTimImageHeader final
{
    u32 mSizeInBytes; // includes header size
    PSX_RECT mImageRect;
};

struct TimInfo final
{
    s16 mRenderWidth;
    s16 mHeight;
    u16 mTPage;
    u16 mClut;
};

static void LoadTIM(TimInfo* pInfo, const u8* timBuffer, TPageAbr abr)
{
    const PsxTimHeader* pHeader = reinterpret_cast<const PsxTimHeader*>(timBuffer);
    const PsxTimImageHeader* pImgHeader = nullptr;
    if (pHeader->mFlag == 2) // 16 bit
    {
        pImgHeader = reinterpret_cast<const PsxTimImageHeader*>(timBuffer + (sizeof(u32) * 2)); // skip magic marker and flags
        pInfo->mClut = 0;
    }
    else
    {
        s32 clutSkip = pHeader->mClutCount * pHeader->mNumClutColours * 2;
        pImgHeader = reinterpret_cast<const PsxTimImageHeader*>(timBuffer + sizeof(PsxTimHeader) + clutSkip);

        PSX_RECT clutRect = {static_cast<s16>(pHeader->mClutX), static_cast<s16>(pHeader->mClutY), static_cast<s16>(pHeader->mNumClutColours), static_cast<s16>(1)};
        PSX_LoadImage16_4F5E20(&clutRect, (u8*) &pHeader[1]);

        pInfo->mClut = static_cast<u16>(PSX_getClut_4F6350(pHeader->mClutX, pHeader->mClutY));
    }

    if (pHeader->mFlag == 2) // 16 bit
    {
        // Raw pixel data, convert it
        PSX_LoadImage16_4F5E20(&pImgHeader->mImageRect, (u8*) &pImgHeader[1]);
    }
    else
    {
        // Bytes or nibbles of pal indices, don't convert it
        PSX_LoadImage_4F5FB0(&pImgHeader->mImageRect, (u8*) &pImgHeader[1]);
    }

    TPageMode mode = TPageMode::e16Bit_2;
    if (pHeader->mFlag == 2) // 16 bit
    {
        mode = TPageMode::e16Bit_2;
    }
    else if (pHeader->mFlag == 9) // 8 bit
    {
        mode = TPageMode::e8Bit_1;
    }
    else if (pHeader->mFlag == 8) // 4 bit
    {
        mode = TPageMode::e4Bit_0;
    }
    else
    {
        ALIVE_FATAL("Unknown TIM flags");
    }

    s32 widthMultipler = 1;
    if (mode == TPageMode::e4Bit_0)
    {
        widthMultipler = 4;
    }
    else if (mode == TPageMode::e8Bit_1)
    {
        widthMultipler = 2;
    }

    pInfo->mRenderWidth = static_cast<s16>(pImgHeader->mImageRect.w * widthMultipler);
    pInfo->mHeight = pImgHeader->mImageRect.h;
    pInfo->mTPage = static_cast<u16>(PSX_getTPage_4F60E0(mode, abr, pImgHeader->mImageRect.x, pImgHeader->mImageRect.y));
}

class RenderTest_AllPrims final
{
public:
    RenderTest_AllPrims()
    {
        InitTestRender();
    }

    void Render(PrimHeader** ppOt)
    {
        static PSX_Pos16 xy = {};
        static s16 ypos = 0;
        ypos++;
        if (ypos > 30)
        {
            ypos = 0;
        }
        xy.x = ypos;
        xy.y = ypos;
        InitType_ScreenOffset_4F5BB0(&mScreenOffset, &xy);
        OrderingTable_Add_4F8AA0(OtLayer(ppOt, Layer::eLayer_InBirdPortal_30), &mScreenOffset.mBase);

        static PSX_RECT clipRect = {};
        clipRect.x = 80;
        clipRect.y = 50;
        clipRect.w = 640 - 300;
        clipRect.h = 480 - 200;

        Init_PrimClipper_4F5B80(&mPrimClipper, &clipRect);
        // OrderingTable_Add_4F8AA0(&pOrderingTable[30], &mPrimClipper.field_0_header);

        // Tiles
        for (s32 i = 0; i < 10; i++)
        {
            OrderingTable_Add_4F8AA0(OtLayer(ppOt, Layer::eLayer_InBirdPortal_30), &mTiles[i].mBase.header);
        }
        OrderingTable_Add_4F8AA0(OtLayer(ppOt, Layer::eLayer_InBirdPortal_30), &mTile8.mBase.header);
        OrderingTable_Add_4F8AA0(OtLayer(ppOt, Layer::eLayer_InBirdPortal_30), &mTile16.mBase.header);
        OrderingTable_Add_4F8AA0(OtLayer(ppOt, Layer::eLayer_InBirdPortal_30), &mTile.mBase.header);

        // Sprites
        OrderingTable_Add_4F8AA0(OtLayer(ppOt, Layer::eLayer_InBirdPortal_30), &mSprt8.mBase.header);
        OrderingTable_Add_4F8AA0(OtLayer(ppOt, Layer::eLayer_InBirdPortal_30), &mSprt8_TPage.mBase);

        OrderingTable_Add_4F8AA0(OtLayer(ppOt, Layer::eLayer_InBirdPortal_30), &mSprt16.mBase.header);
        OrderingTable_Add_4F8AA0(OtLayer(ppOt, Layer::eLayer_InBirdPortal_30), &mSprt16_TPage.mBase);

        OrderingTable_Add_4F8AA0(OtLayer(ppOt, Layer::eLayer_InBirdPortal_30), &mSprt.mBase.header);
        OrderingTable_Add_4F8AA0(OtLayer(ppOt, Layer::eLayer_InBirdPortal_30), &mSprt_TPage.mBase);

        // Lines
        OrderingTable_Add_4F8AA0(OtLayer(ppOt, Layer::eLayer_InBirdPortal_30), &mLineF2.mBase.header);
        OrderingTable_Add_4F8AA0(OtLayer(ppOt, Layer::eLayer_InBirdPortal_30), &mLineF3.mBase.header);
        OrderingTable_Add_4F8AA0(OtLayer(ppOt, Layer::eLayer_InBirdPortal_30), &mLineF4.mBase.header);

        OrderingTable_Add_4F8AA0(OtLayer(ppOt, Layer::eLayer_InBirdPortal_30), &mLineG2.mBase.header);
        OrderingTable_Add_4F8AA0(OtLayer(ppOt, Layer::eLayer_InBirdPortal_30), &mLineG3.mBase.header);
        OrderingTable_Add_4F8AA0(OtLayer(ppOt, Layer::eLayer_InBirdPortal_30), &mLineG4.mBase.header);

        for (s32 i = 0; i < 4; i++)
        {
            OrderingTable_Add_4F8AA0(OtLayer(ppOt, Layer::eLayer_InBirdPortal_30), &mPolyFT4[i].mBase.header);
        }

        OrderingTable_Add_4F8AA0(OtLayer(ppOt, Layer::eLayer_InBirdPortal_30), &mPolyGT4.mBase.header);
        OrderingTable_Add_4F8AA0(OtLayer(ppOt, Layer::eLayer_InBirdPortal_30), &mPolyF4.mBase.header);
        OrderingTable_Add_4F8AA0(OtLayer(ppOt, Layer::eLayer_InBirdPortal_30), &mPolyG4.mBase.header);

        // Polys
        OrderingTable_Add_4F8AA0(OtLayer(ppOt, Layer::eLayer_InBirdPortal_30), &mPolyF3.mBase.header);
        OrderingTable_Add_4F8AA0(OtLayer(ppOt, Layer::eLayer_InBirdPortal_30), &mPolyG3.mBase.header);
        OrderingTable_Add_4F8AA0(OtLayer(ppOt, Layer::eLayer_InBirdPortal_30), &mPolyFT3.mBase.header);
        OrderingTable_Add_4F8AA0(OtLayer(ppOt, Layer::eLayer_InBirdPortal_30), &mPolyGT3.mBase.header);
    }

private:
    void InitTestRender()
    {
        {
            PolyG3_Init_4F8890(&mPolyG3);

            SetRGB0(&mPolyG3, 255, 0, 0);
            SetRGB1(&mPolyG3, 0, 255, 0);
            SetRGB2(&mPolyG3, 0, 0, 255);

            SetXY0(&mPolyG3, 180, 190);
            SetXY1(&mPolyG3, 180, 190 + 20);
            SetXY2(&mPolyG3, 180 + (20 * 2), 190);
        }

        {
            PolyF3_Init(&mPolyF3);

            SetRGB0(&mPolyF3, 255, 255, 0);

            SetXY0(&mPolyF3, 240, 190);
            SetXY1(&mPolyF3, 240, 190 + 20);
            SetXY2(&mPolyF3, 240 + (20 * 2), 190);
        }

        {
            PolyFT3_Init(&mPolyFT3);

            TimInfo timInfo = {};
            LoadTIM(&timInfo, &tim_16_bit[0], TPageAbr::eBlend_3);

            SetRGB0(&mPolyFT3, 127, 127, 127);

            Poly_Set_Blending_4F8A20(&mPolyFT3.mBase.header, 1);
            Poly_Set_SemiTrans_4F8A60(&mPolyFT3.mBase.header, 1);
            SetTPage(&mPolyFT3, timInfo.mTPage);
            SetClut(&mPolyFT3, timInfo.mClut);

            const s16 xpos = 30;
            const s16 ypos = 160;
            const s16 w = timInfo.mRenderWidth * 2; // All width doubled due to PC doubling the render width
            const s16 h = timInfo.mHeight;

            SetXY0(&mPolyFT3, xpos, ypos);
            SetXY1(&mPolyFT3, xpos, ypos + h);
            SetXY2(&mPolyFT3, xpos + w, ypos);

            // This assumes the texture data is at 0,0 in the active texture page
            SetUV0(&mPolyFT3, 0, 0);
            SetUV1(&mPolyFT3, 0, static_cast<u8>(timInfo.mHeight));
            SetUV2(&mPolyFT3, static_cast<u8>(timInfo.mRenderWidth), 0);
        }

        {
            PolyGT3_Init(&mPolyGT3);

            TimInfo timInfo = {};
            LoadTIM(&timInfo, &tim_16_bit[0], TPageAbr::eBlend_0);

            SetRGB0(&mPolyGT3, 255, 0, 255);
            // Much like PolyGT4 these have no effect, so same behavior as PolyFT3
            SetRGB1(&mPolyGT3, 255, 0, 0);
            SetRGB2(&mPolyGT3, 0, 255, 0);

            Poly_Set_Blending_4F8A20(&mPolyGT3.mBase.header, 0);
            Poly_Set_SemiTrans_4F8A60(&mPolyGT3.mBase.header, 0);
            SetTPage(&mPolyGT3, timInfo.mTPage);
            SetClut(&mPolyGT3, timInfo.mClut);

            const s16 xpos = 180;
            const s16 ypos = 90;
            const s16 w = timInfo.mRenderWidth * 2; // All width doubled due to PC doubling the render width
            const s16 h = timInfo.mHeight;

            SetXY0(&mPolyGT3, xpos, ypos);
            SetXY1(&mPolyGT3, xpos, ypos + h);
            SetXY2(&mPolyGT3, xpos + w, ypos);

            // This assumes the texture data is at 0,0 in the active texture page
            SetUV0(&mPolyGT3, 0, 0);
            SetUV1(&mPolyGT3, 0, static_cast<u8>(timInfo.mHeight));
            SetUV2(&mPolyGT3, static_cast<u8>(timInfo.mRenderWidth), 0);
        }

        {
            PolyF4_Init_4F8830(&mPolyF4);

            SetRGB0(&mPolyF4, 255, 255, 255);

            SetXY0(&mPolyF4, 180, 160);
            SetXY1(&mPolyF4, 180, 160 + 20);
            SetXY2(&mPolyF4, 180 + (20 * 2), 160);
            SetXY3(&mPolyF4, 180 + (20 * 2), 160 + 20);
        }

        {
            for (s16 i = 0; i < 4; i++)
            {
                PolyFT4_Init(&mPolyFT4[i]);

                TimInfo timInfo = {};
                if (i == 0)
                {
                    LoadTIM(&timInfo, &tim_16_bit[0], TPageAbr::eBlend_0);
                    Poly_Set_Blending_4F8A20(&mPolyFT4[i].mBase.header, 0);
                    Poly_Set_SemiTrans_4F8A60(&mPolyFT4[i].mBase.header, 0);
                }
                else if (i == 1)
                {
                    LoadTIM(&timInfo, &tim_8_bit[0], TPageAbr::eBlend_1);
                    Poly_Set_Blending_4F8A20(&mPolyFT4[i].mBase.header, 1);
                    Poly_Set_SemiTrans_4F8A60(&mPolyFT4[i].mBase.header, 1);
                }
                else if (i == 2)
                {
                    LoadTIM(&timInfo, &tim_8_bit2[0], TPageAbr::eBlend_2);
                    Poly_Set_Blending_4F8A20(&mPolyFT4[i].mBase.header, 1);
                    Poly_Set_SemiTrans_4F8A60(&mPolyFT4[i].mBase.header, 1);
                }
                else
                {
                    LoadTIM(&timInfo, &tim_4_bit[0], TPageAbr::eBlend_3);
                    Poly_Set_Blending_4F8A20(&mPolyFT4[i].mBase.header, 1);
                    Poly_Set_SemiTrans_4F8A60(&mPolyFT4[i].mBase.header, 1);
                }

                SetRGB0(&mPolyFT4[i], 127, 127, 127);
                SetTPage(&mPolyFT4[i], timInfo.mTPage);
                SetClut(&mPolyFT4[i], timInfo.mClut);

                const s16 xpos = 30 + (150 * i);
                const s16 ypos = 20;
                const s16 w = timInfo.mRenderWidth * 2; // All width doubled due to PC doubling the render width
                const s16 h = timInfo.mHeight;

                SetXY0(&mPolyFT4[i], xpos, ypos);
                SetXY1(&mPolyFT4[i], xpos, ypos + h);
                SetXY2(&mPolyFT4[i], xpos + w, ypos);
                SetXY3(&mPolyFT4[i], xpos + w, ypos + h);

                // This assumes the texture data is at 0,0 in the active texture page
                SetUV0(&mPolyFT4[i], 0, 0);
                SetUV1(&mPolyFT4[i], 0, static_cast<u8>(timInfo.mHeight));
                SetUV2(&mPolyFT4[i], static_cast<u8>(timInfo.mRenderWidth), 0);
                SetUV3(&mPolyFT4[i], static_cast<u8>(timInfo.mRenderWidth), static_cast<u8>(timInfo.mHeight));
            }
        }

        {
            PolyGT4_Init(&mPolyGT4);

            TimInfo timInfo = {};
            LoadTIM(&timInfo, &tim_16_bit[0], TPageAbr::eBlend_0);

            // So it appears that only RGB0 changes the colour, so GT4 behaves the same
            // as FT4.
            SetRGB0(&mPolyGT4, 0, 255, 0);
            SetRGB1(&mPolyGT4, 0, 0, 0);
            SetRGB2(&mPolyGT4, 0, 0, 0);
            SetRGB3(&mPolyGT4, 0, 0, 0);

            Poly_Set_Blending_4F8A20(&mPolyGT4.mBase.header, 0);
            Poly_Set_SemiTrans_4F8A60(&mPolyGT4.mBase.header, 1);
            SetTPage(&mPolyGT4, timInfo.mTPage);
            SetClut(&mPolyGT4, timInfo.mClut);

            const s16 xpos = 30;
            const s16 ypos = 90;
            const s16 w = timInfo.mRenderWidth * 2; // All width doubled due to PC doubling the render width
            const s16 h = timInfo.mHeight;

            SetXY0(&mPolyGT4, xpos, ypos);
            SetXY1(&mPolyGT4, xpos, ypos + h);
            SetXY2(&mPolyGT4, xpos + w, ypos);
            SetXY3(&mPolyGT4, xpos + w, ypos + h);

            // This assumes the texture data is at 0,0 in the active texture page
            SetUV0(&mPolyGT4, 0, 0);
            SetUV1(&mPolyGT4, 0, static_cast<u8>(timInfo.mHeight));
            SetUV2(&mPolyGT4, static_cast<u8>(timInfo.mRenderWidth), 0);
            SetUV3(&mPolyGT4, static_cast<u8>(timInfo.mRenderWidth), static_cast<u8>(timInfo.mHeight));
        }

        {
            PolyG4_Init_4F88B0(&mPolyG4);

            SetRGB0(&mPolyG4, 255, 0, 0);
            SetRGB1(&mPolyG4, 0, 0, 255);
            SetRGB2(&mPolyG4, 0, 255, 0);
            SetRGB3(&mPolyG4, 255, 0, 255);

            SetXY0(&mPolyG4, 240, 160);
            SetXY1(&mPolyG4, 240, 160 + 20);
            SetXY2(&mPolyG4, 240 + (20 * 2), 160);
            SetXY3(&mPolyG4, 240 + (20 * 2), 160 + 20);
        }

        {
            Line_F2_Init(&mLineF2);
            SetRGB0(&mLineF2, 0, 0, 255);
            SetXY0(&mLineF2, 30, 9);
            SetXY1(&mLineF2, 30 + 80, 9);
        }

        {
            Line_F3_Init(&mLineF3);
            SetRGB0(&mLineF3, 55, 255, 55);
            SetXY0(&mLineF3, 30, 11);
            SetXY1(&mLineF3, 30 + 80, 11);
            SetXY2(&mLineF3, 30 + 80, 15);
        }

        {
            Line_F4_Init(&mLineF4);
            SetRGB0(&mLineF4, 55, 155, 155);

            SetXY0(&mLineF4, 30, 18);
            SetXY1(&mLineF4, 30 + 140, 18);
            SetXY2(&mLineF4, 30 + 140, 85);
            SetXY3(&mLineF4, 30, 85);
        }

        {
            LineG2_Init(&mLineG2);
            SetRGB0(&mLineG2, 255, 255, 0);
            SetRGB1(&mLineG2, 255, 0, 255);

            SetXY0(&mLineG2, 320, 90);
            SetXY1(&mLineG2, 420, 90);
        }

        {
            LineG3_Init(&mLineG3);

            SetRGB0(&mLineG3, 55, 55, 90);
            SetRGB1(&mLineG3, 155, 60, 255);
            SetRGB2(&mLineG3, 50, 255, 255);

            SetXY0(&mLineG3, 320, 120);
            SetXY1(&mLineG3, 420, 120);
            SetXY2(&mLineG3, 320, 100);
        }

        {
            LineG4_Init(&mLineG4);

            SetRGB0(&mLineG4, 255, 0, 0);
            SetRGB1(&mLineG4, 0, 255, 0);
            SetRGB2(&mLineG4, 0, 0, 255);
            SetRGB3(&mLineG4, 255, 0, 255);

            SetXY0(&mLineG4, 320, 140);
            SetXY1(&mLineG4, 420, 140);
            SetXY2(&mLineG4, 320, 180);
            SetXY3(&mLineG4, 420, 180);
        }

        {
            for (u8 i = 0; i < 10; i++)
            {
                Init_Tile1(&mTiles[i]);
                SetRGB0(&mTiles[i], 255, i * 12, i * 12);
                SetXY0(&mTiles[i], 30 + (i * 5), 10);
            }
        }

        {
            Init_Tile8(&mTile8);
            SetRGB0(&mTile8, 55, 80, 55);
            SetXY0(&mTile8, 180, 5);
        }

        {
            Init_Tile16(&mTile16);
            SetRGB0(&mTile16, 100, 255, 0);
            SetXY0(&mTile16, 200, 2);
        }

        {
            Init_Tile(&mTile);
            SetRGB0(&mTile, 30, 50, 90);
            SetXY0(&mTile, 450, 80);
            mTile.field_14_w = 50;
            mTile.field_16_h = 90;
        }

        {
            Init_Sprt_8(&mSprt8);
            SetRGB0(&mSprt8, 127, 127, 127);
            SetXY0(&mSprt8, 520, 80);

            TimInfo timInfo = {};
            LoadTIM(&timInfo, &tim_16_bit[0], TPageAbr::eBlend_0);

            SetClut(&mSprt8, timInfo.mClut);
            SetUV0(&mSprt8, 30, 30);

            Init_SetTPage_4F5B60(&mSprt8_TPage, 0, 0, timInfo.mTPage);
        }

        {
            Init_Sprt_16(&mSprt16);
            SetRGB0(&mSprt16, 127, 127, 127);
            SetXY0(&mSprt16, 520, 120);

            TimInfo timInfo = {};
            LoadTIM(&timInfo, &tim_16_bit[0], TPageAbr::eBlend_0);

            SetClut(&mSprt8, timInfo.mClut);
            SetUV0(&mSprt16, 30, 30);

            Init_SetTPage_4F5B60(&mSprt16_TPage, 0, 0, timInfo.mTPage);
        }

        {
            Sprt_Init_4F8910(&mSprt);
            SetRGB0(&mSprt, 255, 127, 127);
            SetXY0(&mSprt, 550, 120);

            TimInfo timInfo = {};
            LoadTIM(&timInfo, &tim_16_bit[0], TPageAbr::eBlend_0);

            SetClut(&mSprt, timInfo.mClut);
            SetUV0(&mSprt, 10, 10);

            mSprt.field_14_w = 50;
            mSprt.field_16_h = 50;

            Init_SetTPage_4F5B60(&mSprt_TPage, 0, 0, timInfo.mTPage);
        }
    }

    Line_F2 mLineF2 = {};
    Line_F3 mLineF3 = {};
    Line_F4 mLineF4 = {};

    Line_G2 mLineG2 = {};
    Line_G3 mLineG3 = {};
    Line_G4 mLineG4 = {};

    Poly_G3 mPolyG3 = {};
    Poly_F3 mPolyF3 = {};
    Poly_FT3 mPolyFT3 = {};
    Poly_GT3 mPolyGT3 = {};

    Poly_G4 mPolyG4 = {};
    Poly_F4 mPolyF4 = {};
    Poly_FT4 mPolyFT4[4] = {};
    Poly_GT4 mPolyGT4 = {};

    Prim_Tile_1 mTiles[10] = {};
    Prim_Tile_8 mTile8 = {};
    Prim_Tile_16 mTile16 = {};
    Prim_Tile mTile = {};

    Prim_SetTPage mSprt8_TPage = {};
    Prim_Sprt_8 mSprt8 = {};

    Prim_SetTPage mSprt16_TPage = {};
    Prim_Sprt_16 mSprt16 = {};

    Prim_SetTPage mSprt_TPage = {};
    Prim_Sprt mSprt = {};

    // TODO: Type 2
    // TODO: Type 0x83 (move image?)
    // TODO: Type 0x84 (used in gas rendering)

    Prim_ScreenOffset mScreenOffset = {};
    Prim_PrimClipper mPrimClipper = {};
};

class Poly_F3_Test final
{
public:
    Poly_F3_Test()
    {
        for (auto& poly : mPolys)
        {
            PolyF3_Init(&poly);
        }
        Update();

        SetRGB0(&mPolys[0], 127, 127, 127);
        SetRGB0(&mPolys[1], 255, 0, 0);
        SetRGB0(&mPolys[2], 0, 255, 0);
        SetRGB0(&mPolys[3], 0, 0, 255);

        Poly_Set_SemiTrans_4F8A60(&mPolys[2].mBase.header, TRUE);
        Poly_Set_SemiTrans_4F8A60(&mPolys[3].mBase.header, TRUE);

        Poly_Set_Blending_4F8A20(&mPolys[0].mBase.header, TRUE);
        Poly_Set_Blending_4F8A20(&mPolys[1].mBase.header, TRUE);
    }

    void Render(PrimHeader** ppOt)
    {
        for (s32 i = 0; i < 4; i++)
        {
            OrderingTable_Add_4F8AA0(OtLayer(ppOt, Layer::eLayer_InBirdPortal_30), &mPolys[i].mBase.header);
        }
    }

    void Update()
    {
        mWidth++;
        if (mWidth > 100)
        {
            mWidth = 60;
        }

        s16 xpos = mXPos;
        for (auto& poly : mPolys)
        {
            SetXY0(&poly, xpos, mYPos);
            SetXY1(&poly, xpos, mYPos + mHeight);
            SetXY2(&poly, xpos + (mWidth * 2), mYPos);

            xpos += (mWidth * 2) - 60;
        }
    }

private:
    s16 mWidth = 60;
    s16 mHeight = 150;
    s16 mXPos = 50;
    s16 mYPos = 50 / 2;
    Poly_F3 mPolys[4];
};


class Poly_F_Test final
{
public:
    Poly_F_Test()
    {
        PolyF3_Init(&mPoly_F3);
        SetRGB0(&mPoly_F3, 255, 0, 0);
        Poly_Set_SemiTrans_4F8A60(&mPoly_F3.mBase.header, FALSE);
        Poly_Set_Blending_4F8A20(&mPoly_F3.mBase.header, FALSE);

        SetXY0(&mPoly_F3, 50, 80);
        SetXY1(&mPoly_F3, 300, 200);
        SetXY2(&mPoly_F3, 200, 100);

        PolyF4_Init_4F8830(&mPoly_F4);
        SetRGB0(&mPoly_F4, 255, 0, 255);
        Poly_Set_SemiTrans_4F8A60(&mPoly_F4.mBase.header, FALSE);
        Poly_Set_Blending_4F8A20(&mPoly_F4.mBase.header, FALSE);

        SetXY0(&mPoly_F4, 350, 100);
        SetXY1(&mPoly_F4, 550, 100);
        SetXY2(&mPoly_F4, 300, 200);
        SetXY3(&mPoly_F4, 500, 50);

        for (s32 i = 0; i < 4; i++)
        {
            Init_Tile8(&mPoly_F4_Verts[i]);
        }
        SetRGB0(&mPoly_F4_Verts[0], 255, 0, 0);
        SetXY0(&mPoly_F4_Verts[0], 350, 100);

        SetRGB0(&mPoly_F4_Verts[1], 0, 255, 0);
        SetXY0(&mPoly_F4_Verts[1], 550, 100);

        SetRGB0(&mPoly_F4_Verts[2], 0, 0, 255);
        SetXY0(&mPoly_F4_Verts[2], 300, 200);

        SetRGB0(&mPoly_F4_Verts[3], 255, 255, 255);
        SetXY0(&mPoly_F4_Verts[3], 500, 50);
    }

    void Render(PrimHeader** ppOt)
    {
        for (s32 i = 0; i < 4; i++)
        {
            OrderingTable_Add_4F8AA0(OtLayer(ppOt, Layer::eLayer_InBirdPortal_30), &mPoly_F4_Verts[i].mBase.header);
        }

        OrderingTable_Add_4F8AA0(OtLayer(ppOt, Layer::eLayer_InBirdPortal_30), &mPoly_F3.mBase.header);
        OrderingTable_Add_4F8AA0(OtLayer(ppOt, Layer::eLayer_InBirdPortal_30), &mPoly_F4.mBase.header);
    }

    void Update()
    {
    }

private:
    Poly_F3 mPoly_F3;
    Poly_F4 mPoly_F4;
    Prim_Tile_8 mPoly_F4_Verts[4];
};


class RenderTest final : public BaseGameObject
{
public:
    RenderTest()
    {
        // Don't hack the vtable else our virtuals won't get called and we can't hack the correct one back since we don't know the address of our vtable.
        DisableVTableHack disableHack;

        BaseGameObject_ctor_4DBFA0(1, 1);

        field_6_flags.Set(BaseGameObject::eDrawable_Bit4);
        field_6_flags.Set(BaseGameObject::eUpdatable_Bit2);

        gObjList_drawables_5C1124->Push_Back(this);
    }

    virtual BaseGameObject* VDestructor(s32 flags) override
    {
        Destruct();
        if (flags & 1)
        {
            ae_delete_free_495540(this);
        }
        return this;
    }

    virtual void VUpdate() override
    {
        field_1C_update_delay = 4;
        mPoly_F3_Test.Update();
        mPoly_F_Test.Update();
    }

    virtual void VRender(PrimHeader** ppOt) override
    {
        PSX_RECT screen = {0, 0, 640, 240};
        PSX_ClearImage_4F5BD0(&screen, 0, 0, 0);

        //pScreenManager_5BB5F4->InvalidateRect_40EC10(0, 0, 640, 240);

        mPoly_F_Test.Render(ppOt);

        mPoly_F3_Test.Render(ppOt);

        mAllPrims.Render(ppOt);
    }

    void Destruct()
    {
        gObjList_drawables_5C1124->Remove_Item(this);
    }

private:
    RenderTest_AllPrims mAllPrims;
    Poly_F3_Test mPoly_F3_Test;
    Poly_F_Test mPoly_F_Test;
};

#include "ScreenManager.hpp"
#include "VRam.hpp"

static u16 RGB888toRGB565(u32 r, u32 g, u32 b)
{
    return static_cast<u16>((r >> 3 << 11) + (g >> 2 << 5) + (b >> 3));
}

const u16 kTestImagePal[16] = {
    RGB888toRGB565(237, 28, 36),
    RGB888toRGB565(255, 255, 255),
    RGB888toRGB565(127, 127, 127),
    RGB888toRGB565(63, 72, 204),
    RGB888toRGB565(34, 177, 76),
    RGB888toRGB565(255, 174, 201),
    RGB888toRGB565(255, 127, 39),
    RGB888toRGB565(163, 73, 164),
};

const u8 kTestImg[4][8] = {
    {0, 0, 1, 2, 2, 2, 1, 3},
    {0, 0, 1, 1, 2, 1, 1, 3},
    {4, 4, 5, 1, 2, 1, 6, 7},
    {4, 4, 1, 5, 2, 6, 1, 7},
};

inline static u8 AsByte(u8 nibble1, u8 nibble2)
{
    return (nibble2 << 4) | nibble1;
}

// Pack kTestImg nibbles into bytes
const u8 kTestImage[4][4] = {
    {AsByte(kTestImg[0][0], kTestImg[0][1]), AsByte(kTestImg[0][2], kTestImg[0][3]), AsByte(kTestImg[0][4], kTestImg[0][5]), AsByte(kTestImg[0][6], kTestImg[0][7])},
    {AsByte(kTestImg[1][0], kTestImg[1][1]), AsByte(kTestImg[1][2], kTestImg[1][3]), AsByte(kTestImg[1][4], kTestImg[1][5]), AsByte(kTestImg[1][6], kTestImg[1][7])},
    {AsByte(kTestImg[2][0], kTestImg[2][1]), AsByte(kTestImg[2][2], kTestImg[2][3]), AsByte(kTestImg[2][4], kTestImg[2][5]), AsByte(kTestImg[2][6], kTestImg[2][7])},
    {AsByte(kTestImg[3][0], kTestImg[3][1]), AsByte(kTestImg[3][2], kTestImg[3][3]), AsByte(kTestImg[3][4], kTestImg[3][5]), AsByte(kTestImg[3][6], kTestImg[3][7])},
};

// Pack kTestImage bytes into a RLE compressed buffer
const u8 kTestImageCompressed[] = {
    8,
    0, // u16 width
    4,
    0,            // u16 height
    AsByte(0, 8), // (black pixel count, direct copy count) both are limited to max 16,16 since 4 bits each
    kTestImage[0][0],
    kTestImage[0][1],
    kTestImage[0][2],
    kTestImage[0][3],

    AsByte(0, 8),
    kTestImage[1][0],
    kTestImage[1][1],
    kTestImage[1][2],
    kTestImage[1][3],

    AsByte(0, 8),
    kTestImage[2][0],
    kTestImage[2][1],
    kTestImage[2][2],
    kTestImage[2][3],

    AsByte(0, 8),
    kTestImage[3][0],
    kTestImage[3][1],
    kTestImage[3][2],
    kTestImage[3][3],
};

class AnimRenderTest final : public BaseGameObject
{
public:
    AnimRenderTest()
    {
        DisableVTableHack disableHack;

        BaseGameObject_ctor_4DBFA0(1, 1);

        field_6_flags.Set(BaseGameObject::eDrawable_Bit4);
        field_6_flags.Set(BaseGameObject::eUpdatable_Bit2);

        gObjList_drawables_5C1124->Push_Back(this);

        Init();
    }

    virtual void VRender(PrimHeader** ot) override
    {
        mAnim[0].vRender_40B820(40 + (1 * 85), 40 + (1 * 90), ot, 0, 0);
        mAnim[1].vRender_40B820(40 + (2 * 85), 40 + (1 * 90), ot, 0, 0);
        mAnim[2].vRender_40B820(40 + (1 * 85), 40 + (2 * 90), ot, 0, 0);
        mAnim[3].vRender_40B820(40 + (2 * 85), 40 + (2 * 90), ot, 0, 0);
        mAnim[4].vRender_40B820(180 + 90, 170 + 45, ot, 0, 0);

        OrderingTable_Add_4F8AA0(OtLayer(ot, Layer::eLayer_InBirdPortal_30), &mPolyFT4[0].mBase.header);

        pScreenManager_5BB5F4->InvalidateRect_40EC10(0, 0, 640, 240);
    }

    virtual void VUpdate() override
    {
    }

    virtual BaseGameObject* VDestructor(s32 flags) override
    {
        Destruct();
        if (flags & 1)
        {
            ae_delete_free_495540(this);
        }
        return this;
    }

    void Destruct()
    {
        gObjList_drawables_5C1124->Remove_Item(this);
    }

private:
    void Init()
    {
        const AnimRecord& loadingRec = AnimRec(AnimId::Loading_Icon);
        const AnimRecord& abeGibRec = AnimRec(AnimId::Abe_Body_Gib);
        ResourceManager::LoadResourceFile_49C170(loadingRec.mBanName, nullptr);
        ResourceManager::LoadResourceFile_49C170(abeGibRec.mBanName, nullptr);

        for (s32 i = 0; i < 5; i++)
        {
            SetVTable(&mAnim[i], 0x544290);

            if (i < 2)
            {
                // 4 bit
                mAnimRes[i] = ResourceManager::GetLoadedResource_49C2A0(ResourceManager::Resource_Animation, loadingRec.mResourceId, TRUE, FALSE);
                mAnim[i].Init_40A030(loadingRec.mFrameTableOffset, gObjList_animations_5C1A24, this, loadingRec.mMaxW, loadingRec.mMaxH, mAnimRes[i], 1, 0, 0);
            }
            else
            {
                // 8 bit
                mAnimRes[i] = ResourceManager::GetLoadedResource_49C2A0(ResourceManager::Resource_Animation, abeGibRec.mResourceId, TRUE, FALSE);
                mAnim[i].Init_40A030(abeGibRec.mFrameTableOffset, gObjList_animations_5C1A24, this, abeGibRec.mMaxW, abeGibRec.mMaxH, mAnimRes[i], 1, 0, 0);
            }
            // No 16 bit test case because there are simply no 16bit sprites at all in the game data

            mAnim[i].field_4_flags.Clear(AnimFlags::eBit16_bBlending);
            mAnim[i].field_4_flags.Clear(AnimFlags::eBit15_bSemiTrans);

            mAnim[i].field_14_scale = FP_FromDouble(2.0);

            mAnim[i].field_C_render_layer = Layer::eLayer_MainMenuButton_38;
            mAnim[i].field_B_render_mode = TPageAbr::eBlend_1;

            mAnim[i].field_A_b = 127;
            mAnim[i].field_9_g = 127;
            mAnim[i].field_8_r = 127;
        }

        // 4 bit o
        mAnim[0].field_4_flags.Clear(AnimFlags::eBit16_bBlending);
        mAnim[0].field_4_flags.Clear(AnimFlags::eBit15_bSemiTrans);
        mAnim[0].field_4_flags.Set(AnimFlags::eBit5_FlipX);
        mAnim[0].field_14_scale = FP_FromDouble(1.0);

        // 4 bit s
        mAnim[1].field_4_flags.Clear(AnimFlags::eBit16_bBlending);
        mAnim[1].field_4_flags.Set(AnimFlags::eBit15_bSemiTrans);
        mAnim[1].field_14_scale = FP_FromDouble(2.0);

        // 8 bit o
        mAnim[2].field_4_flags.Clear(AnimFlags::eBit16_bBlending);
        mAnim[2].field_4_flags.Clear(AnimFlags::eBit15_bSemiTrans);
        mAnim[2].field_14_scale = FP_FromDouble(1.0);

        // 8 bit s
        mAnim[3].field_4_flags.Clear(AnimFlags::eBit16_bBlending);
        mAnim[3].field_4_flags.Set(AnimFlags::eBit15_bSemiTrans);

        PSX_RECT pr = {};
        Pal_Allocate_483110(&pr, 16);
        pr.w = 16;
        pr.h = 1;
        PSX_LoadImage16_4F5E20(&pr, (u8*) &kTestImagePal[0]);

        for (s16 i = 0; i < 1; i++)
        {
            PolyFT4_Init(&mPolyFT4[i]);

            SetRGB0(&mPolyFT4[i], 127, 127, 127);
            SetTPage(&mPolyFT4[i], 0);
            SetClut(&mPolyFT4[i], static_cast<s16>(PSX_getClut_4F6350(pr.x, pr.y)));

            const s16 xpos = 20 + (150 * i);
            const s16 ypos = 10;
            const s16 w = 255; // All width doubled due to PC doubling the render width
            const s16 h = 80;

            SetXY0(&mPolyFT4[i], xpos, ypos);
            SetXY1(&mPolyFT4[i], xpos, ypos + h);
            SetXY2(&mPolyFT4[i], xpos, ypos);
            SetXY3(&mPolyFT4[i], xpos + w, ypos + h);

            // This assumes the texture data is at 0,0 in the active texture page
            SetUV0(&mPolyFT4[i], 6, 0);
            SetUV1(&mPolyFT4[i], 2, 0);
            SetUV2(&mPolyFT4[i], 2, 0);
            SetUV3(&mPolyFT4[i], 2, 4);

            const void* ptr = &kTestImageCompressed[0];
            SetPrimExtraPointerHack(&mPolyFT4[i], ptr);
        }
    }

    u8** mAnimRes[5];
    Animation mAnim[5];
    Poly_FT4 mPolyFT4[1];
};

void DebugHelpers_Init()
{
#if DEVELOPER_MODE
    if (Input_IsVKPressed_4EDD40(VK_LCONTROL))
    {
        return;
    }

    //if (IsAlive())
    {
        ae_new<ObjectDebugger>();
        ae_new<DebugConsole>();
        ae_new<DebugPathRenderer>();

        DEV_CONSOLE_MESSAGE_C("Debug Console Active. Open with ~ (Tilde)", 7, 0, 150, 255);
    }
#endif

    //#if RENDER_TEST
    // Test rendering diff prim types
    // ae_new<RenderTest>(); // Will get nuked at LVL/Path change
    //ae_new<AnimRenderTest>();
    //#endif
}

[[nodiscard]] std::vector<u8> FS::ReadFile(const std::string& filePath)
{
    std::vector<u8> buffer;

    if (!ReadFileInto(buffer, filePath))
    {
        return {};
    }

    return buffer;
}

[[nodiscard]] std::string FS::GetPrefPath()
{
#if MOBILE
    s8* prefPath = SDL_GetPrefPath("Oddworld", "Abes Exoddus");
    std::string str = std::string(prefPath);
    SDL_free(prefPath);
    return str;
#else
    return "";
#endif
}

std::vector<std::string> SplitString(const std::string& s, s8 seperator)
{
    std::vector<std::string> output;

    std::string::size_type prev_pos = 0, pos = 0;

    while ((pos = s.find(seperator, pos)) != std::string::npos)
    {
        std::string substring(s.substr(prev_pos, pos - prev_pos));

        output.push_back(substring);

        prev_pos = ++pos;
    }

    output.push_back(s.substr(prev_pos, pos - prev_pos)); // Last word

    return output;
}

bool StringStartsWith(std::string mainStr, std::string toMatch)
{
    // std::string::find returns 0 if toMatch is found at starting
    if (mainStr.find(toMatch) == 0)
        return true;
    else
        return false;
}

std::string StringToLower(std::string s)
{
    std::string r;
    for (auto c : s)
    {
        s8 l = c;
        if (l >= 65 && l <= 90)
        {
            l += 32;
        }

        r += l;
    }

    return r;
}

std::string IntToHexString(s32 v)
{
    std::stringstream ss;
    ss << "0x" << std::hex << v;
    return ss.str();
}

std::string EscapeUnknownCharacters(std::string text)
{
    std::stringstream output;

    for (auto c : text)
    {
        if (c >= 32 && c <= 126)
        {
            output << c;
        }
        else
        {
            output << "\\x" << std::setfill('0') << std::setw(2) << std::hex << (s32) c;
        }
    }

    return output.str();
}

[[nodiscard]] BaseGameObject* FindObjectOfType(AETypes id)
{
    for (s32 baseObjIdx = 0; baseObjIdx < gBaseGameObject_list_BB47C4->Size(); baseObjIdx++)
    {
        BaseGameObject* pBaseGameObject = gBaseGameObject_list_BB47C4->ItemAt(baseObjIdx);

        if (!pBaseGameObject)
        {
            break;
        }

        if (pBaseGameObject->field_4_typeId == id)
            return pBaseGameObject;
    }
    return nullptr;
}

void Cheat_OpenAllDoors()
{
    for (s32 i = 0; i < 256; i++)
    {
        SwitchStates_Set_465FF0(static_cast<u16>(i), 1);
    }

    DEV_CONSOLE_MESSAGE("(CHEAT) All doors opened", 4);
}

s32 sNextLinePrim = 0;
s32 sNextPolyF4Prim = 0;
Line_G2 sLinePrimBuffer[1024];
Poly_F4 sPolyF4PrimBuffer[1024];

void DEV::DebugFillRect(PrimHeader** ot, Layer layer, s32 x, s32 y, s32 width, s32 height, u8 r, u8 g, u8 b, bool worldspace, bool semiTransparent)
{
    Poly_F4* mPolyF4 = &sPolyF4PrimBuffer[++sNextPolyF4Prim];
    *mPolyF4 = {};
    PolyF4_Init_4F8830(mPolyF4);

    const auto camOffset = gMap_5C3030.field_24_camera_offset;

    if (worldspace)
    {
        x -= FP_GetExponent(camOffset.field_0_x);
        y -= FP_GetExponent(camOffset.field_4_y);

        x = static_cast<s32>(x / 0.575);
        y = static_cast<s32>(y / 0.575);
        width = static_cast<s32>(width / 0.575);
        height = static_cast<s32>(height / 0.575);
    }

    SetRGB0(mPolyF4, r, g, b);

    SetXY0(mPolyF4, static_cast<s16>(x), static_cast<s16>(y));
    SetXY1(mPolyF4, static_cast<s16>(x), static_cast<s16>(y + height));
    SetXY2(mPolyF4, static_cast<s16>(x + width), static_cast<s16>(y));
    SetXY3(mPolyF4, static_cast<s16>(x + width), static_cast<s16>(y + height));

    Poly_Set_SemiTrans_4F8A60(&mPolyF4->mBase.header, semiTransparent);

    OrderingTable_Add_4F8AA0(OtLayer(ot, layer), &mPolyF4->mBase.header);
    pScreenManager_5BB5F4->InvalidateRect_40EC10(0, 0, 640, 240);
}

void DEV::DebugDrawRect(PrimHeader** ot, Layer layer, s32 x, s32 y, s32 width, s32 height, u8 r, u8 g, u8 b, bool worldspace, bool semiTransparent)
{
    DebugDrawLine(ot, layer, x, y, x + width, y, r, g, b, worldspace, semiTransparent);
    DebugDrawLine(ot, layer, x + width, y, x + width, y + height, r, g, b, worldspace, semiTransparent);
    DebugDrawLine(ot, layer, x + width, y + height, x, y + height, r, g, b, worldspace, semiTransparent);
    DebugDrawLine(ot, layer, x, y + height, x, y, r, g, b, worldspace, semiTransparent);
}

void DEV::DebugDrawLine(PrimHeader** ot, Layer layer, s32 x1, s32 y1, s32 x2, s32 y2, u8 r, u8 g, u8 b, bool worldspace, bool semiTransparent)
{
    Line_G2* mLineG2 = &sLinePrimBuffer[++sNextLinePrim];
    LineG2_Init(mLineG2);

    const auto camOffset = gMap_5C3030.field_24_camera_offset;

    if (worldspace)
    {
        x1 -= FP_GetExponent(camOffset.field_0_x);
        y1 -= FP_GetExponent(camOffset.field_4_y);

        x2 -= FP_GetExponent(camOffset.field_0_x);
        y2 -= FP_GetExponent(camOffset.field_4_y);

        x1 = static_cast<s32>(x1 / 0.575);
        x2 = static_cast<s32>(x2 / 0.575);
    }

    // TODO: Might as well use Line_F2 here
    SetRGB0(mLineG2, r, g, b);
    SetXY0(mLineG2, static_cast<s16>(x1), static_cast<s16>(y1));

    SetRGB1(mLineG2, r, g, b);
    SetXY1(mLineG2, static_cast<s16>(x2), static_cast<s16>(y2));

    Poly_Set_SemiTrans_4F8A60(&mLineG2->mBase.header, semiTransparent);

    OrderingTable_Add_4F8AA0(OtLayer(ot, layer), &mLineG2->mBase.header);
    pScreenManager_5BB5F4->InvalidateRect_40EC10(0, 0, 640, 240);
}

void DEV::DebugDrawText(PrimHeader** ot, Layer layer, std::string& text, s32 x, s32 y, u8 r, u8 g, u8 b, bool worldspace, bool semiTransparent)
{
    const auto camOffset = gMap_5C3030.field_24_camera_offset;

    if (worldspace)
    {
        x -= FP_GetExponent(camOffset.field_0_x);
        y -= FP_GetExponent(camOffset.field_4_y);
    }

    g_DebugGlobalFontPolyIndex = g_DebugGlobalFont.DrawString_4337D0(ot, text.c_str(), x - (g_DebugGlobalFont.MeasureWidth_433700(text.c_str()) / 2), static_cast<s16>(y), semiTransparent ? TPageAbr::eBlend_1 : TPageAbr::eBlend_0, 0, 0, layer, r, g, b, g_DebugGlobalFontPolyIndex, FP_FromDouble(1.0), 640, 0);
    g_DebugGlobalFontPolyIndex = g_DebugGlobalFont.DrawString_4337D0(ot, text.c_str(), x - (g_DebugGlobalFont.MeasureWidth_433700(text.c_str()) / 2) + 1, static_cast<s16>(y + 1), semiTransparent ? TPageAbr::eBlend_1 : TPageAbr::eBlend_0, 0, 0, layer, 0, 0, 0, g_DebugGlobalFontPolyIndex, FP_FromDouble(1.0), 640, 0);
}

void DEV::DebugOnFrameDraw(PrimHeader** ppOt)
{
    sNextLinePrim = 0;
    sNextPolyF4Prim = 0;
    g_DebugGlobalFontPolyIndex = 0;

    if (g_EnabledRaycastRendering)
    {
        for (auto rc : g_RaycastDebugList)
        {
            if (rc.pLine != nullptr)
            {
                const s32 hitX = FP_GetExponent(rc.hitX);
                const s32 hitY = FP_GetExponent(rc.hitY);

                DEV::DebugDrawLine(ppOt, Layer::eLayer_MainMenuButton_38, FP_GetExponent(rc.x1), FP_GetExponent(rc.y1), hitX, hitY, 255, 255, 0, true, true);


                DEV::DebugDrawLine(ppOt, Layer::eLayer_MainMenuButton_38, hitX - 1, hitY - 1, hitX + 1, hitY + 1, 255, 255, 255, true, true);
                DEV::DebugDrawLine(ppOt, Layer::eLayer_MainMenuButton_38, hitX + 1, hitY - 1, hitX - 1, hitY + 1, 255, 255, 255, true, true);
            }
            else
            {
                DEV::DebugDrawLine(ppOt, Layer::eLayer_MainMenuButton_38, FP_GetExponent(rc.x1), FP_GetExponent(rc.y1), FP_GetExponent(rc.x2), FP_GetExponent(rc.y2), 0, 255, 0, true, true);
            }
        }
    }

    g_RaycastDebugList.clear();
}

bool IsStringNumber(const std::string& s)
{
    return !s.empty() && std::find_if(s.begin(), s.end(), [](s8 c)
                                      { return !std::isdigit(c); })
                             == s.end();
}
