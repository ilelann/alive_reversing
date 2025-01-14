#pragma once

#include "../AliveLibCommon/FunctionFwd.hpp"
#include "Map.hpp"
#include "BaseAnimatedWithPhysicsGameObject.hpp"
#include "SwitchStates.hpp"

namespace AO {

enum class PullRingSwitchSound : s16
{
    eNone_0 = 0,
    eWellExit_1 = 1,
    RingBellHammer_2 = 2,
    eDoorEffect_3 = 3,
};

struct Path_PullRingRope final : public Path_TLV
{
    s16 field_18_id;
    SwitchOp field_1A_action;
    u16 field_1C_rope_length;
    u16 field_1E_scale;
    PullRingSwitchSound field_20_on_sound;
    PullRingSwitchSound field_22_off_sound;
    s16 field_24_sound_direction;
    s16 field_26_pad;
};
ALIVE_ASSERT_SIZEOF(Path_PullRingRope, 0x28);

class Rope;

class PullRingRope final : public BaseAnimatedWithPhysicsGameObject
{
public:
    EXPORT PullRingRope* ctor_4546B0(Path_PullRingRope* pTlv, s32 tlvInfo);

    EXPORT BaseGameObject* dtor_454910();

    virtual BaseGameObject* VDestructor(s32 flags) override;

    EXPORT PullRingRope* Vdtor_454D80(s32 flags);

    virtual void VScreenChanged() override;

    EXPORT void VScreenChanged_454D70();

    EXPORT Bool32 vIsNotBeingPulled_454D60();

    EXPORT s16 Pull_454CB0(BaseAliveGameObject* pFrom);

    virtual void VUpdate() override;

    EXPORT void VUpdate_4549A0();

    s32 field_D4_padding[4];
    s32 field_E4_stay_in_state_ticks;
    s32 field_E8_tlv_info;
    enum class States : s16
    {
        eIdle_0 = 0,
        eBeingPulled_1 = 1,
        eTriggerEvent_2 = 2,
        eReturnToIdle_3 = 3
    };
    States field_EC_state;
    s16 field_EE_id;
    SwitchOp field_F0_action;
    s16 field_F2_pad;
    BaseAliveGameObject* field_F4_pPuller;
    Rope* field_F8_pRope;
    PullRingSwitchSound field_FC_on_sound;
    PullRingSwitchSound field_FE_off_sound;
    s16 field_100_sound_direction;
    s16 field_102;
};
ALIVE_ASSERT_SIZEOF(PullRingRope, 0x104);


} // namespace AO
