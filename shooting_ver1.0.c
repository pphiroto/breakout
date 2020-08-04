/*****
全方位シューティング
g1853347 坂下啓斗
最後に先生に言われた、「緑の敵がプレイヤーを必ず囲むように出現し、プレイヤーの動きに合わせて三角形が小さくなる」が実現できませんでした。
なので、その直前のバージョンを提出します。
緑の敵は複数体で緑の敵同士で繋がり、ずっとまっすぐに進みます。
****/

#include<stdio.h>
#include<handy.h>
#include<math.h>
#include<stdlib.h>
#include<time.h>


#define WINDOWSIZE 800
#define MAGAZINE 5
#define ENEMY 60
#define ALARM 60


/* プレイヤーの座標 */
typedef struct Player {
	int x; int y; //座標
	int size;
	int live; //生存判定
} Player;

/* 弾の座標 */
typedef struct Bullet {
	double x; double y; //座標
	double vx; double vy; //その弾の進むべき方向（速さ）
	double live; //弾を作るときに１になり、敵にあたると０になる
} Bullet;

typedef struct Enemy {
	double x; double y; //敵
	double vx; double vy; //敵の動き
	int type;
	int size;
	int live; //存在するか否か
	int in; //跳ね返りのための変数
	int num; //どの画面端から出現したか
	int alarm; //緑の敵が出現前に出す矢印のカウントを数えるもの
} Enemy;

//プロトタイプ宣言
void DrawPlayer(Player p, int lay);
Player MovePlayer(Player p, hgevent *e);
Bullet MakeBullet(Bullet b, Player p, hgevent *e, int lay);
void DrawBullet(Bullet b, int lay);
Bullet MoveBullet(Bullet b);
Enemy MakeEnemy(Enemy e1, Enemy e2, Enemy e3, Player p, int j);
void DrawEnemy(Enemy e1, int lay);
Enemy MoveEnemy(Enemy e, Player p);
int HitCheck(double x1, double y1, double x2, double y2, double size, int live1, int live2);
Player LineCheck(Enemy e1, Enemy e2, Player p);
void DrawAlarm(Enemy e, int lay);
void DrawLine(Enemy e1, Enemy e2, int lay);
void LevelupScene();
int GameoverScene();
void TitleScene();



