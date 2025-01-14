#include "stdafx.h"
#include "Door.hpp"
#include "Function.hpp"
#include "Path.hpp"
#include "stdlib.hpp"
#include "SwitchStates.hpp"
#include "Events.hpp"
#include "Sfx.hpp"
#include "Abe.hpp"
#include "MusicTrigger.hpp"
#include "Sound/Midi.hpp"
#include "DDCheat.hpp"
#include "BaseAliveGameObject.hpp"

struct Door_Info final
{
    s32 field_0_frameTableOffset_closed;
    s32 field_4_frameTableOffset_open;
    s32 field_8_maxW;
    s32 field_C_maxH;
};
ALIVE_ASSERT_SIZEOF(Door_Info, 0x10);

const AnimId sDoorAnimIdTable_544888[16][2] = {
    {AnimId::Door_Mines_Closed, AnimId::Door_Mines_Open},
    {AnimId::Door_Mines_Closed, AnimId::Door_Mines_Open},
    {AnimId::Door_Temple_Closed, AnimId::Door_Temple_Open},
    {AnimId::Door_Temple_Closed, AnimId::Door_Temple_Open},
    {AnimId::Door_Temple_Closed, AnimId::Door_Temple_Open},
    {AnimId::Door_Feeco_Closed, AnimId::Door_Feeco_Open},
    {AnimId::Door_Barracks_Closed, AnimId::Door_Barracks_Open},
    {AnimId::Door_Temple_Closed, AnimId::Door_Temple_Open},
    {AnimId::Door_Bonewerkz_Closed, AnimId::Door_Bonewerkz_Open},
    {AnimId::Door_Brewery_Closed, AnimId::Door_Brewery_Open},
    {AnimId::Door_Brewery_Closed, AnimId::Door_Brewery_Open},
    {AnimId::Door_Temple_Closed, AnimId::Door_Temple_Open},
    {AnimId::Door_Feeco_Closed, AnimId::Door_Feeco_Open},
    {AnimId::Door_Barracks_Closed, AnimId::Door_Barracks_Open},
    {AnimId::Door_Bonewerkz_Closed, AnimId::Door_Bonewerkz_Open},
    {AnimId::Door_Mines_Closed, AnimId::Door_Mines_Open}};


