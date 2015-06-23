/* Name: main.c
 *
 * Project: Monaka: AVR MIDI device on Low-Speed USB
 * Author: Yoshitaka Kuwata
 * Creation Date: 2009-05-05
 * Copyright: (c) 2009,10 by Morecat lab.
 *
 * Based on : Martin Homuth-Rosemann
 * Copyright: (c) 2008 by Martin Homuth-Rosemann.
 * License: GPL v2 or later.
 *
 */

#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <avr/wdt.h>
#include <util/delay.h>

#include "usbdrv.h"
#include "oddebug.h"

#include "monaka.h"


/* ----------------------------------------------- */
/* グローバル変数 */

static uchar conf;		/* mode */
static uchar midi_clock_ratio;	/* MIDI clock wait */
static uchar midi_clock_wait = 1; /* MIDI clock wait */
static uchar midi_clock_control;  /* FLAG: control midi clock by start/stop */
static uchar midi_clock_trig_count;
static uchar midi_clock_out = 1; /* FLAG: need to output clock  */

static uint16_t midi_clock_noclock = TRIG_RESET_INTERVAL; /* counter to reset midi_clock_wait */

static uint16_t servo_wait = SERVO_POLLING_WAIT_LOOP; /* counter for servo timing */

#ifdef GINGER
#include "ginger.c"
#endif

#ifdef PEPPER
#include "pepper.c"
#endif

#define POLL_ANALOG_STOP 0
#define POLL_ANALOG_WAIT 1
#define POLL_ANALOG_CONT 2

#define CONVERTING_CH_NONE (-1)

// uchar pollMode = POLL_ANALOG_STOP;
uchar pollMode = POLL_ANALOG_WAIT;
char convertingCh = 0;	/* 変換中(変換中のチャネル番号） */

uchar adVals[4];		/* ADの最大チャンネル数でバッファを確保すること */

#include "freq_table.c"


/* EEPROM上の変数の定義 */
static uchar __attribute__((section(".eeprom"))) e_conf = INITIAL_CONFIG;
static uchar __attribute__((section(".eeprom"))) e_midi_clock_control = INITIAL_MIDI_CLOCK_CONTROL;

// This deviceDescrMIDI[] is based on 
// http://www.usb.org/developers/devclass_docs/midi10.pdf
// Appendix B. Example: Simple MIDI Adapter (Informative)

// B.1 Device Descriptor
static PROGMEM char deviceDescrMIDI[] = {	/* USB device descriptor */
  18,			/* sizeof(usbDescriptorDevice): length of descriptor in bytes */
  USBDESCR_DEVICE,	/* descriptor type */
  0x10, 0x01,		/* USB version supported */
  0,			/* device class: defined at interface level */
  0,			/* subclass */
  0,			/* protocol */
  8,			/* max packet size */
  USB_CFG_VENDOR_ID,	/* 2 bytes */
  USB_CFG_DEVICE_ID,	/* 2 bytes */
  USB_CFG_DEVICE_VERSION,	/* 2 bytes */
  1,			/* manufacturer string index */
  2,			/* product string index */
  0,			/* serial number string index */
  1,			/* number of configurations */
};

