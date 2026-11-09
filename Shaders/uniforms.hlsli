#pragma once

#define OBJECT_SLOT         b0
#define FRAME_SLOT          b1
#define LIGHT_SLOT          b2
#define MATERIAL_SLOT       b3

#define VERTEX_UNIFORM_SPACE         space1
#define FRAGMENT_UNIFORM_SPACE       space3

struct Object
{
    float4x4 model;
    float4x4 normalMatrix;
};

struct Frame
{
    float4x4 view;
    float4x4 projection;
    float4x4 viewProjection;
    float3 cameraPosition;
    float time;
};