//=============================================================================
//
// タイトル画面処理 [title.cpp]
// Author : 
//
//=============================================================================
#include "title.h"
#include "input.h"
#include "fade.h"



//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define BUTTON_SUM					(3)		// BUTTONサイズ
#define BUTTON_WIDTH				(100)	// BUTTONサイズ
#define BUTTON_HEIGHT				(20)	// 

#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// 背景サイズ
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(4)				// テクスチャの数

#define TEXTURE_WIDTH_LOGO			(480)			// ロゴサイズ
#define TEXTURE_HEIGHT_LOGO			(80)			// 

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char* g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/bg000.jpg",
	"data/TEXTURE/title001.png",
	"data/TEXTURE/title002.png",
	"data/TEXTURE/title003.png",

};



static BOOL		g_Load = FALSE;
static TITLE	g_Title;
static BUTTON	g_Button[BUTTON_SUM];


static float	alpha;
static BOOL		flag_alpha;


//static float	effect_dx;
//static float	effect_dy;

int Up_stauts = 0;


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitTitle(void)
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


	// 変数の初期化
	g_Title.w = TEXTURE_WIDTH;
	g_Title.h = TEXTURE_HEIGHT;
	g_Title.pos = XMFLOAT3(g_Title.w / 2, g_Title.h / 2, 0.0f);
	g_Title.texNo = 0;

	g_Button[0].w = BUTTON_WIDTH;
	g_Button[0].h = BUTTON_HEIGHT;
	g_Button[0].pos = XMFLOAT3(g_Button[0].w / 2, g_Button[0].h / 2, 0.0f);
	g_Button[0].texNo = 1;
	g_Button[0].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	g_Button[1].w = BUTTON_WIDTH;
	g_Button[1].h = BUTTON_HEIGHT;
	g_Button[1].pos = XMFLOAT3(g_Button[1].w / 2, g_Button[1].h / 2, 0.0f);
	g_Button[1].texNo = 2;
	g_Button[1].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	//g_Button[2].w = BUTTON_WIDTH;
	//g_Button[2].h = BUTTON_HEIGHT;
	//g_Button[2].pos = XMFLOAT3(g_Button[1].w / 2, g_Button[1].h / 2, 0.0f);
	//g_Button[2].texNo = 3;
	//g_Button[2].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	alpha = 1.0f;
	flag_alpha = TRUE;

	//effect_dx = 100.0f;
	//effect_dy = 100.0f;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitTitle(void)
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
void UpdateTitle(void)
{
	if (GetKeyboardTrigger(DIK_UP) || IsButtonTriggered(0, BUTTON_UP))
	{
		Up_stauts--;

	}
	else if (GetKeyboardTrigger(DIK_DOWN) || IsButtonTriggered(0, BUTTON_DOWN))
	{
		Up_stauts++;
	}
	if (Up_stauts == -1)
	{
		Up_stauts = 1;
	}
	Up_stauts = Up_stauts % 2;
	if (GetKeyboardTrigger(DIK_RETURN) || IsButtonTriggered(0, BUTTON_START) || IsButtonTriggered(0, BUTTON_B))
	{// Enter押したら、ステージを切り替える
		if (Up_stauts == 0)
		{
			SetFade(FADE_OUT, MODE_GAME);
			return;
		}
		else if (Up_stauts == 1)
		{
			SetFade(FADE_OUT, MODE_TUTORIAL);
			return;
		}
	

		


	}
	switch (Up_stauts)
	{
	case 0:
		g_Button[0].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
		g_Button[1].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		g_Button[2].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		break;

	case 1:
		g_Button[0].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		g_Button[1].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
		g_Button[2].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		break;

	//case 2:
	//	g_Button[0].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	//	g_Button[1].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	//	g_Button[2].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	//	break;

	default:
		break;
	}




#ifdef _DEBUG	// デバッグ情報を表示する
	//PrintDebugProc("Player:↑ → ↓ ←　Space\n");

#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawTitle(void)
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

	// タイトルの背景を描画
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Title.texNo]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLeftTop(g_VertexBuffer, 0.0f, 0.0f, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	// STARTのBUTTONを描画
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Button[0].texNo]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor(g_VertexBuffer, +200.0f, +300, g_Button[0].w, g_Button[0].h, 0.0f, 0.0f, 1.0f, 1.0f, g_Button[0].color);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Button[1].texNo]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor(g_VertexBuffer, +200.0f, +350, g_Button[1].w, g_Button[1].h, 0.0f, 0.0f, 1.0f, 1.0f, g_Button[1].color);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	//{
	//	// テクスチャ設定
	//	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Button[2].texNo]);

	//	// １枚のポリゴンの頂点とテクスチャ座標を設定
	//	SetSpriteColor(g_VertexBuffer, +200.0f, +400, g_Button[2].w, g_Button[2].h, 0.0f, 0.0f, 1.0f, 1.0f, g_Button[2].color);

	//	// ポリゴン描画
	//	GetDeviceContext()->Draw(4, 0);
	//}


}





