//
// Pepper-monaka mode:4 rolling
//  Two analog out
//  Twp analog in
//
//     2015/06/29
// 
//   Copyright (c) 2015 by Yoshitaka Kuwata
//   Copyright (c) 2015 by morecat_lab
//

import monaka.*;
import themidibus.*; 

monaka pepper;

//ボールの座標用変数
float xPos,yPos;

//オフセット用変数
int xOffset = 0;
int yOffset = 0;

void setup() {
  //3D画面設定
  size(600,600,P3D);
  pepper = new monaka(4);
  //ワイヤーフレームなし
  noStroke();
}

void draw() {
  //背景色
  background(220);
  //画面上半分の塗色
  fill(50);
  //画面上半分の矩形
  rect(0,0,width,height/2);
  
  if (mousePressed) {  // reset position
    xPos = yPos = 0;
  }

  //読み込み値をオフセット値に設定する
  if (xOffset == 0) 
    xOffset = pepper.analogInput(1);
  if (yOffset == 0)
    yOffset = pepper.analogInput(0);
  println("offsetx = " + xOffset + "Analog[0] = " + yOffset);  

  //ボールの速度の計算  
  float xSpeed=pepper.analogInput(1) - xOffset;
  float ySpeed=pepper.analogInput(0) - yOffset;
  //ボール移動量の計算
  xPos+=-xSpeed;
  yPos+=-ySpeed;
  //ボール位置の設定
  translate(width/2+xPos,height*4/5,-200+yPos);

  //影の塗色
  fill(50);
  //影の座標と大きさ
  ellipse(-25,30,80,80/5);

  //直線光の設定
  directionalLight(255, 255, 255, -1, 1, 0);
  //ボールの塗色
  fill(255,100,50);
  //ボール描画
  sphere(50);
}

