

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





//*****************************************************************************
// グローバル変数
//*****************************************************************************
Texture2D		g_Texture : register(t0);
SamplerState	g_SamplerState : register(s0);





struct PS_IN {
	float4 position	: SV_POSITION;
	float4 normal	: NORMAL0;
	float4 color	: COLOR0;
	float2 texcoord : TEXCOORD0;
};

//=============================================================================
// ピクセルシェーダ
//=============================================================================
float4 main(in PS_IN input) : SV_Target
{
	float4 outDiffuse = g_Texture.Sample(g_SamplerState,input.texcoord);

	//outDiffuse *= input.color *Diffuse;
	outDiffuse = outDiffuse * Diffuse;

	return outDiffuse;

	//return float4(1.0f, 1.0f, 1.0f, 1.0f);
}
