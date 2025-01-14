#include "stdafx.h"
#include "GameEnderController.hpp"
#include "ResourceManager.hpp"
#include "stdlib.hpp"
#include "DDCheat.hpp"
#include "SwitchStates.hpp"
#include "Events.hpp"
#include "BirdPortal.hpp"
#include "Map.hpp"
#include "PauseMenu.hpp"
#include "ObjectIds.hpp"
#include "Abe.hpp"
#include "Alarm.hpp"
#include "Function.hpp"

ALIVE_VAR(1, 0x5C1BC6, s16, sFeeco_Restart_KilledMudCount_5C1BC6, 0);
ALIVE_VAR(1, 0x5C1BC8, s16, sFeecoRestart_SavedMudCount_5C1BC8, 0);

EXPORT void CC CreateGameEnderController_43B7A0()
{
    // Exit if it already exists
    for (s32 i = 0; i < gBaseGameObject_list_BB47C4->Size(); i++)
    {
        BaseGameObject* pObj = gBaseGameObject_list_BB47C4->ItemAt(i);
        if (!pObj)
        {
            break;
        }

        if (pObj->field_4_typeId == AETypes::eGameEnderController_57)
        {
            return;
        }
    }

    // Otherwise create one
    auto pGameEnderController = ae_new<GameEnderController>();
    if (pGameEnderController)
    {
        pGameEnderController->ctor_43B840();
    }
}


s32 CC GameEnderController::CreateFromSaveState_43BD10(const u8* pBuffer)
{
    auto pState = reinterpret_cast<const GameEnderController_State*>(pBuffer);
    auto pGameEnderController = ae_new<GameEnderController>();
    if (pGameEnderController)
    {
        pGameEnderController->ctor_43B840();
        pGameEnderController->field_C_objectId = pState->field_4_obj_id;
        pGameEnderController->field_20_timer = sGnFrame_5C1B84 + pState->field_8_timer;
        pGameEnderController->field_24_state = pState->field_C_state;
    }
    return sizeof(GameEnderController_State);
}

GameEnderController* GameEnderController::ctor_43B840()
{
    BaseGameObject_ctor_4DBFA0(TRUE, 0);
    SetVTable(this, 0x545198);
    field_4_typeId = AETypes::eGameEnderController_57;
    field_24_state = GameEnderController_States::eInit_0;
    Add_Resource_4DC130(ResourceManager::Resource_Animation, ResourceID::kDeathFlareResID);
    return this;
}

BaseGameObject* GameEnderController::VDestructor(s32 flags)
{
    return vdtor_43B8D0(flags);
}

void GameEnderController::VScreenChanged()
{
    vScreenChanged_43BC80();
}

void GameEnderController::VUpdate()
{
    vUpdate_43B920();
}

s32 GameEnderController::VGetSaveState(u8* pSaveBuffer)
{
    return vGetSaveState_43BCD0(reinterpret_cast<GameEnderController_State*>(pSaveBuffer));
}

GameEnderController* GameEnderController::vdtor_43B8D0(s32 flags)
{
    BaseGameObject_dtor_4DBEC0();
    if (flags & 1)
    {
        ae_delete_free_495540(this);
    }
    return this;
}

void GameEnderController::vScreenChanged_43BC80()
{
    if (field_24_state != GameEnderController_States::eInit_0)
    {
        if (sRescuedMudokons_5C1BC2 < 150)
        {
            SwitchStates_Set_465FF0(100u, 0);
        }
    }

    if (gMap_5C3030.field_0_current_level != gMap_5C3030.field_A_level)
    {
        field_6_flags.Set(BaseGameObject::eDead_Bit3);
    }
}

s32 GameEnderController::vGetSaveState_43BCD0(GameEnderController_State* pState)
{
    pState->field_0_type = AETypes::eGameEnderController_57;
    pState->field_4_obj_id = field_C_objectId;
    pState->field_8_timer = field_20_timer - sGnFrame_5C1B84;
    pState->field_C_state = field_24_state;
    return sizeof(GameEnderController_State);
}

