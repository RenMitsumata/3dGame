
/*
 *   structures
 */
struct PS_IN
{
	float4 pos		: SV_POSITION;
	float3 worldPos : POSITION1;
	float3 tangent	: TANGENT0;
	float3 binormal	: BINORMAL0;
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
	float3		Dummy;//16bit境界用
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


Texture2D outTexture : register(t0);	// VRAMのt0に登録されているデータは「outTexture」として扱うよ
Texture2D normalTexture : register(t1);

SamplerState samplerState : register(s0);



float4 main(in PS_IN input) : SV_TARGET{
	
	
	float3 normalMap = (normalTexture.Sample(samplerState,input.texcoord)) * 2.0f - 1.0f;
	normalMap = normalize(normalMap);

	// マイナスの記号は人によって違うから、目で見てやる
	float3 normalW = normalize(normalMap.x * input.tangent + normalMap.y * input.binormal + normalMap.z * input.normal);

	
	// パーピクセルライティング
	// ライトベクトル(仮)->レジスタに書き入れて、CPU側から送る
	// ライトカラーも作って、レジスタから送る
	float3 lightDir = float3(1.0f, -1.0f, 1.0f);
	lightDir = normalize(lightDir);
	// ハーフランバート
	float light = (dot(normalize(-lightDir), normalW));
	light = saturate(light);	// light = max(light,0.0f);

	// 確認用

	float4 posW = mul(input.pos, World);

	
	
	// 反射ベクトルを求める
	float3 refVec = reflect(lightDir,normalW);
	float3 eyev = input.worldPos - cameraPos.xyz;	// posWは光らせたい座標方向のベクトル
	refVec = normalize(refVec);
	eyev = normalize(eyev);

	float specular = dot(normalize(-refVec),normalize(eyev));
	specular = saturate(specular);
	specular = pow(specular, 50);	// pow(基数,指数) -> 指数の部分はレジスタに書き込むべき＝スペキュラパワー
	float4 specularColor = float4(specular, specular, specular, 1.0f);
	// specularは、PS側でテクスチャの色に加算して出す→VSの出力、PSの入力にレイアウトを追加する
	
	
	// 視差マッピング
	
	float3 toEyeT;	// 視線のテクスチャ座標系の座標
	toEyeT.x = dot(cameraPos, -input.binormal);
	toEyeT.y = dot(cameraPos, input.tangent);
	toEyeT.z = dot(cameraPos, input.normal);



	float height = (normalTexture.Sample(samplerState, input.texcoord)).a * 2.0f - 1.0f;	// aは高さマッピングのグレースケール値

	float2 parallax_uv = input.texcoord + height * 0.2f * toEyeT.xy;	// 0.2fは高さマッピングの精度により変える ＋より→はずらしたい量
	
	

	//　　　　↑これで法線と色をテクスチャからとってくる

	/*

	float3 rayDir = input.pos.xyz - cameraPos.xyz;
	rayDir = normalize(rayDir);
	const float HEIGHT_MAX = 0.02f;

	float4 Out;

	float3 Eye;
	Eye.x = dot(rayDir, -input.tangent);
	Eye.y = dot(rayDir, input.binormal);
	Eye.z = dot(rayDir, input.normal);
	rayDir = Eye;


	//頂点座標形状での各ベクトル
	float3 E = normalize(Eye);
	float3 L = normalize(lightDir);

	//スクリーン空間の x,y 座標について、x.y の勾配を取得
	float2 dx, dy;
	dx = ddx(input.texcoord);
	dy = ddy(input.texcoord);

	//頂点座標系上での法線ベクトル
	float3 N = input.normal;

	//テクセルのサンプリング位置のオフセット値を調整する
	float2 offset = E.xy * 0.02f;

	//テクセルのサンプリング位置
	float2 texel = input.texcoord;

	float height = 0.0f;

	bool flg = false;

	for(int i=0;i<33;i++)
	{
		
		float4 NormalMap = normalTexture.Sample(samplerState, texel);

		//高さで比較する
		if (NormalMap.a * HEIGHT_MAX <= height)
		{
			//法線ベクトルを取得
			float3 Normal = 2.0f * NormalMap.rgb - 1.0f;

			//ハーフランバート拡散照明
			float color = dot(Normal,lightDir);
			color = color * 0.5f + 0.5f;
			color = color * color;

			Out = outTexture.Sample(samplerState, texel) * color;
			

			//ループ終了
			flg = true;
			break;
		}

		texel += offset;
		height += E.z * 0.02f;
	}

	Out = float4(Out.xyz, 1.0f);
	return Out + specular;


	*/




	/*

	
	const int DIVISION_NUM = 32;
	const float HEIGHT_MAX = 0.2f;
	float3 rayDir = input.worldPos - cameraPos.xyz;
	rayDir = normalize(rayDir);
	float length = 1.0f * HEIGHT_MAX / DIVISION_NUM;
	float3 deltaRay = rayDir * length;
	float3 rayPos = input.worldPos;
	float objHeight = -HEIGHT_MAX;
	float rayHeight = 0.0f;


	
	for (int i = 0; i < DIVISION_NUM&&objHeight < rayHeight; i++) {
		rayPos += deltaRay;	// 視線を奥に進める
		objHeight = normalTexture.Sample(samplerState, rayPos.xz).a;
		objHeight = objHeight * HEIGHT_MAX - HEIGHT_MAX;
		rayHeight = rayPos.y;
	}
	
	float2 parallax_uv = rayPos.xz;
	

	*/

	//return outTexture.Sample(samplerState, input.texcoord);
	return float4(outTexture.Sample(samplerState, parallax_uv).rgb * light,1.0f) + float4(specularColor.xyz,0.0f);

	


	
}