// B.2 Configuration Descriptor
static PROGMEM char configDescrMIDI[] = { /* USB configuration descriptor */
  9,	   /* sizeof(usbDescrConfig): length of descriptor in bytes */
  USBDESCR_CONFIG,		/* descriptor type */
  101, 0,			/* total length of data returned (including inlined descriptors) */
  2,		      /* number of interfaces in this configuration */
  1,				/* index of this configuration */
  0,				/* configuration name string index */
#if USB_CFG_IS_SELF_POWERED
  USBATTR_SELFPOWER,		/* attributes */
#else
    0x80, /* USBATTR_BUSPOWER,   attributes */
#endif
  USB_CFG_MAX_BUS_POWER / 2,	/* max USB current in 2mA units */

// B.3 AudioControl Interface Descriptors
// The AudioControl interface describes the device structure (audio function topology) 
// and is used to manipulate the Audio Controls. This device has no audio function 
// incorporated. However, the AudioControl interface is mandatory and therefore both 
// the standard AC interface descriptor and the classspecific AC interface descriptor 
// must be present. The class-specific AC interface descriptor only contains the header 
// descriptor.

// B.3.1 Standard AC Interface Descriptor
// The AudioControl interface has no dedicated endpoints associated with it. It uses the 
// default pipe (endpoint 0) for all communication purposes. Class-specific AudioControl 
// Requests are sent using the default pipe. There is no Status Interrupt endpoint provided.
  /* descriptor follows inline: */
  9,			/* sizeof(usbDescrInterface): length of descriptor in bytes */
  USBDESCR_INTERFACE,	/* descriptor type */
  0,			/* index of this interface */
  0,			/* alternate setting for this interface */
  0,			/* endpoints excl 0: number of endpoint descriptors to follow */
  1,			/* */
  1,			/* */
  0,			/* */
  0,			/* string index for interface */

// B.3.2 Class-specific AC Interface Descriptor
// The Class-specific AC interface descriptor is always headed by a Header descriptor 
// that contains general information about the AudioControl interface. It contains all 
// the pointers needed to describe the Audio Interface Collection, associated with the 
// described audio function. Only the Header descriptor is present in this device 
// because it does not contain any audio functionality as such.
  /* descriptor follows inline: */
  9,			/* sizeof(usbDescrCDC_HeaderFn): length of descriptor in bytes */
  36,			/* descriptor type */
  1,			/* header functional descriptor */
  0x0, 0x01,		/* bcdADC */
  9, 0,			/* wTotalLength */
  1,			/* */
  1,			/* */

// B.4 MIDIStreaming Interface Descriptors

// B.4.1 Standard MS Interface Descriptor
  /* descriptor follows inline: */
  9,			/* length of descriptor in bytes */
  USBDESCR_INTERFACE,	/* descriptor type */
  1,			/* index of this interface */
  0,			/* alternate setting for this interface */
  2,			/* endpoints excl 0: number of endpoint descriptors to follow */
  1,			/* AUDIO */
  3,			/* MS */
  0,			/* unused */
  0,			/* string index for interface */

// B.4.2 Class-specific MS Interface Descriptor
  /* descriptor follows inline: */
  7,			/* length of descriptor in bytes */
  36,			/* descriptor type */
  1,			/* header functional descriptor */
  0x0, 0x01,		/* bcdADC */
  65, 0,			/* wTotalLength */

// B.4.3 MIDI IN Jack Descriptor
  /* descriptor follows inline: */
  6,			/* bLength */
  36,			/* descriptor type */
  2,			/* MIDI_IN_JACK desc subtype */
  1,			/* EMBEDDED bJackType */
  1,			/* bJackID */
  0,			/* iJack */

  /* descriptor follows inline: */
  6,			/* bLength */
  36,			/* descriptor type */
  2,			/* MIDI_IN_JACK desc subtype */
  2,			/* EXTERNAL bJackType */
  2,			/* bJackID */
  0,			/* iJack */

//B.4.4 MIDI OUT Jack Descriptor
  /* descriptor follows inline: */
  9,			/* length of descriptor in bytes */
  36,			/* descriptor type */
  3,			/* MIDI_OUT_JACK descriptor */
  1,			/* EMBEDDED bJackType */
  3,			/* bJackID */
  1,			/* No of input pins */
  2,			/* BaSourceID */
  1,			/* BaSourcePin */
  0,			/* iJack */

  /* descriptor follows inline: */
  9,			/* bLength of descriptor in bytes */
  36,			/* bDescriptorType */
  3,			/* MIDI_OUT_JACK bDescriptorSubtype */
  2,			/* EXTERNAL bJackType */
  4,			/* bJackID */
  1,			/* bNrInputPins */
  1,			/* baSourceID (0) */
  1,			/* baSourcePin (0) */
  0,			/* iJack */


// B.5 Bulk OUT Endpoint Descriptors

//B.5.1 Standard Bulk OUT Endpoint Descriptor
  /* descriptor follows inline: */
  9,			/* bLenght */
  USBDESCR_ENDPOINT,	/* bDescriptorType = endpoint */
  0x1,			/* bEndpointAddress OUT endpoint number 1 */
  3,			/* bmAttributes: 2:Bulk, 3:Interrupt endpoint */
  8, 0,			/* wMaxPacketSize */
  10,			/* bIntervall in ms */
  0,			/* bRefresh */
  0,			/* bSyncAddress */

// B.5.2 Class-specific MS Bulk OUT Endpoint Descriptor
  /* descriptor follows inline: */
  5,			/* bLength of descriptor in bytes */
  37,			/* bDescriptorType */
  1,			/* bDescriptorSubtype */
  1,			/* bNumEmbMIDIJack  */
  1,			/* baAssocJackID (0) */


//B.6 Bulk IN Endpoint Descriptors

//B.6.1 Standard Bulk IN Endpoint Descriptor
  /* descriptor follows inline: */
  9,			/* bLenght */
  USBDESCR_ENDPOINT,	/* bDescriptorType = endpoint */
  0x81,			/* bEndpointAddress IN endpoint number 1 */
  3,			/* bmAttributes: 2: Bulk, 3: Interrupt endpoint */
  8, 0,			/* wMaxPacketSize */
  10,			/* bIntervall in ms */
  0,			/* bRefresh */
  0,			/* bSyncAddress */

// B.6.2 Class-specific MS Bulk IN Endpoint Descriptor
  /* descriptor follows inline: */
  5,			/* bLength of descriptor in bytes */
  37,			/* bDescriptorType */
  1,			/* bDescriptorSubtype */
  1,			/* bNumEmbMIDIJack (0) */
  3,			/* baAssocJackID (0) */
};


