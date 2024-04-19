//=============================================================================
//
// �o���b�g���� [bullet.h]
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
#define EBULLET_MAX		(100)			// �o���b�g��Max��
#define EBULLET_SPEED	(4.0f)		// �o���b�g�̈ړ��X�s�[�h


// �o���b�g�\����
struct EBULLET
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
HRESULT InitEBullet(void);
void UninitEBullet(void);
void UpdateEBullet(void);
void DrawEBullet(void);

EBULLET *GetEBullet(void);
void SetEBullet(XMFLOAT3 pos);


