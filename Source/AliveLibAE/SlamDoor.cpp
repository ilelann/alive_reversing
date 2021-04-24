#include "stdafx.h"
#include "SlamDoor.hpp"
#include "Function.hpp"
#include "stdlib.hpp"
#include "Game.hpp"
#include "SwitchStates.hpp"
#include "Events.hpp"
#include "Sfx.hpp"

struct SlamDoor_Data
{
    s32 field_0_frameTableOffset;
    s32 field_4_ppRes;
    s32 field_8_frameTableOffset;
    s16 field_C_maxH;
    s16 field_E_maxW;
};

const AnimId sSlamDoorData_547168[15][3] =
{
    { AnimId::Slam_Door_Industrial_Closing, AnimId::Slam_Door_Industrial_Closed, AnimId::Slam_Door_Industrial_Opening },
    { AnimId::Slam_Door_Industrial_Closing, AnimId::Slam_Door_Industrial_Closed, AnimId::Slam_Door_Industrial_Opening },
    { AnimId::Slam_Door_Vault_Closing, AnimId::Slam_Door_Vault_Closed, AnimId::Slam_Door_Vault_Opening },
    { AnimId::Slam_Door_Vault_Closing, AnimId::Slam_Door_Vault_Closed, AnimId::Slam_Door_Vault_Opening },
    { AnimId::Slam_Door_Vault_Closing, AnimId::Slam_Door_Vault_Closed, AnimId::Slam_Door_Vault_Opening },
    { AnimId::Slam_Door_Industrial_Closing, AnimId::Slam_Door_Industrial_Closed, AnimId::Slam_Door_Industrial_Opening },
    { AnimId::Slam_Door_Industrial_Closing, AnimId::Slam_Door_Industrial_Closed, AnimId::Slam_Door_Industrial_Opening },
    { AnimId::Slam_Door_Vault_Closing, AnimId::Slam_Door_Vault_Closed, AnimId::Slam_Door_Vault_Opening },
    { AnimId::Slam_Door_Industrial_Closing, AnimId::Slam_Door_Industrial_Closed, AnimId::Slam_Door_Industrial_Opening },
    { AnimId::Slam_Door_Industrial_Closing, AnimId::Slam_Door_Industrial_Closed, AnimId::Slam_Door_Industrial_Opening },
    { AnimId::Slam_Door_Industrial_Closing, AnimId::Slam_Door_Industrial_Closed, AnimId::Slam_Door_Industrial_Opening },
    { AnimId::Slam_Door_Vault_Closing, AnimId::Slam_Door_Vault_Closed, AnimId::Slam_Door_Vault_Opening },
    { AnimId::Slam_Door_Industrial_Closing, AnimId::Slam_Door_Industrial_Closed, AnimId::Slam_Door_Industrial_Opening },
    { AnimId::Slam_Door_Industrial_Closing, AnimId::Slam_Door_Industrial_Closed, AnimId::Slam_Door_Industrial_Opening },
    { AnimId::Slam_Door_Industrial_Closing, AnimId::Slam_Door_Industrial_Closed, AnimId::Slam_Door_Industrial_Opening },
};

TintEntry sSlamDoorTints_5603B0[18] =
{
    { 1, 102u, 87u, 118u },
    { 2, 102u, 87u, 118u },
    { 3, 102u, 87u, 118u },
    { 4, 102u, 87u, 118u },
    { 5, 102u, 87u, 118u },
    { 6, 102u, 87u, 118u },
    { 7, 102u, 87u, 118u },
    { 8, 102u, 87u, 118u },
    { 9, 102u, 87u, 118u },
    { 0x0A, 102u, 87u, 118u },
    { 0x0B, 102u, 87u, 118u },
    { 0x0C, 102u, 87u, 118u },
    { 0x0D, 102u, 87u, 118u },
    { 0x0E, 102u, 87u, 118u },
    { -1, 102u, 87u, 118u },
    { 0, 0, 0, 0 },
    { 0, 0, 0, 0 },
    { 0, 0, 0, 0 }
};

struct Quicksave_Obj_SlamDoor
{
    AETypes field_0_id;
    TlvItemInfoUnion field_4_tlv;
};
ALIVE_ASSERT_SIZEOF_ALWAYS(Quicksave_Obj_SlamDoor, 8);

