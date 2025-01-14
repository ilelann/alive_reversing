#include "CamConverter.hpp"
#include "../../AliveLibAE/ResourceManager.hpp"
#include "LvlReaderWriter.hpp"
#include "relive_api_exceptions.hpp"
#include "../../AliveLibAE/ScreenManager.hpp"
#include "../../AliveLibCommon/CamDecompressor.hpp"
#include "../../AliveLibCommon/FG1Reader.hpp"
#include <lodepng/lodepng.h>
#include <iostream>

namespace ReliveAPI {

    
static u32 RGB565ToRGB888(u16 pixel)
{
    const u8 r5 = ((pixel >> 11) & 0x1F);
    const u8 g6 = ((pixel >> 5) & 0x3F);
    const u8 b5 = (pixel & 0x1F);

    const u32 r8 = ((r5 * 527) + 23) >> 6;
    const u32 g8 = ((g6 * 259) + 33) >> 6;
    const u32 b8 = ((b5 * 527) + 23) >> 6;

    const u32 rgb888 = (b8 << 16) | (g8 << 8) | r8;
    return rgb888;
}

static void RGB565ToPngBuffer(const u16* camBuffer, std::vector<u8>& outPngData)
{
    u32 dst[240][640] = {};
    for (u32 y = 0; y < 240; y++)
    {
        for (u32 x = 0; x < 640; x++)
        {
            dst[y][x] = RGB565ToRGB888(*camBuffer);
            camBuffer++;
        }
    }

    // we're going to encode with a state rather than a convenient function, because enforcing a color type requires setting options
    lodepng::State state;
    // input color type
    state.info_raw.colortype = LCT_RGBA;
    state.info_raw.bitdepth = 8;

    // output color type
    state.info_png.color.colortype = LCT_RGB;
    state.info_png.color.bitdepth = 8;
    state.encoder.auto_convert = 0; // without this, it would ignore the output color type specified above and choose an optimal one instead

    const auto error = lodepng::encode(outPngData, reinterpret_cast<const u8*>(&dst[0][0]), 640, 240, state);
    if (error)
    {
        std::cout << "encoder error " << error << ": " << lodepng_error_text(error) << std::endl;
    }
    /*
    else
    {
        lodepng::save_file(buffer, pFileName);
    }
    */
}

struct FG1Buffers
{
    void SetPixel(u32 layer, u32 x, u32 y, u16 pixel)
    {
        mFg1[layer][y][x] = pixel;
    }
    u16 mFg1[4][240][640];
};

class ApiFG1Reader final : public BaseFG1Reader
{
public:
    ApiFG1Reader(FG1Format format)
        : BaseFG1Reader(format)
    {
        // Dynamically allocated due to the huge stack space it would consume
        mFg1Buffers = new FG1Buffers();
    }

    ~ApiFG1Reader()
    {
        delete mFg1Buffers;
    }

    u16 ConvertPixel(u16 pixel)
    {
        return ((pixel >> 15) << 5)
                      | ((pixel & 31) << 11)
                      | (((pixel >> 5) & 31) << 6)
                      | (((pixel >> 10) & 31) << 0);
    }

    void BltRect(u32 xpos, u32 ypos, u32 width, u32 height, u32 layer, const u16* pSrcPixels, const u32* pBitMask)
    {
        mUsedLayers[layer] = true;

        for (u32 y = ypos; y < height; y++)
        {
            for (u32 x = xpos; x < width; x++)
            {
                if (x < 640 && y < 240)
                {
                    u16 pixelVal = 0xFFFF;
                    if (pSrcPixels)
                    {
                        // Read RGB565 pixel value for AO
                        pixelVal = ConvertPixel(*pSrcPixels);

                        // If its not a "transparent" pixel set to white
                        if (pixelVal != 0)
                        {
                            pixelVal = 0xFFFF;
                        }
                        pSrcPixels++;
                    }
                    else if (pBitMask)
                    {
                        // Bitfield in AE
                        const u32 bits = pBitMask[y - ypos];
                        if (!((bits >> (x - xpos)) & 1))
                        {
                            pixelVal = 0;
                        }
                    }

                    mFg1Buffers->SetPixel(layer, x, y, pixelVal);
                }
            }
        }
    }

    void OnPartialChunk(const Fg1Chunk& rChunk) override
    {
        const u16* pPixels = nullptr;
        const u32* pBitMap = nullptr;
        if (mFormat == FG1Format::AO)
        {
            pPixels = reinterpret_cast<const u16*>((&rChunk) + 1);
        }
        else
        {
            pBitMap = reinterpret_cast<const u32*>((&rChunk) + 1);
        }
        BltRect(rChunk.field_4_xpos_or_compressed_size,
                rChunk.field_6_ypos,
                rChunk.field_8_width + rChunk.field_4_xpos_or_compressed_size,
                rChunk.field_A_height + rChunk.field_6_ypos,
                rChunk.field_2_layer_or_decompressed_size,
                pPixels,
                pBitMap);
    }

