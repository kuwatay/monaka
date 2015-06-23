/**
  * MIDI-OSC bridge for Monaka
  * Copyright (C) 2010 by Yoshitaka Kuwata
  *  morecat_lab
  *  2010/05/04
  *
  * REQUIRED PACKAGE for processing 1.1
  * - theMidiBus : http://smallbutdigital.com/themidibus.php
  * - G4P  : http://www.lagers.org.uk/g4p/index.html
  * - oscP5 : handling for OSC messages
  */

import guicomponents.*;  // GUI library
import themidibus.*; // the MIDI BUS library
import oscP5.*;
import netP5.*;

// for MIDI
MidiBus myBus;       // The MidiBus
int channel = 0;     // midi channel = 1 (fix)
int ccNumber = 122;  // local control

// for OSC 
OscP5 oscP5;
NetAddress myRemoteLocation;

/* should read from dialog box */
int port = 8080;
String prefix = "/blinken";

GPanel pnlControls;
GLabel label1, label2, label3;
GButton b1,b2,b3,b4;
GCombo cboDevices, cboConfigNo;
GTextField oscPrefix,oscPort;

GWindow windConfig;
GPanel pnlControls2;

String midiOut[];

void setup() {
   size(400,400);
   smooth();

   // for MIDI side
   MidiBus.list();

   // for OSC side
   oscP5 = new OscP5(this,port);
   myRemoteLocation = new NetAddress("127.0.0.1",port);

   // add label
   G4P.setColorScheme(this, GCScheme.GREY_SCHEME);
   G4P.setFont(this, "Verdana", 14);
   G4P.messagesEnabled(false);

   pnlControls = new GPanel(this,"Main Panel",0,0,400,400);
   pnlControls.setOpaque(true);
   pnlControls.setCollapsed(false);


   label1 = new GLabel(this, "Monaka-OSC by mrorecat_lab", 20, 5, 360, 30);
   label1.setBorder(0);
   label1.setFont("Verdana", 20);
   label1.setTextAlign(LEFT);
   label1.setOpaque(true);

   pnlControls.add(label1);

   label2 = new GLabel(this, "OSC Port Number", 10, 60, 150, 20);
   label2.setBorder(0);
   label2.setFont("Verdana", 14);
   pnlControls.add(label2);

   oscPort = new GTextField(this, "" + port, 160, 60, 220, 20);
   pnlControls.add(oscPort);

   label2 = new GLabel(this, "OSC Prefix", 10, 90, 150, 20);
   label2.setBorder(0);
   label2.setFont("Verdana", 14);
   pnlControls.add(label2);

   oscPrefix = new GTextField(this, prefix, 160, 90, 220, 20);
   pnlControls.add(oscPrefix);

   label2 = new GLabel(this, "MIDI Device", 10, 120, 150, 20);
   label2.setBorder(0);
   label2.setFont("Verdana", 14);
   pnlControls.add(label2);

   // list MIDI device (only output device)
   midiOut = MidiBus.availableOutputs();
   String[] devName = new String[midiOut.length]; 
   for (int i = 0 ; i < midiOut.length; i++) {
     // devName[i] = (midiOut[i].length() >= 22) ? midiOut[i].substring(0,22) : midiOut[i];
     devName[i] = midiOut[i];
   }
   cboDevices = new GCombo(this, devName, 4, 160, 120, 220);
   cboDevices.setSelected(0);
   cboDevices.setFont("Verdana", 14);
   pnlControls.add(cboDevices);

   b1= new GButton(this, "START", 20,230,80,20);
   b1.setBorder(1);
   b1.setFont("Verdana", 14);
   pnlControls.add(b1);

   b2= new GButton(this, "STOP", 110,230,80,20);
   b2.setBorder(1);
   b2.setFont("Verdana", 14);
   pnlControls.add(b2);

   b3= new GButton(this, "CONF", 200,230,80,20);
   b3.setBorder(1);
   b3.setFont("Verdana", 14);
   pnlControls.add(b3);

   b4= new GButton(this, "QUIT", 290,230,80,20);
   b4.setBorder(1);
   b4.setFont("Verdana", 14);
   pnlControls.add(b4);

}