Door* Door::ctor_41E250(Path_Door* pTlvData, s32 tlvInfo)
{
    BaseAnimatedWithPhysicsGameObject_ctor_424930(0);

    SetVTable(this, 0x5449BC); // vTbl_Door_5449BC
    field_4_typeId = AETypes::eDoor_33;

    field_F4_tlvInfo = tlvInfo;
    field_F8_door_type = pTlvData->field_1E_type;
    field_FE_start_state = pTlvData->field_20_start_state;

    if (pTlvData->field_40_close_after_use)
    {
        if (pTlvData->field_1_tlv_state)
        {
            field_FE_start_state = eClosed_1;
        }
    }

    field_100_switch_id = pTlvData->field_1A_id;
    if (pTlvData->field_1A_id == 1)
    {
        field_100_switch_id = 0;
    }

    field_FA_door_number = pTlvData->field_18_door_number;
    if (gMap_5C3030.field_0_current_level == LevelIds::eFeeCoDepot_5)
    {
        switch (field_FA_door_number)
        {
            case 30000:
                if (sVisitedBonewerks_5C1C02)
                {
                    field_FE_start_state = eClosed_1;
                }
                break;

            case 30001:
            case 30004:
                if (sVisitedBarracks_5C1C04)
                {
                    field_FE_start_state = eClosed_1;
                }
                break;

            case 30002:
                if (sVisitedBonewerks_5C1C02)
                {
                    SwitchStates_Set_465FF0(255u, 1);
                }

                if (sVisitedBarracks_5C1C04)
                {
                    SwitchStates_Set_465FF0(254u, 1);
                }

                if (sVisitedFeecoEnder_5C1C06)
                {
                    SwitchStates_Set_465FF0(255u, 0);
                    SwitchStates_Set_465FF0(254u, 0);
                }
                break;

            case 30003:
                if (sVisitedBonewerks_5C1C02)
                {
                    if (sVisitedBarracks_5C1C04)
                    {
                        if (sVisitedFeecoEnder_5C1C06)
                        {
                            field_FE_start_state = eOpen_0;
                        }
                    }
                    if (sVisitedBonewerks_5C1C02)
                    {
                        SwitchStates_Set_465FF0(250u, 1);
                    }
                }

                if (sVisitedBarracks_5C1C04)
                {
                    SwitchStates_Set_465FF0(251u, 1);
                }
                if (sVisitedFeecoEnder_5C1C06)
                {
                    SwitchStates_Set_465FF0(253u, 1);
                }
                break;
            default:
                break;
        }
    }

    if (field_F8_door_type == DoorTypes::eTasksDoor_3)
    {
        field_102_hub_ids[0] = pTlvData->field_22_hub1;
        field_102_hub_ids[1] = pTlvData->field_22_hub2;
        field_102_hub_ids[2] = pTlvData->field_22_hub3;
        field_102_hub_ids[3] = pTlvData->field_22_hub4;
        field_102_hub_ids[4] = pTlvData->field_22_hub5;
        field_102_hub_ids[5] = pTlvData->field_22_hub6;
        field_102_hub_ids[6] = pTlvData->field_22_hub7;
        field_102_hub_ids[7] = pTlvData->field_22_hub8;

        if (SwitchStates_Get_466020(field_102_hub_ids[0]) && SwitchStates_Get_466020(field_102_hub_ids[1]) && SwitchStates_Get_466020(field_102_hub_ids[2]) && SwitchStates_Get_466020(field_102_hub_ids[3]) && SwitchStates_Get_466020(field_102_hub_ids[4]) && SwitchStates_Get_466020(field_102_hub_ids[5]) && SwitchStates_Get_466020(field_102_hub_ids[6]) && SwitchStates_Get_466020(field_102_hub_ids[7]))
        {
            SwitchStates_Do_Operation_465F00(field_100_switch_id, SwitchOp::eSetTrue_0);
        }
        else
        {
            SwitchStates_Do_Operation_465F00(field_100_switch_id, SwitchOp::eSetFalse_1);
        }
    }

    if (field_FE_start_state == eOpen_0)
    {
        if (SwitchStates_Get_466020(field_100_switch_id))
        {
            // Its open but the switch id is on which causes it to close
            field_FC_current_state = eClosed_1;
        }
        else
        {
            field_FC_current_state = eOpen_0;
        }
    }
    else
    {
        if (SwitchStates_Get_466020(field_100_switch_id))
        {
            // Its closed but the id is on which causes it to open
            field_FC_current_state = eOpen_0;
        }
        else
        {
            field_FC_current_state = eClosed_1;
        }
    }

    if ((sActiveHero_5C1B68->field_106_current_motion == eAbeMotions::Motion_114_DoorEnter_459470 || sActiveHero_5C1B68->field_106_current_motion == eAbeMotions::Motion_115_DoorExit_459A40) && field_FC_current_state == eClosed_1 && field_FA_door_number == sActiveHero_5C1B68->field_1A0_door_id)
    {
        // Force open is abe is in the door
        field_FC_current_state = eOpen_0;
    }

    if (field_F8_door_type == DoorTypes::eTasksWithSecretMusicDoor_2)
    {
        field_102_hub_ids[0] = pTlvData->field_22_hub1;
        field_102_hub_ids[1] = pTlvData->field_22_hub2;
        field_102_hub_ids[2] = pTlvData->field_22_hub3;
        field_102_hub_ids[3] = pTlvData->field_22_hub4;
        field_102_hub_ids[4] = pTlvData->field_22_hub5;
        field_102_hub_ids[5] = pTlvData->field_22_hub6;
        field_102_hub_ids[6] = pTlvData->field_22_hub7;
        field_102_hub_ids[7] = pTlvData->field_22_hub8;
    }

    const AnimRecord& closedRec = AnimRec(sDoorAnimIdTable_544888[static_cast<s32>(gMap_5C3030.field_0_current_level)][0]);
    const AnimRecord& openRec = AnimRec(sDoorAnimIdTable_544888[static_cast<s32>(gMap_5C3030.field_0_current_level)][1]);
    u8** ppRes = Add_Resource_4DC130(ResourceManager::Resource_Animation, ResourceID::kF2p3dorResID);
    if (!ppRes || openRec.mFrameTableOffset == 0)
    {
        field_6_flags.Clear(BaseGameObject::eDrawable_Bit4);
        field_6_flags.Set(BaseGameObject::eDead_Bit3);
        return this;
    }

    if (field_FC_current_state == eOpen_0)
    {
        if (gMap_5C3030.field_22_overlayID == 108)
        {
            const AnimRecord& rec = AnimRec(AnimId::Door_BarracksMetal_Open);
            Animation_Init_424E10(rec.mFrameTableOffset, rec.mMaxW, rec.mMaxH, ppRes, 1, 1u);
        }
        else
        {
            Animation_Init_424E10(openRec.mFrameTableOffset, openRec.mMaxW, openRec.mMaxH, ppRes, 1, 1u);
        }
    }
    else
    {
        if (gMap_5C3030.field_22_overlayID == 108)
        {
            const AnimRecord& rec = AnimRec(AnimId::Door_BarracksMetal_Closed);
            Animation_Init_424E10(rec.mFrameTableOffset, rec.mMaxW, rec.mMaxH, ppRes, 1, 1u);
        }
        else
        {
            Animation_Init_424E10(closedRec.mFrameTableOffset, closedRec.mMaxW, closedRec.mMaxH, ppRes, 1, 1u);
        }
    }

    if (pTlvData->field_16_scale != Scale_short::eFull_0)
    {
        if (pTlvData->field_16_scale == Scale_short::eHalf_1)
        {
            field_CC_sprite_scale = FP_FromDouble(0.5);
            field_D6_scale = 0;
            field_20_animation.field_C_render_layer = Layer::eLayer_BeforeShadow_Half_6;
        }
    }
    else
    {
        field_CC_sprite_scale = FP_FromInteger(1);
        field_D6_scale = 1;
        field_20_animation.field_C_render_layer = Layer::eLayer_BeforeShadow_25;
    }

    FP* xOff = &field_B8_xpos;
    FP* yOff = &field_BC_ypos;

    FP tlvXMid = FP_FromInteger((pTlvData->field_8_top_left.field_0_x + pTlvData->field_C_bottom_right.field_0_x) / 2);
    PathLine* pathLine = nullptr;

    if (sCollisions_DArray_5C1128->Raycast_417A60(
            tlvXMid,
            FP_FromInteger(pTlvData->field_8_top_left.field_2_y),
            tlvXMid,
            FP_FromInteger(pTlvData->field_C_bottom_right.field_2_y),
            &pathLine,
            xOff,
            yOff,
            (field_D6_scale == 1) ? 15 : 240))
    {
        // Move up off the line we hit
        *yOff -= FP_FromInteger(12) * field_CC_sprite_scale;

        // Snap on X
        *xOff = FP_FromInteger(SnapToXGrid_449930(field_CC_sprite_scale, FP_GetExponent(*xOff)));
    }
    else
    {
        // Couldn't glue to the floor.. just use the TLV pos
        *xOff = FP_FromInteger(pTlvData->field_8_top_left.field_0_x + 12);
        *yOff = FP_FromInteger(pTlvData->field_8_top_left.field_2_y + 24);
    }

    // Add on the TLV offset
    *xOff += FP_FromInteger(pTlvData->field_36_x_offset);
    *yOff += FP_FromInteger(pTlvData->field_38_y_offset);

    // Another OWI special
    FP yAdjustHack = {};
    if ((gMap_5C3030.field_0_current_level != LevelIds::eBarracks_6 && gMap_5C3030.field_0_current_level != LevelIds::eBarracks_Ender_13) || gMap_5C3030.field_22_overlayID == 108)
    {
        if (gMap_5C3030.field_0_current_level == LevelIds::eBonewerkz_8 || gMap_5C3030.field_0_current_level == LevelIds::eBonewerkz_Ender_14)
        {
            yAdjustHack = FP_FromInteger(10) * field_CC_sprite_scale;
        }
    }
    else
    {
        yAdjustHack = FP_FromInteger(14) * field_CC_sprite_scale;
    }
    *yOff += yAdjustHack;

    if (field_FC_current_state == eOpen_0)
    {
        field_20_animation.field_4_flags.Clear(AnimFlags::eBit3_Render);
    }

    field_20_animation.field_4_flags.Clear(AnimFlags::eBit2_Animate);
    field_DC_bApplyShadows |= 2u;

    return this;
}

