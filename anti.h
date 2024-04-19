//=============================================================================
//
// バレット処理 [abullet.h]
// Author : 
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"
#include "sprite.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define ABULLET_MAX		(10)			// バレットのMax数
#define ABULLET_SPEED	(1.0f)		// バレットの移動スピード


// バレット構造体
struct ABULLET
{
	BOOL				use;				// true:使っている  false:未使用
	float				w, h;				// 幅と高さ
	XMFLOAT3			pos;				// バレットの座標
	XMFLOAT3			rot;				// バレットの回転量
	XMFLOAT3			move;				// バレットの移動量
	int					countAnim;			// アニメーションカウント
	int					patternAnim;		// アニメーションパターンナンバー
	int					texNo;				// 何番目のテクスチャーを使用するのか

};




//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitABullet(void);
void UninitABullet(void);
void UpdateABullet(void);
void DrawABullet(void);

ABULLET *GetABullet(void);
void SetABullet(XMFLOAT3 pos);



