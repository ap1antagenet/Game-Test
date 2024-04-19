//=============================================================================
//
// プレイヤー処理 [enemy.cpp]
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
static ID3D11Buffer* g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char* g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/enemy1.png",

};


static BOOL		g_Load = FALSE;			// 初期化を行ったかのフラグ
static ENEMY	g_Enemy[ENEMY_MAX];	// プレイヤー構造体

int enemyTotal;
int enemycount;


static INTERPOLATION_DATA g_MoveTbl0[] = {
	//座標									回転率							拡大率							時間
	{ XMFLOAT3(50.0f,  50.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f),	60 },
	{ XMFLOAT3(50.0f,  250.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f),	60 },
	{ XMFLOAT3(250.0f, 250.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f),	60 },
	{ XMFLOAT3(250.0f,  50.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f),	60 },

};


static INTERPOLATION_DATA g_MoveTbl1[] = {
	//座標									回転率							拡大率							時間
	{ XMFLOAT3(900.0f,  50.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f),	60 },
	{ XMFLOAT3(900.0f,  250.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f),	60 },
	{ XMFLOAT3(700.0f, 250.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f),	60 },
	{ XMFLOAT3(700.0f,  50.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f),	60 },
};


static INTERPOLATION_DATA g_MoveTbl2[] = {
	//座標									回転率							拡大率							時間
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
// 初期化処理
//=============================================================================
HRESULT InitEnemy(void)
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


	// 構造体の初期化

	enemycount = 0;
	CreateEnemy(1);



	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
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
// 更新処理
//=============================================================================
void UpdateEnemy(void)
{
	

	for (int i = 0; i < ENEMY_MAX; i++)
	{
		PLAYER* player = GetPlayer();

		// 生きてるプレイヤーだけ処理をする
		if (g_Enemy[i].use == TRUE)
		{
			// 地形との当たり判定用に座標のバックアップを取っておく
			XMFLOAT3 pos_old = g_Enemy[i].pos;

			// アニメーション  
			g_Enemy[i].countAnim += 1.0f;
			if (g_Enemy[i].countAnim > ANIM_WAIT)
			{
				g_Enemy[i].countAnim = 0.0f;
				// パターンの切り替え
				g_Enemy[i].patternAnim = (g_Enemy[i].patternAnim + 1) % ANIM_PATTERN_NUM;
			}

			//回転する
			/*g_Enemy[i].rot.z += -0.1f;
			*/

			// 移動処理
			if (g_Enemy[i].tblMax > 0)	// 線形補間を実行する？
			{	// 線形補間の処理
				int nowNo = (int)g_Enemy[i].time;			// 整数分であるテーブル番号を取り出している
				int maxNo = g_Enemy[i].tblMax;				// 登録テーブル数を数えている
				int nextNo = (nowNo + 1) % maxNo;			// 移動先テーブルの番号を求めている
				INTERPOLATION_DATA* tbl = g_MoveTblAdr[g_Enemy[i].tblNo];	// 行動テーブルのアドレスを取得

				XMVECTOR nowPos = XMLoadFloat3(&tbl[nowNo].pos);	// XMVECTORへ変換
				XMVECTOR nowRot = XMLoadFloat3(&tbl[nowNo].rot);	// XMVECTORへ変換
				XMVECTOR nowScl = XMLoadFloat3(&tbl[nowNo].scl);	// XMVECTORへ変換

				XMVECTOR Pos = XMLoadFloat3(&tbl[nextNo].pos) - nowPos;	// XYZ移動量を計算している
				XMVECTOR Rot = XMLoadFloat3(&tbl[nextNo].rot) - nowRot;	// XYZ回転量を計算している
				XMVECTOR Scl = XMLoadFloat3(&tbl[nextNo].scl) - nowScl;	// XYZ拡大率を計算している

 				float nowTime = g_Enemy[i].time - nowNo;	// 時間部分である少数を取り出している

				Pos *= nowTime;								// 現在の移動量を計算している
				Rot *= nowTime;								// 現在の回転量を計算している
				Scl *= nowTime;								// 現在の拡大率を計算している

				// 計算して求めた移動量を現在の移動テーブルXYZに足している＝表示座標を求めている
				XMStoreFloat3(&g_Enemy[i].pos, nowPos + Pos);

				// 計算して求めた回転量を現在の移動テーブルに足している
				XMStoreFloat3(&g_Enemy[i].rot, nowRot + Rot);

				// 計算して求めた拡大率を現在の移動テーブルに足している
				XMStoreFloat3(&g_Enemy[i].scl, nowScl + Scl);
				g_Enemy[i].w = TEXTURE_WIDTH * g_Enemy[i].scl.x;
				g_Enemy[i].h = TEXTURE_HEIGHT * g_Enemy[i].scl.y;

				// frameを使て時間経過処理をする
				g_Enemy[i].time += 1.0f / tbl[nowNo].frame;	// 時間を進めている
				if ((int)g_Enemy[i].time >= maxNo)			// 登録テーブル最後まで移動したか？
				{
					g_Enemy[i].time -= maxNo;				// ０番目にリセットしつつも小数部分を引き継いでいる
				}

			}

			//{
			//	//自動的に移動している
			//	// MAP外チェック
			//	BG* bg = GetBG();

			//	float speed = g_Enemy[i].move.x;

			//	//右へ動いている
			//	g_Enemy[i].pos.x += g_Enemy[i].direction * speed;

			//	if (g_Enemy[i].pos.x > bg->w)
			//	{
			//		g_Enemy[i].pos.x = bg->w;
			//		g_Enemy[i].move.x *= -1;	//次からは左へ行く
			//	}

			//	if (g_Enemy[i].pos.x < 0.0f)
			//	{
			//		g_Enemy[i].pos.x = 0.0f;
			//		g_Enemy[i].move.x *= -1;	//次からは右へ行く
			//	}

			//}




			// 移動が終わったらPLAYERとの当たり判定

			for (int j = 0; j < PLAYER_MAX; j++)
			{
				//生きてるエネミー？
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
						//当たっていたら
						//弾も消す
						player[j].use = FALSE;
						//エネミーを消す
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
				//		player[j].use = FALSE;
				//		//エネミーを消す
				//	/*	g_Enemy[i].use = FALSE;*/
				//		break;
				//	}
				//}
			}


			// バレット処理
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




#ifdef _DEBUG	// デバッグ情報を表示する


#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawEnemy(void)
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

	for (int i = 0; i < ENEMY_MAX; i++)
	{
		if (g_Enemy[i].use == TRUE)		// このプレイヤーが使われている？
		{									// Yes
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Enemy[i].texNo]);

			//プレイヤーの位置やテクスチャー座標を反映
			float px = g_Enemy[i].pos.x;	// プレイヤーの表示位置X
			float py = g_Enemy[i].pos.y;	// プレイヤーの表示位置Y
			float pw = g_Enemy[i].w;		// プレイヤーの表示幅
			float ph = g_Enemy[i].h;		// プレイヤーの表示高さ

			// アニメーション用
			//float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// テクスチャの幅
			//float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// テクスチャの高さ
			//float tx = (float)(g_Enemy[i].patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// テクスチャの左上X座標
			//float ty = (float)(g_Enemy[i].patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	// テクスチャの左上Y座標

			float tw = 1.0f;	// テクスチャの幅
			float th = 1.0f;	// テクスチャの高さ
			float tx = 0.0f * g_Enemy[i].patternAnim;	// テクスチャの左上X座標
			float ty = 0.0f;	// テクスチャの左上Y座標

			// １枚のポリゴンの頂点とテクスチャ座標を設定


			SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
				g_Enemy[i].rot.z);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}
	}


}


