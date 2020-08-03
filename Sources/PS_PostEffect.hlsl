
/*
 *   structures
 */
struct PS_IN
{
	float4 pos		: SV_POSITION;
	float3 cameraPos : POSITION1;
	float2 texcoord : TEXCOORD0;
};




Texture2D outTexture : register(t0);	// VRAMのt0に登録されているデータは「outTexture」として扱うよ
Texture2D posTexture : register(t1);
Texture2D normalTexture : register(t2);
Texture2D depthTexture: register(t3);

SamplerState samplerState : register(s0);



float4 main(in PS_IN input) : SV_TARGET{

	float4 outDiffuse;
	//ブルーム処理
	float4 diffuse;
	for(int y = -10; y < 11; y++){
		for(int x = -10; x < 11; x++){
			float2 texcoord = input.texcoord + float2(x*2/960.0f,y*2/540.0f);	// ガウスフィルター(1ピクセルとなりはUVでどれくらい？)
			float4 color = outTexture.Sample(samplerState,texcoord);
			float light = color.r * 0.299f + color.g * 0.587f + color.b * 0.114f;	// 輝度
			if(light > 1.0f && (x != 0 || y != 0)){
				diffuse += color / (x * x + y * y);
			}
		}
	}
	outDiffuse = outTexture.Sample(samplerState,input.texcoord);
	outDiffuse += diffuse * 0.5f;
	outDiffuse.a = 1.0f;



	// パーピクセルライティング
	// ライトベクトル(仮)->レジスタに書き入れて、CPU側から送る
	// ライトカラーも作って、レジスタから送る
	float3 lightDir = float3(1.0f, -1.0f, 1.0f);
	lightDir = normalize(lightDir);

	// ハーフランバート	
	float light = (dot(normalize(-lightDir), normalTexture.Sample(samplerState, input.texcoord).xyz));
	light = saturate(light);	// light = max(light,0.0f);

	// 確認用

	float3 posW = posTexture.Sample(samplerState, input.texcoord).xyz;



	// 反射ベクトルを求める
	float3 refVec = reflect(lightDir, normalTexture.Sample(samplerState, input.texcoord).xyz);
	float3 eyev = posW - input.cameraPos.xyz;	// posWは光らせたい座標方向のベクトル
	refVec = normalize(refVec);
	eyev = normalize(eyev);

	float specular = dot(normalize(-refVec),normalize(eyev));
	specular = saturate(specular);
	specular = pow(specular, 50);	// pow(基数,指数) -> 指数の部分はレジスタに書き込むべき＝スペキュラパワー
	float4 specularColor = float4(specular, specular, specular, 1.0f);
	// specularは、PS側でテクスチャの色に加算して出す→VSの出力、PSの入力にレイアウトを追加する


	outDiffuse += specularColor;

	return outDiffuse;


	

	////　　　　↑これで法線と色をテクスチャからとってくる



	////return outTexture.Sample(samplerState, input.texcoord);
	//return float4(outTexture.Sample(samplerState, parallax_uv).rgb * light,1.0f) + float4(specularColor.xyz,0.0f);





}