// CRTL HLSL Output
struct SceneParams {
RWTexture2D<float4> image;
float test_constant;
};

RWTexture2D<float4> scene_image : register(u0, space0);
cbuffer scene_test_constant_cbv : register(b0, space0) {
	float scene_test_constant;
}

struct RayGenParams {
float4 color;
StructuredBuffer<float4> data;
};

StructuredBuffer<float4> RayGen_params_data : register(t0, space0);
cbuffer RayGen_params_cbv : register(b1, space0){
	float4 RayGen_params_color;
}

cbuffer RayGen_scale_factor_cbv : register(b2, space0) {
	float RayGen_scale_factor;
}

[shader("raygeneration")]
void RayGen()
{
RayGenParams RayGen_params;
RayGen_params.color = RayGen_params_color;
RayGen_params.data = RayGen_params_data;
{
uint2 pixel = DispatchRaysIndex().xy;
float4 c;
c = RayGen_params.color * scene_test_constant + RayGen_params.data[0];
scene_image[pixel] = RayGen_params.color * RayGen_scale_factor * c;
}
}

