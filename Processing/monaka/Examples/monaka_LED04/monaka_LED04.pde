//
// Pepper-monaka mode:1 test
//  Four digital out
//

import monaka.*;
import themidibus.*;

monaka pepper;

void setup() {
  size(400,400);
  background(0);
  
  pepper = new monaka(1);
}

void draw() {
  int i;
  if (mousePressed == true) {
    background(255);
    fill(0);
    if (mouseButton == LEFT) {
      for (i=0; i< 5; i++) {
        pepper.digitalOutput(i, 1);
        pepper.digitalOutput(mod4(i-1), 0);
        delay(100);
      }
    } else if (mouseButton == RIGHT) {
      for (i=4; i>= 0; i--) {
        pepper.digitalOutput(i, 1);
        pepper.digitalOutput(mod4(i+1), 0);
        delay(100);
      }    }
  } else {
    for (i=0 ; i < 4 ; i++) {
      pepper.digitalOutput(i, 0);
    }
  }
}

int mod4(int x) {
  return (x % 4);
}