void GameEnderController::vUpdate_43B920()
{
    if (Event_Get_422C00(kEventDeathReset))
    {
        field_6_flags.Set(BaseGameObject::eDead_Bit3);
    }

    switch (field_24_state)
    {
        case GameEnderController_States::eInit_0:
            if (SwitchStates_Get_466020(100u))
            {
                field_24_state = GameEnderController_States::eDetermineEnding_1;
            }
            break;

        case GameEnderController_States::eDetermineEnding_1:
        {
            auto pBirdPortal = static_cast<BirdPortal*>(sObjectIds_5C1B70.Find_449CF0(sActiveHero_5C1B68->field_1A8_portal_id));
            if (pBirdPortal)
            {
                if (pBirdPortal->field_28_state == BirdPortal::PortalStates::CollapseTerminators_10)
                {
                    auto pAlarm = sObjectIds_5C1B70.Find_449CF0(sAlarmObjId_550D70);
                    if (pAlarm)
                    {
                        pAlarm->field_6_flags.Set(BaseGameObject::eDead_Bit3);
                    }

                    pBirdPortal->field_6_flags.Set(BaseGameObject::eDead_Bit3);
                    sActiveHero_5C1B68->field_6_flags.Set(BaseGameObject::eDead_Bit3);

                    // Good ending
                    if (sRescuedMudokons_5C1BC2 >= 150)
                    {
                        gAbeBulletProof_5C1BDA = 0;
                        sFeeco_Restart_KilledMudCount_5C1BC6 = 0;
                        sFeecoRestart_SavedMudCount_5C1BC8 = 0;

                        if (pPauseMenu_5C9300)
                        {
                            pPauseMenu_5C9300->field_6_flags.Set(BaseGameObject::eDead_Bit3);
                            pPauseMenu_5C9300 = nullptr;
                        }

                        if (sRescuedMudokons_5C1BC2 >= 300)
                        {
                            // Perfect ending
                            gMap_5C3030.SetActiveCam_480D30(LevelIds::eBrewery_Ender_10, 1, 17, CameraSwapEffects::eUnknown_11, 17, 0);
                            field_24_state = GameEnderController_States::eAngelicEnding_5;
                        }
                        else
                        {
                            // Good enough ending
                            gMap_5C3030.SetActiveCam_480D30(LevelIds::eBrewery_Ender_10, 1, 18, CameraSwapEffects::eUnknown_11, 17, 0);
                            field_24_state = GameEnderController_States::eGoodEnding_4;
                        }
                    }
                    else
                    {
                        if (sKilledMudokons_5C1BC0 >= 255)
                        {
                            // Very bad ending
                            gAbeBulletProof_5C1BDA = TRUE;
                            gMap_5C3030.SetActiveCam_480D30(LevelIds::eBrewery_Ender_10, 1, 15, CameraSwapEffects::eUnknown_11, 18, 0);
                            field_24_state = GameEnderController_States::eBadEnding_3;
                        }
                        else
                        {
                            // Bad ending
                            gAbeBulletProof_5C1BDA = FALSE;
                            gMap_5C3030.SetActiveCam_480D30(LevelIds::eBrewery_Ender_10, 1, 16, CameraSwapEffects::eUnknown_11, 18, 0);
                            field_24_state = GameEnderController_States::eBadEnding_3;
                            sRescuedMudokons_5C1BC2 = sFeecoRestart_SavedMudCount_5C1BC8;
                            sKilledMudokons_5C1BC0 = sFeeco_Restart_KilledMudCount_5C1BC6;
                        }
                        sVisitedBonewerks_5C1C02 = FALSE;
                        sVisitedBarracks_5C1C04 = FALSE;
                        sVisitedFeecoEnder_5C1C06 = FALSE;
                    }
                }
            }
        }
        break;

        case GameEnderController_States::eBadEnding_3:
            if (sInputObject_5BD4E0.isHeld(InputCommands::Enum::eUnPause_OrConfirm) || sInputObject_5BD4E0.isHeld(InputCommands::Enum::eBack))
            {
                gMap_5C3030.SetActiveCam_480D30(LevelIds::eFeeCoDepot_5, 1, 1, CameraSwapEffects::eInstantChange_0, 0, 0);
                field_24_state = GameEnderController_States::eFinish_2;
            }
            break;

        case GameEnderController_States::eGoodEnding_4:
            if (sInputObject_5BD4E0.isHeld(InputCommands::Enum::eUnPause_OrConfirm) || sInputObject_5BD4E0.isHeld(InputCommands::Enum::eBack))
            {
                gMap_5C3030.SetActiveCam_480D30(LevelIds::eCredits_16, 1, 1, CameraSwapEffects::eInstantChange_0, 0, 0);
                field_24_state = GameEnderController_States::eFinish_2;
            }
            break;

        case GameEnderController_States::eAngelicEnding_5:
            if (sInputObject_5BD4E0.isHeld(InputCommands::Enum::eUnPause_OrConfirm) || sInputObject_5BD4E0.isHeld(InputCommands::Enum::eBack))
            {
                gMap_5C3030.SetActiveCam_480D30(LevelIds::eBrewery_Ender_10, 1, 20, CameraSwapEffects::eInstantChange_0, 0, 0);
                field_24_state = GameEnderController_States::eAngelicEndingCredits_6;
            }
            break;

        case GameEnderController_States::eAngelicEndingCredits_6:
            if (sInputObject_5BD4E0.isHeld(InputCommands::Enum::eUnPause_OrConfirm) || sInputObject_5BD4E0.isHeld(InputCommands::Enum::eBack))
            {
                gMap_5C3030.SetActiveCam_480D30(LevelIds::eCredits_16, 2, 1, CameraSwapEffects::eInstantChange_0, 0, 0);
                gMap_5C3030.field_CE_free_all_anim_and_palts = TRUE;
                field_24_state = GameEnderController_States::eFinish_2;
            }
            break;

        default:
            return;
    }
}
