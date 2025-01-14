#pragma once

#include "Psx.hpp"
#include "../AliveLibCommon/FunctionFwd.hpp"
#include "FixedPoint.hpp"

namespace AO {

enum eLineTypes : u8
{
    eFloor_0 = 0,
    eWallLeft_1 = 1,
    eWallRight_2 = 2,
    eCeiling_3 = 3,
    eBackGroundFloor_4 = 4,
    eBackGroundWallLeft_5 = 5,
    eBackGroundWallRight_6 = 6,

    eUnknown_32 = 32,
    eUnknown_36 = 36,
};

class PathLine final
{
public:
    PSX_RECT field_0_rect;
    u8 field_8_type;
    s8 field_9_pad;
    s8 field_A_pad;
    s8 field_B_pad;
    s32 field_C_previous;
    s32 field_10_next;

    EXPORT PathLine* MoveOnLine_40CA20(FP* pXPos, FP* pYPos, const FP distToMove);
};
ALIVE_ASSERT_SIZEOF(PathLine, 20);

struct CollisionInfo;

class Collisions final
{
public:
    EXPORT static void CC Factory_40CEC0(const CollisionInfo* pCollisionInfo, const u8* pPathData);

    EXPORT void dtor_40CFB0();

    EXPORT Collisions* ctor_40CF30(const CollisionInfo* pCollisionInfo, const u8* pPathData);

    EXPORT PathLine* Add_Dynamic_Collision_Line_40C8A0(s16 x1, s16 y1, s16 x2, s16 y2, s8 mode);

    EXPORT s16 RayCast_40C410(FP X1_16_16, FP Y1_16_16, FP X2_16_16, FP Y2_16_16, PathLine** ppLine, FP* hitX, FP* hitY, u32 modeMask);

    EXPORT PathLine* PreviousLine_40C940(PathLine* pLine);

    EXPORT PathLine* NextLine_40C9B0(PathLine* pLine);

    PathLine* field_0_pArray;
    u16 field_4_current_item_count;
    s32 field_8_item_count;
    s32 field_C_max_count;
};
ALIVE_ASSERT_SIZEOF(Collisions, 0x10);

EXPORT PSX_RECT* CCSTD Rect_Clear_40C920(PSX_RECT* pRect);

ALIVE_VAR_EXTERN(Collisions*, sCollisions_DArray_504C6C);


} // namespace AO
