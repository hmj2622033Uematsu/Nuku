#include"DxLib.h"
#include"stdlib.h"
#include "Nuku.h"

// 
enum { P, M, S, B }; // 物の種類
enum {TITLE, PLAY, OVER}; // シーン
const int OB_MAX = 4;
int imgOB[OB_MAX];
const int ENEMY_MAX = 100; // 敵機の数の最大値
const int OB_W[OB_MAX] = { 50,121, 71, 71 }; // 後で書き足す
const int OB_H[OB_MAX] = { 25,218, 124, 124 };

//struct OBJECT bamboo[ENEMY_MAX]; // 敵機用の構造体の配列

// 竹を表示する関数
void drawOB(int x, int y, int type)
{
	DrawGraph(x - OB_W[type] / 2, y - OB_H[type] / 2, imgOB[type], true);
}

// 影をつけた文字列を表示する関数
void drawText(int x, int y, int col, const char* txt, int v, int siz)
{
	SetFontSize(siz);
	DrawFormatString(x + 2, y + 2, 0x000000, txt, v);
	DrawFormatString(x, y, col, txt, v);
}

int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	const int WIDTH = 950, HEIGHT = 540; // ウィンドウの幅と高さのピクセル数
	SetWindowText("Nuku"); // ウィンドウタイトル
	SetGraphMode(WIDTH, HEIGHT, 32);
	ChangeWindowMode(true); // ウィンドウモードで起動
	if (DxLib_Init() == -1) return -1; // ライブラリ初期化　エラーが起きたら終了
	SetDrawScreen(DX_SCREEN_BACK); // 描画面を裏画面にする

	//int imgOB = LoadGraph("image/object.png");

	// 画像の読み込み
	int imgBG = LoadGraph("image/BG.png");

	imgOB[M] = LoadGraph("image/Bamboo.png");
	imgOB[S] = LoadGraph("image/MiniBam.png");
	imgOB[P] = LoadGraph("image/MiniBam.png");
	imgOB[B] = LoadGraph("image/ki.png");

	int imgP1 = LoadGraph("image/playerT1.png");
	int imgP2 = LoadGraph("image/playerT2.png");
	int imgKI[3] = {
		LoadGraph("image/ki.png"),
		LoadGraph("image/ki2.png"),
		LoadGraph("image/ki3.png")
	};

	int imgSlash1 = LoadGraph("image/slash1.png");
	int imgSlash2 = LoadGraph("image/slash2.png");
	int imgSlashB1 = LoadGraph("image/slashB1.png");
	int imgSlashB2 = LoadGraph("image/slashB2.png");

	int BamMaxT = 2; // 画面内に存在できる竹の数

	int timer = 2000; // 制限時間
	int efTimer = 0;
	int pow = 0; // パワー

	int score = 0; // スコア
	int highScore = 0; // ハイスコア

	int scene = PLAY; // シーン初期化

	// マウスの状態を記憶する変数
	int currentMouse = 0; // 現在のフレーム
	static int prevMouse = 0; // 1フレーム前

	// 竹を動かす配列
	const int Bam_MAX = 8;
	int bambooX[Bam_MAX], bambooY[Bam_MAX], bambooType[Bam_MAX], bambooState[Bam_MAX];
	for (int i = 0; i < Bam_MAX; i++)
	{
		bambooX[i] = 300; // 落ちてくる位置
		bambooY[i] = -100; //
		bambooType[i] = M + rand() % 2; // 落ちてくる竹の種類
		bambooState[i] = 1; // 竹が存在しているか
	}

	// 斬撃
	int swordX = WIDTH / 2;
	int swordY = HEIGHT / 2 + 150;
	int swordtype = P;


	while (1) // メインループ
	{
		int val = GetRand(1); // 落ちる場所に使う
		ClearDrawScreen(); // 画面をクリアする

		switch (scene)
		{
		case TITLE:
			break;
		case PLAY:
			DrawGraph(0, 0, imgBG, false); // 背景
			timer--;
			efTimer++;

			if (timer < 1700)BamMaxT = 4; // 時間ごとに竹が増える
			if (timer < 1500)BamMaxT = 6; // 時間ごとに竹が増える

			// 竹を動かす
			for (int i = 0; i < BamMaxT; i++)
			{
				bambooY[i] = bambooY[i] + 1 + i; // 竹が落ちてくる
				// 画面の下から外に出たかを判定
				if (bambooY[i] > HEIGHT + 50)
				{
					bambooX[i] = 300 + val * 320; // 右か左に落ちてくる
					bambooY[i] = -100;
					bambooType[i] = M + rand() % 2;
					bambooState[i] = 1;
				}
				// ヒットチェック
				if (bambooState[i] != 0) // 竹が存在していたらヒットチェック
				{
					int dx = abs(bambooX[i] - swordX); // x軸
					int dy = abs(bambooY[i] - swordY); // y軸
					int wid = OB_W[swordtype] / 2 + OB_W[bambooType[i]] / 2; // -4
					int hei = OB_H[swordtype] / 2 + OB_H[bambooType[i]] / 2; // -4

					if (dx < wid && dy < hei) // 接触しているか
					{
						int col = GetColor(6, 2, 26);
						//DrawBox(swordX - OB_W[swordtype] / 2, swordY - OB_H[swordtype] / 2, swordX + OB_W[swordtype] / 2, swordY + OB_H[swordtype] / 2, col, true);
						if (bambooType[i] == S || swordtype == S)
						{
							score += 50;
							if (bambooType[i] == M) { score += 100; }
							bambooState[i] = 0;
						}

					}
				}
				if (bambooState[i] == 1) // 竹が存在していたら描画
				{
					drawOB(bambooX[i], bambooY[i], bambooType[i]);
				}
			}

			// スコアの表示
			SetFontSize(30);
			DrawFormatString(5, 5, GetColor(0, 255, 0), "score %d", score);

			// タイムの表示
			DrawFormatString(800, 5, GetColor(0, 255, 0), "time %d", timer);

			// 現在のマウスの入力状態を取得
			currentMouse = GetMouseInput();

			// マウスを離した瞬間の判定
			// 前は押されていたかつ今は押されていない
			int mouseX, mouseY;
			GetMousePoint(&mouseX, &mouseY);
			if ((prevMouse & MOUSE_INPUT_LEFT) && !(currentMouse & MOUSE_INPUT_LEFT) && mouseX <= 473)
			{
				//printfDx("マウス左\n");
				DrawGraph(350, 300, imgP2, true); // プレイヤー
				if (pow >= 50)
				{
					swordtype = S;
					DrawGraph(-5, 270, imgSlashB2, true); // 斬撃強
				}
				else
				{
					DrawGraph(165, 355, imgSlash2, true); // 斬撃
				}
				swordX -= 200;
			}
			else if ((prevMouse & MOUSE_INPUT_LEFT) && !(currentMouse & MOUSE_INPUT_LEFT) && mouseX >= 473)
			{
				//printfDx("マウス右\n");
				DrawGraph(350, 300, imgP2, true); // プレイヤー
				if (pow >= 50)
				{
					swordtype = S;
					DrawGraph(535, 270, imgSlashB1, true); // 斬撃強
				}
				else
				{
					DrawGraph(540, 355, imgSlash1, true); // 斬撃
				}
				swordX += 200;
			}
			else
			{
				swordtype = P;
				DrawGraph(350, 300, imgP1, true); // プレイヤー
				swordX = WIDTH / 2;
			}

			// 1フレーム前の状態を保存
			prevMouse = currentMouse;

			// チャージ
			if (currentMouse == 1)
			{
				pow++;
			}
			else
			{
				pow = 0;
			}
			// パワーが溜まったら見た目を変える
			if (pow >= 50)
			{
				DrawGraph(270, 130, imgKI[(efTimer / 4) % 3], true); // 気を解放しろ
				DrawGraph(350, 300, imgP1, true); // プレイヤー
			}

			if (score >= highScore) highScore = score;
			break;
			
		case OVER:
			break;
		}

		//// 座標確認
		//DrawFormatString(400, 50, GetColor(255, 255, 255), "(%d, %d)", mouseX, mouseY);
		//DrawFormatString(400, 70, GetColor(255, 255, 255), "(%d)", timer);
		//DrawFormatString(400, 90, GetColor(255, 255, 255), "(%d)", prevMouse);
		//DrawFormatString(400, 110, GetColor(255, 255, 255), "(%d)", currentMouse);
		//DrawFormatString(400, 130, GetColor(255, 255, 255), "(%d)", pow);
		//DrawFormatString(400, 150, GetColor(255, 255, 255), "(%d)", score);

		ScreenFlip(); // 裏画面の内容を表画面に反映させる
		WaitTimer(33);
		if (ProcessMessage() == -1)break; // Windowsから情報を受け取りエラーが起きたら終了
		if (CheckHitKey(KEY_INPUT_ESCAPE) == 1)break; // ESCキーが押されたら終了
	}

	DxLib_End(); // DXライブラリ使用の終了処理
	return 0; // ソフトの終了
}


