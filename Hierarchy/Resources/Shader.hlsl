//////////////////////////////////////////////////////////////////////
// HLSL File:
// This example is compiled using the fxc shader compiler.
// It is possible directly compile HLSL in VS2013
//////////////////////////////////////////////////////////////////////

// This first constant buffer is special.
// The framework looks for particular variables and sets them automatically.
// See the CommonApp comments for the names it looks for.
cbuffer CommonApp
{
	float4x4 g_WVP;
	float4 g_lightDirections[MAX_NUM_LIGHTS];
	float3 g_lightColours[MAX_NUM_LIGHTS];
	int g_numLights;
	float4x4 g_InvXposeW;
	float4x4 g_W;
};


// When you define your own cbuffer you can use a matching structure in your app but you must be careful to match data alignment.
// Alternatively, you may use shader reflection to find offsets into buffers based on variable names.
// The compiler may optimise away the entire cbuffer if it is not used but it shouldn't remove indivdual variables within it.
// Any 'global' variables that are outside an explicit cbuffer go
// into a special cbuffer called "$Globals". This is more difficult to work with
// because you must use reflection to find them.
// Also, the compiler may optimise individual globals away if they are not used.
cbuffer MyApp
{
	float	g_frameCount;
	float3	g_waveOrigin;
}


// VSInput structure defines the vertex format expected by the input assembler when this shader is bound.
// You can find a matching structure in the C++ code.
struct VSInput
{
	float4 pos:POSITION;
	float4 colour:COLOUR0;
	float3 normal:NORMAL;
	float2 tex:TEXCOORD;
};

// PSInput structure is defining the output of the vertex shader and the input of the pixel shader.
// The variables are interpolated smoothly across triangles by the rasteriser.
struct PSInput
{
	float4 pos:SV_Position;
	float4 colour:COLOUR0;
	float3 normal:NORMAL;
	float2 tex:TEXCOORD;
	float4 mat:COLOUR1;
	float3 pos_ws:WORLDPOS;
};

// PSOutput structure is defining the output of the pixel shader, just a colour value.
struct PSOutput
{
	float4 colour:SV_Target;
};

// Define several Texture 'slots'
Texture2D g_materialMap;
Texture2D g_texture0;
Texture2D g_texture1;
Texture2D g_texture2;


// Define a state setting 'slot' for the sampler e.g. wrap/clamp modes, filtering etc.
SamplerState g_sampler;

// The vertex shader entry point. This function takes a single vertex and transforms it for the rasteriser.
void VSMain(const VSInput input, out PSInput output)
{
	//float4 pre_processed_pos = input.pos * float4(1, sin(g_frameCount * 0.03 + input.pos.x * 0.02 + input.pos.z * 0.02), 1, 1);
	output.pos = mul(input.pos, g_WVP);
	//output.colour = lerp(float4(1, 1, 0, 1), float4(0, 1, 0, 1), input.pos.y - 10);

	output.colour = input.colour;//lerp(float4(0, 0, 1, 1), float4(0, 0, 0.5, 1), pre_processed_pos.y*0.02) - 0.2;
	output.normal = input.normal;
	output.pos_ws = input.pos;
	output.tex = input.tex;
	float2 scaledTex = input.tex*(1.f / 32.f);
	output.mat = g_materialMap.SampleLevel(g_sampler, float2(scaledTex.x, 1 - scaledTex.y), 0);
}

// The pixel shader entry point. This function writes out the fragment/pixel colour.
void PSMain(const PSInput input, out PSOutput output)
{
	float4 pattern1 = g_texture0.Sample(g_sampler, input.tex);
	float4 pattern2 = g_texture1.Sample(g_sampler, input.tex);
	float4 pattern3 = g_texture2.Sample(g_sampler, input.tex);

	float4 patternFinal = lerp(lerp(lerp(pattern2, pattern1, input.mat.x), pattern2, input.mat.y), pattern3, input.mat.z);

	float3 sum = float3(0.f, 0.f, 0.f);

	for (int i = 0; i < g_numLights; ++i) {
		float dProd = max(dot(normalize(input.normal), g_lightDirections[i]), 0);

		sum += dProd * g_lightColours[i];
	}

	output.colour = float4(sum * patternFinal.xyz, 1.f);

	//output.colour = /*input.mat; */ patternFinal; // 'return' the colour value for this fragment.*/

}