public void createConfigWindow(){
   windConfig = new GWindow(this, "Controls",600,400, 400, 400, false, null);
//   windConfig.setColorScheme(this, GCScheme.GREY_SCHEME);
//   windConfig.setFont(this, "Verdana", 14);
//   windConfig.messagesEnabled(false);

   label1 = new GLabel(this, "Monaka Configrator", 20, 5, 360, 30);
   label1.setBorder(0);
   label1.setFont("Verdana", 20);
   label1.setTextAlign(LEFT);
   label1.setOpaque(true);
 
   windConfig.add(label1);
   // windConfig.setExitBehaviour(GWindow.SHUTDOWN_ON_EXIT);

//   label3 = new GLabel(this, "Change Config to", 230, 220, 200, 30);
//   label3.setBorder(0);
//   label3.setFont("Verdana", 14);
//   pnlControls.add(label3);

//  String[] configName = new String [] {
//  "Config 1", "Config 2", "Config 3", "Config 4", "Config 5",
//  "Config 6", "Config 7", "Config 8", "Config 9" }; 
//  cboConfigNo = new GCombo(this, configName, 4, 210, 260, 180);
//  cboConfigNo.setSelected(0);
//  cboConfigNo.setFont("Verdana", 14);
//  pnlControls.add(cboConfigNo);

  b1= new GButton(this, "CHANGE", 50,360,80,20);
  b1.setBorder(1);
  b1.setFont("Verdana", 14);
  windConfig.add(b1);

  b2= new GButton(this, "CLOSE", 250,360,80,20);
  b2.setBorder(1);
  b2.setFont("Verdana", 14);
  windConfig.add(b2);

}

public void handleComboEvents(GCombo combo){
  if(cboConfigNo == combo){
 //    lblAction.setText("Color changed to " + cboColor.selectedText());
  } else if (cboDevices == combo) {
  }
}

void handleButtonEvents(GButton button) {
//   print(button.getText()+"\t\t");
  switch(button.eventType){
//  case GButton.PRESSED:
//    System.out.println("PRESSED");
//    break;
//  case GButton.RELEASED:
//    System.out.println("RELEASED");
//    break;
  case GButton.CLICKED:
//    System.out.println("CLICKED");
    if (button == b1) {
       // send CCC message to monaka
      println("send CC message to " + midiOut[cboDevices.selectedIndex()] + " to switch mode " + cboConfigNo.selectedIndex());
      myBus = new MidiBus(this, 0, cboDevices.selectedIndex());
      myBus.sendControllerChange(channel, ccNumber, cboConfigNo.selectedIndex() + 1); // Send a controllerChange
    } else if (button == b3) {
      createConfigWindow();
    } else if (button == b4) {
       exit();
    }
    break;
//  default:
//    println("Unknown mouse event");
  }
}	

public void handleOptionEvents(GOption selected, GOption deselected){
}

public void handleSliderEvents(GSlider slider){
}
void draw() {
//  pushMatrix();
  // background(imgBgImage);
//  popMatrix();
}


//public void controlEvent(ControlEvent theEvent) {
//  int channel = 0;
//  int ccNumber = 122; // local control
//   if (theEvent.isController() ) {
//     // println("(CONTROLLER) " + theEvent.controller().name() + " " + theEvent.controller().value());
//     if (theEvent.controller() == b1) {
//       // send CCC message to monaka
//       println("send CC message to " + midiOut[(int)r1.value() -1] + " to switch mode " + r2.value());
//       myBus = new MidiBus(this, 0, (int)r1.value() - 1);
//       myBus.sendControllerChange(channel, ccNumber, (int)r2.value()); // Send a controllerChange
//     } else if (theEvent.controller() == b2) {
//        exit();
//     }
//   } else if (theEvent.isGroup()) {
//     // println("(GROUP) " + theEvent.group().name() + " " +  theEvent.group().value());
//   }
//}
//

void oscEvent(OscMessage theOscMessage) {
  println("### received an osc message. with address pattern "+theOscMessage.addrPattern());

  /* check if theOscMessage has the address pattern we are looking for. */  
  if(theOscMessage.checkAddrPattern(prefix + "/led")==true) {
    println("##### address pattern match"+theOscMessage.addrPattern() + "type " + theOscMessage.typetag());
    /* check if the typetag is the right one. */
    if(theOscMessage.checkTypetag("iii")) {
      /* parse theOscMessage and extract the values from the osc message arguments. */
      int firstValue = theOscMessage.get(0).intValue();  
      int secondValue = theOscMessage.get(1).intValue();
      int thirdValue = theOscMessage.get(2).intValue();
      print("### received an osc message /blinken/led with typetag iii.");
      println(" values: "+firstValue+", "+secondValue+", "+thirdValue);
      return;
    }  
  } 
}

// EOF


