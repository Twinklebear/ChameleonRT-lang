/*
struct SceneParams {
    RWTexture2D<float4> image;
    float test_constant;
};
*/

// in SceneParams scene;
RWTexture2D<float4> scene_image : register(u0, space0);
cbuffer scene_cbv : register(b0, space0)
{
    float scene_test_constant;
}

struct RayGenParams {
    float4 color;
};

cbuffer RayGen_params_cbv : register(b1, space0)
{
    float4 RayGen_params_color;
}

cbuffer scale_factor_cbv : register(b2, space0)
{
    float scale_factor;
}

[shader("raygeneration")]
void RayGen() {
    RayGenParams params;
    params.color = RayGen_params_color;
    {
        uint2 pixel = DispatchRaysIndex().xy;
        float4 c;
        c = params.color;
        scene_image[pixel] = params.color * scale_factor;
    }
}