/*------------メイン----------*/
int main() {
	int window = HgOpen(WINDOWSIZE, WINDOWSIZE);
	doubleLayer layers = HgWAddDoubleLayer(window); //ダブルバッファを作る
	int MainScene;
	HgSetEventMask(HG_MOUSE_DOWN | HG_KEY_DOWN);
	hgevent *event;
	int i, j, r; //ループカウンタ
	int l1, l2; //ライン用の変数
	double frequency = 0.6; //敵の出る頻度（初期値は１.２秒）
	int gameover = 0; //ゲームオーバー判定
	srand(time(NULL)); //乱数列の初期化

	TitleScene(); //タイトル画面を表示する


	//タイトル画面から進んだらこのループに行く
	for(;;) {
		//いろいろ初期化
		j=0;
		l1=0, l2=0;
		int shoot = 0;
		Player player = {WINDOWSIZE/2, WINDOWSIZE/2, 10, 1}; //プレイヤーの初期座標
		Bullet bullet[MAGAZINE] = {};
		Enemy enemy[ENEMY] = {};
		HgClear();

		HgSetIntervalTimer(frequency); //敵の出る頻度

		/*ゲーム中はこのループを繰り返す*/
		for(;;) {
			MainScene = HgLSwitch(&layers); //ダブルバッファ
			HgLClear(MainScene);
			event = HgEventNonBlocking(); //イベントを取得する
			for(i=0; i<MAGAZINE; i++) { //弾を描画する
				DrawBullet(bullet[i], MainScene);
			}
			if(event != NULL) {
				if(event->type == HG_TIMER_FIRE) { //タイマが作動した時、
					enemy[j] = MakeEnemy(enemy[j], enemy[j-1], enemy[j-2], player, j); //敵を作る
					j++;
				}
			}
			for(i=0; i<ENEMY; i++) {
				if(enemy[i].live == 1 ) {
					DrawEnemy(enemy[i], MainScene); //敵が生きていたら、敵を描画する
					if(enemy[i].type == 2) { //敵タイプが２なら、alarmカウントを＋１する
						enemy[i].alarm++;
						if(enemy[i].in == 1) {
							if(l1 == 0) {
								l1 = i;
							} else {
								l2 = i;
								DrawLine(enemy[l1], enemy[l2], MainScene); //緑の敵同士をラインでつなぐ
								player = LineCheck(enemy[l1], enemy[l2], player); //ラインとの当たり判定を測定する
								l1 = l2;
							}
						}
					}
					for(r=0; r<MAGAZINE; r++) {
						enemy[i].live = HitCheck(bullet[r].x, bullet[r].y, enemy[i].x, enemy[i].y,  enemy[i].size, enemy[i].live, bullet[r].live); //敵を描画する際に各敵に関して敵と弾との当たり判定をする
						if(enemy[i].live == 0) bullet[r].live = 0;
					}

					player.live = HitCheck(player.x, player.y, enemy[i].x, enemy[i].y, player.size, player.live, player.live); //各敵を描画する際に各敵とプレイヤーとの当たり判定を調べる
				}
			}
			if(event != NULL) {
				if(event->type == HG_MOUSE_DOWN) { //マウスが押されたら
					if(shoot<MAGAZINE) {
						bullet[shoot] = MakeBullet(bullet[shoot], player, event, MainScene); //弾を作る
						if(shoot!=MAGAZINE-1) shoot++; //shootで定めた数だけしか画面内に弾は存在することができない。
						else shoot = 0;
					}
				}
			}
			player = MovePlayer(player, event); //プレイヤーを動かす
			for(i=0; i<MAGAZINE; i++) {
				bullet[i] = MoveBullet(bullet[i]); //弾を動かす
			}
			for(i=0; i<ENEMY; i++) {
				if(enemy[i].live == 1) {
					enemy[i] = MoveEnemy(enemy[i], player); //敵を動かす
				}
			}

			DrawPlayer(player, MainScene); //プレイヤーを描画する（これが処理の最後にあるのは、各種描画と判定が終わった後に描画することで、プレイヤーが何で死んだのかをわかりやすくするため）

			if(player.live == 0) { //ゲームオーバー判定
				break;
			}
			if(j == ENEMY) { //敵を一定数出現させたら（一定時間たったら）レベルアップし（敵の出現頻度を早くさせ）、ループを抜ける
				break;
			}
		}
		if(player.live == 0) { //プレイヤーが死んでいたらゲームオーバー画面にいく
			HgSetIntervalTimer(-1);  //タイマを止める
			gameover = GameoverScene();
		} else {	//プレイヤーが生きていたらレベルアップし、最初から
			LevelupScene();
			frequency*=0.75;
		}
		if(gameover == 1) break; //ゲームオーバー画面で終わるを押すと、このループも抜け、クローズする
	}

	HgClose();
	return 0;
}
/*---------------メイン終わり--------------*/


/*プレイヤーを描画する*/
void DrawPlayer(Player p, int lay)
{
	HgWSetColor(lay, HG_BLACK);
	HgWSetFillColor(lay, HG_BLUE);
	HgWCircleFill(lay, p.x, p.y, p.size, 1);
}


/*キー操作によってプレイヤーを動かす*/
Player MovePlayer(Player p, hgevent *e)
{
	if (e != NULL) {
		if(e->type == HG_KEY_DOWN) {
			switch (e->ch) {
				case HG_U_ARROW:
				case 'w':
				if(p.y < WINDOWSIZE) p.y += 15;
				break;
				case HG_L_ARROW:
				case 'a':
				if(p.x > 0)	p.x += -15;
				break;
				case HG_D_ARROW:
				case 's':
				if(p.y > 0) p.y += -15;
				break;
				case HG_R_ARROW:
				case 'd':
				if(p.x < WINDOWSIZE) p.x += 15;
				break;
			}
		}
	}
	return p;
}


/*弾を描画する*/
void DrawBullet(Bullet b, int lay)
{
	if(b.live == 1) {
		HgWSetColor(lay, HG_BLACK);
		HgWSetFillColor(lay, HG_YELLOW);
		HgWCircleFill(lay, b.x, b.y, 3, 1);
	}
}


/*クリックを検知して弾を作る*/
Bullet MakeBullet(Bullet b, Player p, hgevent *e, int lay)
{
	b.live = 1; //弾を作る
	/*フィールド*/
	double rad = atan2(e->x-p.x, e->y-p.y); //クリックされた地点とプレイヤーとの角度を測る

	b.vx = sin(rad)*5;	//x成分と
	b.vy = cos(rad)*5;	//y成分の速度を格納する

	b.x = p.x;
	b.y = p.y;
	return b;
}