uchar usbFunctionDescriptor(usbRequest_t * rq) {
  if (rq->wValue.bytes[1] == USBDESCR_DEVICE) {
    usbMsgPtr = (uchar *) deviceDescrMIDI;
    return sizeof(deviceDescrMIDI);
  } else {		/* must be config descriptor */
    usbMsgPtr = (uchar *) configDescrMIDI;
    return sizeof(configDescrMIDI);
  }
}


static uchar sendEmptyFrame;

/* ------------------------------------------------------------------------- */
/* ----------------------------- USB interface ----------------------------- */
/* ------------------------------------------------------------------------- */

uchar usbFunctionSetup(uchar data[8]) {
  usbRequest_t *rq = (void *) data;

  if ((rq->bmRequestType & USBRQ_TYPE_MASK) == USBRQ_TYPE_CLASS) {	/* class request type */

    /*  Prepare bulk-in endpoint to respond to early termination   */
    if ((rq->bmRequestType & USBRQ_DIR_MASK) ==
	USBRQ_DIR_HOST_TO_DEVICE)
      sendEmptyFrame = 1;
  }
  
  return 0xff;
}


/*---------------------------------------------------------------------------*/
/* usbFunctionRead                                                           */
/*---------------------------------------------------------------------------*/

uchar usbFunctionRead(uchar * data, uchar len) {
	data[0] = 0;
	data[1] = 0;
	data[2] = 0;
	data[3] = 0;
	data[4] = 0;
	data[5] = 0;
	data[6] = 0;

	return 7;
}


/*---------------------------------------------------------------------------*/
/* usbFunctionWrite                                                          */
/*---------------------------------------------------------------------------*/

uchar usbFunctionWrite(uchar * data, uchar len) {
  /* flipPortBit(&PORTB, 5);		// DEBUG LED */
  /* parseMidiMessage(data, len); */
  return 1;
}

/*---------------------------------------------------------------------------*/
/* usbFunctionWriteOut                                                       */
/*                                                                           */
/* this Function is called if a MIDI Out message (from PC) arrives.          */
/*                                                                           */
/*---------------------------------------------------------------------------*/

void usbFunctionWriteOut(uchar * data, uchar len) {
  /* flipPortBit(&PORTB, 5);		*/
  parseMidiMessage(data, len);
}

/*----------------------------------------------- */
/* Key Event処理 */

/* keyEventのQueue */
struct keyEvent {
  uchar kind;
  uchar key;
  uchar onOff;
};

#define EVENT_KEY 0
#define EVENT_CONTROLLER 1
#define KEY_OFF 0
#define KEY_ON 1
#define MAX_KEY_EVENT 32
#define KEY_EVENT_QUEUE_MASK (MAX_KEY_EVENT-1)
struct keyEvent keyEventQueue[MAX_KEY_EVENT];
uchar urptr, uwptr, iwptr;

uchar keyEventsFree (struct keyEvent **ptr) {
  return (MAX_KEY_EVENT - uwptr - 1);
}

void keyEventAppend(uchar kind, uchar key, uchar onOff) {
  uchar iwnxt;
  iwnxt = (iwptr +1 ) & KEY_EVENT_QUEUE_MASK;
  if (iwnxt == urptr )
    return;			/* buffer overflow */
  keyEventQueue[iwptr].kind = kind;
  keyEventQueue[iwptr].key = key;
  keyEventQueue[iwptr].onOff = onOff;
  iwptr = iwnxt;
}