Bool32 Door::vIsOpen_41EB00()
{
    return field_FC_current_state == eOpen_0;
}

void Door::vOpen_41EB20()
{
    if (field_FC_current_state != eOpen_0)
    {
        field_FC_current_state = eOpening_2;
    }
}

void Door::vClose_41EB50()
{
    if (field_FC_current_state != eClosed_1)
    {
        field_FE_start_state = eClosed_1;
        field_FC_current_state = eClosing_3;
        Path_TLV* pTlv = sPath_dword_BB47C0->TLV_From_Offset_Lvl_Cam_4DB770(field_F4_tlvInfo);
        pTlv->field_1_tlv_state = 1;
    }
}

void Door::vSetOpen_41EBA0()
{
    field_FC_current_state = eOpen_0;
}

void Door::vSetClosed_41EBC0()
{
    field_FC_current_state = eClosed_1;
}

void Door::vScreenChange_41F080()
{
    field_6_flags.Set(BaseGameObject::eDead_Bit3);
}

Door* Door::vdtor_41E9D0(s32 flags)
{
    dtor_41EA00();
    if (flags & 1)
    {
        ae_delete_free_495540(this);
    }
    return this;
}

void Door::PlaySound_41EA90()
{
    s16 volume = 0;
    if (field_F8_door_type != DoorTypes::eBasicDoor_0 || field_CC_sprite_scale != FP_FromInteger(1))
    {
        volume = 60;
    }
    else
    {
        volume = 90;
    }

    SFX_Play_46FBA0(SoundEffect::DoorEffect_57, volume, 900);
    SFX_Play_46FBA0(SoundEffect::DoorEffect_57, volume, 0);
}