BaseGameObject* SlamDoor::VDestructor(s32 flags)
{
    return vdtor_4AFD20(flags);
}

s32 SlamDoor::VGetSaveState(u8* pSaveBuffer)
{
    return vGetSaveState_4C09D0(pSaveBuffer);
}

void SlamDoor::VUpdate()
{
    vUpdate_4AFD50();
}

SlamDoor* SlamDoor::ctor_4AF700(Path_SlamDoor* pTlv, TlvItemInfoUnion tlvInfo)
{
    ctor_408240(0);
    SetVTable(this, 0x547288);
    field_C_objectId = tlvInfo.all; // todo: check this
    field_6_flags.Set(Options::eCanExplode_Bit7);

    field_128_switch_id = pTlv->field_14_id;

    field_118_flags.Clear(SlamDoor_Flags_118::e118_Bit2_Open);
    field_118_flags.Clear(SlamDoor_Flags_118::e118_Bit4_Inverted);
    field_118_flags.Clear(SlamDoor_Flags_118::e118_Bit5_Delete);

    if (pTlv->field_10_bStart_closed == Choice_short::eNo_0)
    {
        field_118_flags.Set(SlamDoor_Flags_118::e118_Bit2_Open);
    }

    if (pTlv->field_16_bStart_inverted == Choice_short::eYes_1)
    {
        field_118_flags.Set(SlamDoor_Flags_118::e118_Bit4_Inverted);
    }

    if (pTlv->field_18_bDelete == Choice_short::eYes_1)
    {
        field_118_flags.Set(SlamDoor_Flags_118::e118_Bit5_Delete);
    }

    const s32 currentLevelId = static_cast<s32>(gMap_5C3030.field_0_current_level);

    const AnimRecord& rec = AnimRec(sSlamDoorData_547168[currentLevelId][2]);
    Animation_Init_424E10(rec.mFrameTableOffset, rec.mMaxW, rec.mMaxH, Add_Resource_4DC130(ResourceManager::Resource_Animation, rec.mResourceId), 1, 1u);

    field_4_typeId = AETypes::eSlamDoor_122;

    field_B8_xpos = FP_FromInteger(((pTlv->field_8_top_left.field_0_x
        + pTlv->field_C_bottom_right.field_0_x)
        / 2));

    field_BC_ypos = FP_FromInteger(pTlv->field_8_top_left.field_2_y);
    field_12C_tlvInfo = tlvInfo;

    if (pTlv->field_12_scale == Scale_short::eHalf_1)
    {
        field_CC_sprite_scale = FP_FromDouble(0.5);
        field_20_animation.field_C_render_layer = Layer::eLayer_6;
        field_D6_scale = 0;
    }
    else
    {
        field_CC_sprite_scale = FP_FromDouble(1.0);
        field_20_animation.field_C_render_layer = Layer::eLayer_25;
        field_D6_scale = 1;
    }

    if (field_118_flags.Get(SlamDoor_Flags_118::e118_Bit4_Inverted))
    {
        field_20_animation.field_4_flags.Set(AnimFlags::eBit6_FlipY);
        field_D8_yOffset = FP_GetExponent(field_CC_sprite_scale * FP_FromDouble(-68.0));
    }

    s32 switchState = SwitchStates_Get_466020(field_128_switch_id);
    s32 bitIsOpen = static_cast<s32>(field_118_flags.Get(SlamDoor_Flags_118::e118_Bit2_Open));

    if (switchState == bitIsOpen)
    {
        field_118_flags.Set(SlamDoor_Flags_118::e118_Bit1_bClosed);
    }
    else
    {
        field_118_flags.Clear(SlamDoor_Flags_118::e118_Bit1_bClosed);
    }

    SetTint_425600(sSlamDoorTints_5603B0, gMap_5C3030.field_0_current_level);

    FP hitX;
    FP hitY;

    if (sCollisions_DArray_5C1128->Raycast_417A60(
        field_B8_xpos,
        field_BC_ypos,
        field_B8_xpos,
        field_BC_ypos + FP_FromDouble(24.0),
        &field_100_pCollisionLine,
        &hitX,
        &hitY,
        field_D6_scale != 0 ? 1 : 16) == 1)
    {
        field_BC_ypos = hitY;
    }

    if (field_20_animation.field_4_flags.Get(AnimFlags::eBit5_FlipX))
    {
        field_124_x1 = FP_GetExponent((ScaleToGridSize_4498B0(field_CC_sprite_scale) / FP_FromDouble(2.0)) + FP_FromInteger(FP_GetExponent(field_B8_xpos)));
    }
    else
    {
        field_124_x1 = FP_GetExponent(FP_FromInteger(FP_GetExponent(field_B8_xpos)) - (ScaleToGridSize_4498B0(field_CC_sprite_scale) / FP_FromDouble(2.0)));
    }

    field_126_y1 = FP_GetExponent(field_BC_ypos);

    if (field_118_flags.Get(SlamDoor_Flags_118::e118_Bit1_bClosed))
    {
        PathLine* pPathLine = nullptr;

        if (field_CC_sprite_scale == FP_FromDouble(1.0))
        {
            const FP lineHeight = FP_FromDouble(80.0);

            field_11C_pCollisionLine_6_2 = sCollisions_DArray_5C1128->Add_Dynamic_Collision_Line_417FA0(
                field_124_x1,
                FP_GetExponent(FP_FromInteger(field_126_y1) - lineHeight),
                field_124_x1,
                field_126_y1,
                2);
            const FP x2 = FP_FromInteger(field_124_x1) + ScaleToGridSize_4498B0(field_CC_sprite_scale);
            const FP y1 = FP_FromInteger(field_126_y1)
                - (field_CC_sprite_scale * FP_FromDouble(80.0));
            const FP x1 = ScaleToGridSize_4498B0(field_CC_sprite_scale) + FP_FromInteger(field_124_x1);
            pPathLine = sCollisions_DArray_5C1128->Add_Dynamic_Collision_Line_417FA0(
                FP_GetExponent(x1),
                FP_GetExponent(y1),
                FP_GetExponent(x2),
                field_126_y1,
                1);
        }
        else
        {
            const FP lineHeight = field_CC_sprite_scale * FP_FromDouble(80.0);

            field_11C_pCollisionLine_6_2 = sCollisions_DArray_5C1128->Add_Dynamic_Collision_Line_417FA0(
                field_124_x1,
                FP_GetExponent(FP_FromInteger(field_126_y1) - lineHeight),
                field_124_x1,
                field_126_y1,
                6);
            const FP x2 = FP_FromInteger(field_124_x1) + ScaleToGridSize_4498B0(field_CC_sprite_scale);
            const FP y1 = FP_FromInteger(field_126_y1) -(field_CC_sprite_scale * FP_FromDouble(80.0));
            const FP x1 = ScaleToGridSize_4498B0(field_CC_sprite_scale) + FP_FromInteger(field_124_x1);
            pPathLine = sCollisions_DArray_5C1128->Add_Dynamic_Collision_Line_417FA0(
                FP_GetExponent(x1),
                FP_GetExponent(y1),
                FP_GetExponent(x2),
                field_126_y1,
                5);
        }
        field_120_pCollisionLine_5_1 = pPathLine;

        const AnimRecord& animRec = AnimRec(sSlamDoorData_547168[currentLevelId][1]);
        field_20_animation.Set_Animation_Data_409C80(animRec.mFrameTableOffset, 0);
    }
    else
    {
        field_20_animation.field_4_flags.Clear(AnimFlags::eBit3_Render);
        field_11C_pCollisionLine_6_2 = 0;
        field_120_pCollisionLine_5_1 = 0;
    }

    field_118_flags.Set(SlamDoor_Flags_118::e118_Bit3_bLastFrame);
    field_DC_bApplyShadows |= 2u;

    return this;
}

