#pragma once

#include "JsonReaderBase.hpp"

namespace ReliveAPI {
class JsonReaderAE final : public JsonReaderBase
{
public:
    std::pair<std::vector<CameraNameAndTlvBlob>, std::vector<::PathLine>> Load(const std::string& fileName);
};
} // namespace ReliveAPI
