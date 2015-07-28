/*
 * 3ch分のアナログ入力をグラフとして表示するサンプル
 * monaka mode=6
 */

import monaka.*;
import themidibus.*; 

monaka pepper;
PFont font;

Scope scopeX;
Scope scopeY;
Scope scopeZ;

void setup() {
  // ウィンドウサイズを320x480に設定
  size(320, 480);

  // テキスト表示用フォントをロードして設定
  font = loadFont("CourierNewPS-BoldMT-12.vlw");
  textFont(font, 12);

  // 3ch分の波形表示用インスタンスを生成
  scopeX = new Scope(40, 40, 200, 100, "X axis");  
  scopeY = new Scope(40, 190, 200, 100, "Y axis");  
  scopeZ = new Scope(40, 340, 200, 100, "Z axis");  

  // Pepper Monakaのインスタンスを生成
  pepper = new monaka(6);
  
}

void draw() {
  // 背景を黒で塗りつぶす
  background(0);
  
  // 3ch分を描画
  scopeX.updateAndDraw(pepper.analogInput(0));
  scopeY.updateAndDraw(pepper.analogInput(1));
  scopeZ.updateAndDraw(pepper.analogInput(2));
}

