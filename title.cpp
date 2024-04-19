//=============================================================================
//
// �^�C�g����ʏ��� [title.cpp]
// Author : 
//
//=============================================================================
#include "title.h"
#include "input.h"
#include "fade.h"



//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define BUTTON_SUM					(3)		// BUTTON�T�C�Y
#define BUTTON_WIDTH				(100)	// BUTTON�T�C�Y
#define BUTTON_HEIGHT				(20)	// 

#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// �w�i�T�C�Y
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(4)				// �e�N�X�`���̐�

#define TEXTURE_WIDTH_LOGO			(480)			// ���S�T�C�Y
#define TEXTURE_HEIGHT_LOGO			(80)			// 

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char* g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/bg000.jpg",
	"data/TEXTURE/title001.png",
	"data/TEXTURE/title002.png",
	"data/TEXTURE/title003.png",

};



static BOOL		g_Load = FALSE;
static TITLE	g_Title;
static BUTTON	g_Button[BUTTON_SUM];


static float	alpha;
static BOOL		flag_alpha;


//static float	effect_dx;
//static float	effect_dy;

int Up_stauts = 0;


//=============================================================================
// ����������
//=============================================================================
HRESULT InitTitle(void)
{
	ID3D11Device* pDevice = GetDevice();

	//�e�N�X�`������
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TexturName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}


	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);


	// �ϐ��̏�����
	g_Title.w = TEXTURE_WIDTH;
	g_Title.h = TEXTURE_HEIGHT;
	g_Title.pos = XMFLOAT3(g_Title.w / 2, g_Title.h / 2, 0.0f);
	g_Title.texNo = 0;

	g_Button[0].w = BUTTON_WIDTH;
	g_Button[0].h = BUTTON_HEIGHT;
	g_Button[0].pos = XMFLOAT3(g_Button[0].w / 2, g_Button[0].h / 2, 0.0f);
	g_Button[0].texNo = 1;
	g_Button[0].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	g_Button[1].w = BUTTON_WIDTH;
	g_Button[1].h = BUTTON_HEIGHT;
	g_Button[1].pos = XMFLOAT3(g_Button[1].w / 2, g_Button[1].h / 2, 0.0f);
	g_Button[1].texNo = 2;
	g_Button[1].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	//g_Button[2].w = BUTTON_WIDTH;
	//g_Button[2].h = BUTTON_HEIGHT;
	//g_Button[2].pos = XMFLOAT3(g_Button[1].w / 2, g_Button[1].h / 2, 0.0f);
	//g_Button[2].texNo = 3;
	//g_Button[2].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	alpha = 1.0f;
	flag_alpha = TRUE;

	//effect_dx = 100.0f;
	//effect_dy = 100.0f;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitTitle(void)
{
	if (g_Load == FALSE) return;

	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (g_Texture[i])
		{
			g_Texture[i]->Release();
			g_Texture[i] = NULL;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateTitle(void)
{
	if (GetKeyboardTrigger(DIK_UP) || IsButtonTriggered(0, BUTTON_UP))
	{
		Up_stauts--;

	}
	else if (GetKeyboardTrigger(DIK_DOWN) || IsButtonTriggered(0, BUTTON_DOWN))
	{
		Up_stauts++;
	}
	if (Up_stauts == -1)
	{
		Up_stauts = 1;
	}
	Up_stauts = Up_stauts % 2;
	if (GetKeyboardTrigger(DIK_RETURN) || IsButtonTriggered(0, BUTTON_START) || IsButtonTriggered(0, BUTTON_B))
	{// Enter��������A�X�e�[�W��؂�ւ���
		if (Up_stauts == 0)
		{
			SetFade(FADE_OUT, MODE_GAME);
			return;
		}
		else if (Up_stauts == 1)
		{
			SetFade(FADE_OUT, MODE_TUTORIAL);
			return;
		}
	

		


	}
	switch (Up_stauts)
	{
	case 0:
		g_Button[0].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
		g_Button[1].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		g_Button[2].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		break;

	case 1:
		g_Button[0].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		g_Button[1].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
		g_Button[2].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		break;

	//case 2:
	//	g_Button[0].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	//	g_Button[1].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	//	g_Button[2].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	//	break;

	default:
		break;
	}




#ifdef _DEBUG	// �f�o�b�O����\������
	//PrintDebugProc("Player:�� �� �� ���@Space\n");

#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawTitle(void)
{
	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// �}�g���N�X�ݒ�
	SetWorldViewProjection2D();

	// �v���~�e�B�u�g�|���W�ݒ�
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// �}�e���A���ݒ�
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	// �^�C�g���̔w�i��`��
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Title.texNo]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLeftTop(g_VertexBuffer, 0.0f, 0.0f, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	// START��BUTTON��`��
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Button[0].texNo]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor(g_VertexBuffer, +200.0f, +300, g_Button[0].w, g_Button[0].h, 0.0f, 0.0f, 1.0f, 1.0f, g_Button[0].color);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Button[1].texNo]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor(g_VertexBuffer, +200.0f, +350, g_Button[1].w, g_Button[1].h, 0.0f, 0.0f, 1.0f, 1.0f, g_Button[1].color);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	//{
	//	// �e�N�X�`���ݒ�
	//	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Button[2].texNo]);

	//	// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
	//	SetSpriteColor(g_VertexBuffer, +200.0f, +400, g_Button[2].w, g_Button[2].h, 0.0f, 0.0f, 1.0f, 1.0f, g_Button[2].color);

	//	// �|���S���`��
	//	GetDeviceContext()->Draw(4, 0);
	//}


}





