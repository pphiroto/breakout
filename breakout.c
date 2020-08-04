#include<stdio.h>
#include<handy.h>
#define WINDOWSIZE 800
#define wx 100
#define wy 40 //box１つの幅。この二つをいじると、boxの数を変えられる
#define bvx 16 //barの移動速度

int main() {
    int vx = 3,vy = 2; //玉の移動速度
    int bx = 360; //barの初期x座標
    int cx = bx+40, cy = 114; //玉の初期座標
    int lx,ly; //ラインを引く座標
    int i,j; //カウンタ変数
    int stock = 2; //残機
    int a = WINDOWSIZE/wx, b = (WINDOWSIZE-480)/wy;
    int box[a][b];
    for (i=0;i<a;i++) { //box内の数字を全て０にする
        for (j=0;j<b;j++) {
            box[i][j] = 0;
        }
    }
    int clear =0; //クリア判定するための関数
    int through = 0; //貫通モードの判定
    doubleLayer layers;
    int layerID;
    int windowID = HgOpen(WINDOWSIZE,WINDOWSIZE);
    int layer1 = HgWAddLayer(0);
    int layer2 = HgWAddLayer(0);
    int layer3 = HgWAddLayer(0);
    layers = HgWAddDoubleLayer(windowID); //ダブルバッファを作る
    HgSetEventMask(HG_MOUSE_DOWN | HG_KEY_DOWN | HG_MOUSE_DRAG);
    hgevent *event;
    HgSetFillColor(HG_GRAY); //ブロックの色


    //ブロックを配置する際にわかりやすいようにラインを引く
    for (lx=0; lx<WINDOWSIZE; lx += wx) HgWLine(layer1,lx,480,lx,WINDOWSIZE);
    for (ly=0; ly<WINDOWSIZE; ly += wy) HgWLine(layer1,0,ly+480,WINDOWSIZE,ly+480);
    //このラインはブロックを確定した時に消す




    HgWText(layer1,250,400,"クリックまたはドラッグでブロックを配置します。\nスペースキーを押すとブロックが確定します。\n");
    HgSetColor(HG_WHITE);
    for (;;) { //ブロックの配置
        event = HgEvent();
        if (event->type ==  (HG_MOUSE_DRAG)||(HG_MOUSE_DOWN)) {
            int x = event->x;
            int y = event->y;
            for (i=0;i<a;i++) {
                for (j=0;j<b;j++) {
                    if ((x>wx*i && x<wx*(i+1)) && ((y>480+wy*j) && (y<480+wy*(j+1)))) {
                      //どこかがドラッグかクリックされる度に判定する。ドラッグかクリックされた場所を１とし、他の場所を０とする
                        box[i][j] = 1;
                        HgWBoxFill(windowID,i*wx,480+j*wy,wx,wy,1);
                    }
                }
            }
        }
        if (event->type == HG_KEY_DOWN) {
          //スペースキーを押すと確定する
            switch (event->ch) {
            case ' ': break;
            } break;
        }
    }
    HgLClear(layer1);






    HgWText(layer1,200,400,"矢印キーの左右で移動、スペースキーで弾を発射する");
    HgWSetFont(layer2,HG_G,30);
    HgWSetColor(layer3,HG_RED);

    for (;;) {
      //一番大きなループ、クリアおよびゲームオーバーを判定する
        if (clear == a*b) break;
        if (stock == -1) break;
        HgLClear(layer2);
        HgWText(layer2,10,10,"残機: %d",stock);
        cy = 114; //下の座標まで落ちた球を上にあげる
        for (;;) {
            layerID = HgLSwitch(&layers);
            HgLClear(layerID);
            HgWBox(layerID,bx,99,80,10);
            HgWCircle(layerID,bx+40,cy,5);
            event = HgEvent();
            HgLClear(layer1);
            if (event->type == HG_KEY_DOWN) {
              //barを移動させる、弾を発射する
                switch (event->ch) {
                case HG_L_ARROW:
                    if (bx>0) bx += -bvx;
                    break;
                case HG_R_ARROW:
                    if (bx<WINDOWSIZE-80) bx += bvx;
                    break;
                case HG_U_ARROW:
                    if (through == 0) {
                      through = 1;
                      HgLClear(layer3);
                      HgWText(layer3,700,10,"貫通モードON");
                    } else {
                      through = 0;
                      HgLClear(layer3);
                    }
                    break;
                }
            } if (event->ch == ' ') break;
        }


        cx = bx+40;
        HgSetFillColor(HG_WHITE);//あたったboxはこれで塗りつぶす

        //弾を発射した後
        for (;;) {
          //弾を発射したら、玉とbarをバラバラに動かす
            clear = 0;
            for (i=0;i<a;i++) {
              //boxに格納されている値を全て判定する
                for (j=0;j<b;j++) {
                  //すべて０になっていたらクリアにする
                    if (box[i][j] == 0) clear += 1;
                }
            }
            if (clear == a*b) break;
            layerID = HgLSwitch(&layers);
            HgLClear(layerID);
            HgWBox(layerID,bx,99,80,10);
            HgWCircle(layerID,cx,cy,5); //ダブルバッファで描写する
            cx += vx;
            cy += vy;
            event = HgEventNonBlocking();
            if (event != NULL)  {
              //barを移動させる
                switch (event->ch) {
                case HG_L_ARROW:
                    if (bx>0) bx += -bvx;
                    break;
                case HG_R_ARROW:
                    if (bx<WINDOWSIZE-80) bx += bvx;
                    break;
                case HG_U_ARROW:
                    if (through == 0) {
                      through = 1;
                      HgLClear(layer3);
                      HgWText(layer3,700,10,"貫通モードON");
                    } else {
                      through = 0;
                      HgLClear(layer3);
                    }
                    break;
                }
              }
            if (cx>WINDOWSIZE || cx<0) vx *= -1;
             //windowの端で跳ね返る
            if (cy>WINDOWSIZE || ((cy>=99 && cy<=109) && (cx >bx && cx<bx+80))) vy *= -1;
            //バーに当たった時に跳ね返る
            for (i = 0; i<a; i++) {
              //boxにあたった時に跳ね返る
                for (j = 0; j<b; j++) {
                    if ((((cy>=479+wy*j)&&(cy<=481+wy*j)) || ((cy>=479+wy*(j+1))&&(cy<=481+wy*(j+1)))) && (box[i][j]==1)) {
                        if ((cx>=wx*i) && (cx<=wx*(i+1))) {
                            if (through == 0) vy *= -1;
                            box[i][j] = 0;
                            HgWBoxFill(windowID,i*wx,480+j*wy,wx,wy,1);
                        }
                    }
                    else if ((((cx>=(wx-1)*i)&&(cx<=(wx+1)*i)) || ((cx>=(wx-1)*(i+1))&&(cx<=(wx+1)*(i+1)))) && (box[i][j]==1)) {
                        if ((cy>=480+wy*j) && (cy<=480+wy*(j+1))) {
                            if (through ==0) vx *= -1;
                            box[i][j] = 0;
                            HgWBoxFill(windowID,i*wx,480+j*wy,wx,wy,1);
                        }
                    }
                }
            }
            if (cy<-5) {
              //ボールが下に落ちた時の判定
                HgWText(layer1,250,400,"矢印キーの左右で移動、スペースキーで次の弾を発射");
                stock--;
                break;
            }
        }
    }

    if (clear==a*b) {
      //クリアの表示
        HgSetColor(HG_RED);
        HgClear();
        HgSetFont(HG_TB,100);
        HgText(118,WINDOWSIZE/2,"Game Clear!!");
        HgSetFont(HG_G,20);
        HgSetColor(HG_BLACK);
    }

    if (stock == -1) {
      //ゲームオーバーの判定
        HgSetColor(HG_BLACK);
        HgClear();
        HgSetFont(HG_TB,100);
        HgText(145,WINDOWSIZE/2,"Game Over");
        HgSetFont(HG_G,20);
    }
    HgText(250,300,"何かキーを押すと終了します。");
    HgGetChar();


    HgClose();
    return 0;
}
