//=============================================================================
//
// バレット処理 [bullet.cpp]
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
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(40/2)	// キャラサイズ
#define TEXTURE_HEIGHT				(90/2)	// 
#define TEXTURE_MAX					(1)		// テクスチャの数

#define TEXTURE_PATTERN_DIVIDE_X	(3)		// アニメパターンのテクスチャ内分割数（X)
#define TEXTURE_PATTERN_DIVIDE_Y	(1)		// アニメパターンのテクスチャ内分割数（Y)
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// アニメーションパターン数
#define ANIM_WAIT					(4)		// アニメーションの切り替わるWait値


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char* g_TexturName[] = {
	"data/TEXTURE/bullet00.png",
};

static BOOL		g_Load = FALSE;			// 初期化を行ったかのフラグ
static BULLET	g_ABullet[BULLET_MAX];	// バレット構造体


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitBullet(void)
{
	ID3D11Device* pDevice = GetDevice();

	//テクスチャ生成
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


	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);


	// バレット構造体の初期化
	for (int i = 0; i < BULLET_MAX; i++)
	{
		g_ABullet[i].use = FALSE;			// 未使用（発射されていない弾）
		g_ABullet[i].w = TEXTURE_WIDTH;
		g_ABullet[i].h = TEXTURE_HEIGHT;
		g_ABullet[i].pos = XMFLOAT3(300, 300.0f, 0.0f);
		g_ABullet[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_ABullet[i].texNo = 0;

		g_ABullet[i].countAnim = 0;
		g_ABullet[i].patternAnim = 0;

		g_ABullet[i].move = XMFLOAT3(0.0f, -BULLET_SPEED, 0.0f);	// 移動量を初期化
	}

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
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
// 更新処理
//=============================================================================
void UpdateBullet(void)
{
	int bulletCount = 0;				// 処理したバレットの数

	for (int i = 0; i < BULLET_MAX; i++)
	{
		if (g_ABullet[i].use == TRUE)	// このバレットが使われている？
		{								// Yes
			// アニメーション  
			g_ABullet[i].countAnim++;
			if ((g_ABullet[i].countAnim % ANIM_WAIT) == 0)
			{
				// パターンの切り替え
				g_ABullet[i].patternAnim = (g_ABullet[i].patternAnim + 1) % ANIM_PATTERN_NUM;
			}

			// バレットの移動処理
			XMVECTOR pos = XMLoadFloat3(&g_ABullet[i].pos);
			XMVECTOR move = XMLoadFloat3(&g_ABullet[i].move);
			pos += move;
			XMStoreFloat3(&g_ABullet[i].pos, pos);

			// 画面外まで進んだ？
			if (g_ABullet[i].pos.y < (-g_ABullet[i].h / 2))		// 自分の大きさを考慮して画面外か判定している
			{
				g_ABullet[i].use = false;
			}
			if (g_ABullet[i].pos.y > (SCREEN_HEIGHT + g_ABullet[i].h / 2))	// 自分の大きさを考慮して画面外か判定している
			{
				g_ABullet[i].use = false;
			}

			// 当たり判定処理

			ENEMY* enemy = GetEnemy();

			for (int j = 0; j < ENEMY_MAX; j++)
			{
				//生きてるエネミー？
				if (enemy[j].use == TRUE)
				{
					//箱形判定
					/*BOOL ans=CollisionBB(
										g_ABullet[i].pos,
										g_ABullet[i].w,
										g_ABullet[i].h,
										enemy[j].pos,
										enemy[j].w,
										enemy[j].h
										);*/


										//丸形判定
					BOOL ans = CollisionBC(
						g_ABullet[i].pos,
						enemy[j].pos,
						g_ABullet[i].w / 2,
						enemy[j].w / 2

					);

					if (ans == TRUE)
					{
						//当たっていたら
						//弾も消す
						g_ABullet[i].use = FALSE;
						//エネミーを消す
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

				//箱形判定の複雑な書き方、関数用意していない
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
				//	//バウンディングボックスで処理してみる
				//	if (
				//		(br>el) &&
				//		(bl<er) &&
				//		(bd>eu) &&
				//		(bu<ed)
				//		)
				//	{
				//		//当たっていたら
				//		//弾も消す
				//		g_ABullet[i].use = FALSE;
				//		//エネミーを消す
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
						//当たっていたら
						//弾も消す
						g_ABullet[i].use = FALSE;
						//エネミーを消す
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
// 描画処理
//=============================================================================
void DrawBullet(void)
{
	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// マトリクス設定
	SetWorldViewProjection2D();

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// マテリアル設定
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	for (int i = 0; i < BULLET_MAX; i++)
	{
		if (g_ABullet[i].use == TRUE)		// このバレットが使われている？
		{									// Yes
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_ABullet[i].texNo]);

			//バレットの位置やテクスチャー座標を反映
			float px = g_ABullet[i].pos.x;	// バレットの表示位置X
			float py = g_ABullet[i].pos.y;	// バレットの表示位置Y
			float pw = g_ABullet[i].w;		// バレットの表示幅
			float ph = g_ABullet[i].h;		// バレットの表示高さ

			float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// テクスチャの幅
			float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// テクスチャの高さ
			float tx = (float)(g_ABullet[i].patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// テクスチャの左上X座標
			float ty = (float)(g_ABullet[i].patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	// テクスチャの左上Y座標

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteColorRotation(g_VertexBuffer,
				px, py, pw, ph,
				tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
				g_ABullet[i].rot.z);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}
	}

}


//=============================================================================
// バレット構造体の先頭アドレスを取得
//=============================================================================
BULLET* GetBullet(void)
{
	return &g_ABullet[0];
}


//=============================================================================
// バレットの発射設定
//=============================================================================
void SetBullet(XMFLOAT3 pos)
{
	// もし未使用の弾が無かったら発射しない( =これ以上撃てないって事 )
	for (int i = 0; i < BULLET_MAX; i++)
	{
		if (g_ABullet[i].use == FALSE)		// 未使用状態のバレットを見つける
		{
			g_ABullet[i].use = TRUE;			// 使用状態へ変更する
			g_ABullet[i].pos = pos;			// 座標をセット
			return;							// 1発セットしたので終了する
		}
	}
}