void Door::dtor_41EA00()
{
    SetVTable(this, 0x5449BC); // vTbl_Door_5449BC
    Path::TLV_Reset_4DB8E0(field_F4_tlvInfo, -1, 0, 0);

    // Note: Skipping intermediate base class that has been optimized out
    BaseAnimatedWithPhysicsGameObject_dtor_424AD0();
}

void Door::vUpdate_41EBE0()
{
    if (Event_Get_422C00(kEventDeathReset))
    {
        field_6_flags.Set(BaseGameObject::eDead_Bit3);
    }

    if (sActiveHero_5C1B68->field_106_current_motion == eAbeMotions::Motion_114_DoorEnter_459470 || sActiveHero_5C1B68->field_106_current_motion == eAbeMotions::Motion_115_DoorExit_459A40)
    {
        if (field_FC_current_state == Door::eStates::eClosed_1 && field_FA_door_number == sActiveHero_5C1B68->field_1A0_door_id)
        {
            field_20_animation.field_4_flags.Clear(AnimFlags::eBit3_Render);
            field_FC_current_state = Door::eStates::eOpen_0;
        }
    }
    else
    {
        field_FA_door_number = -1;
        if (field_F8_door_type == DoorTypes::eTasksWithSecretMusicDoor_2 || field_F8_door_type == DoorTypes::eTasksDoor_3)
        {
            if (SwitchStates_Get_466020(field_102_hub_ids[0])
                && SwitchStates_Get_466020(field_102_hub_ids[1])
                && SwitchStates_Get_466020(field_102_hub_ids[2])
                && SwitchStates_Get_466020(field_102_hub_ids[3])
                && SwitchStates_Get_466020(field_102_hub_ids[4])
                && SwitchStates_Get_466020(field_102_hub_ids[5])
                && SwitchStates_Get_466020(field_102_hub_ids[6])
                && SwitchStates_Get_466020(field_102_hub_ids[7]))
            {
                if (!SwitchStates_Get_466020(field_100_switch_id) && field_F8_door_type == DoorTypes::eTasksWithSecretMusicDoor_2)
                {
                    SND_SEQ_Play_4CAB10(SeqId::SecretMusic_32, 1, 127, 127);
                    auto pMusicTrigger = ae_new<MusicTrigger>();
                    if (pMusicTrigger)
                    {
                        pMusicTrigger->ctor_47FF10(MusicTriggerMusicType::eChime_5, TriggeredBy::eTimer_0, 0, 0);
                    }
                }
                SwitchStates_Do_Operation_465F00(field_100_switch_id, SwitchOp::eSetTrue_0);
            }
            else
            {
                SwitchStates_Do_Operation_465F00(field_100_switch_id, SwitchOp::eSetFalse_1);
            }
        }
        switch (field_FC_current_state)
        {
            case Door::eStates::eOpen_0:
                field_20_animation.field_4_flags.Clear(AnimFlags::eBit3_Render);

                if ((!field_FE_start_state && SwitchStates_Get_466020(field_100_switch_id))
                    || (field_FE_start_state == Door::eStates::eClosed_1 && !SwitchStates_Get_466020(field_100_switch_id)))
                {
                    field_FC_current_state = Door::eStates::eClosing_3;
                    if (gMap_5C3030.field_22_overlayID == 108)
                    {
                        const AnimRecord& rec = AnimRec(AnimId::Door_BarracksMetal_Open);
                        field_20_animation.Set_Animation_Data_409C80(rec.mFrameTableOffset, nullptr);
                    }
                    else
                    {
                        const AnimRecord& animRec = AnimRec(sDoorAnimIdTable_544888[static_cast<s32>(gMap_5C3030.field_0_current_level)][1]);
                        field_20_animation.Set_Animation_Data_409C80(animRec.mFrameTableOffset, nullptr);
                    }

                    field_20_animation.field_4_flags.Clear(AnimFlags::eBit19_LoopBackwards);
                    field_20_animation.field_4_flags.Set(AnimFlags::eBit3_Render);
                }
                break;

            case Door::eStates::eClosed_1:
                field_20_animation.field_4_flags.Set(AnimFlags::eBit3_Render);
                field_20_animation.field_4_flags.Set(AnimFlags::eBit2_Animate);

                if ((field_FE_start_state == Door::eStates::eClosed_1 && SwitchStates_Get_466020(field_100_switch_id)) || (field_FE_start_state == Door::eStates::eOpen_0 && !SwitchStates_Get_466020(field_100_switch_id)))
                {
                    field_FC_current_state = Door::eStates::eOpening_2;
                    if (gMap_5C3030.field_22_overlayID == 108)
                    {
                        const AnimRecord& rec = AnimRec(AnimId::Door_BarracksMetal_Open);
                        field_20_animation.Set_Animation_Data_409C80(rec.mFrameTableOffset, nullptr);
                    }
                    else
                    {
                        const AnimRecord& animRec = AnimRec(sDoorAnimIdTable_544888[static_cast<s32>(gMap_5C3030.field_0_current_level)][1]);
                        field_20_animation.Set_Animation_Data_409C80(animRec.mFrameTableOffset, nullptr);
                    }

                    field_20_animation.SetFrame_409D50(3);
                    field_20_animation.field_4_flags.Set(AnimFlags::eBit19_LoopBackwards);
                    field_20_animation.field_4_flags.Set(AnimFlags::eBit3_Render);
                    PlaySound_41EA90();
                }
                break;

            case Door::eStates::eOpening_2:
                field_20_animation.field_4_flags.Set(AnimFlags::eBit3_Render);
                field_20_animation.field_4_flags.Set(AnimFlags::eBit2_Animate);

                if (field_20_animation.field_4_flags.Get(AnimFlags::eBit18_IsLastFrame))
                {
                    field_FC_current_state = Door::eStates::eOpen_0;
                }
                break;

            case Door::eStates::eClosing_3:
                field_20_animation.field_4_flags.Set(AnimFlags::eBit3_Render);
                field_20_animation.field_4_flags.Set(AnimFlags::eBit2_Animate);

                if (field_20_animation.field_4_flags.Get(AnimFlags::eBit18_IsLastFrame))
                {
                    field_FC_current_state = Door::eStates::eClosed_1;
                    PlaySound_41EA90();
                }
                break;

            default:
                return;
        }
    }
}

