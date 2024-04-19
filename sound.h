//=============================================================================
//
// サウンド処理 [sound.h]
//
//=============================================================================
#pragma once

#include <windows.h>
#include "xaudio2.h"						// サウンド処理で必要

//*****************************************************************************
// サウンドファイル
//*****************************************************************************
enum 
{
	SOUND_LABEL_SE_BOMB,		// 爆発音
	SOUND_LABEL_SE_SHOT,		// 
	SOUND_LABEL_SE_LOCK,
	SOUND_LABEL_BGM_DZ,		// BGM DZ
	SOUND_LABEL_BGM_HMY,
	SOUND_LABEL_BGM_HIYE,


	SOUND_LABEL_MAX,
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
bool InitSound(HWND hWnd);
void UninitSound(void);
void PlaySound(int label);
void StopSound(int label);
void StopSound(void);

