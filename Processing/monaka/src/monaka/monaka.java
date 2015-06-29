/*
 *  monaka package for Processing
 *
 *     version 0.9
 *      2015/06/23
 *
 *  Copyright (c) 2015 by Yoshitaka Kuwata
 *  Copyright (c) 2015 by morecat_lab
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as published
 *   by the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Library General Public License for more details.
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

package monaka;

import themidibus.*;
import processing.core.*;


/**
 * Monaka class provides wrapper for Processing sketches.
 * <p>
 * <h4>Usage of Monaka </h4>
 * <p>
 * Constract a monaka object with mode value {@link #monaka(int mode)}, then use IO functions such as {@link #digitalOutput(int pin, int val)}, {@link #analogOutput(int pin, int val)}, {@link #digitalInput(int pin)}, and {@link #analogInput(int pin)}.
 * Note that this package depends on theMidiBus. (http://www.smallbutdigital.com/themidibus.php)
 * theMidiBus package is required to use Monaka on Processing.
 *
 * @version 009
 * @author Yoshitaka Kuwata, morecat_lab
 */

public class monaka {
    private MidiBus monakaDev;
    private final int defaultMode = 1;
    private final int modeCC = 122;
    private final String defaultName = "PEPPER-Monaka"; // only support pepper
    private final int channel = 0;			// use channel 1 only

    private int[] digitalInputValues = {0,0,0,0};
    private int[] analogInputValues  = {0,0,0,0};

    private final boolean debug   = true;  // enable debug message
    private final boolean debugCB = false; // disable debug message of callback
    /**
     * current setting of monaka mode
     */
    public int mode = defaultMode;

    /**
     * Constructs a new monaka device, and set to the default mode.
     *
     * @see #setMode(int mode)
     * @see #listMidiBus()
     */
    public monaka() {
	listMidiBus();
	monakaDev = new MidiBus(this, defaultName, defaultName);
	setMode(mode = defaultMode);
    }

    /**
     * Constructs a new monaka device, and set to the mode.
     *
     * @param mode monaka mode
     * @see #setMode(int mode)
     * @see #listMidiBus()
     */
    public monaka(int mode) {
	listMidiBus();
	monakaDev = new MidiBus(this, defaultName, defaultName);
	setMode(this.mode = mode);
    }

    /**
     * List all installed MIDI devices. The index, name and type (input/output/unavailable) of each devices will be indicated. 
     *
     */
    public void listMidiBus() {
	if (debug) {
	    MidiBus.list();
	}
    }

    /**
     * set mode of monaka Device
     *
     * @param mode monaka mode
     */
    public boolean setMode(int mode) {
	if ((mode == 0) || (mode > 8)) {
	    return false;	// mode error
	}
	monakaDev.sendControllerChange(channel, modeCC, mode);
	return true;
    }

    /**
     * Output Digital value
     *
     * @param pin pin number, 0 to 4, depend on mode
     * @param val value, 0 (low) or 1 (high)
     * @return true, if digitalOut is succeeded. Otherwise false.
     * @see #setMode(int mode)
     * @see #analogOutput(int pin, int val)
     */
    public boolean digitalOutput(int pin, int val) {
	if (pin > 3) {
	    return false;
	}
	if (val == 0) {
	    monakaDev.sendNoteOff(channel, 60 + pin, 0);
	} else {
	    monakaDev.sendNoteOn(channel, 60 + pin, 127);
	}
	return true;
    }

    /**
     * Output Analog value
     *
     * @param pin pin number, 0 to 3, depend on mode
     * @param val value, 0 (to 127)
     * @return true, if analogOut is succeeded. Otherwise false.
     * @see #setMode(int mode)
     * @see #digitalOutput(int pin, int val)
     */
    public boolean analogOutput(int pin, int val) {
	if (pin > 3) {
	    return false;
	}
	if (val > 127) {
	    if (debug) {
		System.err.println("\nWarning: Bad value in analogOut["+ val +"]");
	    }
	    return false;
	}
	monakaDev.sendControllerChange(channel, 22 + pin, val);
	return true;
    }

    /**
     * Input Digital value
     *
     * @param pin pin number, 0 to 3, depend on mode
     * @return 0 or 1
     * @see #setMode(int mode)
     * @see #analogInput(int pin)
     */
    public int digitalInput(int pin) {
	if (pin > 3) {
	    return 0;
	}
	return digitalInputValues[pin];
    }

    /**
     * Input Analog value
     *
     * @param pin pin number, 0 to 3, depend on mode
     * @return 0 to 127
     * @see #setMode(int mode)
     * @see #digitalInput(int pin)
     */
    public int analogInput(int pin) {
	if (pin > 3) {
	    return 0;
	}
	return analogInputValues[pin];
    }

    /**
     * Direct send Note On
     *
     * @param noteNo note number
     * @param velocity velocity value
     * @see #setMode(int mode)
     */
    public void sendNoteOn(int noteNo, int velocity) {
	monakaDev.sendNoteOn(channel, noteNo, velocity);
    }

    /**
     * Direct send Note Off
     *
     * @param noteNo note number
     * @param velocity velocity value
     * @see #setMode(int mode)
     */
    public void sendNoteOff(int noteNo, int velocity) {
	monakaDev.sendNoteOff(channel, noteNo, velocity);
    }

    /**
     * Call Back function for noteOn. Users should not use this function.
     *
     * @param channel MIDI channel number. Monaka use only channel 1 (value =0)
     * @param pitch pitch number
     * @param velocity velocity value
     * @see #setMode(int mode)
     */
    public void noteOn(int channel, int pitch, int velocity) {
	if (debugCB) {
	    System.err.println("\nNoteOn:["+ channel +"][" + pitch + "][" + velocity + "]");
	}
	if (channel == this.channel) {
	    if ((pitch >= 60) && (pitch <= 63)) {
		digitalInputValues[pitch - 60] = velocity;
	    }
	}
    }

    /**
     * Call Back function for noteOn. Users should not use this function.
     *
     * @param channel MIDI channel number. Monaka use only channel 1 (value =0)
     * @param pitch pitch number
     * @param velocity velocity value
     * @see #setMode(int mode)
     */
    public void noteOff(int channel, int pitch, int velocity) {
	if (debugCB) {
	    System.err.println("\nNoteOff:["+ channel +"][" + pitch + "][" + velocity + "]");
	}
	if (channel == this.channel) {
	    if ((pitch >= 60) && (pitch <= 63)) {
		digitalInputValues[pitch - 60] = 0;
	    }
	}
    }

    /**
     * Call Back function for controllerChange. Users should not use this function.
     *
     * @param channel MIDI channel number. Monaka use only channel 1 (value =0)
     * @param number Controller number
     * @param value value for controller
     * @see #setMode(int mode)
     */
    public void controllerChange(int channel, int number, int value) {
	if (debugCB) {
	    System.err.println("\nControllerChange:["+ channel +"][" + number + "][" + value + "]");
	}
	if (channel == this.channel) {
	    if ((number >= 22) && (number <= 24)) {
		analogInputValues[number - 22] = value;
	    }
	}
    }
}
