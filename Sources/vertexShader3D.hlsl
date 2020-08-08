

//*****************************************************************************
// 定数バッファ
//*****************************************************************************

// マトリクスバッファ(b0)

cbuffer MatrixBuffer : register(b0)
{
	matrix World;
	matrix View;
	matrix Projection;
}



float4x4 InvTangentMatrix(float3 tangent, float3 binormal, float3 normal) {
	float4x4 mat = { float4(tangent,0.0f),
					 float4(binormal,0.0f),
					 float4(normal,0.0f),
					{0.0f,0.0f,0.0f,1.0f} };
	return transpose(mat);
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
	float4 color	: COLOR0;
	float2 texcoord : TEXCOORD0;
};

//=============================================================================
// 頂点シェーダ
//=============================================================================
void main(in VS_IN input,out VS_OUT output)
{
	// WVP行列を求める
	matrix wvp;
	
	wvp = mul(World, View);
	
	wvp = mul(wvp, Projection);
	
	// positionの処理：
	// 入力されたpositionを画面上のポジションに変更
	output.position = mul(input.position, wvp);
	

	// colorの処理：
	// 頂点カラーなので特にいじらない（もはや、持ってくる必要なし？）
	output.color = input.color;

	// texcoordの処理：
	// ps側でいじるので、特にいじらない
	output.texcoord = input.texcoord;

}

