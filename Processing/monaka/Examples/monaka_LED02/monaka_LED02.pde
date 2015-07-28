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
  if (mousePressed == true) {
    background(255);
    fill(0);
    if (mouseButton == LEFT)
      pepper.digitalOutput(0, 1);
    if (mouseButton == RIGHT) 
      pepper.digitalOutput(1, 1);
  } else {
    background(0);
    fill(255);
    if (mouseButton == LEFT)
      pepper.digitalOutput(0, 0);
    if (mouseButton == RIGHT) 
      pepper.digitalOutput(1, 0);
  }
}

