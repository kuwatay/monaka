/* sound demo with soundclipher */

import arb.soundcipher.*;
import monaka.*;
import themidibus.*; 

PFont myFont;
monaka pepper;
SoundCipher sc = new SoundCipher(this);

int cnt = 0;

void setup() {
  size(250,250);
  pepper = new monaka(4);
  myFont = loadFont("CourierNewPSMT-24.vlw");
  textFont(myFont, 24);
  frameRate(8);
}

void draw() {
  int v;
  v = pepper.analogInput(1);  // read analog port
  background(v*2);  // Display Control
  fill(255, 0, 0);
  text("Analog[0] = " + v ,30, 135);
  text("Analog[1] = "+ pepper.analogInput(0), 30, 155);
  if (++cnt >= 8) {
    sc.instrument(random(80));
    cnt = 0;
  }
//  sc.instrument(1);
//  sc.playNote(random(40) + 60 - v/2, random(50) + 70, 0.2);
//  sc.playNote( 60 - v/4, random(50) + 70, 0.2);
  sc.playNote( 60 - v/4, pepper.analogInput(0)/2 + 70, 0.2);

}
