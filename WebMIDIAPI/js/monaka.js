//
// Web MIDI wrapper for monaka
// monaka.js for Chrome
// 
//  (c) 2015 by morecat_lab
//
// based on poormidi.js at https://github.com/tadfmac/poormidi
//

var monaka = function() {
  this.midi = null;
  this.input = null;
  this.output = null;
  this.timer = null;
  this.analogInputValues = [0,0,0,0];
  this.digitalInputValues = [0,0,0,0];
  this.analogInEvent = null;
  this.digitalInEvent = null;

  this.success = function(access){
    console.log("monaka.success()");
    this.midi = access;
    this.refreshPorts();
    this.midi.onstatechange = this.onStateChange;
  }.bind(this);

  this.failure = function(msg){
    console.log("monaka.failure(): "+msg);
  }.bind(this);

  this.setAnalogInEventCallback = function(func) {  // define a callback function for analog in
    this.analogInEvent = func;
  }

  this.setDigitalInEventCallback = function(func) {  // define a callback function for digital in
    this.digitalInEvent = func;
  }
  
  this.onMidiEvent = function(e){   // MIDIevent handler
    if (e.data[0] == 0xB0) {  // Control Change
      console.log("receive cc " + e.data[1] + " " + e.data[2]);
      if ((e.data[1] >= 22) && (e.data[1] <= 24)) {
        this.analogInputValues[e.data[1] - 22] = e.data[2];
        if (this.analogInEvent != null) {  // callback analog in
          this.analogInEvent(this.analogInputValues);
        }
      }
    } else if (e.data[0] == 0x90) { // note on
      console.log("receive noteOn " + e.data[1] + " " + e.data[2]);
      if ((e.data[1] >= 60) && (e.data[1] <= 63)) {
        this.digitalInputValues[e.data[1] - 60] = e.data[2];
        if (this.digitalInEvent != null) {  // callback digital in
          this.digitalInEvent(this.digitalInputValues);
        }
      }
    } else if (e.data[0] == 0x80) { // note off
      console.log("receive noteOff " + e.data[1] + " " + e.data[2]);
      if ((e.data[1] >= 60) && (e.data[1] <= 63)) {
        this.digitalInputValues[e.data[1] - 60] = 0;
        if (this.digitalInEvent != null) { // callback digital in
          this.digitalInEvent(this.digitalInputValues);
        }
      }
    }
  }.bind(this);

  this.digitalOutput = function(pin, val) {
    if (pin > 3) 
      return false;
    if (val == 0) {
      this.sendNoteOff(60 + pin, 0);
    } else {
      this.sendNoteOn(channel, 60 + pin, 127);
    }
    return true;
  }

  this.analogOutput = function(pin, val) {
    if (pin > 3) 
      return false;
    if (val > 127)
      return false;
    this.sendControllerChange(channel, 22 + pin, val);
    return true;
  }

  this.digitalInput = function(pin) {
    if (pin > 3)
      return false;
    return digitalInputValues[pin];
  }

  this.analogInput = function(pin) {
    if (pin > 3)
      return false;
    return analogInputValues[pin];
  }

  this.setHandler = function(func){
    console.log("monaka.setHandler()");
    this.onMidiEvent = func.bind(this);
  }.bind(this);

  this.sendNoteOn = function(note,velocity){
    if(this.output != null){
      console.log("monaka.sendNoteOn(" + note + "," + velocity + ") to :"+this.output.name);
      this.output.send([0x90,note&0x7f,velocity&0x7f]);
    }
  }.bind(this);

  this.onStateChange = function(){
    console.log("monaka.onStateChange()");
    if(this.timer != null){
      clearTimeout(this.timer);
    }
    this.timer = setTimeout(function(){
      this.refreshPorts();
      this.timer = null;
    }.bind(this),300);
  }.bind(this);

  this.sendNoteOff = function(note, velocity){
    if(this.output != null){
      console.log("monaka.sendNoteOff(" + note + "," + velocity + ") to :"+this.output.name);
      this.output.send([0x80,note,0]);
    }
  }.bind(this);

  this.sendControlChange = function(ccno,value){
    if(this.output != null){
      console.log("monaka.sendControlChange() output to :"+this.output.name);
      this.output.send([0xB0,ccno&0x7f,value&0x7f]);
    }
  }.bind(this);

  this.setMode = function(modeNo){
    console.log("monaka.setMode(" + modeNo + ")");
    if ((modeNo <= 0) || (modeNo > 8))
       return;  // mode error
    if (this.output != null)
      this.sendControlChange(122, modeNo);  // change mode of monaka
  }

  this.refreshPorts = function(){
    console.log("monaka.refreshPorts()");
    this.input = null;
    this.output = null;
    // input
    var it = this.midi.inputs.values();
    for(var o = it.next(); !o.done; o = it.next()){
      if (o.value.name == "PEPPER-Monaka") {
        this.input = o.value;  // only 1st pepper-monaka
        console.log("monaka.refreshPorts() input: "+this.input.name);
        this.input.onmidimessage = this.onMidiEvent;
        break;
      }
    }
    // output
    var ot = this.midi.outputs.values();
    for(var o = ot.next(); !o.done; o = ot.next()){
      if (o.value.name == "PEPPER-Monaka") {
        this.output =o.value;  // only 1st pepper-monaka
        console.log("monaka.refreshPorts() output: "+this.output.name);
        break;
      }
    }
  }.bind(this);

  this.onConnect = function(e){
    console.log("monaka.onConnect()");
  }
  this.onDisConnect = function(e){
    console.log("monaka.onDisConnect()");
  }

  navigator.requestMIDIAccess().then(this.success,this.failure);
};