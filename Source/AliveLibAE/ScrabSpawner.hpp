#pragma once

#include "BaseGameObject.hpp"
#include "../AliveLibCommon/FunctionFwd.hpp"
#include "Path.hpp"
#include "Scrab.hpp"

struct Path_ScrabSpawner final : public Path_Scrab
{
    s16 field_2C_switch_id;
    ScrabSpawnDirection field_2E_spawn_direction;
};
ALIVE_ASSERT_SIZEOF_ALWAYS(Path_ScrabSpawner, 0x30);

struct ScrabSpawner_State final
{
    AETypes field_0_type;
    s16 field_2_pad;
    s32 field_4_tlvInfo;
    s32 field_8_state;
    s32 field_C_spawned_scrab_id;
};
ALIVE_ASSERT_SIZEOF_ALWAYS(ScrabSpawner_State, 0x10);

class ScrabSpawner final : public BaseGameObject
{
public:
    EXPORT ScrabSpawner* ctor_4AB450(Path_ScrabSpawner* pTlv, s32 tlvInfo);
    EXPORT static s32 CC CreateFromSaveState_4ABEB0(const u8* pBuffer);

    virtual BaseGameObject* VDestructor(s32 flags) override;
    virtual void VUpdate() override;
    virtual s32 VGetSaveState(u8* pSaveBuffer) override;

private:
    EXPORT ScrabSpawner* vdtor_4AB4E0(s32 flags);
    EXPORT void dtor_4AB720();
    EXPORT s32 vGetSaveState_4ABF50(ScrabSpawner_State* pSaveState);
    EXPORT void vUpdate_4AB510();

private:
    s32 field_20_tlvInfo;
    s16 field_24_switch_id;
    ScrabSpawnDirection field_26_spawn_direction;
    Path_TLV field_28_tlv_data;
    s32 field_38_state;
    s32 field_3C_spawned_scrab_id;
    s16 field_40_bFindSpawnedScrab;
    s16 field_42_padding;
};
ALIVE_ASSERT_SIZEOF(ScrabSpawner, 0x44);
