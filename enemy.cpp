//=============================================================================
//
// �v���C���[���� [enemy.cpp]
// Author : 
//
//=============================================================================
#include "enemy.h"
#include "player.h"
#include "input.h"
#include "bg.h"
#include "collision.h"
#include "ebullet.h"
#include "sound.h"
#include "effect.h"

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
static ID3D11Buffer* g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char* g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/enemy1.png",

};


static BOOL		g_Load = FALSE;			// ���������s�������̃t���O
static ENEMY	g_Enemy[ENEMY_MAX];	// �v���C���[�\����

int enemyTotal;
int enemycount;


static INTERPOLATION_DATA g_MoveTbl0[] = {
	//���W									��]��							�g�嗦							����
	{ XMFLOAT3(50.0f,  50.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f),	60 },
	{ XMFLOAT3(50.0f,  250.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f),	60 },
	{ XMFLOAT3(250.0f, 250.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f),	60 },
	{ XMFLOAT3(250.0f,  50.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f),	60 },

};


static INTERPOLATION_DATA g_MoveTbl1[] = {
	//���W									��]��							�g�嗦							����
	{ XMFLOAT3(900.0f,  50.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f),	60 },
	{ XMFLOAT3(900.0f,  250.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f),	60 },
	{ XMFLOAT3(700.0f, 250.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f),	60 },
	{ XMFLOAT3(700.0f,  50.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f),	60 },
};


static INTERPOLATION_DATA g_MoveTbl2[] = {
	//���W									��]��							�g�嗦							����
	{ XMFLOAT3(450.0f,  50.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f),	60 },
	{ XMFLOAT3(450.0f,  250.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f),	60 },
	{ XMFLOAT3(300.0f, 50.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f),	60 },
	{ XMFLOAT3(600.0f,  50.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f),	60 },
	{ XMFLOAT3(450.0f,  250.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f),	60 },

};


static INTERPOLATION_DATA* g_MoveTblAdr[] =
{
	g_MoveTbl0,
	g_MoveTbl1,
	g_MoveTbl2,

};

