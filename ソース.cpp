#include"DxLib.h"
#include"stdlib.h"

// 
enum{P, M, S};
const int OB_MAX = 3;
int imgOB[OB_MAX];
const int OB_W[OB_MAX] = { 50,121, 71 }; // 後で書き足す
const int OB_H[OB_MAX] = { 50,218, 124 };

// 竹を表示する関数
void drawOB(int x, int y, int type)
{
	DrawGraph(x - OB_W[type] / 2, y - OB_H[type] / 2, imgOB[type], true);
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

	int imgP1 = LoadGraph("image/playerT1.png");
	int imgP2 = LoadGraph("image/playerT2.png");

	int BamMaxT = 2; // 画面内に存在できる竹の数
	int timer = 1000; // 制限時間



	// 竹を動かす配列
	const int Bam_MAX = 8;
	int bambooX[Bam_MAX], bambooY[Bam_MAX], bambooType[Bam_MAX];
	for (int i = 0; i < Bam_MAX; i++)
	{
		bambooX[i] = 300; // 落ちてくる位置
		bambooY[i] = -100;
		bambooType[i] = M + rand() % 2;
	}

	// 
	int swordX = WIDTH / 2;
	int swordY = HEIGHT / 2 + 150;
	int swordtype = P;


	while (1) // メインループ
	{
		int val = GetRand(1); // 落ちる場所に使う
		ClearDrawScreen(); // 画面をクリアする
		
		DrawGraph(0, 0, imgBG, false); // 背景
		timer--;

		if (timer < 500)BamMaxT = 3; // 時間ごとに竹が増える
		if (timer < 200)BamMaxT = 5; // 時間ごとに竹が増える


		// 刀を動かす
		drawOB(swordX, swordY,swordtype);
		
		// 竹を動かす
		for (int i = 0; i < BamMaxT; i++)
		{
			bambooY[i] = bambooY[i] + 1 + i;
			// 画面の下から外に出たかを判定
			if (bambooY[i] > HEIGHT + 50)
			{
				bambooX[i] = 300 + val * 320; // 右か左に落ちてくる
				bambooY[i] = -100;
				bambooType[i] = M + rand() % 2;
			}
			// ヒットチェック
			int dx = abs(bambooX[i] - swordX); // x軸
			int dy = abs(bambooY[i] - swordY); // y軸
			int wid = OB_W[swordtype] / 2 + OB_W[bambooType[i]] / 2 - 4;
			int hei = OB_H[swordtype] / 2 + OB_H[bambooType[i]] / 2 - 4;
			if (dx < wid && dy < hei) // 接触しているか
			{
				int col = GetColor(6, 2, 26);
				DrawBox(swordX - OB_W[swordtype] / 2, swordY - OB_H[swordtype] / 2, swordX + OB_W[swordtype] / 2, swordY + OB_H[swordtype] / 2, col, true);
			}

			drawOB(bambooX[i], bambooY[i], bambooType[i]);
		}

		// マウスの状態を記憶する変数
		int currentMouse = 0; // 現在のフレーム
		static int prevMouse = 0; // 1フレーム前


		// 現在のマウスの入力状態を取得
		currentMouse = GetMouseInput();
	
		// マウスを離した瞬間の判定
		// 前は押されていたかつ今は押されていない
		int mouseX, mouseY;
		GetMousePoint(&mouseX, &mouseY);
		if ((prevMouse & MOUSE_INPUT_LEFT) && !(currentMouse & MOUSE_INPUT_LEFT) && mouseX <= 473 )
		{
			//printfDx("マウス左\n");
			DrawGraph(350, 300, imgP2, true); // プレイヤー
			swordX -= 200;
		}
		else if ((prevMouse & MOUSE_INPUT_LEFT) && !(currentMouse & MOUSE_INPUT_LEFT) && mouseX >= 473)
		{
			//printfDx("マウス右\n");
			drawOB(swordX, swordY,swordtype);
			DrawGraph(350, 300, imgP2, true); // プレイヤー
			swordX += 200;
		}
		else
		{
			DrawGraph(350, 300, imgP1, true); // プレイヤー
			swordX = WIDTH / 2;
		}

		// 1フレーム前の状態を保存
		prevMouse = currentMouse;

		// チャージ
		int pow = 0;
		if (currentMouse == 1)
		{
			pow++;
		}
		else
		{
			pow++;
		}

		// 座標確認
		DrawFormatString(400, 50, GetColor(255, 255, 255), "(%d, %d)", mouseX, mouseY);
		DrawFormatString(400, 70, GetColor(255, 255, 255), "(%d)", timer);
		DrawFormatString(400, 90, GetColor(255, 255, 255), "(%d)", prevMouse);
		DrawFormatString(400, 110, GetColor(255, 255, 255), "(%d)", currentMouse);
		DrawFormatString(400, 130, GetColor(255, 255, 255), "(%d)", pow);
		//DrawFormatString(400, 150, GetColor(255, 255, 255), "(%d)", );

		ScreenFlip(); // 裏画面の内容を表画面に反映させる
		WaitTimer(33);
		if (ProcessMessage() == -1)break; // Windowsから情報を受け取りエラーが起きたら終了
		if (CheckHitKey(KEY_INPUT_ESCAPE) == 1)break; // ESCキーが押されたら終了
	}





	DxLib_End(); // DXライブラリ使用の終了処理
	return 0; // ソフトの終了
}