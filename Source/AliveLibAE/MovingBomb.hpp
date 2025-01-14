#pragma once

#include "BaseAliveGameObject.hpp"
#include "Path.hpp"
#include "../AliveLibCommon/FunctionFwd.hpp"

struct Path_MovingBomb final : public Path_TLV
{
    u16 field_10_speed;
    s16 field_12_id;
    Choice_short field_14_bStart_type_triggered_by_alarm;
    Scale_short field_16_scale;
    s16 field_18_max_rise;
    s16 field_1A_disable_resources;
    u16 field_1C_start_speed;
    s16 field_1E_persist_offscreen;
};
ALIVE_ASSERT_SIZEOF_ALWAYS(Path_MovingBomb, 0x20);

struct Path_MovingBombStopper final : public Path_TLV
{
    s16 field_10_min;
    s16 field_12_max;
};
ALIVE_ASSERT_SIZEOF_ALWAYS(Path_MovingBombStopper, 0x14);

class MovingBomb final : public BaseAliveGameObject
{
public:
    EXPORT MovingBomb* ctor_46FD40(Path_MovingBomb* pTlv, s32 tlvInfo);

    virtual BaseGameObject* VDestructor(s32 flags) override;
    virtual void VUpdate() override;
    virtual void VRender(PrimHeader** ppOt) override;
    virtual void VOnThrowableHit(BaseGameObject* pFrom) override;
    virtual s16 VTakeDamage_408730(BaseGameObject* pFrom) override;
    virtual void VScreenChanged() override;

private:
    EXPORT MovingBomb* vdtor_470040(s32 flags);
    EXPORT void dtor_4700C0();
    EXPORT void BlowUp_470070();
    EXPORT void vRender_4707D0(PrimHeader** ot);
    EXPORT void vScreenChanged_470B90();
    EXPORT void FollowLine_470950();
    EXPORT s16 vTakeDamage_470990(BaseGameObject* pFrom);
    EXPORT void vOnThrowableHit_470800(BaseGameObject* pObj);
    EXPORT s16 HitObject_470830();
    EXPORT void vUpdate_4701E0();

private:
    enum class States : s16
    {
        eTriggeredByAlarm_0 = 0,
        eTriggeredBySwitch_1 = 1,
        eMoving_2 = 2,
        eStopMoving_3 = 3,
        eWaitABit_4 = 4,
        eToMoving_5 = 5,
        eBlowingUp_6 = 6,
        eKillMovingBomb_7 = 7
    };
    States field_118_state;
    s16 field_11A_padding;
    s32 field_11C_tlvInfo;
    s32 field_120_timer;
    FP field_124_speed;
    s16 field_128_switch_id;
    s16 field_12A_min;
    s16 field_12C_max;
    s16 field_12E_padding;
    s32 field_130_sound_channels;
    s16 field_134_disable_resources;
    s16 field_136_persist_offscreen;
};
ALIVE_ASSERT_SIZEOF(MovingBomb, 0x138);
