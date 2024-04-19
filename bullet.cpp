//=============================================================================
//
// �o���b�g���� [bullet.cpp]
// Author : 
//
//=============================================================================
#include "bullet.h"
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
	"data/TEXTURE/bullet00.png",
};

static BOOL		g_Load = FALSE;			// ���������s�������̃t���O
static BULLET	g_ABullet[BULLET_MAX];	// �o���b�g�\����


//=============================================================================
// ����������
//=============================================================================
HRESULT InitBullet(void)
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
	for (int i = 0; i < BULLET_MAX; i++)
	{
		g_ABullet[i].use = FALSE;			// ���g�p�i���˂���Ă��Ȃ��e�j
		g_ABullet[i].w = TEXTURE_WIDTH;
		g_ABullet[i].h = TEXTURE_HEIGHT;
		g_ABullet[i].pos = XMFLOAT3(300, 300.0f, 0.0f);
		g_ABullet[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_ABullet[i].texNo = 0;

		g_ABullet[i].countAnim = 0;
		g_ABullet[i].patternAnim = 0;

		g_ABullet[i].move = XMFLOAT3(0.0f, -BULLET_SPEED, 0.0f);	// �ړ��ʂ�������
	}

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitBullet(void)
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
void UpdateBullet(void)
{
	int bulletCount = 0;				// ���������o���b�g�̐�

	for (int i = 0; i < BULLET_MAX; i++)
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

			ENEMY* enemy = GetEnemy();

			for (int j = 0; j < ENEMY_MAX; j++)
			{
				//�����Ă�G�l�~�[�H
				if (enemy[j].use == TRUE)
				{
					//���`����
					/*BOOL ans=CollisionBB(
										g_ABullet[i].pos,
										g_ABullet[i].w,
										g_ABullet[i].h,
										enemy[j].pos,
										enemy[j].w,
										enemy[j].h
										);*/


										//�ی`����
					BOOL ans = CollisionBC(
						g_ABullet[i].pos,
						enemy[j].pos,
						g_ABullet[i].w / 2,
						enemy[j].w / 2

					);

					if (ans == TRUE)
					{
						//�������Ă�����
						//�e������
						g_ABullet[i].use = FALSE;
						//�G�l�~�[������
						enemy[j].use = FALSE;

						PlaySound(SOUND_LABEL_SE_BOMB);

						 
						SetEffect(enemy[j].pos.x, enemy[j].pos.y, 100);

						AddScore(100);

						if (GetScore() <= 1000)
						{
							CreateEnemy(1);
						}
						else if (1000 < GetScore() <= 5000)
						{
							CreateEnemy(2);
						}
						else if (5000 < GetScore() <= 10000)
						{
							CreateEnemy(3);
						}
						else if (GetScore() > 10000)
						{
							CreateEnemy(5);
						}

						break;
					}

				}

				//���`����̕��G�ȏ������A�֐��p�ӂ��Ă��Ȃ�
				//{
				//	float br = g_ABullet[i].pos.x + g_ABullet[i].w / 2;
				//	float bl = g_ABullet[i].pos.x - g_ABullet[i].w / 2;
				//	float bu = g_ABullet[i].pos.y - g_ABullet[i].h / 2;
				//	float bd = g_ABullet[i].pos.y + g_ABullet[i].h / 2;
				//	
				//	float er = enemy[j].pos.x + enemy[j].w / 2;
				//	float el = enemy[j].pos.x - enemy[j].w / 2;
				//	float eu = enemy[j].pos.y - enemy[j].h / 2;
				//	float ed = enemy[j].pos.y + enemy[j].h / 2;
				//	
				//	//�o�E���f�B���O�{�b�N�X�ŏ������Ă݂�
				//	if (
				//		(br>el) &&
				//		(bl<er) &&
				//		(bd>eu) &&
				//		(bu<ed)
				//		)
				//	{
				//		//�������Ă�����
				//		//�e������
				//		g_ABullet[i].use = FALSE;
				//		//�G�l�~�[������
				//		enemy[j].use = FALSE;
				//		break;
				//	}
				//}
			}

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
void DrawBullet(void)
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

	for (int i = 0; i < BULLET_MAX; i++)
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
BULLET* GetBullet(void)
{
	return &g_ABullet[0];
}


//=============================================================================
// �o���b�g�̔��ːݒ�
//=============================================================================
void SetBullet(XMFLOAT3 pos)
{
	// �������g�p�̒e�����������甭�˂��Ȃ�( =����ȏ㌂�ĂȂ����Ď� )
	for (int i = 0; i < BULLET_MAX; i++)
	{
		if (g_ABullet[i].use == FALSE)		// ���g�p��Ԃ̃o���b�g��������
		{
			g_ABullet[i].use = TRUE;			// �g�p��Ԃ֕ύX����
			g_ABullet[i].pos = pos;			// ���W���Z�b�g
			return;							// 1���Z�b�g�����̂ŏI������
		}
	}
}