void Door::VScreenChanged()
{
    vScreenChange_41F080();
}

void Door::VUpdate()
{
    vUpdate_41EBE0();
}

BaseGameObject* Door::VDestructor(s32 flags)
{
    return vdtor_41E9D0(flags);
}

// ================================================================================================

ALIVE_VAR(1, 0xBB4AA0, FP, sTrainDoorXPos_BB4AA0, {});
ALIVE_VAR(1, 0xBB4AA4, FP, sTrainDoorYPos_BB4AA4, {});

TrainDoor* TrainDoor::ctor_4DD090(Path_TrainDoor* pTlv, s32 tlvInfo)
{
    // Note: Useless intermediate base ctor omitted
    BaseAnimatedWithPhysicsGameObject_ctor_424930(0);

    SetVTable(this, 0x547AFC);

    field_4_typeId = AETypes::eDoor_33;
    field_F4_tlvInfo = tlvInfo;

    const AnimRecord& rec = AnimRec(AnimId::Door_Train_Closing);
    u8** ppRes = Add_Resource_4DC130(ResourceManager::Resource_Animation, rec.mResourceId);
    Animation_Init_424E10(rec.mFrameTableOffset, rec.mMaxW, rec.mMaxH, ppRes, 1, 1);

    field_B8_xpos = FP_FromInteger(pTlv->field_8_top_left.field_0_x + 12);
    field_BC_ypos = FP_FromInteger(pTlv->field_8_top_left.field_2_y + 24);

    sTrainDoorXPos_BB4AA0 = field_B8_xpos;
    sTrainDoorYPos_BB4AA4 = field_BC_ypos;

    if (pTlv->field_1_tlv_state)
    {
        const AnimRecord& animRec = AnimRec(AnimId::Door_Train_Closed);
        field_20_animation.Set_Animation_Data_409C80(animRec.mFrameTableOffset, 0);
        field_FC_current_state = eClosed_1;
    }
    else
    {
        field_20_animation.field_4_flags.Clear(AnimFlags::eBit2_Animate);
        field_20_animation.field_4_flags.Clear(AnimFlags::eBit3_Render);
        field_FC_current_state = eOpen_0;
    }
    field_20_animation.field_4_flags.Set(AnimFlags::eBit5_FlipX, pTlv->field_10_direction & 1);

    return this;
}

