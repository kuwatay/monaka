// Pepper-monaka mode:4 monaka_autolight
//  Two analog out
//  Twp analog in
//   Copyright (c) 2015 by Yoshitaka Kuwata
//   Copyright (c) 2015 by morecat_lab

import monaka.*;
import themidibus.*; 

monaka pepper;
PFont myFont;

void setup() {
  size(250,250);
  background(0);
  
  pepper = new monaka(4);
  myFont = loadFont("CourierNewPSMT-24.vlw");
  textFont(myFont, 24);
  frameRate(20);
}

void draw() {
  int v = pepper.analogInput(0);
  background(0);
  text("Analog[0] = "+ pepper.analogInput(0), 30, 135);
  text("Analog[1] = "+ pepper.analogInput(1), 30, 155);

  // LED Control
  pepper.analogOutput(0, v);
}

