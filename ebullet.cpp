//=============================================================================
//
// �o���b�g���� [bullet.cpp]
// Author : 
//
//=============================================================================
#include "ebullet.h"
#include "enemy.h"
#include "player.h"
#include "collision.h"
#include "score.h"
#include "effect.h"
#include "sound.h"
#include "anti.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(50/2)	// �L�����T�C�Y
#define TEXTURE_HEIGHT				(90/2)	// 
#define TEXTURE_MAX					(1)		// �e�N�X�`���̐�

#define TEXTURE_PATTERN_DIVIDE_X	(1)		// �A�j���p�^�[���̃e�N�X�`�����������iX)
#define TEXTURE_PATTERN_DIVIDE_Y	(1)		// �A�j���p�^�[���̃e�N�X�`�����������iY)
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// �A�j���[�V�����p�^�[����
#define ANIM_WAIT					(4)		// �A�j���[�V�����̐؂�ւ��Wait�l


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[] = {
	"data/TEXTURE/bullet01.png",
};

static BOOL		g_Load = FALSE;			// ���������s�������̃t���O
static EBULLET	g_EBullet[EBULLET_MAX];	// �o���b�g�\����


//=============================================================================
// ����������
//=============================================================================
HRESULT InitEBullet(void)
{
	ID3D11Device *pDevice = GetDevice();

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
	for (int i = 0; i < EBULLET_MAX; i++)
	{
		g_EBullet[i].use   = FALSE;			// ���g�p�i���˂���Ă��Ȃ��e�j
		g_EBullet[i].w     = TEXTURE_WIDTH;
		g_EBullet[i].h     = TEXTURE_HEIGHT;
		g_EBullet[i].pos   = XMFLOAT3(300, 300.0f, 0.0f);
		g_EBullet[i].rot   = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_EBullet[i].texNo = 0;
		  
		g_EBullet[i].countAnim = 0;
		g_EBullet[i].patternAnim = 0;
		  
		g_EBullet[i].move = XMFLOAT3(0.0f, +EBULLET_SPEED, 0.0f);	// �ړ��ʂ�������
	}
	
	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitEBullet(void)
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
void UpdateEBullet(void)
{
	int ebulletCount = 0;				// ���������o���b�g�̐�

	for (int i = 0; i < EBULLET_MAX; i++)
	{
		if (g_EBullet[i].use == TRUE)	// ���̃o���b�g���g���Ă���H
		{								// Yes
			// �A�j���[�V����  
			g_EBullet[i].countAnim++;
			if ((g_EBullet[i].countAnim % ANIM_WAIT) == 0)
			{
				// �p�^�[���̐؂�ւ�
				g_EBullet[i].patternAnim = (g_EBullet[i].patternAnim + 1) % ANIM_PATTERN_NUM;
			}

			// �o���b�g�̈ړ�����
			{
				//XMVECTOR pos = XMLoadFloat3(&g_EBullet[i].pos);
				//XMVECTOR move = XMLoadFloat3(&g_EBullet[i].move);
				//pos += move;
				//XMStoreFloat3(&g_EBullet[i].pos, pos);

			}


			

			{

				////�O�p�֐��Ōv�Z����
				PLAYER* player = GetPlayer();
				ABULLET*abullet = GetABullet();


				if (abullet[0].use == TRUE) 
				{
					XMVECTOR epos = XMLoadFloat3(&g_EBullet[i].pos);
					XMVECTOR vec = XMLoadFloat3(&abullet[0].pos) - epos;
					float angle = atan2f(vec.m128_f32[1], vec.m128_f32[0]);
					float speed = 2.0f;

					g_EBullet[i].pos.x += cosf(angle) * speed;
					g_EBullet[i].pos.y += sinf(angle) * speed;

				}

				else
				{
					XMVECTOR epos = XMLoadFloat3(&g_EBullet[i].pos);
					XMVECTOR vec = XMLoadFloat3(&player[0].pos) - epos;
					float angle = atan2f(vec.m128_f32[1], vec.m128_f32[0]);
					float speed = 2.0f;

					g_EBullet[i].pos.x += cosf(angle) * speed;
					g_EBullet[i].pos.y += sinf(angle) * speed;
				}
			}


			// ��ʊO�܂Ői�񂾁H
			if (g_EBullet[i].pos.y < (-g_EBullet[i].h/2))		// �����̑傫�����l�����ĉ�ʊO�����肵�Ă���
			{
				g_EBullet[i].use = false;
			}
			if (g_EBullet[i].pos.y > (SCREEN_HEIGHT+ g_EBullet[i].h/2))	// �����̑傫�����l�����ĉ�ʊO�����肵�Ă���
			{
				g_EBullet[i].use = false;
			}

			// �����蔻�菈��
			PLAYER* player = GetPlayer();

			for (int j = 0; j < PLAYER_MAX; j++)
			{
				//�����Ă�G�l�~�[�H
				if (player[j].use == TRUE)
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
									g_EBullet[i].pos,
									player[j].pos,
									g_EBullet[i].w/2,
									player[j].w/2
									);


					if (ans == TRUE)
					{
						//�������Ă�����
						//�e������
						g_EBullet[i].use = FALSE;
						//�G�l�~�[������
						player[j].use = FALSE;
						PlaySound(SOUND_LABEL_SE_BOMB);


						SetEffect(player[j].pos.x, player[j].pos.y, 100);

					
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


			ebulletCount++;
		}
	}


}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawEBullet(void)
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

	for (int i = 0; i < EBULLET_MAX; i++)
	{
		if (g_EBullet[i].use == TRUE)		// ���̃o���b�g���g���Ă���H
		{									// Yes
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_EBullet[i].texNo]);

			//�o���b�g�̈ʒu��e�N�X�`���[���W�𔽉f
			float ex = g_EBullet[i].pos.x;	// �o���b�g�̕\���ʒuX
			float ey = g_EBullet[i].pos.y;	// �o���b�g�̕\���ʒuY
			float ew = g_EBullet[i].w;		// �o���b�g�̕\����
			float eh = g_EBullet[i].h;		// �o���b�g�̕\������

			float bw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// �e�N�X�`���̕�
			float bh = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// �e�N�X�`���̍���
			float bx = (float)(g_EBullet[i].patternAnim % TEXTURE_PATTERN_DIVIDE_X) * bw;	// �e�N�X�`���̍���X���W
			float by = (float)(g_EBullet[i].patternAnim / TEXTURE_PATTERN_DIVIDE_X) * bh;	// �e�N�X�`���̍���Y���W

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColorRotation(g_VertexBuffer, 
				ex, ey, ew, eh, 
				bx, by, bw, bh,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
				g_EBullet[i].rot.z);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}
	}

}


//=============================================================================
// �o���b�g�\���̂̐擪�A�h���X���擾
//=============================================================================
EBULLET *GetEBullet(void)
{
	return &g_EBullet[0];
}


//=============================================================================
// �o���b�g�̔��ːݒ�
//=============================================================================
void SetEBullet(XMFLOAT3 pos)
{
	// �������g�p�̒e�����������甭�˂��Ȃ�( =����ȏ㌂�ĂȂ����Ď� )
	for (int i = 0; i < EBULLET_MAX; i++)
	{
		if (g_EBullet[i].use == FALSE)		// ���g�p��Ԃ̃o���b�g��������
		{
			g_EBullet[i].use = TRUE;			// �g�p��Ԃ֕ύX����
			g_EBullet[i].pos = pos;			// ���W���Z�b�g
			return;							// 1���Z�b�g�����̂ŏI������
		}
	}
}