void SlamDoor::dtor_4B0620()
{
    SetVTable(this, 0x547288);

    if (!(field_118_flags.Get(SlamDoor_Flags_118::e118_Bit5_Delete)) || field_118_flags.Get(SlamDoor_Flags_118::e118_Bit1_bClosed))
    {
        Path::TLV_Reset_4DB8E0(field_12C_tlvInfo.all, -1, 0, 0);
    }

    if (field_11C_pCollisionLine_6_2)
    {
        Rect_Clear_418040(&field_11C_pCollisionLine_6_2->field_0_rect);
    }

    if (field_120_pCollisionLine_5_1)
    {
        Rect_Clear_418040(&field_120_pCollisionLine_5_1->field_0_rect);
    }

    dtor_4080B0();
}

SlamDoor* SlamDoor::vdtor_4AFD20(s32 flags)
{
    dtor_4B0620();
    if (flags & 1)
    {
        ae_delete_free_495540(this);
    }
    return this;
}

void SlamDoor::vUpdate_4AFD50()
{
    if (Event_Get_422C00(kEventDeathReset))
    {
        field_6_flags.Set(BaseGameObject::eDead_Bit3);
    }

    const bool stateUnchanged = SwitchStates_Get_466020(field_128_switch_id) == static_cast<s32>(field_118_flags.Get(SlamDoor_Flags_118::e118_Bit2_Open));
    if (!field_118_flags.Get(SlamDoor_Flags_118::e118_Bit1_bClosed))
    {
        if (field_20_animation.field_4_flags.Get(AnimFlags::eBit18_IsLastFrame))
        {
            if (field_20_animation.field_4_flags.Get(AnimFlags::eBit3_Render))
            {
                field_20_animation.field_4_flags.Clear(AnimFlags::eBit3_Render);

                if (field_118_flags.Get(SlamDoor_Flags_118::e118_Bit5_Delete))
                {
                    field_6_flags.Set(BaseGameObject::eDead_Bit3);
                }
                SFX_Play_46FBA0(SoundEffect::DoorEffect_57, 100, 900);
                SFX_Play_46FBA0(SoundEffect::DoorEffect_57, 100, -100);
                field_118_flags.Set(SlamDoor_Flags_118::e118_Bit3_bLastFrame);
            }
        }
    }

    if (field_118_flags.Get(SlamDoor_Flags_118::e118_Bit1_bClosed))
    {
        if (field_20_animation.field_4_flags.Get(AnimFlags::eBit18_IsLastFrame))
        {
            if (!field_118_flags.Get(SlamDoor_Flags_118::e118_Bit3_bLastFrame))
            {
                field_118_flags.Set(SlamDoor_Flags_118::e118_Bit3_bLastFrame);
                SFX_Play_46FBA0(SoundEffect::DoorEffect_57, 100, 900);
                SFX_Play_46FBA0(SoundEffect::DoorEffect_57, 100, -100);
            }
        }
    }

    if (stateUnchanged != field_118_flags.Get(SlamDoor_Flags_118::e118_Bit1_bClosed))
    {
        field_118_flags.Clear(SlamDoor_Flags_118::e118_Bit3_bLastFrame);
        field_118_flags.Toggle(SlamDoor_Flags_118::e118_Bit1_bClosed);

        if (stateUnchanged)
        {
            field_20_animation.field_4_flags.Set(AnimFlags::eBit3_Render);

            const AnimRecord& animRec = AnimRec(sSlamDoorData_547168[static_cast<s32>(gMap_5C3030.field_0_current_level)][2]);
            field_20_animation.Set_Animation_Data_409C80(animRec.mFrameTableOffset, nullptr);

            if (field_CC_sprite_scale == FP_FromInteger(1))
            {
                field_11C_pCollisionLine_6_2 = sCollisions_DArray_5C1128->Add_Dynamic_Collision_Line_417FA0(
                    field_124_x1,
                    FP_GetExponent(FP_FromInteger(field_126_y1) - (FP_FromInteger(80) * FP_FromInteger(1))),
                    field_124_x1,
                    field_126_y1,
                    1);
                field_120_pCollisionLine_5_1 = sCollisions_DArray_5C1128->Add_Dynamic_Collision_Line_417FA0(
                    FP_GetExponent(ScaleToGridSize_4498B0(field_CC_sprite_scale) + FP_FromInteger(field_124_x1)),
                    FP_GetExponent(FP_FromInteger(field_126_y1) - (FP_FromInteger(80) * field_CC_sprite_scale)),
                    FP_GetExponent(FP_FromInteger(field_124_x1) + ScaleToGridSize_4498B0(field_CC_sprite_scale)),
                    field_126_y1,
                    2);
            }
            else
            {
                field_11C_pCollisionLine_6_2 = sCollisions_DArray_5C1128->Add_Dynamic_Collision_Line_417FA0(
                    field_124_x1,
                    FP_GetExponent(FP_FromInteger(field_126_y1) - (FP_FromInteger(80) * field_CC_sprite_scale)),
                    field_124_x1,
                    field_126_y1,
                    5);
                field_120_pCollisionLine_5_1 = sCollisions_DArray_5C1128->Add_Dynamic_Collision_Line_417FA0(
                    FP_GetExponent(ScaleToGridSize_4498B0(field_CC_sprite_scale) + FP_FromInteger(field_124_x1)),
                    FP_GetExponent(FP_FromInteger(field_126_y1) - (FP_FromInteger(80) * field_CC_sprite_scale)),
                    FP_GetExponent(FP_FromInteger(field_124_x1) + ScaleToGridSize_4498B0(field_CC_sprite_scale)),
                    field_126_y1,
                    6);
            }

            PSX_RECT bRect = {};
            vGetBoundingRect_424FD0(&bRect, 1);

            if (field_118_flags.Get(SlamDoor_Flags_118::e118_Bit4_Inverted))
            {
                bRect.y += FP_GetExponent(FP_FromInteger(-110) * field_CC_sprite_scale);
                bRect.h += FP_GetExponent(FP_FromInteger(-110) * field_CC_sprite_scale);
            }

            for (s32 i = 0; i < gBaseAliveGameObjects_5C1B7C->Size(); i++)
            {
                auto pObj = gBaseAliveGameObjects_5C1B7C->ItemAt(i);
                if (!pObj)
                {
                    break;
                }
                if (pObj->field_20_animation.field_4_flags.Get(AnimFlags::eBit3_Render))
                {
                    if (pObj->field_4_typeId != AETypes::eSlamDoor_122)
                    {
                        PSX_RECT bObjRect = {};
                        pObj->vGetBoundingRect_424FD0(&bObjRect, 1);

                        // Some hack that prevents Abe getting knocked back when rolling or falling near a closing slam door
                        bObjRect.x += 3;

                        if (PSX_Rects_overlap_no_adjustment(&bRect, &bObjRect) &&
                            pObj->field_CC_sprite_scale == field_CC_sprite_scale)
                        {
                            ClearInsideSlamDoor_4B0530(pObj, bRect.x, bRect.w);
                        }
                    }
                }
            }
        }
        else
        {
            const AnimRecord& animRec = AnimRec(sSlamDoorData_547168[static_cast<s32>(gMap_5C3030.field_0_current_level)][0]);
            field_20_animation.Set_Animation_Data_409C80(animRec.mFrameTableOffset, 0);
            Rect_Clear_418040(&field_11C_pCollisionLine_6_2->field_0_rect);
            field_11C_pCollisionLine_6_2 = nullptr;

            Rect_Clear_418040(&field_120_pCollisionLine_5_1->field_0_rect);
            field_120_pCollisionLine_5_1 = nullptr;
        }
    }

    if (field_118_flags.Get(SlamDoor_Flags_118::e118_Bit1_bClosed))
    {
        PSX_RECT bRect = {};
        vGetBoundingRect_424FD0(&bRect, 1);

        if (field_118_flags.Get(SlamDoor_Flags_118::e118_Bit4_Inverted))
        {
            bRect.y += FP_GetExponent(FP_FromInteger(-110) * field_CC_sprite_scale);
            bRect.h += FP_GetExponent(FP_FromInteger(-110) * field_CC_sprite_scale) - FP_GetExponent(FP_FromInteger(20) * field_CC_sprite_scale);
        }

        for (s32 i = 0; i < gBaseAliveGameObjects_5C1B7C->Size(); i++)
        {
            auto pObj = gBaseAliveGameObjects_5C1B7C->ItemAt(i);
            if (!pObj)
            {
                break;
            }

            if (pObj->field_20_animation.field_4_flags.Get(AnimFlags::eBit3_Render))
            {
                if (pObj->field_4_typeId != AETypes::eSlamDoor_122 && pObj->field_4_typeId != AETypes::eGrenade_65)
                {
                    PSX_RECT bObjRect = {};
                    pObj->vGetBoundingRect_424FD0(&bObjRect, 1);

                    if (FP_GetExponent(pObj->field_B8_xpos) > bRect.x &&
                        FP_GetExponent(pObj->field_B8_xpos) < bRect.w &&
                        PSX_Rects_overlap_no_adjustment(&bRect, &bObjRect))
                    {
                        if (pObj->field_CC_sprite_scale == field_CC_sprite_scale ||
                            (pObj->field_4_typeId == AETypes::eSlog_126 && field_CC_sprite_scale == FP_FromInteger(1)))
                        {
                            ClearInsideSlamDoor_4B0530(pObj, bRect.x, bRect.w);
                        }
                    }
                }
            }
        }
    }

    field_6_flags.Set(BaseGameObject::eCanExplode_Bit7, field_20_animation.field_4_flags.Get(AnimFlags::eBit3_Render));

}