//=============================================================================
// Enemy構造体の先頭アドレスを取得
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
			// 0番だけ線形補間で動かしてみる

			g_Enemy[i + enemycount].pos = XMFLOAT3(50.0f, 100.0f, 0.0f);	// 中心点から表示

			g_Enemy[i + enemycount].use = TRUE;
			g_Enemy[i + enemycount].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);		// 回転率
			g_Enemy[i + enemycount].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);		// 拡大縮小
			g_Enemy[i + enemycount].w = TEXTURE_WIDTH;
			g_Enemy[i + enemycount].h = TEXTURE_HEIGHT;
			g_Enemy[i + enemycount].texNo = 0;
			g_Enemy[i + enemycount].right = TRUE;
			g_Enemy[i + enemycount].countAnim = 0;
			g_Enemy[i + enemycount].patternAnim = 0;
			g_Enemy[i + enemycount].move = XMFLOAT3(4.0f, 0.0f, 0.0f);		// 移動量
			//g_Enemy[i + enemycount].direction = (rand() % 2 == 0) ? -1 : 1;


			g_Enemy[i + enemycount].time = 0.0f;			// 線形補間用のタイマーをクリア
			g_Enemy[i + enemycount].tblNo = 0;			// 再生する行動データテーブルNoをセット
			g_Enemy[i + enemycount].tblMax = 0;			// 再生する行動データテーブルのレコード数をセット
			g_Enemy[i + enemycount].time = 0.0f;		// 線形補間用のタイマーをクリア
			g_Enemy[i + enemycount].tblNo = 0;		// 再生するアニメデータの先頭アドレスをセット
			g_Enemy[i + enemycount].tblMax = sizeof(g_MoveTbl0) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
			break;

		case 1:
			// 1番だけ線形補間で動かしてみる
	
			g_Enemy[i + enemycount].pos = XMFLOAT3(900.0f, 100.0f, 0.0f);	// 中心点から表示

			g_Enemy[i + enemycount].use = TRUE;
			g_Enemy[i + enemycount].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);		// 回転率
			g_Enemy[i + enemycount].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);		// 拡大縮小
			g_Enemy[i + enemycount].w = TEXTURE_WIDTH;
			g_Enemy[i + enemycount].h = TEXTURE_HEIGHT;
			g_Enemy[i + enemycount].texNo = 0;
			g_Enemy[i + enemycount].right = TRUE;
			g_Enemy[i + enemycount].countAnim = 0;
			g_Enemy[i + enemycount].patternAnim = 0;
			g_Enemy[i + enemycount].move = XMFLOAT3(4.0f, 0.0f, 0.0f);		// 移動量
			//g_Enemy[i + enemycount].direction = (rand() % 2 == 0) ? -1 : 1;

			g_Enemy[i + enemycount].time = 0.0f;			// 線形補間用のタイマーをクリア
			g_Enemy[i + enemycount].tblNo = 0;			// 再生する行動データテーブルNoをセット
			g_Enemy[i + enemycount].tblMax = 0;			// 再生する行動データテーブルのレコード数をセット
			g_Enemy[i + enemycount].time = 0.0f;		// 線形補間用のタイマーをクリア
			g_Enemy[i + enemycount].tblNo = 1;		// 再生するアニメデータの先頭アドレスをセット
			g_Enemy[i + enemycount].tblMax = sizeof(g_MoveTbl1) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
			break;

		case 2:
			// 2番だけ線形補間で動かしてみる

			g_Enemy[i + enemycount].pos = XMFLOAT3(450.0f, 100.0f, 0.0f);	// 中心点から表示

			g_Enemy[i + enemycount].use = TRUE;
			g_Enemy[i + enemycount].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);		// 回転率
			g_Enemy[i + enemycount].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);		// 拡大縮小
			g_Enemy[i + enemycount].w = TEXTURE_WIDTH;
			g_Enemy[i + enemycount].h = TEXTURE_HEIGHT;
			g_Enemy[i + enemycount].texNo = 0;
			g_Enemy[i + enemycount].right = TRUE;
			g_Enemy[i + enemycount].countAnim = 0;
			g_Enemy[i + enemycount].patternAnim = 0;
			g_Enemy[i + enemycount].move = XMFLOAT3(4.0f, 0.0f, 0.0f);		// 移動量
			//g_Enemy[i + enemycount].direction = (rand() % 2 == 0) ? -1 : 1;


			g_Enemy[i + enemycount].time = 0.0f;			// 線形補間用のタイマーをクリア
			g_Enemy[i + enemycount].tblNo = 0;			// 再生する行動データテーブルNoをセット
			g_Enemy[i + enemycount].tblMax = 0;			// 再生する行動データテーブルのレコード数をセット
			g_Enemy[i + enemycount].time = 0.0f;		// 線形補間用のタイマーをクリア
			g_Enemy[i + enemycount].tblNo = 2;		// 再生するアニメデータの先頭アドレスをセット
			g_Enemy[i + enemycount].tblMax = sizeof(g_MoveTbl2) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
			break;

		}

	}
	enemycount += no;

	PlaySound(SOUND_LABEL_SE_LOCK);
	g_Load = TRUE;


}