//=============================================================================
// ����������
//=============================================================================
HRESULT InitEnemy(void)
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


	// �\���̂̏�����

	enemycount = 0;
	CreateEnemy(1);



	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitEnemy(void)
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
void UpdateEnemy(void)
{
	

	for (int i = 0; i < ENEMY_MAX; i++)
	{
		PLAYER* player = GetPlayer();

		// �����Ă�v���C���[��������������
		if (g_Enemy[i].use == TRUE)
		{
			// �n�`�Ƃ̓����蔻��p�ɍ��W�̃o�b�N�A�b�v������Ă���
			XMFLOAT3 pos_old = g_Enemy[i].pos;

			// �A�j���[�V����  
			g_Enemy[i].countAnim += 1.0f;
			if (g_Enemy[i].countAnim > ANIM_WAIT)
			{
				g_Enemy[i].countAnim = 0.0f;
				// �p�^�[���̐؂�ւ�
				g_Enemy[i].patternAnim = (g_Enemy[i].patternAnim + 1) % ANIM_PATTERN_NUM;
			}

			//��]����
			/*g_Enemy[i].rot.z += -0.1f;
			*/

			// �ړ�����
			if (g_Enemy[i].tblMax > 0)	// ���`��Ԃ����s����H
			{	// ���`��Ԃ̏���
				int nowNo = (int)g_Enemy[i].time;			// �������ł���e�[�u���ԍ������o���Ă���
				int maxNo = g_Enemy[i].tblMax;				// �o�^�e�[�u�����𐔂��Ă���
				int nextNo = (nowNo + 1) % maxNo;			// �ړ���e�[�u���̔ԍ������߂Ă���
				INTERPOLATION_DATA* tbl = g_MoveTblAdr[g_Enemy[i].tblNo];	// �s���e�[�u���̃A�h���X���擾

				XMVECTOR nowPos = XMLoadFloat3(&tbl[nowNo].pos);	// XMVECTOR�֕ϊ�
				XMVECTOR nowRot = XMLoadFloat3(&tbl[nowNo].rot);	// XMVECTOR�֕ϊ�
				XMVECTOR nowScl = XMLoadFloat3(&tbl[nowNo].scl);	// XMVECTOR�֕ϊ�

				XMVECTOR Pos = XMLoadFloat3(&tbl[nextNo].pos) - nowPos;	// XYZ�ړ��ʂ��v�Z���Ă���
				XMVECTOR Rot = XMLoadFloat3(&tbl[nextNo].rot) - nowRot;	// XYZ��]�ʂ��v�Z���Ă���
				XMVECTOR Scl = XMLoadFloat3(&tbl[nextNo].scl) - nowScl;	// XYZ�g�嗦���v�Z���Ă���

 				float nowTime = g_Enemy[i].time - nowNo;	// ���ԕ����ł��鏭�������o���Ă���

				Pos *= nowTime;								// ���݂̈ړ��ʂ��v�Z���Ă���
				Rot *= nowTime;								// ���݂̉�]�ʂ��v�Z���Ă���
				Scl *= nowTime;								// ���݂̊g�嗦���v�Z���Ă���

				// �v�Z���ċ��߂��ړ��ʂ����݂̈ړ��e�[�u��XYZ�ɑ����Ă��遁�\�����W�����߂Ă���
				XMStoreFloat3(&g_Enemy[i].pos, nowPos + Pos);

				// �v�Z���ċ��߂���]�ʂ����݂̈ړ��e�[�u���ɑ����Ă���
				XMStoreFloat3(&g_Enemy[i].rot, nowRot + Rot);

				// �v�Z���ċ��߂��g�嗦�����݂̈ړ��e�[�u���ɑ����Ă���
				XMStoreFloat3(&g_Enemy[i].scl, nowScl + Scl);
				g_Enemy[i].w = TEXTURE_WIDTH * g_Enemy[i].scl.x;
				g_Enemy[i].h = TEXTURE_HEIGHT * g_Enemy[i].scl.y;

				// frame���g�Ď��Ԍo�ߏ���������
				g_Enemy[i].time += 1.0f / tbl[nowNo].frame;	// ���Ԃ�i�߂Ă���
				if ((int)g_Enemy[i].time >= maxNo)			// �o�^�e�[�u���Ō�܂ňړ��������H
				{
					g_Enemy[i].time -= maxNo;				// �O�ԖڂɃ��Z�b�g�������������������p���ł���
				}

			}

			//{
			//	//�����I�Ɉړ����Ă���
			//	// MAP�O�`�F�b�N
			//	BG* bg = GetBG();

			//	float speed = g_Enemy[i].move.x;

			//	//�E�֓����Ă���
			//	g_Enemy[i].pos.x += g_Enemy[i].direction * speed;

			//	if (g_Enemy[i].pos.x > bg->w)
			//	{
			//		g_Enemy[i].pos.x = bg->w;
			//		g_Enemy[i].move.x *= -1;	//������͍��֍s��
			//	}

			//	if (g_Enemy[i].pos.x < 0.0f)
			//	{
			//		g_Enemy[i].pos.x = 0.0f;
			//		g_Enemy[i].move.x *= -1;	//������͉E�֍s��
			//	}

			//}




			// �ړ����I�������PLAYER�Ƃ̓����蔻��

			for (int j = 0; j < PLAYER_MAX; j++)
			{
				//�����Ă�G�l�~�[�H
				if (player[j].use == TRUE)
				{
					BOOL ans = CollisionBB(g_Enemy[i].pos,
						g_Enemy[i].w,
						g_Enemy[i].h,
						player[j].pos,
						player[j].w,
						player[j].h);
					if (ans == TRUE)
					{
						//�������Ă�����
						//�e������
						player[j].use = FALSE;
						//�G�l�~�[������
						/*	g_Enemy[i].use = FALSE;*/
						break;
					}
				}

				//{
				//	float pr = player[j].pos.x + player[j].w / 2;
				//	float pl = player[j].pos.x - player[j].w / 2;
				//	float pu = player[j].pos.y - player[j].h / 2;
				//	float pd = player[j].pos.y + player[j].h / 2;

				//	float er = g_Enemy[i].pos.x + g_Enemy[i].w / 2;
				//	float el = g_Enemy[i].pos.x - g_Enemy[i].w / 2;
				//	float eu = g_Enemy[i].pos.y - g_Enemy[i].h / 2;
				//	float ed = g_Enemy[i].pos.y + g_Enemy[i].h / 2;

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
				//		player[j].use = FALSE;
				//		//�G�l�~�[������
				//	/*	g_Enemy[i].use = FALSE;*/
				//		break;
				//	}
				//}
			}


			// �o���b�g����
			if (GetKeyboardTrigger(DIK_SPACE))
			{
				XMFLOAT3 pos = g_Enemy[i].pos;

				pos.y += 70.0f;
				//pos.x -= 50.0f;


				SetEBullet(pos);


			}
			if (IsButtonTriggered(0, BUTTON_B))
			{

				XMFLOAT3 pos = g_Enemy[i].pos;

				pos.y += 70.0f;
				//pos.x -= 50.0f;


				SetEBullet(pos);
			}

		}

	}




#ifdef _DEBUG	// �f�o�b�O����\������


#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawEnemy(void)
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

	for (int i = 0; i < ENEMY_MAX; i++)
	{
		if (g_Enemy[i].use == TRUE)		// ���̃v���C���[���g���Ă���H
		{									// Yes
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Enemy[i].texNo]);

			//�v���C���[�̈ʒu��e�N�X�`���[���W�𔽉f
			float px = g_Enemy[i].pos.x;	// �v���C���[�̕\���ʒuX
			float py = g_Enemy[i].pos.y;	// �v���C���[�̕\���ʒuY
			float pw = g_Enemy[i].w;		// �v���C���[�̕\����
			float ph = g_Enemy[i].h;		// �v���C���[�̕\������

			// �A�j���[�V�����p
			//float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// �e�N�X�`���̕�
			//float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// �e�N�X�`���̍���
			//float tx = (float)(g_Enemy[i].patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// �e�N�X�`���̍���X���W
			//float ty = (float)(g_Enemy[i].patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	// �e�N�X�`���̍���Y���W

			float tw = 1.0f;	// �e�N�X�`���̕�
			float th = 1.0f;	// �e�N�X�`���̍���
			float tx = 0.0f * g_Enemy[i].patternAnim;	// �e�N�X�`���̍���X���W
			float ty = 0.0f;	// �e�N�X�`���̍���Y���W

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�


			SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
				g_Enemy[i].rot.z);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}
	}


}