BaseGameObject* TrainDoor::VDestructor(s32 flags)
{
    return vdtor_4DD1D0(flags);
}

void TrainDoor::VUpdate()
{
    vUpdate_4DD2A0();
}

TrainDoor* TrainDoor::vdtor_4DD1D0(s32 flags)
{
    dtor_4DD200();
    if (flags & 1)
    {
        ae_delete_free_495540(this);
    }
    return this;
}

void TrainDoor::dtor_4DD200()
{
    SetVTable(this, 0x547AFC);

    if (field_FC_current_state == eOpen_0)
    {
        Path::TLV_Reset_4DB8E0(field_F4_tlvInfo, -1, 0, 0);
    }
    else
    {
        Path::TLV_Reset_4DB8E0(field_F4_tlvInfo, 1, 0, 0);
    }
    BaseAnimatedWithPhysicsGameObject_dtor_424AD0();
}

void TrainDoor::vUpdate_4DD2A0()
{
    if (field_FC_current_state == eOpen_0)
    {
        // Wait for Abe to GTFO
        if (sActiveHero_5C1B68->field_106_current_motion != eAbeMotions::Motion_115_DoorExit_459A40 && sActiveHero_5C1B68->field_106_current_motion != eAbeMotions::Motion_114_DoorEnter_459470)
        {
            // Then close
            const AnimRecord& animRec = AnimRec(AnimId::Door_Train_Closing);
            field_20_animation.Set_Animation_Data_409C80(animRec.mFrameTableOffset, 0);
            field_20_animation.field_4_flags.Set(AnimFlags::eBit2_Animate);
            field_20_animation.field_4_flags.Set(AnimFlags::eBit3_Render);
            field_FC_current_state = eClosed_1;
        }
    }

    field_B8_xpos = sTrainDoorXPos_BB4AA0 + FP_FromInteger(sTweakX_5C1BD0);
    field_BC_ypos = sTrainDoorYPos_BB4AA4 + FP_FromInteger(sTweakY_5C1BD4);
}
