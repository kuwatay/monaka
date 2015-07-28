// Pepper-monaka mode:6 test
//  Twp analog in
//
//     2015/06/29
// 
//   Copyright (c) 2015 by Yoshitaka Kuwata
//   Copyright (c) 2015 by morecat_lab

import monaka.*;
import themidibus.*; 

monaka pepper;
PFont myFont;

void setup() {
  size(250,250);
  background(0);
  
  pepper = new monaka(6);
  myFont = loadFont("CourierNewPSMT-24.vlw");
  textFont(myFont, 24);
  
  frameRate(20);
}

void draw() {
  background(0);
  text("Analog[0] = "+ pepper.analogInput(0), 30, 105);
  text("Analog[1] = "+ pepper.analogInput(1), 30, 125);
  text("Analog[2] = "+ pepper.analogInput(2), 30, 145);
}