    void OnFullChunk(const Fg1Chunk& rChunk) override
    {
        BltRect(rChunk.field_4_xpos_or_compressed_size,
                rChunk.field_6_ypos,
                rChunk.field_8_width + rChunk.field_4_xpos_or_compressed_size,
                rChunk.field_A_height + rChunk.field_6_ypos,
                rChunk.field_2_layer_or_decompressed_size,
                nullptr,
                nullptr);
    }

    u8** Allocate(u32 len) override
    {
        u8** pHolder = new u8*;
        *pHolder = new u8[len];
        return pHolder;
    }

    void Deallocate(u8** ptr) override
    {
        delete[] * ptr;
        delete ptr;
    }

    void SaveLayers(CameraImageAndLayers& outData)
    {
        for (u32 i = 0; i < 4; i++)
        {
            if (mUsedLayers[i])
            {
                const u16* pPixels = &mFg1Buffers->mFg1[i][0][0];
                RGB565ToPngBuffer(pPixels, BufferForLayer(outData, i));
            }
        }
    }

private:
    std::vector<u8>& BufferForLayer(CameraImageAndLayers& outData, u32 layer)
    {
        if (mFormat == FG1Format::AO)
        {
            switch (layer)
            {
                case 0:
                    return outData.mForegroundLayer;

                case 1:
                    return outData.mBackgroundLayer;
            }
        }
        else
        {
            switch (layer)
            {
                case 0:
                    return outData.mBackgroundWellLayer;

                case 1:
                    return outData.mBackgroundLayer;

                case 2:
                    return outData.mForegroundWellLayer;

                case 3:
                    return outData.mForegroundLayer;
            }
        }

        // Should never get here
        return outData.mBackgroundLayer;
    }

    // 2 layers in AO, 4 layers in AE
    bool mUsedLayers[4] = {};
    FG1Buffers* mFg1Buffers = nullptr;
};

static void AppendCamSegment(s32 x, s32 y, s32 width, s32 height, u16* pDst, const u16* pSrcPixels)
{
    const u16* pSrc = pSrcPixels;
    for (s32 y1 = y; y1 < height; y1++)
    {
        memcpy(&pDst[x + (y1 * 640)], &pSrc[(y1 * width)], width * sizeof(s16));
    }
}

CamConverterAO::CamConverterAO(const ChunkedLvlFile& camFile, CameraImageAndLayers& outData)
{
    std::optional<LvlFileChunk> bitsRes = camFile.ChunkByType(ResourceManager::Resource_Bits);
    if (bitsRes)
    {
        u16 camBuffer[640 * 240] = {};
        const u16* pIter = reinterpret_cast<const u16*>(bitsRes->Data().data());
        for (s16 xpos = 0; xpos < 640; xpos += 16)
        {
            const u16 slice_len = *pIter;
            pIter++; // Skip len

            AppendCamSegment(xpos, 0, 16, 240, camBuffer, pIter);

            // To next slice
            pIter += (slice_len / sizeof(s16));
        }
        RGB565ToPngBuffer(camBuffer, outData.mCameraImage);
    }

    std::optional<LvlFileChunk> fg1Res = camFile.ChunkByType(ResourceManager::Resource_FG1);
    if (fg1Res)
    {
        ApiFG1Reader reader(BaseFG1Reader::FG1Format::AO);
        reader.Iterate(reinterpret_cast<const FG1ResourceBlockHeader*>(fg1Res->Data().data()));
        reader.SaveLayers(outData);
    }
}

CamConverterAE::CamConverterAE(const ChunkedLvlFile& camFile, CameraImageAndLayers& outData)
{
    std::optional<LvlFileChunk> bitsRes = camFile.ChunkByType(ResourceManager::Resource_Bits);
    if (bitsRes)
    {
        u16 camBuffer[640 * 240] = {};
        u8 vlcBuffer[0x7E00] = {};
        CamDecompressor decompressor;
        const u16* pIter = reinterpret_cast<const u16*>(bitsRes->Data().data());
        for (s16 xpos = 0; xpos < 640; xpos += 16)
        {
            const u16 stripSize = *pIter;
            pIter++;

            if (stripSize > 0)
            {
                decompressor.vlc_decode(pIter, reinterpret_cast<u16*>(vlcBuffer));
                decompressor.process_segment(reinterpret_cast<u16*>(vlcBuffer), 0);
                AppendCamSegment(xpos, 0, 16, 240, camBuffer, decompressor.mDecompressedStrip);
            }

            pIter += (stripSize / sizeof(u16));
        }
        RGB565ToPngBuffer(camBuffer, outData.mCameraImage);
    }

    std::optional<LvlFileChunk> fg1Res = camFile.ChunkByType(ResourceManager::Resource_FG1);
    if (fg1Res)
    {
        ApiFG1Reader reader(BaseFG1Reader::FG1Format::AE);
        reader.Iterate(reinterpret_cast<const FG1ResourceBlockHeader*>(fg1Res->Data().data()));
        reader.SaveLayers(outData);
    }
}
} // namespace ReliveAPI
