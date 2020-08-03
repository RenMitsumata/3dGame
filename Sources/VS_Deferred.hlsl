struct CONSTANT {
	matrix world;
	matrix view;
	matrix proj;
	float2 nearAndFar;
	float2 dummy;
};

cbuffer constantBuffer : register(b0) {
	CONSTANT constant;
}

// 入力構造体(※cpp側の入力レイアウトに合わせること！)
struct VS_IN {
	float4 position	: POSITION0;
	float4 normal	: NORMAL0;
	float4 color	: COLOR0;
	float2 texcoord : TEXCOORD0;
};

// 出力構造体(※ps側の引数と一致させること！)→入力構造体と一致させなくてよい
struct VS_OUT {
	float4 position	: SV_POSITION;
	float4 posW		: POSITION1;
	float4 posWV	: POSITION2;
	float4 normalW	: NORMAL0;
	float4 color	: COLOR0;
	float2 texcoord : TEXCOORD0;
};

//=============================================================================
// 頂点シェーダ
//=============================================================================
void main(in VS_IN input, out VS_OUT output)
{
	// WVP行列を求める
	matrix wvp;
	wvp = mul(constant.world, constant.view);
	wvp = mul(wvp, constant.proj);

	// position : wvp変換済み座標
	output.position = mul(input.position, wvp);

	// posW : ワールド変換済み座標
	output.posW = mul(input.position, constant.world);

	// posWV : ワールドビュー変換済み座標
	output.posWV = mul(input.position, constant.world);
	output.posWV = mul(output.posWV, constant.view);
	output.posWV.w = constant.nearAndFar.y - constant.nearAndFar.x;

	// normalW : ワールド変換済み法線
	input.normal.w = 0.0f;
	output.normalW = mul(input.normal, constant.world);
	output.normalW = normalize(output.normalW);

	// colorとtexcoordはパススルー
	output.color = input.color;
	output.texcoord = input.texcoord;
}