/*
struct SceneParams {
    RWTexture2D<float4> image;
    float test_constant;
};
*/

//in SceneParams scene;
RWTexture2D<float4> scene_image : register(u0, space0);
cbuffer scene_cbv : register(b0, space0) {
    float scene_test_constant;
}

struct RayGenParams {
    float4 color;
};

cbuffer RayGen_params_cbv : register(b1, space0) {
    float4 RayGen_params_color;
}

[[shader("raygeneration")]]
void RayGen()
{
    uint2 pixel = DispatchRaysIndex().xy;
    float4 c;
    c = RayGen_params.color;
    scene_image[pixel] = RayGen_params.color;
}

