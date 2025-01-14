#pragma once

#include "../AliveLibCommon/FunctionFwd.hpp"
#include "Path.hpp"
#include "BaseGameObject.hpp"

struct Path_FlyingSlig_Data final
{
    Scale_short field_0_scale;
    s16 field_2_state;
    s16 field_4_hi_pause_time;
    s16 field_6_patrol_pause_min;
    s16 field_8_patrol_pause_max;
    s16 field_A_direction;
    s16 field_C_panic_delay;
    s16 field_E_give_up_chase_delay;
    s16 field_10_prechase_delay;
    s16 field_12_slig_id;
    s16 field_14_listen_time;
    s16 field_16_trigger_id;
    s16 field_18_grenade_delay;
    s16 field_1A_max_velocity;
    s16 field_1C_launch_id;
    s16 field_1E_persistant;
};
ALIVE_ASSERT_SIZEOF_ALWAYS(Path_FlyingSlig_Data, 0x20);

struct Path_FlyingSlig final : public Path_TLV
{
    Path_FlyingSlig_Data field_10_data;
};
ALIVE_ASSERT_SIZEOF_ALWAYS(Path_FlyingSlig, 0x30);

struct Path_FlyingSligSpawner final : public Path_TLV
{
    Path_FlyingSlig_Data field_10;
};
ALIVE_ASSERT_SIZEOF_ALWAYS(Path_FlyingSligSpawner, 0x30);

struct FlyingSligSpawner_State final
{
    AETypes field_0_type;
    //s16 field_2_pad;
    s32 field_4_tlvInfo;
    s32 field_8_bSpawned;
    s32 field_C_spawned_slig_obj_id;
};
ALIVE_ASSERT_SIZEOF_ALWAYS(FlyingSligSpawner_State, 0x10);

class FlyingSligSpawner final : public BaseGameObject
{
public:
    EXPORT static s32 CC CreateFromSaveState_43B690(const u8* pBuffer);

    EXPORT FlyingSligSpawner* ctor_433D50(Path_FlyingSligSpawner* pTlv, s32 tlvInfo);

    virtual BaseGameObject* VDestructor(s32 flags) override;
    virtual void VUpdate() override;
    virtual s32 VGetSaveState(u8* pSaveBuffer) override;

private:
    EXPORT void dtor_434030();
    EXPORT FlyingSligSpawner* vdtor_433DE0(s32 flags);
    EXPORT void vUpdate_433E10();
    EXPORT s32 vGetSaveState_43B730(FlyingSligSpawner_State* pSaveState);

private:
    s32 field_20_tlvInfo;
    s32 field_24_spawned_slig_id;
    s16 field_28_trigger_id;
    s16 field_2A_pad;
    Path_TLV field_2C_tlv_header;
    s32 field_3C_bSpawned;
    s16 field_40_bFirstUpdate;
    s16 field_42_pad;
};
ALIVE_ASSERT_SIZEOF(FlyingSligSpawner, 0x44);
