#pragma once

#include "BaseGameObject.hpp"

struct CdlLOC;

EXPORT void CC Get_fmvs_sectors_494460(const char* fmvName1, const char* fmvName2, const char* fmvName3, DWORD* fmvSectorPos1, DWORD* fmvSectorPos2, DWORD* fmvSectorPos3);
EXPORT char CC DDV_Play_493210(const char* pDDVName);
bool AreMovieSkippingInputsHeld();

class Movie : public BaseGameObject
{
public:
    virtual BaseGameObject* VDestructor(s32 flags) override;
    virtual void VUpdate() override;
    virtual void VScreenChanged() override;

    EXPORT void Init_4DFF60(s32 a2, CdlLOC* pCdPos, s16 bUnknown, s16 flags, s16 volume);
    EXPORT Movie* ctor_4DFDE0(s32 id, DWORD pos, s16 bUnknown, s16 flags, s16 volume);
    EXPORT void vUpdate_4E0030();
    EXPORT BaseGameObject* vdtor_4DFE80(s32 flags);
private:
    EXPORT void DeInit_4E0210();

    s16 field_20_unused;
    s16 field_22_volume;
    s32 field_24_padding;
    s32 field_28_padding;
    u8** field_2C_ppRes;
    s32 field_30_padding;
    s32 field_34_padding;
    s32 field_38_unused;
    s32 field_3C_unused;
    s32 field_40_padding;
    char field_44_cd_loc_min;
    char field_45_cd_loc_sec;
    char field_46_cd_loc_sector;
    char field_47_padding;
};
ALIVE_ASSERT_SIZEOF(Movie, 0x48);

ALIVE_VAR_EXTERN(s32, sMovie_ref_count_BB4AE4);
