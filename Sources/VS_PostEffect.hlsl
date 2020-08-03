

/*
 *   structures
 */
struct VS_IN
{
	float4 pos		: POSITION0;
	float2 texcoord : TEXCOORD0;
};

struct VS_OUT
{
	float4 pos		: SV_POSITION;
	float3 cameraPos : POSITION1;
	float2 texcoord : TEXCOORD0;
};



float4x4 InvTangentMatrix(float3 tangent, float3 binormal, float3 normal) {
	float4x4 mat = { float4(tangent,0.0f),
					 float4(binormal,0.0f),
					 float4(normal,0.0f),
					{0.0f,0.0f,0.0f,1.0f} };
	return transpose(mat);
}


/*
 *   constant buffers
 */
cbuffer ConstantBuf : register(b0)
{
	matrix projMatrix;
	float4 cameraPos;
};




/*
 *   entry point
 */
void main(in VS_IN input, out VS_OUT output)
{
	float4 pos = float4(input.pos);


	// positionの処理：
	// 入力されたpositionを画面上のポジションに変更

	output.pos = mul(pos, projMatrix);

	// texcoordの処理：
	// ps側でいじるので、特にいじらない
	output.texcoord = input.texcoord;

	output.cameraPos = cameraPos;
}