static inline uchar keyEventAvailable(struct keyEvent **ptr) {
  *ptr = &keyEventQueue[urptr];
  if(iwptr >= urptr) {
    return iwptr - urptr;
  } else {  /* buffer end is between read and write pointer, return continuous range */
    return MAX_KEY_EVENT - urptr;
  }
}

/* The following function must be called after uartRxBytesAvailable() to
 * remove the bytes from the receiver buffer.
 */
static inline void  keyEventDidReadEvents(uchar numEvents) {
  urptr = (urptr + numEvents) & KEY_EVENT_QUEUE_MASK;
}

static inline void keyEventQueueClear() {
  urptr = uwptr = 0;
}

/* ----------------------------------------------- */

void wait( unsigned char time) {
  unsigned char j,k;
  for (j = 0; j < time; j++)
    for (k = 0; k < 250; k++)
      ;
}

/* ----------------------------------------------- */

uint16_t getTp(uchar toneNo) {
  return pgm_read_word(&freq_table[toneNo]);
}

/* POLY OUT */
void polyOut(uchar note, uchar val) {
  if (! POLY_OUT_CHECK()) {
    return;
  }
  uchar i = note - 60;	/* テーブルは60から開始されていることを仮定 */
  if ((i >= polyOutMin) && (i < polyOutMax)) {
    if (val != 0) {
      setPortBit(portInfoOut[i].port, portInfoOut[i].bit, 1); /* SET PORT HIGH */
      portInfoOut[i].status = TRIG_WIDTH;
    } else {
      if (! TRIG_OUT_CHECK()) {
	setPortBit(portInfoOut[i].port, portInfoOut[i].bit, 0); /* SET PORT LOW */
	portInfoOut[i].status = 0;
      }
    }
  }
}

/* polyOutの初期化(ALL NOTE OFF)をする */
void polyOutInit() {
  if (! POLY_OUT_CHECK()) {
    return;
  }
  uchar i;
  for (i =  polyOutMin ; i < (polyOutMax - polyOutMin) ; i++) {
    if (portInfoOut[i].status > 0) {
      setPortBit(portInfoOut[i].port, portInfoOut[i].bit, 0); /* SET PORT LOW */
      portInfoOut[i].status = 0;
    }
  }
}

/* TRIGの処理を行う */
void polyOutHelper() {
  if (CLOCK_OUT_CHECK()) {
    if ((midi_clock_trig_count > 0) &&
	(--midi_clock_trig_count == 0)) {
      CLOCK_OUT_LOW();
    }
    /* reset midi clock counter */
    if ((midi_clock_noclock > 0) &&
	(--midi_clock_noclock == 0)) { /* loop 1000 before reset clock_wait */
      midi_clock_wait = 1;	      /* STARTが来てからしばらく立つのでカウンターをリセットする */
    }
  }

  if (! TRIG_OUT_CHECK()) {
    return;
  }
  uchar i;
  for (i = polyOutMin ; i < (polyOutMax - polyOutMin) ; i++) {
    if ((portInfoOut[i].status > 0) &&
	((--portInfoOut[i].status) == 0)) {
      setPortBit(portInfoOut[i].port, portInfoOut[i].bit, 0); /* SET PORT LOW */
    }
  }
}

void servoOut() {
  if (!SERVO_CHECK()) {
    return;
  }
  if (--servo_wait == 0) {
    SERVO_OUT_PULSE();
    servo_wait = SERVO_POLLING_WAIT_LOOP;
  }
}


/* MIDI_CLOCK LOGICを初期化する */
void midiClockInit(uchar v) {
  midi_clock_ratio = v & 0x3f;
  midi_clock_control = (v >> 6) & 1;
  if (midi_clock_control == 1) { /* START/STOP制御を行う場合には */
    midi_clock_out = 0;		 /* STARTが来るまで停止しておく */
  } else {
    midi_clock_out = 1;		/* それ以外の場合には常に出力する */
  }
}

void sndOut(uchar note, uchar val) {
  if (! SND_OUT_CHECK()) {
    return;
  }
  if (val == 0) {
    SND_OFF;
  } else {
    SND_ON;
  }
}

/* sndOutの初期化(ALL NOTE OFF)をする */
void sndOutInit() {
  if (SND_OUT_CHECK()) {
    SND_OFF;
  }
}

