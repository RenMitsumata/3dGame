

/*
 *   structures
 */
struct VS_IN
{
	float4 pos		: POSITION0;
	float4 tangent  : TANGENT0;
	float4 binormal : BINORMAL0;
	float4 normal	: NORMAL0;
	float4 color	: COLOR0;
	float2 texcoord : TEXCOORD0;
};

struct VS_OUT
{
	float4 pos		: SV_POSITION;
	float3 worldPos : POSITION1;
	float3 tangent	: TANGENT0;
	float3 binormal : BINORMAL0;
	float3 normal	: NORMAL0;
	float2 texcoord : TEXCOORD0;
};


struct MATERIAL
{
	float4		Ambient;
	float4		Diffuse;
	float4		Specular;
	float4		Emission;
	float		Shininess;
	float3		Dummy;//16bit‹«ŠE—p
};



float4x4 InvTangentMatrix(float3 tangent, float3 binormal, float3 normal) {
	float4x4 mat = { float4(tangent,0.0f),
					 float4(binormal,0.0f),
					 float4(normal,0.0f),
					{0.0f,0.0f,0.0f,1.0f} };
	return transpose(mat);
}

struct LIGHT
{
	float4		Direction;
	float4		Diffuse;
	float4		Ambient;
};



/*
 *   constant buffers
 */
cbuffer ConstantBuf : register(b0)
{
	matrix World;
	matrix View;
	matrix Projection;
	matrix WorldViewProjection;
	matrix WorldInvTransMatrix;
	float4 cameraPos;
};


cbuffer MaterialBuffer : register(b3)
{
	MATERIAL	Material;
}

cbuffer LightBuffer : register(b4)
{
	LIGHT		Light;
}


/*
 *   entry point
 */
void main(in VS_IN input, out VS_OUT output)
{
	matrix wvp;
	wvp = mul(World, View);
	wvp = mul(wvp, Projection);

	output.pos = mul(input.pos, wvp);


	input.normal.w = 0.0f;
	output.normal = mul(input.normal, WorldInvTransMatrix).xyz;
	
	
	output.worldPos = mul(input.pos, World).xyz;

	output.texcoord = input.texcoord;
	
	
	input.tangent.w = 0.0f;
	output.tangent = mul(input.tangent, WorldInvTransMatrix).xyz;
	
	
	input.binormal.w = 0.0f;
	output.binormal = mul(input.binormal, WorldInvTransMatrix).xyz;
	
}