struct SceneParams {
    // It looks like HLSL doesn't distinguish writeonly/readonly vs. readwrite,
    // so it's simplest to adopt the HLSL naming here and do either RWTexture2D
    // or RWBuffer, meaning readwrite, or Texture2D, Buffer, meaning read only.
    RWTexture2D<float4> image;
};

//in SceneParams scene;
RWTexture2D<float4> scene_image : register(u0, space0);

struct RayGenParams {
    float4 color;
};

ConstantBuffer<RayGenParams> RayGen_params : register(b0, space1);

[[shader("raygeneration")]]
void RayGen()
{
    uint2 pixel = DispatchRaysIndex().xy;
    float4 c;
    c = RayGen_params.color;
    scene_image[pixel] = RayGen_params.color;
}