void initializeKeyStatus() {
  int i;
  for (i = 0 ; i < MAX_KEYS ; i++) {
    keyCountButton[i] = 0;
    buttonStatus[i] = 1;
    matrixLedStatus[i] = 0;	/* LED off */
  }
}

/* ポリのキースキャン */
void scanKeyPressed(void) {
  if (! SCAN_KEY_CHECK()) {
    return;
  }
  uchar x;
  uchar i;
  for (i = scanKeyMin; i < scanKeyMax ; i++) {
    if (keyCountButton[i] > 0) {
      /* in the wait loop for 2nd check */
      keyCountButton[i] --;
      if (keyCountButton[i] == 0) {
	/* now, we check 2nd time */
	x = getPortBit(portInfoIn[i].port, portInfoIn[i].bit);	/* read PORT */
	if (x != (buttonStatus[i] & 1)) { /* status changed */
	  if (x == 0) {			  /* NOTE ON */
	    KEY_MODE_READ(x);
	    KEY_EVENT_SEND(portInfoIn[i].tone_no, x);
	    SET_LED(1);
	  } else {		/* NOTE OFF */
	    KEY_EVENT_SEND(portInfoIn[i].tone_no, buttonStatus[i] | 0x01);
	    SET_LED(0);
	  }
	  buttonStatus[i] = x;
	}
      }
    } else {
      /* 1st key check */
      x = getPortBit(portInfoIn[i].port, portInfoIn[i].bit);	/* read PORT */
      if (x != buttonStatus[i]) {
	keyCountButton[i] = KEY_COUNT_TH;
      }
    }
  } /* for */
}

/* for monome-like mode */
/* required macro are as follows;
   SCAN_MATRIX()
   SCAN_ROW

   NOTE: never try to run both scanMatrix() and scanKeyPressed() at the same config,
      because keyCountButton[] , buttonStatus[] are shared on both routines;

*/

static uchar scan_col = 0;
static uchar scan_wait_count = 1;

void scanMatrix(void) {
  uchar row;
  uchar x;

  /* check required */
  if (! SCAN_MATRIX_CHECK()) {
    return;
  }

  if ((--scan_wait_count) == 0) {
    /* 前のCOLをHi-Zにする */
    x = (scan_col == 0) ? (SCAN_COL_SIZE - 1) : (scan_col - 1);
    setPortBit0(scanCol[x].ddr, scanCol[x].bit); /* PORT = IN */
    setPortBit1(scanCol[x].port, scanCol[x].bit); /* 'H' */

    /* =============KEY INPUT ===================== */
    /* いったんROWをHi-Zにする */
    for (row = 0 ; row < SCAN_ROW_SIZE ; row++) {
      setPortBit0(scanRow[row].ddr, scanRow[row].bit); /* PORT=IN */
      setPortBit1(scanRow[row].port, scanRow[row].bit); /* 'H' */
    }
    /* COLをLOWにしてキーを読み込み */
    setPortBit1(scanCol[scan_col].ddr, scanCol[scan_col].bit); /* PORT = OUT */
    setPortBit0(scanCol[scan_col].port, scanCol[scan_col].bit); /* 'L' */

    for (row = 0 ; row < SCAN_ROW_SIZE ; row++) {
      uchar note = (scan_col * SCAN_ROW_SIZE) + row;
      x = getPortBit(scanRow[row].in, scanRow[row].bit); /* READ PORT */
      /* チャッタリングの処理 */
      if (x != buttonStatus[note]) {
	if (++keyCountButton[note] > 1) { /* KEY change detected */
	  if (x == 0) {
	    keyEventAppend(EVENT_KEY, note, 1);
	    SET_LED(1);
	  } else {
	    keyEventAppend(EVENT_KEY, note, 0);
	    SET_LED(0);
	  }
	  keyCountButton[note] = 0;
	  buttonStatus[note] = x;
	}
      } else {
	keyCountButton[note] = 0;
      }
    }

    /* COLをHi-Zにする */
    setPortBit0(scanCol[scan_col].ddr, scanCol[scan_col].bit); /* PORT = IN */
    setPortBit1(scanCol[scan_col].port, scanCol[scan_col].bit); /* 'H' */

    /* =============KEY INPUT ===================== */

    /* LED点灯のためROWの設定をする */
    for (row = 0 ; row < SCAN_ROW_SIZE ; row++) {
      /* set row(j) to H(OUT) or L(OUT) */
      if (matrixLedStatus[(scan_col * SCAN_ROW_SIZE) + row] != 0) {
	/* LED ON */
	setPortBit1(scanRow[row].ddr, scanRow[row].bit); /* PORT=OUT */
	setPortBit0(scanRow[row].port, scanRow[row].bit); /* 'L' */
      } else {
	/* LED OFF */
	setPortBit0(scanRow[row].ddr, scanRow[row].bit); /* PORT=IN */
	setPortBit1(scanRow[row].port, scanRow[row].bit); /* 'H' */
      }
    }

    /* set col(i) to H */
    setPortBit1(scanCol[scan_col].port, scanCol[scan_col].bit); /* 'H' */
    setPortBit1(scanCol[scan_col].ddr, scanCol[scan_col].bit); /* PORT = OUT */

    /* set pointer to next column */
    scan_col ++;
    if (scan_col >= SCAN_COL_SIZE) { scan_col = 0; }

    /* set counter */
    scan_wait_count = SCAN_COUNT_TH; /* RESET COUNTER */
  }
  
}

