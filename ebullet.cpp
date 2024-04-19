//=============================================================================
//
// バレット処理 [bullet.cpp]
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
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(50/2)	// キャラサイズ
#define TEXTURE_HEIGHT				(90/2)	// 
#define TEXTURE_MAX					(1)		// テクスチャの数

#define TEXTURE_PATTERN_DIVIDE_X	(1)		// アニメパターンのテクスチャ内分割数（X)
#define TEXTURE_PATTERN_DIVIDE_Y	(1)		// アニメパターンのテクスチャ内分割数（Y)
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// アニメーションパターン数
#define ANIM_WAIT					(4)		// アニメーションの切り替わるWait値


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[] = {
	"data/TEXTURE/bullet01.png",
};

static BOOL		g_Load = FALSE;			// 初期化を行ったかのフラグ
static EBULLET	g_EBullet[EBULLET_MAX];	// バレット構造体


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitEBullet(void)
{
	ID3D11Device *pDevice = GetDevice();

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
	for (int i = 0; i < EBULLET_MAX; i++)
	{
		g_EBullet[i].use   = FALSE;			// 未使用（発射されていない弾）
		g_EBullet[i].w     = TEXTURE_WIDTH;
		g_EBullet[i].h     = TEXTURE_HEIGHT;
		g_EBullet[i].pos   = XMFLOAT3(300, 300.0f, 0.0f);
		g_EBullet[i].rot   = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_EBullet[i].texNo = 0;
		  
		g_EBullet[i].countAnim = 0;
		g_EBullet[i].patternAnim = 0;
		  
		g_EBullet[i].move = XMFLOAT3(0.0f, +EBULLET_SPEED, 0.0f);	// 移動量を初期化
	}
	
	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
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
// 更新処理
//=============================================================================
void UpdateEBullet(void)
{
	int ebulletCount = 0;				// 処理したバレットの数

	for (int i = 0; i < EBULLET_MAX; i++)
	{
		if (g_EBullet[i].use == TRUE)	// このバレットが使われている？
		{								// Yes
			// アニメーション  
			g_EBullet[i].countAnim++;
			if ((g_EBullet[i].countAnim % ANIM_WAIT) == 0)
			{
				// パターンの切り替え
				g_EBullet[i].patternAnim = (g_EBullet[i].patternAnim + 1) % ANIM_PATTERN_NUM;
			}

			// バレットの移動処理
			{
				//XMVECTOR pos = XMLoadFloat3(&g_EBullet[i].pos);
				//XMVECTOR move = XMLoadFloat3(&g_EBullet[i].move);
				//pos += move;
				//XMStoreFloat3(&g_EBullet[i].pos, pos);

			}


			

			{

				////三角関数で計算する
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


			// 画面外まで進んだ？
			if (g_EBullet[i].pos.y < (-g_EBullet[i].h/2))		// 自分の大きさを考慮して画面外か判定している
			{
				g_EBullet[i].use = false;
			}
			if (g_EBullet[i].pos.y > (SCREEN_HEIGHT+ g_EBullet[i].h/2))	// 自分の大きさを考慮して画面外か判定している
			{
				g_EBullet[i].use = false;
			}

			// 当たり判定処理
			PLAYER* player = GetPlayer();

			for (int j = 0; j < PLAYER_MAX; j++)
			{
				//生きてるエネミー？
				if (player[j].use == TRUE)
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
									g_EBullet[i].pos,
									player[j].pos,
									g_EBullet[i].w/2,
									player[j].w/2
									);


					if (ans == TRUE)
					{
						//当たっていたら
						//弾も消す
						g_EBullet[i].use = FALSE;
						//エネミーを消す
						player[j].use = FALSE;
						PlaySound(SOUND_LABEL_SE_BOMB);


						SetEffect(player[j].pos.x, player[j].pos.y, 100);

					
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


			ebulletCount++;
		}
	}


}

//=============================================================================
// 描画処理
//=============================================================================
void DrawEBullet(void)
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

	for (int i = 0; i < EBULLET_MAX; i++)
	{
		if (g_EBullet[i].use == TRUE)		// このバレットが使われている？
		{									// Yes
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_EBullet[i].texNo]);

			//バレットの位置やテクスチャー座標を反映
			float ex = g_EBullet[i].pos.x;	// バレットの表示位置X
			float ey = g_EBullet[i].pos.y;	// バレットの表示位置Y
			float ew = g_EBullet[i].w;		// バレットの表示幅
			float eh = g_EBullet[i].h;		// バレットの表示高さ

			float bw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// テクスチャの幅
			float bh = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// テクスチャの高さ
			float bx = (float)(g_EBullet[i].patternAnim % TEXTURE_PATTERN_DIVIDE_X) * bw;	// テクスチャの左上X座標
			float by = (float)(g_EBullet[i].patternAnim / TEXTURE_PATTERN_DIVIDE_X) * bh;	// テクスチャの左上Y座標

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteColorRotation(g_VertexBuffer, 
				ex, ey, ew, eh, 
				bx, by, bw, bh,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
				g_EBullet[i].rot.z);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}
	}

}


//=============================================================================
// バレット構造体の先頭アドレスを取得
//=============================================================================
EBULLET *GetEBullet(void)
{
	return &g_EBullet[0];
}


//=============================================================================
// バレットの発射設定
//=============================================================================
void SetEBullet(XMFLOAT3 pos)
{
	// もし未使用の弾が無かったら発射しない( =これ以上撃てないって事 )
	for (int i = 0; i < EBULLET_MAX; i++)
	{
		if (g_EBullet[i].use == FALSE)		// 未使用状態のバレットを見つける
		{
			g_EBullet[i].use = TRUE;			// 使用状態へ変更する
			g_EBullet[i].pos = pos;			// 座標をセット
			return;							// 1発セットしたので終了する
		}
	}
}

