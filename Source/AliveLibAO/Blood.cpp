#include "stdafx_ao.h"
#include "Function.hpp"
#include "Blood.hpp"
#include "ResourceManager.hpp"
#include "ScreenManager.hpp"
#include "Game.hpp"
#include "Math.hpp"
#include "Map.hpp"
#include "PsxDisplay.hpp"
#include "stdlib.hpp"
#include "Primitives_common.hpp"
#include <algorithm>

#undef min
#undef max

namespace AO {

void Blood_ForceLink()
{ }

Blood* Blood::ctor_4072B0(FP xpos, FP ypos, FP xOff, FP yOff, FP scale, s16 count)
{
    ctor_417C10();

    SetVTable(this, 0x4BA248);

    field_BC_sprite_scale = scale;

    const AnimRecord rec = AO::AnimRec(AnimId::Blood);
    u8** ppRes = ResourceManager::GetLoadedResource_4554F0(ResourceManager::Resource_Animation, rec.mResourceId, 1, 0);
    Animation_Init_417FD0(rec.mFrameTableOffset, rec.mMaxW, rec.mMaxH, ppRes, 1);

    field_10_anim.field_4_flags.Clear(AnimFlags::eBit15_bSemiTrans);
    field_10_anim.field_8_r = 127;
    field_10_anim.field_9_g = 127;
    field_10_anim.field_A_b = 127;

    if (field_BC_sprite_scale == FP_FromInteger(1))
    {
        field_11C_render_layer = Layer::eLayer_Foreground_36;
    }
    else
    {
        field_11C_render_layer = Layer::eLayer_Foreground_Half_17;
    }

    if (field_BC_sprite_scale != FP_FromInteger(1))
    {
        field_10_anim.SetFrame_402AC0((field_10_anim.Get_Frame_Count_403540() >> 1) + 1);
    }

    field_116_total_count = count;
    field_112_to_render_count = count;

    field_E4_ppResBuf = ResourceManager::Allocate_New_Locked_Resource_454F80(ResourceManager::Resource_Blood, 0, count * sizeof(BloodParticle));
    if (field_E4_ppResBuf)
    {
        field_E8_pResBuf = reinterpret_cast<BloodParticle*>(*field_E4_ppResBuf);
        field_118_timer = 0;

        field_A8_xpos = xpos - FP_FromInteger(12);
        field_AC_ypos = ypos - FP_FromInteger(12);

        field_10E_xpos = FP_GetExponent(xpos - FP_FromInteger(12) + FP_FromInteger(pScreenManager_4FF7C8->field_14_xpos) - pScreenManager_4FF7C8->field_10_pCamPos->field_0_x);
        field_110_ypos = FP_GetExponent(ypos - FP_FromInteger(12) + FP_FromInteger(pScreenManager_4FF7C8->field_16_ypos) - pScreenManager_4FF7C8->field_10_pCamPos->field_4_y);

        if (field_10_anim.field_4_flags.Get(AnimFlags::eBit13_Is8Bit))
        {
            field_10C_texture_mode = TPageMode::e8Bit_1;
        }
        else if (field_10_anim.field_4_flags.Get(AnimFlags::eBit14_Is16Bit))
        {
            field_10C_texture_mode = TPageMode::e16Bit_2;
        }
        else
        {
            field_10C_texture_mode = TPageMode::e4Bit_0;
        }

        u8 u0 = field_10_anim.field_84_vram_rect.x & 0x3F;
        if (field_10C_texture_mode == TPageMode::e8Bit_1)
        {
            u0 = 2 * u0;
        }
        else if (field_10C_texture_mode == TPageMode::e4Bit_0)
        {
            u0 = 4 * u0;
        }

        u8 v0 = field_10_anim.field_84_vram_rect.y & 0xFF;

        FrameHeader* pFrameHeader = reinterpret_cast<FrameHeader*>(&(*field_10_anim.field_20_ppBlock)[field_10_anim.Get_FrameHeader_403A00(-1)->field_0_frame_header_offset]);

        const s16 frameW = pFrameHeader->field_4_width;
        const s16 frameH = pFrameHeader->field_5_height;

        field_10_anim.field_4_flags.Set(AnimFlags::eBit16_bBlending);

        for (s32 i = 0; i < field_116_total_count; i++)
        {
            for (s32 j = 0; j < 2; j++)
            {
                BloodParticle* pParticle = &field_E8_pResBuf[i];
                Prim_Sprt* pSprt = &pParticle->field_10_prims[j];

                Sprt_Init(pSprt);
                Poly_Set_SemiTrans_498A40(&pSprt->mBase.header, 1);

                if (field_10_anim.field_4_flags.Get(AnimFlags::eBit16_bBlending))
                {
                    Poly_Set_Blending_498A00(&pSprt->mBase.header, 1);
                }
                else
                {
                    Poly_Set_Blending_498A00(&pSprt->mBase.header, 0);

                    SetRGB0(pSprt, field_10_anim.field_8_r, field_10_anim.field_9_g, field_10_anim.field_A_b);
                }

                SetClut(pSprt,
                        static_cast<s16>(
                            PSX_getClut_496840(
                                field_10_anim.field_8C_pal_vram_xy.field_0_x,
                                field_10_anim.field_8C_pal_vram_xy.field_2_y)));

                SetUV0(pSprt, u0, v0);
                pSprt->field_14_w = frameW - 1;
                pSprt->field_16_h = frameH - 1;
            }
        }
        // Has its own random seed based on the frame counter.. no idea why
        field_114_rand_seed = static_cast<u8>(gnFrameCount_507670);

        for (s32 i = 0; i < field_112_to_render_count; i++)
        {
            field_E8_pResBuf[i].field_0_x = FP_FromInteger(field_10E_xpos);
            field_E8_pResBuf[i].field_4_y = FP_FromInteger(field_110_ypos);

            const FP randX = (FP_FromInteger(sRandomBytes_4BBE30[field_114_rand_seed++]) / FP_FromInteger(16));
            const FP adjustedX = FP_FromDouble(1.3) * (randX - FP_FromInteger(8));
            field_E8_pResBuf[i].field_8_offx = field_BC_sprite_scale * (xOff + adjustedX);

            const FP randY = (FP_FromInteger(sRandomBytes_4BBE30[field_114_rand_seed++]) / FP_FromInteger(16));
            const FP adjustedY = FP_FromDouble(1.3) * (randY - FP_FromInteger(8));
            field_E8_pResBuf[i].field_C_offy = field_BC_sprite_scale * (yOff + adjustedY);
        }
    }
    else
    {
        field_6_flags.Set(BaseGameObject::eDead_Bit3);
    }
    return this;
}

void Blood::VUpdate()
{
    VUpdate_407750();
}

void Blood::VUpdate_407750()
{
    if (field_118_timer > 0)
    {
        if (field_118_timer > 5)
        {
            field_112_to_render_count -= 10;
        }

        if (field_112_to_render_count <= 0)
        {
            field_112_to_render_count = 0;
            field_6_flags.Set(BaseGameObject::eDead_Bit3);
            return;
        }

        for (s32 i = 0; i < field_112_to_render_count; i++)
        {
            field_E8_pResBuf[i].field_C_offy += FP_FromDouble(1.8);

            field_E8_pResBuf[i].field_8_offx = field_E8_pResBuf[i].field_8_offx * FP_FromDouble(0.9);
            field_E8_pResBuf[i].field_C_offy = field_E8_pResBuf[i].field_C_offy * FP_FromDouble(0.9);

            field_E8_pResBuf[i].field_0_x += field_E8_pResBuf[i].field_8_offx;
            field_E8_pResBuf[i].field_4_y += field_E8_pResBuf[i].field_C_offy;
        }
    }

    field_118_timer++;
}


BaseGameObject* Blood::dtor_4076F0()
{
    SetVTable(this, 0x4BA248);
    if (field_E4_ppResBuf)
    {
        ResourceManager::FreeResource_455550(field_E4_ppResBuf);
    }
    return dtor_417D10();
}

BaseGameObject* Blood::VDestructor(s32 flags)
{
    return Vdtor_407AC0(flags);
}

BaseGameObject* Blood::Vdtor_407AC0(u32 flags)
{
    dtor_4076F0();
    if (flags & 1)
    {
        ao_delete_free_447540(this);
    }
    return this;
}

void Blood::VScreenChanged_407AB0()
{
    field_6_flags.Set(BaseGameObject::eDead_Bit3);
}

void Blood::VRender(PrimHeader** ppOt)
{
    VRender_407810(ppOt);
}

void Blood::VRender_407810(PrimHeader** ppOt)
{
    const auto bufferIdx = gPsxDisplay_504C78.field_A_buffer_index;
    if (gMap_507BA8.Is_Point_In_Current_Camera_4449C0(
            field_B2_lvl_number,
            field_B0_path_number,
            field_A8_xpos,
            field_AC_ypos,
            0))
    {
        PSX_Point xy = {32767, 32767};
        PSX_Point wh = {-32767, -32767};

        for (s32 i = 0; i < field_112_to_render_count; i++)
        {
            BloodParticle* pParticle = &field_E8_pResBuf[i];
            Prim_Sprt* pSprt = &pParticle->field_10_prims[gPsxDisplay_504C78.field_A_buffer_index];

            u8 u0 = field_10_anim.field_84_vram_rect.x & 63;
            if (field_10C_texture_mode == TPageMode::e8Bit_1)
            {
                u0 *= 2;
            }
            else if (field_10C_texture_mode == TPageMode::e4Bit_0)
            {
                u0 *= 4;
            }

            SetUV0(pSprt, u0, static_cast<u8>(field_10_anim.field_84_vram_rect.y));

            FrameHeader* pFrameHeader = reinterpret_cast<FrameHeader*>(
                &(*field_10_anim.field_20_ppBlock)[field_10_anim.Get_FrameHeader_403A00(-1)->field_0_frame_header_offset]);

            pSprt->field_14_w = pFrameHeader->field_4_width - 1;
            pSprt->field_16_h = pFrameHeader->field_5_height - 1;

            const s16 x0 = PsxToPCX(FP_GetExponent(pParticle->field_0_x));
            const s16 y0 = FP_GetExponent(pParticle->field_4_y);

            SetXY0(pSprt, x0, y0);

            if (!field_10_anim.field_4_flags.Get(AnimFlags::eBit16_bBlending))
            {
                SetRGB0(pSprt, field_10_anim.field_8_r, field_10_anim.field_9_g, field_10_anim.field_A_b);
            }

            OrderingTable_Add_498A80(OtLayer(ppOt, field_11C_render_layer), &pSprt->mBase.header);

            xy.field_0_x = std::min(x0, xy.field_0_x);
            xy.field_2_y = std::min(y0, xy.field_2_y);

            wh.field_0_x = std::max(x0, wh.field_0_x);
            wh.field_2_y = std::max(y0, wh.field_2_y);
        }

        s16 tpageY = 256;
        if (!field_10_anim.field_4_flags.Get(AnimFlags::eBit10_alternating_flag)
            && field_10_anim.field_84_vram_rect.y < 256)
        {
            tpageY = 0;
        }

        const auto tpage = PSX_getTPage_4965D0(
            field_10C_texture_mode,
            TPageAbr::eBlend_0,
            field_10_anim.field_84_vram_rect.x & 0xFFC0,
            tpageY);
        Prim_SetTPage* pTPage = &field_EC_tPages[bufferIdx];
        Init_SetTPage_495FB0(pTPage, 0, 0, tpage);
        OrderingTable_Add_498A80(OtLayer(ppOt, field_11C_render_layer), &pTPage->mBase);

        pScreenManager_4FF7C8->InvalidateRect_406E40(
            (xy.field_0_x - 12),
            (xy.field_2_y - 12),
            (wh.field_0_x + 12),
            (wh.field_2_y + 12),
            pScreenManager_4FF7C8->field_2E_idx);
    }
}

void Blood::VScreenChanged()
{
    VScreenChanged_407AB0();
}

} // namespace AO