void matrixOut(uchar note, uchar onOff) {
  /* check required */
  if (! SCAN_MATRIX_CHECK()) {
    return;
  }
  if (note < MAX_KEYS) {
    matrixLedStatus[note] = onOff;
  }
}

/* matrixOutの初期化(ALL NOTE OFF)をする */
void matrixOutInit() {
  uchar i;
  /* check required */
  if (! SCAN_MATRIX_CHECK()) {
    return;
  }
  for (i = 0 ; i < MAX_KEYS ; i++) {
    matrixLedStatus[i] = 0;
  }
}

/*----------------------------------------------- */
/* ノートON処理*/

void NoteON(uchar note, uchar vol) {
  sndOut(note, vol);
  polyOut(note, vol);
  monoOut(note, vol);
  matrixOut(note, vol);
}

/* ノートOFF処理 */

void NoteOFF(uchar note) {
  sndOut(note, 0);
  polyOut(note, 0);
  monoOut(0, 0);
  matrixOut(note, 0);
}

void allNoteOFF() {
  sndOutInit();
  polyOutInit();
  monoOut(0, 0);
  matrixOutInit();
}

/* ------------------------ */

void a_out(uchar ch, uchar val) {
  *(aout[ch].port) = val;
}

/* ------------------------ */

/* AD enable, 1/128 clock  */
#define setupADC() ADCSRA = 0x87
/* Aref, 右揃え,CH=ch */
/* 変換開始 */
#define ADCStart() { ADCSRA |= 0x10;  ADCSRA |= 0x40; }
/* 変換停止 */
#define gioStopAd() ADCSRA = 0x00
/* AD変換値を取得する */
#define gioPollAd() (((ADCSRA & 0x10) == 0) ? -1 : ADC)

#define gioStartAd(c) { \
  setupADC();\
  setupCH(c);	\
  wait(2);\
  ADCStart(); }

/* analog入力を行う */
void analogCheck() {
  int16_t val;
  static int cnt = 1;

  /* アナログ入力が必要か？ */
  if (! AIN_CHECK()) {
    return;
  }

  /* 次のAD開始待ちの場合 */
  if (pollMode == POLL_ANALOG_WAIT) {
    if ((--cnt) == 0) {
      /* ADを再スタートする */
      gioStartAd(convertingCh);
      pollMode = POLL_ANALOG_CONT;
    }
    return;
  }
    
  /* 変換終了時 */
  if ((val = gioPollAd()) >= 0) {
    /* 10bit -> 7bit */
    val = val >> 3;		

    /* 値に変更があったかチェックし、変化があればCCを送出する */
    if ( adVals[(uchar)convertingCh] != val) {
      adVals[(uchar)convertingCh] = val;	/* 値を記録する */
      keyEventAppend(EVENT_CONTROLLER, convertingCh + 22, val);
    }

    /* 次のチャンネルに設定し、POLL_ANALOG_WAITに入る */
    if ((++convertingCh) >= AIN_CH_NUM()) { /* 全チャンネル終了 */
      convertingCh = 0;
    }
    cnt = ANALOG_POLLING_WAIT_LOOP;
    pollMode = POLL_ANALOG_WAIT;
  }
}