s32 SlamDoor::vGetSaveState_4C09D0(u8* pSaveBuffer)
{
    Quicksave_Obj_SlamDoor* pSaveState = reinterpret_cast<Quicksave_Obj_SlamDoor*>(pSaveBuffer);

    pSaveState->field_0_id = AETypes::eSlamDoor_122;
    pSaveState->field_4_tlv = field_12C_tlvInfo;

    return sizeof(Quicksave_Obj_SlamDoor);
}

void SlamDoor::ClearInsideSlamDoor_4B0530(BaseAliveGameObject* pObj, s16 xPosition, s16 width)
{
    if (FP_GetExponent(pObj->field_B8_xpos) - xPosition >= width - FP_GetExponent(pObj->field_B8_xpos))
    {
        pObj->field_B8_xpos = (ScaleToGridSize_4498B0(field_CC_sprite_scale) * FP_FromDouble(0.5)) + FP_FromDouble(1.0) + pObj->field_B8_xpos;
    }
    else
    {
        pObj->field_B8_xpos = pObj->field_B8_xpos - (ScaleToGridSize_4498B0(field_CC_sprite_scale) * FP_FromDouble(0.5));
    }

    if (pObj->field_4_typeId == AETypes::eMudokon2_81 || pObj->field_4_typeId == AETypes::eMudokon_110 || pObj->field_4_typeId == AETypes::eAbe_69)
    {
        pObj->VTakeDamage_408730(this);
    }

    if (pObj->field_4_typeId == AETypes::eBone_11)
    {
        pObj->field_C8_vely = FP_FromInteger(-7);
        pObj->field_C4_velx = -pObj->field_C4_velx;
    }
}

