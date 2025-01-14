#pragma once

#include "../AliveLibCommon/FunctionFwd.hpp"
#include "BaseGameObject.hpp"
#include "Path.hpp"
#include "Font.hpp"

void StatsSign_ForceLink();

struct Path_LCDStatusBoard final : public Path_TLV
{
    s16 field_10_number_of_muds;
    s16 field_12_zulag_number;
    s32 field_14_hidden;
};

class LCDStatusBoard final : public BaseGameObject
{
public:
    virtual BaseGameObject* VDestructor(s32 flags) override;
    virtual void VUpdate() override;
    virtual void VRender(PrimHeader** ppOt) override;
    virtual void VScreenChanged() override;

    EXPORT LCDStatusBoard* ctor_47B600(Path_LCDStatusBoard* params, TlvItemInfoUnion a3);
    EXPORT void dtor_47B7E0();
    EXPORT BaseGameObject* vdtor_47B7B0(s32 flags);
    EXPORT void vUpdate_47B8D0();
    EXPORT void vRender_47B900(PrimHeader** ppOt);
    EXPORT void vScreenChanged_47BC40();

public:
    Alive::Font field_20_font1;
    Alive::Font field_58_font2;
    Alive::Font field_90_font3;
    Alive::Font field_C8_font4;
    s32 field_100_objectId;
    s16 field_104_position_x;
    s16 field_106_position_y;
    s16 field_108_is_hidden;
    s16 field_10A_muds_left_in_area;
};
ALIVE_ASSERT_SIZEOF(LCDStatusBoard, 0x10C);

ALIVE_VAR_EXTERN(s16, sMudokonsInArea_5C1BC4);
ALIVE_VAR_EXTERN(s8, sStatsSignCurrentArea_5C1A20);