/*----------------------------------------------- */
/* メッセージのパース処理*/
/*----------------------------------------------- */
void parseMidiMessage(uchar *data, uchar len) {
  uchar cin = (*data) & 0x0f;	/* CABLE NOを無視する */
  uchar note = (*(data+2)) & 0x7f;
  uchar vol  = (*(data+3)) & 0x7f;

  /* REALTIME MESSAGE */
  if (cin == 0x0f) {		/* SINGLE BYTE */
    if (*(data+1) == 0xf8) {
      /* MIDI TIMING CLOCK */
      midi_clock_noclock = TRIG_RESET_INTERVAL;	/* restart noclock counter
				   noclockが一定値を超えたら強制的にカウンターをリセットする */
      if ((midi_clock_out == 1) && (CLOCK_OUT_CHECK())) {
	if ((midi_clock_wait > 0) &&
	    (-- midi_clock_wait == 0)) {
	  midi_clock_wait = midi_clock_ratio;
	  CLOCK_OUT_HI();
	  midi_clock_trig_count = TRIG_WIDTH; /* reset counter for trig width */
	}
      }
    } else if (*(data+1) == 0xfa || (*(data+1) == 0xfb)) {
      /* START/RESTART */
      if (CLOCK_OUT_CHECK()) {
	CLOCK_START_HI();
	if (midi_clock_control == 1) {
	  /* set clock_out flag */
	  midi_clock_out = 1;
	  /* set clock counter = 1, so that the next clock will generate CLOCK_OUT */
	  midi_clock_wait = 1;
	}
      }
    } else if (*(data+1) == 0xfc) {
      /* STOP */
      if (CLOCK_OUT_CHECK()) {
	CLOCK_START_LOW();
	if (midi_clock_control == 1) {
	  midi_clock_out = 0;
	}
      }
    }
  }

  if (((*(data+1)) & 0x0f) != 0)			/* only channel 1 (=0) */
    return;

  switch(cin) {

    /* NOTE OFF */
  case 8:
    NoteOFF(note);
    SET_LED(0);
    break;

    /* NOTE ON */
  case 9:
    if( vol == 0){
      NoteOFF(note);
      SET_LED(0);
    } else {
      NoteON(note, vol);
      SET_LED(1);
    }
    break;

    /* CONTROL CHANGE */
  case 11:			
    if (note >= 0x16 && note < 0x1c) { /* ANALOG OUT */
      /* アナログ出力 */
      if (AOUT_CHECK() &&	/* have analog-out mode ? */
	  ((note - 0x16) < AOUT_MAX())) { /* have channel ? */
	a_out(note - 0x16, (*(data+3)) * 2);
      } else if (SERVO_CHECK() &&		/* SERVO CONTROL */
		 ((note - 0x16) < SERVO_MAX)) { /* have channel ? */
	SERVO_SET_VALUE(note - 0x16, *(data + 3));
      }
    } else if (note == 122) {		/* local control */
      /* Monakaのモード変更 */
      vol = *(data+3);
      if (vol > 0 && MODE_CHECK(vol)) {	/* インプリされたモードかチェックする */
	eeprom_busy_wait();	/* EEPROMがレディになるまで待つ */
	eeprom_write_byte(&e_conf, vol); /* EEPROMに書き込む */
	conf = vol;
	portInit(conf);
      }
    } else if (note == 126) {		/* MONO MODE ONを流用する */
      /* MIDI CLOCKのタイミングパラメータ */
      vol = *(data+3);
      if (vol > 0) {
	eeprom_busy_wait();	/* EEPROMがレディになるまで待つ */
	eeprom_write_byte(&e_midi_clock_control, vol); /* EEPROMに書き込む */
	midiClockInit(vol);
      }
    } else if ((note == 120) || (note == 123)) {
      /* all note off */
      allNoteOFF();
    } else if (note == 121) {
      /* 122 reset all controller */
      portInit(conf);
    }
    break;

    //  case 0:			/* MISC FUNCTION CODE */
    //  case 1:			/* CABLE EVENT */
    //  case 2:			/* TWO-BYTES SYSTEM COMMON */
    //  case 3:			/* THREE-BYTES SYSTEM COMMON */
    //  case 4:			/* SYS EX START */
    //  case 5:			/* SINGLE BYTE SYSTEM COMMON */
    //  case 6:			/* SYSTEM EX end (with 2 byte) */
    //  case 7:			/* SYSTEM EX end (with 3 byte) */
    //  case 10:		/* POLY KEY PRESS */
    //  case 12:		/* PROGRAM CHANGE */
    //  case 13:		/* CHANNEL PRESSURE */
    //  case 14:		/* PITCH BEND */
    //  case 15:			/* SINGLE BYTE */
    //    break;
  }

  if (len > 4) {
    parseMidiMessage(data+4, len-4);
  }
}


