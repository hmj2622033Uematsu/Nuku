#include"DxLib.h"
#include"stdlib.h"
#include "Nuku.h"

// 
enum { P, M, S, B }; // 物の種類
enum {TITLE, PLAY, OVER}; // シーン
const int OB_MAX = 4;
int imgOB[OB_MAX];
const int ENEMY_MAX = 100; // 敵機の数の最大値
const int OB_W[OB_MAX] = { 50,121, 71, 600 }; // 後で書き足す
const int OB_H[OB_MAX] = { 25,218, 124, 500 };

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

	// 竹
	imgOB[M] = LoadGraph("image/Bamboo.png");
	imgOB[S] = LoadGraph("image/MiniBam.png");
	imgOB[P] = LoadGraph("image/MiniBam.png");
	imgOB[B] = LoadGraph("image/ki.png");
	// プレイヤー
	int imgP1 = LoadGraph("image/playerT1.png");
	int imgP2[2] = {
		LoadGraph("image/playerT2.png"),
		LoadGraph("image/playerT3.png"),
	};
	// 技２
	int imgClock[2] = {
		LoadGraph("image/C1.png"),
		LoadGraph("image/C2.png"),
	};
	// 技３
	int imgUltBam[3] = {
		LoadGraph("image/Ub1.png"),
		LoadGraph("image/Ub2.png"),
		LoadGraph("image/Ub3.png"),
	};
	// 気
	int imgKI[3] = {
		LoadGraph("image/ki.png"),
		LoadGraph("image/ki2.png"),
		LoadGraph("image/ki3.png")
	};
	// 技ゲージ
	int imgUlt[3] = {
		LoadGraph("image/ult.png"),
		LoadGraph("image/ult2.png"),
		LoadGraph("image/ult3.png")
	};
	// ランク
	int imgC = LoadGraph("image/c.png");
	int imgB = LoadGraph("image/b.png");
	int imgA = LoadGraph("image/a.png");
	int imgS = LoadGraph("image/s.png");
	// 斬撃エフェクト
	int imgSlash1 = LoadGraph("image/slash1.png");
	int imgSlash2 = LoadGraph("image/slash2.png");
	int imgSlashB1 = LoadGraph("image/slashB1.png");
	int imgSlashB2 = LoadGraph("image/slashB2.png");
	// 技1
	int imgBomb = LoadGraph("image/bomb.png");
	// タイトル
	int imgTitle = LoadGraph("image/title.png");
	// リザルト
	int imgResult = LoadGraph("image/result.png");
	// 矢印
	int imgArrow = LoadGraph("image/arrow.png");

	// 音
	int bgm = LoadSoundMem("sound/bgm.mp3"); // BGM
	int slashSE = LoadSoundMem("sound/slash.mp3"); // 斬撃
	int slash2SE = LoadSoundMem("sound/slash2.mp3"); // 斬撃強
	int ult1SE = LoadSoundMem("sound/1.mp3"); // 型１
	int ult2SE = LoadSoundMem("sound/2.mp3"); // 型２
	int ult3SE = LoadSoundMem("sound/3.mp3"); // 型３
	int paka = LoadSoundMem("sound/paka.mp3"); // 効果音
	int ultSE = LoadSoundMem("sound/ult.mp3"); // 技
	int kiSE = LoadSoundMem("sound/ki.mp3"); // 気
	int resultSE = LoadSoundMem("sound/result.mp3"); // リザルト

	int BamMaxT = 2; // 画面内に存在できる竹の数

	int arrowP = 380; // 矢印の位置

	int timer = 2000; // 制限時間
	int MTimer = 0; // 画像を動かすタイマー
	int pow = 0; // パワー
	int ult = 0; // 必殺技
	int ultType = 1; // 必殺技の種類
	bool clockFlag = false;
	bool bamFlag = false;
	int efTimer = 100;
	int sp = 0; // 技３

	int score = 0; // スコア
	int highScore = 0; // ハイスコア

	int scene = TITLE; // シーン初期化

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
			PlaySoundMem(bgm, DX_PLAYTYPE_LOOP); // BGM
			score = 0;
			timer = 2000;
			BamMaxT = 2;
			ult = 0;
			sp = 0;
			clockFlag = false;
			bamFlag = false;
			bambooY[1, 2, 3, 4, 5, 6] = -100;
			DrawGraph(0, 0, imgTitle, true); // タイトル画面描画
			if (timer % 60 < 30) drawText(270, 480, 0xffff00, "Press SPACE to Start", 0, 40);
			//DrawFormatString(10, 10, GetColor(255, 255, 255), "HighScore", highScore);
			DrawFormatString(10, 400, GetColor(255, 255, 255), "型１");
			DrawFormatString(10, 450, GetColor(255, 255, 255), "型２");
			DrawFormatString(10, 500, GetColor(255, 255, 255), "型３");
			DrawGraph(80, arrowP, imgArrow, true); // 矢印
			if (CheckHitKey(KEY_INPUT_SPACE))
			{
				scene = PLAY;
			}
			if (CheckHitKey(KEY_INPUT_1))
			{
				ultType = 1;
				arrowP = 380;
			}
			if (CheckHitKey(KEY_INPUT_2))
			{
				ultType = 2;
				arrowP = 430;
			}
			if (CheckHitKey(KEY_INPUT_3))
			{
				ultType = 3;
				arrowP = 480;
			}
			break;
		case PLAY:
			DrawGraph(0, 0, imgBG, false); // 背景
			timer--;
			MTimer++;

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
							if (bambooType[i] == S || swordtype == S || swordtype == B)
							{
								PlaySoundMem(paka, DX_PLAYTYPE_BACK);
								score += 50 + sp;
								timer += 10 + sp;
								ult++;
								if (bambooType[i] == M) { score += 100, timer += 20, ult++; }
								bambooState[i] = 0;
							}

						}
					}
					if (bambooState[i] == 1) // 竹が存在していたら描画
					{
						drawOB(bambooX[i], bambooY[i], bambooType[i]);
					}
					if (timer <= 0)
					{
						bambooY[i] = -100;
						timer = 200;
						scene = OVER;
					}
				}
		

			// スコアの表示
			SetFontSize(30);
			DrawFormatString(5, 5, GetColor(0, 255, 0), "score %d", score);
			DrawFormatString(5, 30, GetColor(0, 255, 0), "high score %d", highScore);
			
			//DrawFormatString(5, 45, GetColor(0, 255, 0), "ult %d", ult);

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
				DrawGraph(350, 300, imgP2[(MTimer / 3) % 2], true); // プレイヤー
				if (pow >= 50)
				{
					swordtype = S;
					PlaySoundMem(slash2SE, DX_PLAYTYPE_BACK);
					DrawGraph(-5, 270, imgSlashB2, true); // 斬撃強
				}
				else 
				{
					DrawGraph(165, 355, imgSlash2, true); // 斬撃
					PlaySoundMem(slashSE, DX_PLAYTYPE_BACK);
				}
				swordX -= 200;
			}
			else if ((prevMouse & MOUSE_INPUT_LEFT) && !(currentMouse & MOUSE_INPUT_LEFT) && mouseX >= 473)
			{
				//printfDx("マウス右\n");
				DrawGraph(350, 300, imgP2[(MTimer / 3) % 2], true); // プレイヤー
				if (pow >= 50)
				{
					swordtype = S;
					PlaySoundMem(slash2SE, DX_PLAYTYPE_BACK);
					DrawGraph(535, 270, imgSlashB1, true); // 斬撃強
				}
				else
				{
					PlaySoundMem(slashSE, DX_PLAYTYPE_BACK);
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
			// 100より増えない
			if (ult >= 100) { ult == 100; }

			// 技 1
			if (CheckHitKey(KEY_INPUT_SPACE) && ult >= 50 && ultType == 1)
			{
				DrawGraph(175, -20, imgBomb, true); //
				PlaySoundMem(ult1SE, DX_PLAYTYPE_BACK);
				ult -= 50;
				score += 200;
				swordtype = B;
			}
			// 技 2
			if (CheckHitKey(KEY_INPUT_SPACE) && ult >= 30 && ultType == 2)
			{
				PlaySoundMem(ult2SE, DX_PLAYTYPE_BACK);
				efTimer = 100;
				clockFlag = true;
				timer += 200;
				ult -= 30;
			}
			// 技3
			if (CheckHitKey(KEY_INPUT_SPACE) && ult >= 30 && ultType == 3)
			{
				PlaySoundMem(ult3SE, DX_PLAYTYPE_BACK);
				efTimer = 100;
				bamFlag = true;
				sp += 2;
				ult -= 30;
			}
			if (CheckHitKey(KEY_INPUT_J))
			{
				ult += 100;
				timer-=500;
			}


			//技2のエフェクト関連
			if (clockFlag == true) { efTimer--; }
			if (efTimer >= 0 && clockFlag == true)
			{
				DrawGraph(400, 120, imgClock[(MTimer / 3) % 2], true);
				DrawFormatString(800, 25, GetColor(255, 255, 0), "time +200");
			}
			//技3のエフェクト関連
			if (bamFlag == true) { efTimer--; }
			if (efTimer >= 0 && bamFlag == true)
			{
				DrawGraph(400, 120, imgUltBam[(MTimer / 4) % 3], true);
				DrawFormatString(10, 60, GetColor(255, 255, 0), "竹からもらえるスコアとタイムが上昇します");
			}
			if (efTimer <= -10)
			{
				clockFlag == false;
				bamFlag == false;
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
				PlaySoundMem(kiSE, DX_PLAYTYPE_BACK);
				DrawGraph(270, 130, imgKI[(MTimer / 4) % 3], true); // 
				DrawGraph(350, 300, imgP1, true); // プレイヤーの描画
			}
			// ゲージ
			if (ultType == 1 && ult >= 50)
			{
				DrawGraph(30, 450, imgUlt[(MTimer / 4) % 3], true); // 
				if (ult >= 100) { DrawGraph(80, 450, imgUlt[(MTimer / 4) % 3], true); } // 
			}
			if (ultType == 2  && ult >= 30 || ultType == 3 && ult >= 30)
			{
				DrawGraph(30, 450, imgUlt[(MTimer / 4) % 3], true); // 
				if (ult >= 60) { DrawGraph(80, 450, imgUlt[(MTimer / 4) % 3], true); } // 
			}
			if (score >= highScore) highScore = score;

		//// 座標確認
		//DrawFormatString(400, 50, GetColor(255, 255, 255), "(%d, %d)", mouseX, mouseY);
		//DrawFormatString(400, 70, GetColor(255, 255, 255), "(%d)", ultType);
			PlaySoundMem(resultSE, DX_PLAYTYPE_BACK);
			break;
			
		case OVER:
			SetFontSize(30);
			DrawGraph(0, 0, imgResult, true); // リザルト画面
			DrawFormatString(50, 10, GetColor(255, 255, 255), "RANK");
			// ランクの描画
			if (score >= 10000)
			{
				DrawGraph(50, 50, imgS, true);
			}
			else if (score >= 6000)
			{
				DrawGraph(50, 50, imgA, true);
			}
			else if (score >= 2000)
			{
				DrawGraph(50, 50, imgB, true);
			}
			else
			{
				DrawGraph(50, 50, imgC, true);
			}
			DrawFormatString(714, 154, GetColor(255, 255, 255), "SCORE %d", score);
			DrawFormatString(714, 254, GetColor(255, 255, 255), "型 %d", ultType);
			timer--;
			if (timer <= 0) 
			{
				scene = TITLE;
			}
			break;
		}


		ScreenFlip(); // 裏画面の内容を表画面に反映させる
		WaitTimer(33);
		if (ProcessMessage() == -1)break; // Windowsから情報を受け取りエラーが起きたら終了
		if (CheckHitKey(KEY_INPUT_ESCAPE) == 1)break; // ESCキーが押されたら終了
	}

	DxLib_End(); // DXライブラリ使用の終了処理
	return 0; // ソフトの終了
}


