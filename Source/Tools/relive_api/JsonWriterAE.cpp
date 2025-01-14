#include "JsonWriterAE.hpp"
#include "../../AliveLibAE/Path.hpp"
#include <jsonxx/jsonxx.h>
#include "TypesCollectionAE.hpp"
#include "JsonReaderBase.hpp"
#include "TlvObjectBase.hpp"

namespace ReliveAPI {
void JsonWriterAE::DebugDumpTlvs(const std::string& prefix, const PathInfo& info, std::vector<u8>& pathResource)
{
    u8* pData = pathResource.data();
    u8* pStart = pData + info.mObjectOffset;
    u8* pEnd = pData + info.mIndexTableOffset;

    Path_TLV* pPathTLV = reinterpret_cast<Path_TLV*>(pStart);
    s32 idx = 0;
    while (pPathTLV && reinterpret_cast<u8*>(pPathTLV) < pEnd)
    {
        idx++;
        DebugDumpTlv(prefix, idx, *pPathTLV);

        // Skip length bytes to get to the start of the next TLV
        u8* ptr = reinterpret_cast<u8*>(pPathTLV);
        u8* pNext = ptr + pPathTLV->field_2_length;
        pPathTLV = reinterpret_cast<Path_TLV*>(pNext);
    }
}

JsonWriterAE::JsonWriterAE(std::unique_ptr<TypesCollectionAE>&& typesCollection, s32 pathId, const std::string& pathBndName, const PathInfo& info)
    : JsonWriterBase(*typesCollection, pathId, pathBndName, info)
    , mTypesCollection{std::move(typesCollection)}
{
    mMapRootInfo.mGame = "AE";
}

JsonWriterAE::JsonWriterAE(s32 pathId, const std::string& pathBndName, const PathInfo& info)
    : JsonWriterAE{std::make_unique<TypesCollectionAE>(), pathId, pathBndName, info}
{
}

JsonWriterAE::~JsonWriterAE() = default;

void JsonWriterAE::ResetTypeCounterMap()
{
    mTypeCounterMap.clear();
}

[[nodiscard]] jsonxx::Array JsonWriterAE::ReadCollisionStream(u8* ptr, s32 numItems)
{
    jsonxx::Array collisionsArray;
    PathLine* pLineIter = reinterpret_cast<PathLine*>(ptr);
    TypesCollectionAE types;

    for (s32 i = 0; i < numItems; i++)
    {
        AELine tmpLine(types, &pLineIter[i]);

        jsonxx::Object properties;
        tmpLine.PropertiesToJson(types, properties);

        collisionsArray << properties;
    }
    return collisionsArray;
}

[[nodiscard]] jsonxx::Array JsonWriterAE::ReadTlvStream(u8* ptr)
{
    jsonxx::Array mapObjects;

    Path_TLV* pPathTLV = reinterpret_cast<Path_TLV*>(ptr);
    while (pPathTLV)
    {
        mTypeCounterMap[pPathTLV->field_4_type.mType]++;
        auto obj = mTypesCollection->MakeTlvAE(pPathTLV->field_4_type.mType, pPathTLV, mTypeCounterMap[pPathTLV->field_4_type.mType]);
        if (obj)
        {
            if (pPathTLV->field_2_length != obj->TlvLen())
            {
                LOG_ERROR(magic_enum::enum_name(pPathTLV->field_4_type.mType) << " size should be " << pPathTLV->field_2_length << " but got " << obj->TlvLen());
                throw ReliveAPI::WrongTLVLengthException();
            }

            mapObjects << obj->InstanceToJson(*mTypesCollection);
        }
        else
        {
            LOG_WARNING("Ignoring type: " << pPathTLV->field_4_type.mType);
        }

        pPathTLV = Path::Next_TLV(pPathTLV); // TODO: Will skip the last entry ??
    }

    return mapObjects;
}

[[nodiscard]] jsonxx::Array JsonWriterAE::AddCollisionLineStructureJson()
{
    AELine tmpLine(*mTypesCollection);
    return tmpLine.PropertiesToJson();
}
} // namespace ReliveAPI
