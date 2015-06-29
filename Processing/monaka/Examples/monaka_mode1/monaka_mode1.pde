//
// Pepper-monaka mode:1 test
//  Four digital out
//
//       2015/06/29
// 
//   Copyright (c) 2015 by Yoshitaka Kuwata
//   Copyright (c) 2015 by morecat_lab
//

import monaka.*;
import themidibus.*;

monaka pepper;
PFont myFont;

void setup() {
  size(250,250);
  background(0);
  
  pepper = new monaka(1);
  myFont = loadFont("CourierNewPSMT-24.vlw");
  textFont(myFont, 24);
  
  frameRate(30);
}

void draw() {
  // check mouse status  
  if (mousePressed == true) {
    background(255);
    fill(0);
    pepper.digitalOutput(0, 1);
    text("ON ",100, 120);
  } else {
    background(0);
    fill(255);
    pepper.digitalOutput(0, 0);
    text("OFF",100, 120);
  }
}

