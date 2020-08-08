

//*****************************************************************************
// �萔�o�b�t�@
//*****************************************************************************

// �}�g���N�X�o�b�t�@(b0)

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


// ���͍\����(��cpp���̓��̓��C�A�E�g�ɍ��킹�邱�ƁI)
struct VS_IN {
	float4 position	: POSITION0;
	float4 normal	: NORMAL0;
	float4 color	: COLOR0;
	float2 texcoord : TEXCOORD0;
};

// �o�͍\����(��ps���̈����ƈ�v�����邱�ƁI)�����͍\���̂ƈ�v�����Ȃ��Ă悢
struct VS_OUT {
	float4 position	: SV_POSITION;
	float4 color	: COLOR0;
	float2 texcoord : TEXCOORD0;
};

//=============================================================================
// ���_�V�F�[�_
//=============================================================================
void main(in VS_IN input,out VS_OUT output)
{
	// WVP�s������߂�
	matrix wvp;
	
	wvp = mul(World, View);
	
	wvp = mul(wvp, Projection);
	
	// position�̏����F
	// ���͂��ꂽposition����ʏ�̃|�W�V�����ɕύX
	output.position = mul(input.position, wvp);
	

	// color�̏����F
	// ���_�J���[�Ȃ̂œ��ɂ�����Ȃ��i���͂�A�����Ă���K�v�Ȃ��H�j
	output.color = input.color;

	// texcoord�̏����F
	// ps���ł�����̂ŁA���ɂ�����Ȃ�
	output.texcoord = input.texcoord;

}

