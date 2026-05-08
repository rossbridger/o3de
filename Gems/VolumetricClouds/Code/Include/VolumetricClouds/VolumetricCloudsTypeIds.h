
#pragma once

namespace VolumetricClouds
{
    // System Component TypeIds
    inline constexpr const char* VolumetricCloudsSystemComponentTypeId = "{2A1CF52D-71BF-4477-85B3-4DB455915441}";
    inline constexpr const char* VolumetricCloudsEditorSystemComponentTypeId = "{F10A212F-1882-488B-B15F-D5885792358B}";

    // Module derived classes TypeIds
    inline constexpr const char* VolumetricCloudsModuleInterfaceTypeId = "{CE59B5F3-728D-4121-B3EF-9EB3C0C113B9}";
    inline constexpr const char* VolumetricCloudsModuleTypeId = "{76F6FCB3-BAC4-4470-B00F-63A0B27ED29B}";
    // The Editor Module by default is mutually exclusive with the Client Module
    // so they use the Same TypeId
    inline constexpr const char* VolumetricCloudsEditorModuleTypeId = VolumetricCloudsModuleTypeId;

    // Interface TypeIds
    inline constexpr const char* VolumetricCloudsRequestsTypeId = "{8BA843FF-A394-435D-A813-4A6E9BF02469}";
} // namespace VolumetricClouds
