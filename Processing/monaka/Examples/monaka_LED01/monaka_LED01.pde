//
// Pepper-monaka mode:1 test
//  Four digital out
//

import monaka.*;
import themidibus.*;  // required !!

monaka pepper;

void setup() {
  size(400,400);
  background(0);
  
  pepper = new monaka(1);
}

void draw() {
  
  if (mousePressed == true) {
    fill(0);
    pepper.digitalOutput(0, 1);
  } else {
    fill(255);
    pepper.digitalOutput(0, 0);
  }
}

