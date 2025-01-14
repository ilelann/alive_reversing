#pragma once

#include "../AliveLibCommon/FunctionFwd.hpp"
#include "BaseGameObject.hpp"

struct Fg1Chunk;

namespace AO {

struct Fg1Block;

enum class LevelIds : s16;

class FG1 final : public BaseGameObject
{
public:
    EXPORT FG1* ctor_4539C0(u8** ppRes);

    EXPORT FG1* Vdtor_453E90(s32 flags);

    virtual BaseGameObject* VDestructor(s32 flags) override;

    virtual void VUpdate() override;

    virtual void VScreenChanged() override;

    EXPORT void VScreenChanged_453DE0();

    virtual void VRender(PrimHeader** ppOt) override;

    EXPORT void VRender_453D50(PrimHeader** ppOt);

    EXPORT void Convert_Chunk_To_Render_Block_453BA0(const Fg1Chunk* pChunk, Fg1Block* pBlock);

    EXPORT BaseGameObject* dtor_453DF0();

    s16 field_10_cam_pos_x;
    s16 field_12_cam_pos_y;
    LevelIds field_14_current_level;
    s16 field_16_current_path;
    s16 field_18_render_block_count;
    s16 field_1A_pad;
    u8** field_1C_ptr;
    Fg1Block* field_20_chnk_res;
};
ALIVE_ASSERT_SIZEOF(FG1, 0x24);

} // namespace AO
