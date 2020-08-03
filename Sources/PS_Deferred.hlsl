
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

cbuffer playerDepth : register(b5)
{
	float4 pDepth;
}


struct PS_IN {
	float4 position	: SV_POSITION;
	float4 posW		: POSITION1;
	float4 posWV	: POSITION2;
	float4 normalW	: NORMAL0;
	float4 color	: COLOR0;
	float2 texcoord : TEXCOORD0;
};

struct PS_OUT {
	float4 outDiffuse  : SV_Target0;
	float4 outPosition : SV_Target1;
	float4 outNormal   : SV_Target2;
	float4 outDepth    : SV_Target3;
};

//*****************************************************************************
// グローバル変数
//*****************************************************************************
Texture2D		g_Texture : register(t0);
SamplerState	g_SamplerState : register(s0);

//=============================================================================
// ピクセルシェーダ
//=============================================================================
void main(in PS_IN input, out PS_OUT output)
{
	// outDiffuse : 色データを返す
	output.outDiffuse = input.color * Diffuse;
	output.outDiffuse *= g_Texture.Sample(g_SamplerState, input.texcoord);

	
	
	

	// outPosition : ワールド座標を返す
	output.outPosition = float4(input.posW.xyz, 1.0f);

	// outNormal : ワールド法線を返す
	float3 normal = input.normalW.xyz;
	normal = (normal + 1.0f) * 0.5f;
	output.outNormal = float4(normal, 1.0f);

	// outDepth : デプスマップを返す（後回し）
	float depth = input.posWV.z / input.posWV.w;
	//float depth = input.posWV.z / 30.0f;
	output.outDepth = float4(depth, 0.0f, 0.0f, 1.0f);
	//output.outDepth = float4(depth, depth, depth, 1.0f);

	// 自分のデプスがプレイヤーのデプスより低ければ、薄くする
	/*if (depth < 0.09f) {
		output.outDiffuse.a *= 0.75f;
	}*/



}



////ブルーム処理
	//float4 diffuse;
	//for(int y = -10; y < 11; y++){
	//	for(int x = -10; x < 11; x++){
	//		float2 texcoord = input.texcoord + float2(x*2/960.0f,y*2/540.0f);	// ガウスフィルター(1ピクセルとなりはUVでどれくらい？)
	//		float4 color = g_Texture.Sample(g_SamplerState,texcoord);
	//		float light = color.r * 0.299f + color.g * 0.587f + color.b * 0.114f;	// 輝度
	//		if(light > 1.0f && (x != 0 || y != 0)){
	//			diffuse += color / (x * x + y * y);
	//		}
	//	}
	//}
	//output.outDiffuse = g_Texture.Sample(g_SamplerState,input.texcoord);
	//output.outDiffuse += diffuse * 0.5f;
	//output.outDiffuse.a = 1.0f;