s32 CC SlamDoor::CreateFromSaveState_4C08B0(const u8* pData)
{
    const Quicksave_Obj_SlamDoor* pSaveState = reinterpret_cast<const Quicksave_Obj_SlamDoor*>(pData);

    if (!ResourceManager::GetLoadedResource_49C2A0(ResourceManager::Resource_Animation, ResourceID::kSlamResID, 0, 0))
    {
        switch (gMap_5C3030.field_0_current_level)
        {
        case LevelIds::eNecrum_2:
        case LevelIds::eMudomoVault_3:
        case LevelIds::eMudancheeVault_4:
        case LevelIds::eMudancheeVault_Ender_7:
        case LevelIds::eMudomoVault_Ender_11:
            ResourceManager::LoadResourceFile_49C170("SLAMVLTS.BAN", nullptr);
            break;
        default:
            ResourceManager::LoadResourceFile_49C170("SLAM.BAN", nullptr);
            break;
        }
    }

    SlamDoor* pSlamDoor = ae_new<SlamDoor>();
    if (pSlamDoor)
    {
        pSlamDoor->ctor_4AF700(static_cast<Path_SlamDoor*>(sPath_dword_BB47C0->TLV_From_Offset_Lvl_Cam_4DB770(pSaveState->field_4_tlv.all)), pSaveState->field_4_tlv);
    }

    return sizeof(Quicksave_Obj_SlamDoor);
}

void SlamDoor_ForceLink()
{
}