/*弾を動かす*/
Bullet MoveBullet(Bullet b)
{
	b.x += b.vx;
	b.y += b.vy;
	return b;
}


/*一定時間ごとに敵を作る*/
Enemy MakeEnemy(Enemy e1, Enemy e2, Enemy e3, Player p, int j)
{
	int num = rand()%6;
	if(num==0||num==1) e1.type=2;
	else if(num==2) e1.type=3;
	else e1.type=1;

	e1.live = 1; //敵に生命を宿す
	/*初期座標を決める*/
	for(;;) {
		int num = rand()%4;
		if(e2.num!=0&&e3.num!=0&&num == 0) {
			e1.x = WINDOWSIZE+10;
			e1.y = rand()%WINDOWSIZE;
			e1.num = 0;
			break;
		}
		if(e2.num!=1&&e3.num!=1&&num == 1) {
			e1.x = rand()%WINDOWSIZE;
			e1.y = WINDOWSIZE+10;
			e1.num = 1;
			break;
		}
		if(e2.num!=2&&e3.num!=2&&num == 2) {
			e1.x = -10;
			e1.y = rand()%WINDOWSIZE;
			e1.num = 2;
			break;
		}
		if(e2.num!=3&&e3.num!=3&&num == 3) {
			e1.x = rand()%WINDOWSIZE;
			e1.y = -10;
			e1.num = 3;
			break;
		}
	}

	double rad = atan2(p.x-e1.x, p.y-e1.y); //誕生した位置からプレイヤーの方向を取得し、そこに向かってまっすぐ進む
	e1.vx = sin(rad)*2;
	e1.vy = cos(rad)*2;

	e1.size = 10;

	return e1;
}


/*敵を描画する*/
void DrawEnemy(Enemy e1, int lay)
{
	if(e1.type == 1) { //type1の時
		HgWSetColor(lay, HG_BLACK);
		HgWSetFillColor(lay, HG_RED);
		HgWCircleFill(lay, e1.x, e1.y, e1.size, 1);
	}

	if(e1.type == 2) { //type2の時
		if(e1.alarm >ALARM) { //alarmが一定回数以上の時
			HgWSetColor(lay, HG_BLACK);
			HgWSetFillColor(lay, HG_GREEN);
			e1.size += 6;
			HgWBoxFill(lay, e1.x, e1.y, e1.size, e1.size, 1);
		}
		else DrawAlarm(e1, lay); //alarmが少ない時
	}

	if(e1.type == 3) { //type3の時
		HgWSetColor(lay, HG_BLACK);
		HgWSetFillColor(lay, HG_PINK);
		HgWCircleFill(lay, e1.x, e1.y, e1.size, 1);
	}

}


/*敵を動かす*/
Enemy MoveEnemy(Enemy e, Player p)
{
	if(e.type == 1) {
		e.x += e.vx;
		e.y += e.vy;
	}
	else if(e.type == 2 && e.alarm>ALARM) {
		e.x += e.vx*0.5;
		e.y += e.vy*0.5;
	}
	else if(e.type == 3) {
		e.x += e.vx*0.6;
		e.y += e.vy*0.6;
		double rad = atan2(p.x-e.x, p.y-e.y); //動く度にプレイヤーとの角度を取得し、方向を定める
		e.vx = sin(rad)*2;
		e.vy = cos(rad)*2;
	}

	if((e.x>0 && e.x<WINDOWSIZE)&&(e.y>0 && e.y<WINDOWSIZE)) {
		e.in = 1;	//画面外から出現し、画面内に入ったことを判定するため
	}
	if((e.x<0 || e.x>WINDOWSIZE)&&(e.in == 1)) {
		e.vx *= -1; //跳ね返り
	}
	if((e.y<0 || e.y>WINDOWSIZE)&&(e.in == 1)) {
		e.vy *= -1; //跳ね返り
	}
	return e;
}


/*x1とy1を基準として当たり判定を調べる*/
int HitCheck(double x1, double y1, double x2, double y2, double size, int live1, int live2)
{
	if(live1 == 1 && live2 == 1) {
		if(((x2>=x1-size)&&(x2<=x1+size))&&((y2>=y1-size)&&(y2<=y1+size))) {
			live1 = 0;
		}
	}
	return live1;
}


