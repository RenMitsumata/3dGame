

//*****************************************************************************
// 定数バッファ
//*****************************************************************************

// マテリアルバッファ
cbuffer MaterialBuffer : register(b0)
{
	float4		Ambient;
	float4		Diffuse;
	float4		Specular;
	float4		Emission;
	float		Shininess;
	float3		Dummy;//16bit境界用
}

cbuffer cameraPos : register(b4) {
	float3 cameraPos;
	float dummy;
}
cbuffer playerDepth : register(b5) {
	float playerDepth;
	float3 dummyV;
}




//*****************************************************************************
// グローバル変数
//*****************************************************************************
Texture2D		g_Texture : register(t0);
SamplerState	g_SamplerState : register(s0);





struct PS_IN {
	float4 position	: SV_POSITION;
	float3 posW		: POSITION1;
	float4 normal	: NORMAL0;
	float4 color	: COLOR0;
	float2 texcoord : TEXCOORD0;
	float2 nearAndFar : TEXCOORD1;
};

//=============================================================================
// ピクセルシェーダ
//=============================================================================
float4 main(in PS_IN input) : SV_Target
{
	float4 outDiffuse = g_Texture.Sample(g_SamplerState,input.texcoord);

	//outDiffuse *= input.color *Diffuse;
	outDiffuse = outDiffuse * Diffuse;
	//outDiffuse.a = 1.0f;

	// このピクセルの深度値は？
	float lengthToCamera = length(cameraPos - input.posW);
	float myDepth = lengthToCamera / (input.nearAndFar.y - input.nearAndFar.x);

	float4 reDif = float4(myDepth, myDepth, myDepth, 1.0f);
	return reDif;
	/*if (playerDepth > myDepth) {
		outDiffuse.a = 0.0f;
	}*/


	return outDiffuse;

	//return float4(1.0f, 1.0f, 1.0f, 1.0f);
}
