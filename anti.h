//=============================================================================
//
// �o���b�g���� [abullet.h]
// Author : 
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"
#include "sprite.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define ABULLET_MAX		(10)			// �o���b�g��Max��
#define ABULLET_SPEED	(1.0f)		// �o���b�g�̈ړ��X�s�[�h


// �o���b�g�\����
struct ABULLET
{
	BOOL				use;				// true:�g���Ă���  false:���g�p
	float				w, h;				// ���ƍ���
	XMFLOAT3			pos;				// �o���b�g�̍��W
	XMFLOAT3			rot;				// �o���b�g�̉�]��
	XMFLOAT3			move;				// �o���b�g�̈ړ���
	int					countAnim;			// �A�j���[�V�����J�E���g
	int					patternAnim;		// �A�j���[�V�����p�^�[���i���o�[
	int					texNo;				// ���Ԗڂ̃e�N�X�`���[���g�p����̂�

};




//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitABullet(void);
void UninitABullet(void);
void UpdateABullet(void);
void DrawABullet(void);

ABULLET *GetABullet(void);
void SetABullet(XMFLOAT3 pos);



