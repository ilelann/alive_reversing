#pragma once

#include "../AliveLibCommon/FunctionFwd.hpp"
#include "Map.hpp"
#include "BaseAnimatedWithPhysicsGameObject.hpp"
#include "ScopedSeq.hpp"

namespace AO {

enum class SwitchState : s16
{
    eWaiting_0 = 0,
    ePulled_1 = 1,
    eFinished_2 = 2
};

enum class SwitchSoundType : s16
{
    eNone = 0,
    eWell_1 = 1,
    eSwitchBellHammer_2 = 2,
    eDoor_3 = 3,
    eElectricWall_4 = 4,
    eSecurityOrb_5 = 5
};

enum class SwitchOp : s16;

struct Path_Switch final : public Path_TLV
{
    s16 field_18_trigger_object;
    SwitchOp field_1A_trigger_object_action;
    u16 field_1C_scale;
    SwitchSoundType field_1E_on_sound;
    SwitchSoundType field_20_off_sound;
    s16 field_22_sound_direction;
};
ALIVE_ASSERT_SIZEOF(Path_Switch, 0x24);


class Switch final : public BaseAnimatedWithPhysicsGameObject
{
public:
    EXPORT Switch* ctor_481110(Path_Switch* pTlv, s32 tlvInfo);

    EXPORT BaseGameObject* dtor_481260();

    virtual BaseGameObject* VDestructor(s32 flags) override;

    EXPORT Switch* Vdtor_481700(s32 flags);

    virtual void VScreenChanged() override;

    EXPORT void VScreenChanged_4816F0();

    virtual void VUpdate() override;

    EXPORT void VUpdate_4812D0();

    // New virtual
    virtual s32 VPull(s16 bLeftDirection);

    EXPORT s32 vPull_481640(s16 bLeftDirection);

    s32 field_D4_padding[4];
    s16 field_E4_switchId;
    s16 field_E6_pad;
    SwitchState field_E8_state;
    s16 field_EA_pad;
    s32 field_EC_tlvInfo;
    s16 field_F0_bPulledFromLeft;
    SwitchOp field_F2_operation;
    SwitchSoundType field_F4_on_sound;
    SwitchSoundType field_F6_off_sound;
    s16 field_F8_sound_direction;
    s16 field_FA_pad;
};
ALIVE_ASSERT_SIZEOF(Switch, 0xFC);

} // namespace AO