/*---------------------------------------------------------------------------*/
/* hardwareInit                                                              */
/*---------------------------------------------------------------------------*/

static void hardwareInit(void) {
  uchar i, j;

  /* activate pull-ups except on USB lines */
  USB_CFG_IOPORT =
    (uchar) ~ ((1 << USB_CFG_DMINUS_BIT) |
	       (1 << USB_CFG_DPLUS_BIT));
  /* all pins input except USB (-> USB reset) */
#ifdef USB_CFG_PULLUP_IOPORT
  /* use usbDeviceConnect()/usbDeviceDisconnect() if available */
  USBDDR = 0;		/* we do RESET by deactivating pullup */
  usbDeviceDisconnect();
#else
  USBDDR = (1 << USB_CFG_DMINUS_BIT) | (1 << USB_CFG_DPLUS_BIT);
#endif

  j = 0;
  while (--j) {		/* USB Reset by device only required on Watchdog Reset */
    i = 0;
    while (--i);	/* delay >10ms for USB reset */
  }
#ifdef USB_CFG_PULLUP_IOPORT
  usbDeviceConnect();
#else
  USBDDR = 0;		/*  remove USB reset condition */
#endif
}


/*---------------------------------------------------------------------------*/
/* main                                                                      */
/*---------------------------------------------------------------------------*/

int main(void) {
  uchar midiMsg[8];
  uchar i, iii;

#ifdef PEPPER
  calibrate();
#endif

  /* WDTを作動させる */
  wdt_enable(WDTO_1S);

  /* AVR-USBのためのHWの初期化をする */
  hardwareInit();

  /* configモードをEEPROMより読み込んで設定する */
  eeprom_busy_wait();
  conf = eeprom_read_byte(&e_conf);

  /* midi_clock_ratio, midi_clock_controlをEEPROMより読み込んで設定する */
  eeprom_busy_wait();
  i = eeprom_read_byte(&e_midi_clock_control);
  midiClockInit(i);

  /* ポートを初期化する */
  portInit(conf);

  usbInit();
  keyEventQueueClear();

  sendEmptyFrame = 0;

  sei();

  for (;;) {			/* main event loop */
    wdt_reset();
    usbPoll();

    /* キーボードをスキャンする */
    scanKeyPressed();

    /* Trigの処理を行う */
    polyOutHelper();

    /* Matrixの処理を行う */
    scanMatrix();

    /* AD変換を行う */
    analogCheck();

    /* SERVOの処理を行う */
    servoOut();

    /* USBに送信する */
    if (usbInterruptIsReady()) {
      uchar eventsRead;
      struct keyEvent *data;
      
      eventsRead = keyEventAvailable(&data);
      if(eventsRead > 0 || sendEmptyFrame){
	if(eventsRead >= 2) {	/* up to 2 events can be sent */
	  eventsRead = 2;
	  /* send an empty block after last data block to indicate transfer end */
	  sendEmptyFrame  = 1;
	} else {
	  sendEmptyFrame  = 0;
	}
	iii = 0;
	for (i = 0; i < eventsRead ; i++) {
	  if ((data-> kind) == EVENT_KEY) {
	    if ((data ->onOff) == KEY_ON) { /* NOTE ON */
	      midiMsg[iii++] = 0x09;
	      midiMsg[iii++] = 0x90;
	      midiMsg[iii++] = data->key;
	      midiMsg[iii++] = 0x7f;
	    } else {		/* NOTE OFF */
	      midiMsg[iii++] = 0x08;
	      midiMsg[iii++] = 0x80;
	      midiMsg[iii++] = data->key;
	      midiMsg[iii++] = 0x00;
	    }
	  } else if ((data->kind) == EVENT_CONTROLLER) {
	      midiMsg[iii++] = 0x0b; /* CONTROL CHANGE */
	      midiMsg[iii++] = 0xb0;
	      midiMsg[iii++] = data->key;
	      midiMsg[iii++] = data->onOff;
	  }
	  keyEventDidReadEvents(1);
	  keyEventAvailable(&data); /* get next event data */
	}
	usbSetInterrupt(midiMsg, eventsRead * 4);
      }
    } /* usbInterruptIsReady() */
  } /* main event loop */
  return 0;
}

/* EOF */
