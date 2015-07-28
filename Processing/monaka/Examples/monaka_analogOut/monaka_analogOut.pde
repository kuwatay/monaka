// Pepper-monaka mode:4 monaka_autolight
//  Two analog out
//  Twp analog in
//   Copyright (c) 2015 by Yoshitaka Kuwata
//   Copyright (c) 2015 by morecat_lab

import monaka.*;
import themidibus.*; 

monaka pepper;
PFont myFont;
int on = 0;

void setup() {
  size(250, 250);
  background(0);

  pepper = new monaka(4);
  myFont = loadFont("CourierNewPSMT-24.vlw");
  textFont(myFont, 24);

  frameRate(10);
}

void draw() {
  // check mouse status
  if (mousePressed == true) {
    on+= 4;
    if (on > 127) {
      on = 127;
    }
  } else {
    on-= 4;
    if (on < 0) {
      on = 0;
    }
  }
  // Display Control
  if (on > 0) {
    background(255);
    fill(0);
    text("ON " + on, 100, 120);
  } else {
    background(0);
    fill(255);
    text("OFF", 100, 120);
  }    
  // LED Control
  pepper.analogOutput(0, on);
}

