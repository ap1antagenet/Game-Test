//=============================================================================
//
// �T�E���h���� [sound.h]
//
//=============================================================================
#pragma once

#include <windows.h>
#include "xaudio2.h"						// �T�E���h�����ŕK�v

//*****************************************************************************
// �T�E���h�t�@�C��
//*****************************************************************************
enum 
{
	SOUND_LABEL_SE_BOMB,		// ������
	SOUND_LABEL_SE_SHOT,		// 
	SOUND_LABEL_SE_LOCK,
	SOUND_LABEL_BGM_DZ,		// BGM DZ
	SOUND_LABEL_BGM_HMY,
	SOUND_LABEL_BGM_HIYE,


	SOUND_LABEL_MAX,
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
bool InitSound(HWND hWnd);
void UninitSound(void);
void PlaySound(int label);
void StopSound(int label);
void StopSound(void);

