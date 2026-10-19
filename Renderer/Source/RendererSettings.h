#pragma once
#include "RendCore.h"
struct RendererStatistics{
    uint32_t DrawCallCount{};
    float RendererThreadFrameTime{};
    float ApplicationThreadFrameTime{};
    uint32_t InstanceCount{};
    uint32_t VertexCount{};

    //mem usage
    uint64_t GPUMemUsage{};
    uint64_t TextureMemUsage{};
    uint64_t GeometryMemUsage{};
};
struct RendererSettings{

    
};