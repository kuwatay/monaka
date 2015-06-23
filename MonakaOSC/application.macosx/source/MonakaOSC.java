import processing.core.*; 
import processing.xml.*; 

import guicomponents.*; 
import themidibus.*; 
import oscP5.*; 
import netP5.*; 

import java.applet.*; 
import java.awt.Dimension; 
import java.awt.Frame; 
import java.awt.event.MouseEvent; 
import java.awt.event.KeyEvent; 
import java.awt.event.FocusEvent; 
import java.awt.Image; 
import java.io.*; 
import java.net.*; 
import java.text.*; 
import java.util.*; 
import java.util.zip.*; 
import java.util.regex.*; 

public class MonakaOSC extends PApplet {

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

  // GUI library
 // the MIDI BUS library
  // OSC library
  // OSC library

boolean debug = true;

// for MIDI
MidiBus myBus;       // The MidiBus
int channel = 0;     // midi channel = 1 (fix)
int ccNumber = 122;  // local control

// for OSC 
OscP5 myOsc;
NetAddress myRemoteLocation;

/* should read from dialog box */
String hostIP = "127.0.0.1";
int port = 8000;
int listenPort = 8080;
String prefix = "/gome";

boolean serverRunning = false;

GPanel pnlControls;
GLabel label1, label2, label3;
GButton b1, b2, b3, b4, b5, b6;
GCombo cboDevices, cboConfigNo, cobConfigDevices;
GTextField oscHost, oscPrefix,oscPort, oscListenPort;

GWindow windConfig;
GPanel pnlControls2;

String midiOut[];

public void setup() {
   size(400,400);
   smooth();

   // for MIDI side
   if (debug) {
     MidiBus.list();
   }

   // add label
   G4P.setColorScheme(this, GCScheme.GREY_SCHEME);
   G4P.setFont(this, "Verdana", 14);
   G4P.messagesEnabled(false);

   pnlControls = new GPanel(this,"Main Panel",0,0,400,400);
   pnlControls.setFont("Verdana", 7);
   pnlControls.setOpaque(true);
   pnlControls.setCollapsed(false);

   label1 = new GLabel(this, "Monaka-OSC by morecat_lab", 20, 5, 360, 30);
   label1.setBorder(0);
   label1.setFont("Verdana", 20);
   label1.setTextAlign(LEFT);
   label1.setOpaque(true);

   pnlControls.add(label1);

   label2 = new GLabel(this, "OSC Host Address", 10, 60, 150, 20);
   label2.setBorder(0);
   label2.setFont("Verdana", 14);
   pnlControls.add(label2);
   oscHost = new GTextField(this, hostIP, 160, 60, 220, 20);
   pnlControls.add(oscHost);

   label2 = new GLabel(this, "OSC Host Port", 10, 90, 150, 20);
   label2.setBorder(0);
   label2.setFont("Verdana", 14);
   pnlControls.add(label2);
   oscPort = new GTextField(this, "" + port, 160, 90, 220, 20);
   pnlControls.add(oscPort);

   label2 = new GLabel(this, "OSC Listen Port", 10, 120, 150, 20);
   label2.setBorder(0);
   label2.setFont("Verdana", 14);
   pnlControls.add(label2);
   oscListenPort = new GTextField(this, "" + listenPort, 160, 120, 220, 20);
   pnlControls.add(oscListenPort);

   label2 = new GLabel(this, "OSC Prefix", 10, 150, 150, 20);
   label2.setBorder(0);
   label2.setFont("Verdana", 14);
   pnlControls.add(label2);
   oscPrefix = new GTextField(this, prefix, 160, 150, 220, 20);
   pnlControls.add(oscPrefix);

   label2 = new GLabel(this, "MIDI Device", 10, 180, 150, 20);
   label2.setBorder(0);
   label2.setFont("Verdana", 14);
   pnlControls.add(label2);

   // list MIDI device (only output device)
   midiOut = MidiBus.availableOutputs();
   cboDevices = new GCombo(this, midiOut, 4, 160, 180, 220);
   cboDevices.setSelected(0);
   cboDevices.setFont("Verdana", 14);
   pnlControls.add(cboDevices);

   b1= new GButton(this, "START", 20,360,80,20);
   b1.setBorder(1);
   b1.setFont("Verdana", 14);
   pnlControls.add(b1);

   b2= new GButton(this, "NOTEOFF", 110,360,80,20);
   b2.setBorder(1);
   b2.setFont("Verdana", 14);
   pnlControls.add(b2);

   b3= new GButton(this, "CONFIG", 200,360,80,20);
   b3.setBorder(1);
   b3.setFont("Verdana", 14);
   pnlControls.add(b3);

   b4= new GButton(this, "QUIT", 290,360,80,20);
   b4.setBorder(1);
   b4.setFont("Verdana", 14);
   pnlControls.add(b4);

}

public void createConfigWindow(){
   if (windConfig != null) {
     windConfig.setVisible(true); // reuse windConfig, instead of create new one.
     return;
   }
   windConfig = new GWindow(this, "Controls",600,400, 400, 400, false, null);
   windConfig.setBackground(0xc6c3c6); // set grey

   label1 = new GLabel(this, "Monaka Configurator", 20, 5, 360, 30);
   label1.setBorder(0);
   label1.setFont("Verdana", 20);
   label1.setTextAlign(LEFT);
   label1.setOpaque(true);
   windConfig.add(label1);

   label1 = new GLabel(this, "Target Monaka", 10, 60, 150, 20);
   label1.setBorder(0);
   label1.setFont("Verdana", 14);
   label1.setTextAlign(LEFT);
   label1.setOpaque(true);
   windConfig.add(label1);

   cobConfigDevices  = new GCombo(this, midiOut, 4, 160, 60, 220);
   cobConfigDevices.setSelected(0);
   cobConfigDevices.setFont("Verdana", 14);
   windConfig.add(cobConfigDevices);

   label1 = new GLabel(this, "Change Mode to", 10, 200, 150, 20);
   label1.setBorder(0);
   label1.setFont("Verdana", 14);
   label1.setTextAlign(LEFT);
   label1.setOpaque(true);
   windConfig.add(label1);
 
   String[] configName = new String [] {
   "Config 1", "Config 2", "Config 3", "Config 4", "Config 5",
   "Config 6", "Config 7", "Config 8", "Config 9" }; 
   cboConfigNo = new GCombo(this, configName, 4, 160, 200, 220);
   cboConfigNo.setSelected(0);
   cboConfigNo.setFont("Verdana", 14);
   windConfig.add(cboConfigNo);

   b5= new GButton(this, "CHANGE", 50,360,80,20);
   b5.setBorder(1);
   b5.setFont("Verdana", 14);
   windConfig.add(b5);

   b6= new GButton(this, "CLOSE", 250,360,80,20);
   b6.setBorder(1);
   b6.setFont("Verdana", 14);
   windConfig.add(b6);

}

public void handleButtonEvents(GButton button) {
  if (button.eventType == GButton.CLICKED) {
    if (button == b1) {
      if (! serverRunning) { // server start
         // open MIDI 
         if (debug) {
           println("Opening " + midiOut[cboDevices.selectedIndex()] + "for Input / Output ");
         }
         myBus = new MidiBus(this,  cboDevices.selectedIndex(),  cboDevices.selectedIndex());

         // start OSC server
         port = (new Integer(oscPort.getText())).intValue();
         listenPort = (new Integer(oscListenPort.getText())).intValue();
         prefix = oscPrefix.getText();
         myOsc = new OscP5(this, listenPort);
         myRemoteLocation = new NetAddress(oscHost.getText(), port); 

         // maintain button status
         b1.setText("STOP");
         b1.setColorScheme(GCScheme.RED_SCHEME);
         serverRunning = true;
      } else { // server stop
         myOsc.stop();  // stop server
         myBus.stop(); // close midibus

         // maintain button status
         b1.setText("START");
         b1.setColorScheme(GCScheme.GREY_SCHEME);
         serverRunning = false;
      }
    } else if (button == b2) { // send 'all note off'
      if (! serverRunning) {
        if (debug) {
          println("send CC message 'ALL NOTE OFF' to " + midiOut[cboDevices.selectedIndex()] );
        }
        myBus = new MidiBus(this, 0, cboDevices.selectedIndex());
        myBus.sendControllerChange(channel, 120, 0); // Send a controllerChange
      }
    } else if (button == b3) { // config
      createConfigWindow();
    } else if (button == b4) { // quit
       exit();
    } else if (button == b5) { // change monaka in sub-window
      if (! serverRunning) { // only when server is off 
         // send CCC message to monaka
        if (debug) {
          println("send CC message to " + midiOut[cobConfigDevices.selectedIndex()] + " to switch mode " + (cboConfigNo.selectedIndex() + 1));
        }
        myBus = new MidiBus(this, 0, cobConfigDevices.selectedIndex());
        myBus.sendControllerChange(channel, ccNumber, cboConfigNo.selectedIndex() + 1); // Send a controllerChange
      }
    } else if (button == b6) { // close sub-window
      windConfig.setVisible(false);
    }
  }
}	


public void draw() {
  //  pushMatrix();
  // background(imgBgImage);
  //  popMatrix();
}


// Receive a noteOn
public void noteOn(int channel, int pitch, int velocity) {
  if (debug) {
     println("Note On :" +" Channel:"+channel+" Pitch:"+pitch+" Velocity:"+velocity);
  }
  sendNote(pitch, 1);
}

// Receive a noteOff
public void noteOff(int channel, int pitch, int velocity) {
  if (debug) {
    println("Note Off:" +" Channel:"+channel+" Pitch:"+pitch+" Velocity:"+velocity);
  }
  sendNote(pitch, 0);
}

public void sendNote(int pitch, int state) {
  OscMessage myMessage = new OscMessage(prefix + "/press");
  int x = pitch % 8;
  int y = pitch / 8;
  myMessage.add(x); /* add an int to the osc message */
  myMessage.add(y); /* add an int to the osc message */
  myMessage.add(state);
  if (debug) {
    print("### sent an osc message " + prefix + "/press with typetag iii.");
    println(" values: "+ x + " , " + y + ", " + state);
  // println("### Send an osc message: " + myMessage.toString());
  // myMessage.print();
  }
  myOsc.send(myMessage, myRemoteLocation); 
}  

// Receive a controllerChange
public void controllerChange(int channel, int number, int value) {
  if (debug) {
    println("Control Change:" +" Channel:"+channel+" Number:"+number+" Value:"+value);
  }
  // do useful stuff here
}

public void oscEvent(OscMessage theOscMessage) {
  // println("### received an osc message: " + theOscMessage.toString());
  // theOscMessage.print();

  /* check if theOscMessage has the address pattern we are looking for. */  
  if(theOscMessage.checkAddrPattern(prefix + "/led")==true) {
    /* check if the typetag is the right one. */
    if(theOscMessage.checkTypetag("iii")) {
      /* parse theOscMessage and extract the values from the osc message arguments. */
      int x = theOscMessage.get(0).intValue();  
      int y = theOscMessage.get(1).intValue();
      int state = theOscMessage.get(2).intValue();
      if (debug) {
        print("### received an osc message " + prefix + "/led with typetag iii.");
        println(" values: "+x+", "+y+", "+state);
      }
      if (state == 1) {
        myBus.sendNoteOn(channel, x + (y * 8), 1);
      } else {
        myBus.sendNoteOn(channel, x + (y * 8), 0);
      }
      return;
    }  
  } 
}



// EOF



  static public void main(String args[]) {
    PApplet.main(new String[] { "--bgcolor=#c0c0c0", "MonakaOSC" });
  }
}
