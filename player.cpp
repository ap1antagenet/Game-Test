//=============================================================================
//
// �v���C���[���� [player.cpp]
// Author : 
//
//=============================================================================
#include "player.h"
#include "input.h"
#include "bg.h"
#include "bullet.h"
#include "ebullet.h"
#include "enemy.h"
#include "collision.h"
#include "sound.h"
#include "score.h"
#include "anti.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(100)	// �L�����T�C�Y
#define TEXTURE_HEIGHT				(100)	// 
#define TEXTURE_MAX					(1)		// �e�N�X�`���̐�

#define TEXTURE_PATTERN_DIVIDE_X	(1)		// �A�j���p�^�[���̃e�N�X�`�����������iX)
#define TEXTURE_PATTERN_DIVIDE_Y	(1)		// �A�j���p�^�[���̃e�N�X�`�����������iY)
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// �A�j���[�V�����p�^�[����
#define ANIM_WAIT					(8)		// �A�j���[�V�����̐؂�ւ��Wait�l


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/player01.png",

};


static BOOL		g_Load = FALSE;			// ���������s�������̃t���O
static PLAYER	g_Player[PLAYER_MAX];	// �v���C���[�\����


//=============================================================================
// ����������
//=============================================================================
HRESULT InitPlayer(void)
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


	// �v���C���[�\���̂̏�����
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		g_Player[i].use = TRUE;
		g_Player[i].pos = XMFLOAT3(500.0f, 400.0f, 0.0f);	// ���S�_����\��
		g_Player[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);		// ��]��
		g_Player[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);		// �g��k��
		g_Player[i].w = TEXTURE_WIDTH;
		g_Player[i].h = TEXTURE_HEIGHT;
		g_Player[i].texNo = 0;

		g_Player[i].countAnim = 0;
		g_Player[i].patternAnim = 0;

		g_Player[i].move = XMFLOAT3(4.0f, 0.0f, 0.0f);		// �ړ���

	}


	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitPlayer(void)
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
void UpdatePlayer(void)
{

	for (int i = 0; i < PLAYER_MAX; i++)
	{
		// �����Ă�v���C���[��������������
		if (g_Player[i].use == TRUE)
		{
			// �n�`�Ƃ̓����蔻��p�ɍ��W�̃o�b�N�A�b�v������Ă���
			XMFLOAT3 pos_old = g_Player[i].pos;

			// �A�j���[�V����  
			g_Player[i].countAnim += 1.0f;
			if (g_Player[i].countAnim > ANIM_WAIT)
			{
				g_Player[i].countAnim = 0.0f;
				// �p�^�[���̐؂�ւ�
				g_Player[i].patternAnim = (g_Player[i].patternAnim + 1) % ANIM_PATTERN_NUM;
			}


			//g_Player[i].rot.z += 0.1f; //�L�����N�^�[��]

			// �L�[���͂ňړ� 
			{
				float speed = g_Player[i].move.x;

				if (GetKeyboardPress(DIK_C))
				{
					speed *= 4;
				}


				if (GetKeyboardPress(DIK_S))
				{
					g_Player[i].pos.y += speed;
				}
				else if (GetKeyboardPress(DIK_W))
				{
					g_Player[i].pos.y -= speed;
				}

				if (GetKeyboardPress(DIK_D))
				{
					g_Player[i].pos.x += speed;
				}
				else if (GetKeyboardPress(DIK_A))
				{
					g_Player[i].pos.x -= speed;
				}

				// �Q�[���p�b�h�łňړ�����
				if (IsButtonPressed(0, BUTTON_DOWN))
				{
					g_Player[i].pos.y += speed;
				}
				else if (IsButtonPressed(0, BUTTON_UP))
				{
					g_Player[i].pos.y -= speed;
				}

				if (IsButtonPressed(0, BUTTON_RIGHT))
				{
					g_Player[i].pos.x += speed;
				}
				else if (IsButtonPressed(0, BUTTON_LEFT))
				{
					g_Player[i].pos.x -= speed;
				}

				// MAP�O�`�F�b�N
				BG* bg = GetBG();

				if (g_Player[i].pos.x < 0.0f)
				{
					g_Player[i].pos.x = 0.0f;
				}

				if (g_Player[i].pos.x > bg->w)
				{
					g_Player[i].pos.x = bg->w;
				}

				if (g_Player[i].pos.y < 0.0f)
				{
					g_Player[i].pos.y = 0.0f;
				}

				if (g_Player[i].pos.y > bg->h)
				{
					g_Player[i].pos.y = bg->h;
				}

				// �ړ����I�������G�l�~�[�Ƃ̓����蔻��

				ENEMY* enemy = GetEnemy();

				for (int j = 0; j < ENEMY_MAX; j++)
				{
					//�����Ă�G�l�~�[�H
					if (enemy[j].use == TRUE)
					{
						BOOL ans = CollisionBB(g_Player[i].pos,
							g_Player[i].w,
							g_Player[i].h,
							enemy[j].pos,
							enemy[j].w,
							enemy[j].h);
						if (ans == TRUE)
						{
							//�������Ă�����
							//�e������
							//g_Player[i].use = FALSE;
							//�G�l�~�[������
							enemy[j].use = FALSE;
							break;
						}

					}

					//{
					//	float pr = g_Player[i].pos.x + g_Player[i].w / 2;
					//	float pl = g_Player[i].pos.x - g_Player[i].w / 2;
					//	float pu = g_Player[i].pos.y - g_Player[i].h / 2;
					//	float pd = g_Player[i].pos.y + g_Player[i].h / 2;

					//	float er = enemy[j].pos.x + enemy[j].w / 2;
					//	float el = enemy[j].pos.x - enemy[j].w / 2;
					//	float eu = enemy[j].pos.y - enemy[j].h / 2;
					//	float ed = enemy[j].pos.y + enemy[j].h / 2;

					//	//�o�E���f�B���O�{�b�N�X�ŏ������Ă݂�
					//	if (
					//		(pr > el) &&
					//		(pl < er) &&
					//		(pd > eu) &&
					//		(pu < ed)
					//		)
					//	{
					//		//�������Ă�����
					//		//�e������
					//		//g_Player[i].use = FALSE;
					//		//�G�l�~�[������
					//		enemy[j].use = FALSE;
					//		break;
					//	}
					//}
				}
				
				// �o���b�g����
				if (GetKeyboardTrigger(DIK_SPACE))
				{
					XMFLOAT3 pos = g_Player[i].pos;

					pos.y -= 10.0f;
					pos.x -= 30.0f;

					
					SetBullet(pos);

					pos.x += 60.0f;

					SetBullet(pos);

					MinScore(20);


					PlaySound(SOUND_LABEL_SE_SHOT);


				}
				if (GetKeyboardTrigger(DIK_F))
				{
					XMFLOAT3 pos = g_Player[i].pos;

					pos.y += 40.0f;
					pos.x -= 25.0f;

					SetABullet(pos);

					pos.x += 50.0f;

					SetABullet(pos);

					//MinScore(20);


					PlaySound(SOUND_LABEL_SE_SHOT);


				}

				if (IsButtonTriggered(0, BUTTON_B))
				{
					XMFLOAT3 pos = g_Player[i].pos;

					pos.y -= 10.0f;
					pos.x -= 30.0f;


					SetBullet(pos);

					pos.x += 60.0f;

					SetBullet(pos);

					MinScore(20);



					PlaySound(SOUND_LABEL_SE_SHOT);
				}
				if (IsButtonTriggered(0, BUTTON_A))
				{
					XMFLOAT3 pos = g_Player[i].pos;

					pos.y += 20.0f;
					pos.x -= 30.0f;

					SetABullet(pos);

					pos.x += 60.0f;

					SetABullet(pos);

					//MinScore(20);


					PlaySound(SOUND_LABEL_SE_SHOT);


				}

			}
		}
	}

#ifdef _DEBUG	// �f�o�b�O����\������


#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawPlayer(void)
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

	for (int i = 0; i < PLAYER_MAX; i++)
	{
		if (g_Player[i].use == TRUE)		// ���̃v���C���[���g���Ă���H
		{									// Yes
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Player[i].texNo]);

			//�v���C���[�̈ʒu��e�N�X�`���[���W�𔽉f
			float px = g_Player[i].pos.x;	// �v���C���[�̕\���ʒuX
			float py = g_Player[i].pos.y;	// �v���C���[�̕\���ʒuY
			float pw = g_Player[i].w;		// �v���C���[�̕\����
			float ph = g_Player[i].h;		// �v���C���[�̕\������

			// �A�j���[�V�����p
			//float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// �e�N�X�`���̕�
			//float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// �e�N�X�`���̍���
			//float tx = (float)(g_Player[i].patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// �e�N�X�`���̍���X���W
			//float ty = (float)(g_Player[i].patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	// �e�N�X�`���̍���Y���W

			float tw = 1.0f;	// �e�N�X�`���̕�
			float th = 1.0f;	// �e�N�X�`���̍���
			float tx = 0.0f* g_Player[i].patternAnim;	// �e�N�X�`���̍���X���W
			float ty = 0.0f;	// �e�N�X�`���̍���Y���W

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
				g_Player[i].rot.z);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}
	}


}


//=============================================================================
// Player�\���̂̐擪�A�h���X���擾
//=============================================================================
PLAYER* GetPlayer(void)
{
	return &g_Player[0];
}




