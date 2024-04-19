//=============================================================================
//
// �o���b�g���� [bullet.cpp]
// Author : 
//
//=============================================================================
#include "anti.h"
#include "enemy.h"
#include "collision.h"
#include "score.h"
#include "ebullet.h"
#include "effect.h"
#include "sound.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(40/2)	// �L�����T�C�Y
#define TEXTURE_HEIGHT				(90/2)	// 
#define TEXTURE_MAX					(1)		// �e�N�X�`���̐�

#define TEXTURE_PATTERN_DIVIDE_X	(3)		// �A�j���p�^�[���̃e�N�X�`�����������iX)
#define TEXTURE_PATTERN_DIVIDE_Y	(1)		// �A�j���p�^�[���̃e�N�X�`�����������iY)
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// �A�j���[�V�����p�^�[����
#define ANIM_WAIT					(4)		// �A�j���[�V�����̐؂�ւ��Wait�l


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char* g_TexturName[] = {
	"data/TEXTURE/abullet.png",
};

static BOOL		g_Load = FALSE;			// ���������s�������̃t���O
static ABULLET	g_ABullet[ABULLET_MAX];	// �o���b�g�\����


//=============================================================================
// ����������
//=============================================================================
HRESULT InitABullet(void)
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


	// �o���b�g�\���̂̏�����
	for (int i = 0; i < ABULLET_MAX; i++)
	{
		g_ABullet[i].use = FALSE;			// ���g�p�i���˂���Ă��Ȃ��e�j
		g_ABullet[i].w = TEXTURE_WIDTH;
		g_ABullet[i].h = TEXTURE_HEIGHT;
		g_ABullet[i].pos = XMFLOAT3(300, 300.0f, 0.0f);
		g_ABullet[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_ABullet[i].texNo = 0;

		g_ABullet[i].countAnim = 0;
		g_ABullet[i].patternAnim = 0;

		g_ABullet[i].move = XMFLOAT3(0.0f, +ABULLET_SPEED, 0.0f);	// �ړ��ʂ�������
	}

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitABullet(void)
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

}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateABullet(void)
{
	int bulletCount = 0;				// ���������o���b�g�̐�

	for (int i = 0; i < ABULLET_MAX; i++)
	{
		if (g_ABullet[i].use == TRUE)	// ���̃o���b�g���g���Ă���H
		{								// Yes
			// �A�j���[�V����  
			g_ABullet[i].countAnim++;
			if ((g_ABullet[i].countAnim % ANIM_WAIT) == 0)
			{
				// �p�^�[���̐؂�ւ�
				g_ABullet[i].patternAnim = (g_ABullet[i].patternAnim + 1) % ANIM_PATTERN_NUM;
			}

			// �o���b�g�̈ړ�����
			XMVECTOR pos = XMLoadFloat3(&g_ABullet[i].pos);
			XMVECTOR move = XMLoadFloat3(&g_ABullet[i].move);
			pos += move;
			XMStoreFloat3(&g_ABullet[i].pos, pos);

			// ��ʊO�܂Ői�񂾁H
			if (g_ABullet[i].pos.y < (-g_ABullet[i].h / 2))		// �����̑傫�����l�����ĉ�ʊO�����肵�Ă���
			{
				g_ABullet[i].use = false;
			}
			if (g_ABullet[i].pos.y > (SCREEN_HEIGHT + g_ABullet[i].h / 2))	// �����̑傫�����l�����ĉ�ʊO�����肵�Ă���
			{
				g_ABullet[i].use = false;
			}

			// �����蔻�菈��

			EBULLET* ebullet = GetEBullet();

			for (int j = 0; j < EBULLET_MAX; j++)
			{
				if (ebullet[j].use == TRUE)
				{
					BOOL ans = CollisionBC(
						g_ABullet[i].pos,
						ebullet[j].pos,
						g_ABullet[i].w / 2,
						ebullet[j].w / 2

					);


					if (ans == TRUE)
					{
						//�������Ă�����
						//�e������
						g_ABullet[i].use = FALSE;
						//�G�l�~�[������
						ebullet[j].use = FALSE;
						PlaySound(SOUND_LABEL_SE_BOMB);

						SetEffect(ebullet[j].pos.x, ebullet[j].pos.y, 100);


						AddScore(10);
						break;
					}
				}
			}

			bulletCount++;
		}
	}


}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawABullet(void)
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

	for (int i = 0; i < ABULLET_MAX; i++)
	{
		if (g_ABullet[i].use == TRUE)		// ���̃o���b�g���g���Ă���H
		{									// Yes
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_ABullet[i].texNo]);

			//�o���b�g�̈ʒu��e�N�X�`���[���W�𔽉f
			float px = g_ABullet[i].pos.x;	// �o���b�g�̕\���ʒuX
			float py = g_ABullet[i].pos.y;	// �o���b�g�̕\���ʒuY
			float pw = g_ABullet[i].w;		// �o���b�g�̕\����
			float ph = g_ABullet[i].h;		// �o���b�g�̕\������

			float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// �e�N�X�`���̕�
			float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// �e�N�X�`���̍���
			float tx = (float)(g_ABullet[i].patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// �e�N�X�`���̍���X���W
			float ty = (float)(g_ABullet[i].patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	// �e�N�X�`���̍���Y���W

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColorRotation(g_VertexBuffer,
				px, py, pw, ph,
				tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
				g_ABullet[i].rot.z);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}
	}

}


//=============================================================================
// �o���b�g�\���̂̐擪�A�h���X���擾
//=============================================================================
ABULLET* GetABullet(void)
{
	return &g_ABullet[0];
}


//=============================================================================
// �o���b�g�̔��ːݒ�
//=============================================================================
void SetABullet(XMFLOAT3 pos)
{
	// �������g�p�̒e�����������甭�˂��Ȃ�( =����ȏ㌂�ĂȂ����Ď� )
	for (int i = 0; i < ABULLET_MAX; i++)
	{
		if (g_ABullet[i].use == FALSE)		// ���g�p��Ԃ̃o���b�g��������
		{
			g_ABullet[i].use = TRUE;			// �g�p��Ԃ֕ύX����
			g_ABullet[i].pos = pos;			// ���W���Z�b�g
			return;							// 1���Z�b�g�����̂ŏI������
		}
	}
}

