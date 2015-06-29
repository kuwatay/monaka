//
// Pepper-monaka mode:2 test
//  Two digital output (Pin2 and Pin3)
//  One tone output (Pin4)
//  One digital Input (Pin5)
//
//       2015/06/29
// 
//   Copyright (c) 2015 by Yoshitaka Kuwata
//   Copyright (c) 2015 by morecat_lab
//

import monaka.*;
import themidibus.*;
import java.util.Map;

monaka pepper;
PFont myFont;

HashMap<String,Integer> midiMap = new HashMap<String,Integer>();
char ky[] = { ' ' };
int keystatus = 0,  nn = 0;
int mousestatus = 0;
String displayString = "Press Key/Mouse";

void setup() {
  size(250,250);
  background(0);
  
  // initialize keymap
  midiMap.put("z", 60);
  midiMap.put("s", 61);
  midiMap.put("x", 62);
  midiMap.put("d", 63);
  midiMap.put("c", 64);
  midiMap.put("v", 65);
  midiMap.put("g", 66);
  midiMap.put("b", 67);
  midiMap.put("h", 68);
  midiMap.put("n", 69);
  midiMap.put("j", 70);
  midiMap.put("m", 71);
  midiMap.put(",", 72);

  pepper = new monaka(2);
  myFont = loadFont("CourierNewPSMT-24.vlw");
  textFont(myFont, 24);
  
  frameRate(30);
}

void draw() {
  // check keystatus
  if ((keystatus == 1) || (mousestatus != 0)){
    background(255);
    fill(0);
  } else {
    background(0);
    fill(255);
  }
  text(displayString, 20, 120);
}

void keyPressed() {
  ky[0] = key;
  String k = new String(ky);
  if ((midiMap.get(k)) != null) {
    nn = midiMap.get(k);
    pepper.sendNoteOn(nn, 127);
    displayString = "Key ON " + k + "(" + nn +")";
    keystatus = 1;
  }
}

void keyReleased() {
  ky[0] = key;
  String k = new String(ky);
  if ((midiMap.get(k)) != null) {
    nn = midiMap.get(k);
    pepper.sendNoteOff(nn, 0);
    displayString = "Key OFF " + k + "(" + nn +")";
    keystatus = 0;
  }
}

void mousePressed() {
  if (mouseButton == LEFT) {
    pepper.sendNoteOn(60, 127);
    mousestatus = 60;
    displayString = "Mouse(L) ON (60)";
  } else if (mouseButton == RIGHT) {
    pepper.sendNoteOn(61, 127);
    mousestatus = 61;
    displayString = "Mouse(R) ON (61)";
  }
}

void mouseReleased() {
  if (mousestatus == 60) {
    pepper.sendNoteOff(60, 0);
    mousestatus = 0;
    displayString = "Mouse(L) OFF (60)";
  } else if (mousestatus == 61) {
    pepper.sendNoteOff(61, 0);
    mousestatus = 0;
    displayString = "Mouse(R) OFF (61)";
  }
}


