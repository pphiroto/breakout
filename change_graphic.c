/***
change_graphic.c
Hiroto Sakashita
2018/10/4
***/

#include<stdio.h>
#include<handy.h>


void draw500(int n) {
  int y = 120;
  HgSetFont(HG_M, 30);
  HgText(100, 20, "500");
  HgText(100, 290, "%d枚", n);
  HgSetFillColor(HgRGB(0.731, 0.689, 0.224));
  HgSetFont(HG_MB, 50);
  for (int i = 0; i < n; i++) {
    y += 10;
    HgCircleFill(125, y, 71, 1);
    HgText(79, y-30, "500");
  }
}

void draw100(int n) {
  int y = 120;
  HgSetFont(HG_M, 30);
  HgText(250, 20, "100");
  HgText(250, 290, "%d枚", n);
  HgSetFillColor(HgRGB(0.720, 0.720, 0.723));
  HgSetFont(HG_MB, 50);
  for (int i = 0; i < n; i++) {
    y += 10;
    HgCircleFill(275, y, 60, 1);
    HgText(229, y-30, "100");
  }
}

void draw50(int n) {
  int y = 120;
  HgSetFont(HG_M, 30);
  HgText(400, 20, "50");
  HgText(392, 290, "%d枚", n);
  HgSetFillColor(HgRGB(0.675, 0.675, 0.678));
  HgSetFont(HG_MB, 44);
  for (int i = 0; i < n; i++) {
    y += 10;
    HgCircleFill(415, y, 53, 1);
    HgText(388, y-27, "50");
  }
}

void draw10(int n) {
  int y = 120;
  HgSetFont(HG_M, 30);
  HgText(549, 20, "10");
  HgText(550, 290, "%d枚", n);
  HgSetFillColor(HgRGB(0.385, 0.200, 0.100));
  HgSetFont(HG_MB, 50);
  for (int i = 0; i < n; i++) {
    y += 10;
    HgCircleFill(565, y, 62, 1);
    HgText(533, y-30, "10");
  }
}

void draw5(int n) {
  int y = 120;
  HgSetFont(HG_M, 30);
  HgText(715, 20, "5");
  HgText(700, 290, "%d枚", n);
  HgSetFillColor(HgRGB(0.531, 0.489, 0.124));
  HgSetFont(HG_MB, 44);
  for (int i = 0; i < n; i++) {
    y += 10;
    HgCircleFill(725, y, 57, 1);
    HgText(710, y-27, "5");
  }
}

void draw1(int n) {
  int y = 120;
  HgSetFont(HG_M, 30);
  HgText(865, 20, "1");
  HgText(850, 290, "%d枚", n);
  HgSetFillColor(HgRGB(0.800, 0.800, 0.800));
  HgSetFont(HG_MB, 44);
  for (int i = 0; i < n; i++) {
    y += 10;
    HgCircleFill(875, y, 49, 1);
    HgText(862, y-27, "1");
  }
}

int main() {
  int money[6] = {500, 100, 50, 10, 5, 1};
  int num[6];
  int total, kingaku;
  int i;
  printf("金額を入力してください : ");
  scanf("%d", &total);
  kingaku = total;
  for (i = 0; i < 6; i++) {
    num[i] = total / money[i];
    total = total % money[i];
  }

  HgOpen(1000, 400);
  HgSetFont(HG_M, 40);
  HgText(440, 348, "%d円", kingaku);
  draw500(num[0]);
  draw100(num[1]);
  draw50(num[2]);
  draw10(num[3]);
  draw5(num[4]);
  draw1(num[5]);

  HgGetChar();
  HgClose();

  return 0;
}