/*ラインとプレイヤーとの当たり判定を調べる*/
Player LineCheck(Enemy e1, Enemy e2, Player p)
{
	if((e1.type == 2 && e2.type == 2)&&(e1.live == 1 && e2.live == 1)&&(e1.alarm>ALARM && e2.alarm>ALARM)) {
		double rad1 = atan2(e2.x+8-e1.x+8, e2.y+8-e1.y+8);
		double rad2 = atan2(p.x-e1.x+8, p.y-e1.y+8);

		if((rad2<rad1+0.03 && rad2>rad1-0.03)&&((p.x>e1.x+7&&p.x<e2.x+7)||(p.x>e2.x+7&&p.x<e1.x+7))&&((p.y>e1.y+7&&p.y<e2.y+7)||(p.y>e2.y+7&&p.y<e1.y+7))) p.live = 0;
	}
	return p;
}


/*緑の敵が出現する前に矢印を描画する*/
void DrawAlarm(Enemy e, int lay)
{
	HgWSetFont(lay, HG_TB, 70);
	HgWSetColor(lay, HG_RED);
	if(e.num == 0) HgWText(lay, e.x-50, e.y, "→");
	if(e.num == 1) HgWText(lay, e.x, e.y-50, "↑");
	if(e.num == 2) HgWText(lay, e.x+50, e.y, "←");
	if(e.num == 3) HgWText(lay, e.x, e.y+50, "↓");
}


/*緑の敵を繋ぐラインを描画する*/
void DrawLine(Enemy e1, Enemy e2, int lay)
{
	if(e1.alarm>ALARM && e2.alarm>ALARM) {
		HgWSetColor(lay, HG_GREEN);
		HgWLine(lay, e1.x+7, e1.y+7, e2.x+7, e2.y+7);
	}
}


/*レベルアップ画面*/
void LevelupScene() {
	HgClear();
	HgSetFont(HG_TB, 100);
	HgText(180, 350, "LevelUp");
	HgSetFont(HG_TB, 50);
	HgText(300, 300, "敵の出現頻度アップ");
	HgSleep(3.00);
	HgGetChar();
}


/*ゲームオーバー画面*/
int GameoverScene() {
	int gameover = 0;
	HgSetFont(HG_TB, 100);
	HgText(170, 360, "GameOver");
	HgBox(100, 100, 200, 50);
	HgBox(500, 100, 200, 50);
	HgSetFont(HG_TB, 50);
	HgText(100, 90, "リトライ");
	HgText(520, 90, "終わる");
	HgSleep(1.00);
	hgevent *event;
	for(;;) {
		event = HgEvent();
		if(event->type == HG_MOUSE_DOWN) {
			if((event->x>100 && event->x<300)&&(event->y>100 && event->y<150)) {
				gameover = 0;
				break;
			} else if((event->x>500 && event->x<700)&&(event->y>100 && event->y<150)) {
				gameover = 1;
				break;
			}
		}
	}
	return gameover;
}


/*タイトル画面*/
void TitleScene()
{
	HgSetFont(HG_TB, 60);
	HgText(110, 600, "全方位シューティング");
	HgSetFont(HG_T, 30);
	HgText(328, 520, "ゲーム説明");
	HgSetFont(HG_T, 20);
	HgText(100, 480,  "・自機　　はWASD(又は矢印）キーで操作します。");
	HgSetFillColor(HG_BLUE);
	HgCircleFill(180, 493, 10, 1);
	HgText(100, 430,  "・画面をクリックすると、自機からクリックした方向に弾を打ちます。");
	HgText(100, 380,  "・敵　　　　　　から、逃げたり、弾でやっつけたりしながら、");
	HgSetFillColor(HG_RED);
	HgCircleFill(160, 393, 10, 1);
	HgSetFillColor(HG_GREEN);
	HgBoxFill(187, 384, 18, 18, 1);
	HgSetFillColor(HG_PINK);
	HgCircleFill(233, 393, 10, 1);
	HgSetFont(HG_T, 35);
	HgText(150, 310,  "ひたすら生き延びてください！");
	HgSetFont(HG_T, 100);
	HgText(170, 100,  "Game Start");
	HgBox(160, 100, 480, 120);
	HgSetFont(HG_TB, 20);
	HgText(375, 70,  "↑click!");


	HgSleep(1.00);
	hgevent *event;
	for(;;) {
		event = HgEvent();
		if(event->type == HG_MOUSE_DOWN) {
			if((event->x>160 && event->x<640)&&(event->y>100 && event->y<220)) {
				break;
			}
		}
	}
}