//=============================================================================
// Enemy�\���̂̐擪�A�h���X���擾
//========================== ===================================================
ENEMY* GetEnemy(void)
{
	return &g_Enemy[0];
}



void CreateEnemy(int no)
{

	for (int i = 0; i < no; i++)
	{
		
		int movetype = rand() % 3;
		switch (movetype)
		{

		case 0:
			// 0�Ԃ������`��Ԃœ������Ă݂�

			g_Enemy[i + enemycount].pos = XMFLOAT3(50.0f, 100.0f, 0.0f);	// ���S�_����\��

			g_Enemy[i + enemycount].use = TRUE;
			g_Enemy[i + enemycount].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);		// ��]��
			g_Enemy[i + enemycount].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);		// �g��k��
			g_Enemy[i + enemycount].w = TEXTURE_WIDTH;
			g_Enemy[i + enemycount].h = TEXTURE_HEIGHT;
			g_Enemy[i + enemycount].texNo = 0;
			g_Enemy[i + enemycount].right = TRUE;
			g_Enemy[i + enemycount].countAnim = 0;
			g_Enemy[i + enemycount].patternAnim = 0;
			g_Enemy[i + enemycount].move = XMFLOAT3(4.0f, 0.0f, 0.0f);		// �ړ���
			//g_Enemy[i + enemycount].direction = (rand() % 2 == 0) ? -1 : 1;


			g_Enemy[i + enemycount].time = 0.0f;			// ���`��ԗp�̃^�C�}�[���N���A
			g_Enemy[i + enemycount].tblNo = 0;			// �Đ�����s���f�[�^�e�[�u��No���Z�b�g
			g_Enemy[i + enemycount].tblMax = 0;			// �Đ�����s���f�[�^�e�[�u���̃��R�[�h�����Z�b�g
			g_Enemy[i + enemycount].time = 0.0f;		// ���`��ԗp�̃^�C�}�[���N���A
			g_Enemy[i + enemycount].tblNo = 0;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
			g_Enemy[i + enemycount].tblMax = sizeof(g_MoveTbl0) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
			break;

		case 1:
			// 1�Ԃ������`��Ԃœ������Ă݂�
	
			g_Enemy[i + enemycount].pos = XMFLOAT3(900.0f, 100.0f, 0.0f);	// ���S�_����\��

			g_Enemy[i + enemycount].use = TRUE;
			g_Enemy[i + enemycount].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);		// ��]��
			g_Enemy[i + enemycount].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);		// �g��k��
			g_Enemy[i + enemycount].w = TEXTURE_WIDTH;
			g_Enemy[i + enemycount].h = TEXTURE_HEIGHT;
			g_Enemy[i + enemycount].texNo = 0;
			g_Enemy[i + enemycount].right = TRUE;
			g_Enemy[i + enemycount].countAnim = 0;
			g_Enemy[i + enemycount].patternAnim = 0;
			g_Enemy[i + enemycount].move = XMFLOAT3(4.0f, 0.0f, 0.0f);		// �ړ���
			//g_Enemy[i + enemycount].direction = (rand() % 2 == 0) ? -1 : 1;

			g_Enemy[i + enemycount].time = 0.0f;			// ���`��ԗp�̃^�C�}�[���N���A
			g_Enemy[i + enemycount].tblNo = 0;			// �Đ�����s���f�[�^�e�[�u��No���Z�b�g
			g_Enemy[i + enemycount].tblMax = 0;			// �Đ�����s���f�[�^�e�[�u���̃��R�[�h�����Z�b�g
			g_Enemy[i + enemycount].time = 0.0f;		// ���`��ԗp�̃^�C�}�[���N���A
			g_Enemy[i + enemycount].tblNo = 1;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
			g_Enemy[i + enemycount].tblMax = sizeof(g_MoveTbl1) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
			break;

		case 2:
			// 2�Ԃ������`��Ԃœ������Ă݂�

			g_Enemy[i + enemycount].pos = XMFLOAT3(450.0f, 100.0f, 0.0f);	// ���S�_����\��

			g_Enemy[i + enemycount].use = TRUE;
			g_Enemy[i + enemycount].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);		// ��]��
			g_Enemy[i + enemycount].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);		// �g��k��
			g_Enemy[i + enemycount].w = TEXTURE_WIDTH;
			g_Enemy[i + enemycount].h = TEXTURE_HEIGHT;
			g_Enemy[i + enemycount].texNo = 0;
			g_Enemy[i + enemycount].right = TRUE;
			g_Enemy[i + enemycount].countAnim = 0;
			g_Enemy[i + enemycount].patternAnim = 0;
			g_Enemy[i + enemycount].move = XMFLOAT3(4.0f, 0.0f, 0.0f);		// �ړ���
			//g_Enemy[i + enemycount].direction = (rand() % 2 == 0) ? -1 : 1;


			g_Enemy[i + enemycount].time = 0.0f;			// ���`��ԗp�̃^�C�}�[���N���A
			g_Enemy[i + enemycount].tblNo = 0;			// �Đ�����s���f�[�^�e�[�u��No���Z�b�g
			g_Enemy[i + enemycount].tblMax = 0;			// �Đ�����s���f�[�^�e�[�u���̃��R�[�h�����Z�b�g
			g_Enemy[i + enemycount].time = 0.0f;		// ���`��ԗp�̃^�C�}�[���N���A
			g_Enemy[i + enemycount].tblNo = 2;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
			g_Enemy[i + enemycount].tblMax = sizeof(g_MoveTbl2) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
			break;

		}

	}
	enemycount += no;

	PlaySound(SOUND_LABEL_SE_LOCK);
	g_Load = TRUE;


}


