//=============================================================================
//
// プレイヤー処理 [player.cpp]
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
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(100)	// キャラサイズ
#define TEXTURE_HEIGHT				(100)	// 
#define TEXTURE_MAX					(1)		// テクスチャの数

#define TEXTURE_PATTERN_DIVIDE_X	(1)		// アニメパターンのテクスチャ内分割数（X)
#define TEXTURE_PATTERN_DIVIDE_Y	(1)		// アニメパターンのテクスチャ内分割数（Y)
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// アニメーションパターン数
#define ANIM_WAIT					(8)		// アニメーションの切り替わるWait値


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/player01.png",

};


static BOOL		g_Load = FALSE;			// 初期化を行ったかのフラグ
static PLAYER	g_Player[PLAYER_MAX];	// プレイヤー構造体


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitPlayer(void)
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


	// プレイヤー構造体の初期化
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		g_Player[i].use = TRUE;
		g_Player[i].pos = XMFLOAT3(500.0f, 400.0f, 0.0f);	// 中心点から表示
		g_Player[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);		// 回転率
		g_Player[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);		// 拡大縮小
		g_Player[i].w = TEXTURE_WIDTH;
		g_Player[i].h = TEXTURE_HEIGHT;
		g_Player[i].texNo = 0;

		g_Player[i].countAnim = 0;
		g_Player[i].patternAnim = 0;

		g_Player[i].move = XMFLOAT3(4.0f, 0.0f, 0.0f);		// 移動量

	}


	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
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
// 更新処理
//=============================================================================
void UpdatePlayer(void)
{

	for (int i = 0; i < PLAYER_MAX; i++)
	{
		// 生きてるプレイヤーだけ処理をする
		if (g_Player[i].use == TRUE)
		{
			// 地形との当たり判定用に座標のバックアップを取っておく
			XMFLOAT3 pos_old = g_Player[i].pos;

			// アニメーション  
			g_Player[i].countAnim += 1.0f;
			if (g_Player[i].countAnim > ANIM_WAIT)
			{
				g_Player[i].countAnim = 0.0f;
				// パターンの切り替え
				g_Player[i].patternAnim = (g_Player[i].patternAnim + 1) % ANIM_PATTERN_NUM;
			}


			//g_Player[i].rot.z += 0.1f; //キャラクター回転

			// キー入力で移動 
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

				// ゲームパッドでで移動処理
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

				// MAP外チェック
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

				// 移動が終わったらエネミーとの当たり判定

				ENEMY* enemy = GetEnemy();

				for (int j = 0; j < ENEMY_MAX; j++)
				{
					//生きてるエネミー？
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
							//当たっていたら
							//弾も消す
							//g_Player[i].use = FALSE;
							//エネミーを消す
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

					//	//バウンディングボックスで処理してみる
					//	if (
					//		(pr > el) &&
					//		(pl < er) &&
					//		(pd > eu) &&
					//		(pu < ed)
					//		)
					//	{
					//		//当たっていたら
					//		//弾も消す
					//		//g_Player[i].use = FALSE;
					//		//エネミーを消す
					//		enemy[j].use = FALSE;
					//		break;
					//	}
					//}
				}
				
				// バレット処理
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

#ifdef _DEBUG	// デバッグ情報を表示する


#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawPlayer(void)
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

	for (int i = 0; i < PLAYER_MAX; i++)
	{
		if (g_Player[i].use == TRUE)		// このプレイヤーが使われている？
		{									// Yes
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Player[i].texNo]);

			//プレイヤーの位置やテクスチャー座標を反映
			float px = g_Player[i].pos.x;	// プレイヤーの表示位置X
			float py = g_Player[i].pos.y;	// プレイヤーの表示位置Y
			float pw = g_Player[i].w;		// プレイヤーの表示幅
			float ph = g_Player[i].h;		// プレイヤーの表示高さ

			// アニメーション用
			//float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// テクスチャの幅
			//float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// テクスチャの高さ
			//float tx = (float)(g_Player[i].patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// テクスチャの左上X座標
			//float ty = (float)(g_Player[i].patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	// テクスチャの左上Y座標

			float tw = 1.0f;	// テクスチャの幅
			float th = 1.0f;	// テクスチャの高さ
			float tx = 0.0f* g_Player[i].patternAnim;	// テクスチャの左上X座標
			float ty = 0.0f;	// テクスチャの左上Y座標

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
				g_Player[i].rot.z);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}
	}


}


//=============================================================================
// Player構造体の先頭アドレスを取得
//=============================================================================
PLAYER* GetPlayer(void)
{
	return &g_Player[0];
}




