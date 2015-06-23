/* pepper.c */
/* pepper-monaka specific routines */

#define USE_8BIT_FTABLE
#ifndef INITIAL_CONFIG
#define INITIAL_CONFIG 8
#endif

struct portConfig_t {
  uchar ddrbIn;			/* DDRB 入力ピンのビット */
  uchar ddrbOut;		/* DDRB 出力ピンのビット */
} portConfig [] = {
  {0x39, 0x00}, /* Config 0 */
  {0x00, 0x39}, /* Config 1 */
  {0x20, 0x19}, /* Config 2 */
  {0x00, 0x39}, /* Config 3 */
  {0x28, 0x11}, /* Config 4 */
  {0x39, 0x00}, /* Config 5 */
  {0x38, 0x01}, /* Config 6 */
  {0x00, 0x39}, /* Config 7 */
  {0x28, 0x11}, /* Config 8 */
};

struct portInfo_t {
  uchar tone_no;
  volatile uchar *port;
  uchar bit;
  uchar status;
};

/* ポートとトーンのマッピングテーブル(出力用) */
struct portInfo_t portInfoOut []= {
  {60, &PORTB, 3, 0},		/* C3 */
  {61, &PORTB, 4, 0},		/* C#3 */
  {62, &PORTB, 0, 0},		/* D3 */
  {63, &PORTB, 5, 0},		/* D#3 */
};

/* ポートとトーンのマッピングテーブル(Config5,6 入力用) */
/* その他のモードでは、portInfoIn[0]の組み込みボタンだけを読込む */
struct portInfo_t portInfoIn []= {
  {60, &PINB, 3, 0},		/* C3 */
  {61, &PINB, 4, 0},		/* C#3 */
  {62, &PINB, 0, 0},		/* D3 */
  {63, &PINB, 5, 0},		/* D#3 */
};

static struct portInfo aout[] = { /* for config 4 */
  {&OCR0A,0},{&OCR1B,0}
};

/* テーブル参照用 */
uchar polyOutMin = 0;
uchar polyOutMax = 2;

uchar scanKeyMin = 0;
uchar scanKeyMax = 4;

/*--------- MONOME MODE KEY SCAN用 ------------- */
struct scanPortInfo_t {
  volatile uchar *port;
  uchar bit;
  volatile uchar *ddr;
  volatile uchar *in;
};

#define SCAN_ROW_SIZE 2
#define SCAN_COL_SIZE 2

struct scanPortInfo_t scanCol []= {
  {&PORTB, 0, &DDRB, &PINB},
  {&PORTB, 5, &DDRB, &PINB},
};

struct scanPortInfo_t scanRow []= {
  {&PORTB, 3, &DDRB, &PINB},
  {&PORTB, 4, &DDRB, &PINB},
};

/*--------- MONOME MODE KEY SCAN用 ------------- */
/* インプリされたモードのチェック */
#define MODE_CHECK(c) ((c) <= 8)

/*----------------------------------------------- */
/* I/O入出力LEDの処理 */

/* not supported on pepper */
#define SET_LED(val) ;

/* timing clock support */
#define CLOCK_OUT_CHECK() (conf == 3)
#define CLOCK_OUT_LOW() {setPortBit(&PORTB, 0, 0); }
#define CLOCK_OUT_HI() {setPortBit(&PORTB, 0, 1); }

#define CLOCK_START_LOW() {setPortBit(&PORTB, 5, 0); }
#define CLOCK_START_HI() {setPortBit(&PORTB, 5, 1); }

/* polyOutのあるモード */
#define POLY_OUT_CHECK() (conf <= 3)

/* trigOutのあるモード */
#define TRIG_OUT_CHECK() (conf == 3)

/* not supported on pepper */
#define monoOut(note, val) ;

/* for ToneOut */
#define SND_OUT_CHECK() (conf == 2)
#define SND_ON  {OCR0A=getTp(note); DDRB |=  0b00000001; } /* PB0=out */
#define SND_OFF {DDRB &= ~0b00000001; } /* PB0=in */

/* key input */
#define MAX_KEYS 4
static uchar keyCountButton[MAX_KEYS];
static uchar buttonStatus[MAX_KEYS];
/* matrix LED status */
static uchar matrixLedStatus[MAX_KEYS];

#define SCAN_KEY_CHECK() ((conf == 2) || (conf == 5))

#define KEY_MODE_READ(x) ;
#define KEY_EVENT_SEND(note, x) {			\
    keyEventAppend(EVENT_KEY, (note), x);		\
  }

#define SCAN_MATRIX_CHECK() (conf == 7)

/* ANALOG IN/OUT */

#define AOUT_CHECK() ((conf == 4) || (conf == 6))
#define AOUT_MAX() ((conf == 4) ? 2 : ((conf == 6) ? 1 : 0))

#define AIN_CHECK() ((conf == 4) || (conf == 6))

#define AIN_CH_NUM() ((conf == 4) ? 2 : ((conf == 6) ? 3 : 0))

#define AIN_CH(ch) ((ch == 0) ? 3 : ((ch == 2) ? 0 : ((conf == 4) ? 0 : 2)))

void setupCH(uchar ch) {
    ADMUX = AIN_CH(ch);
}

/* SERVO */

#define SERVO_CHECK() (conf == 8)
#define SERVO_MAX 2

uchar aOutValue [SERVO_MAX] = { 0, 127 };
#define SERVO_SET_VALUE(ch, val) { aOutValue[ch] = val; }
#define SERVO_OUT_MIN0 64
#define SERVO_OUT_MIN1 127
#define SERVO_CONV_OCR0(v) (SERVO_OUT_MIN0 + ((v)/2))
#define SERVO_CONV_OCR1(v) (SERVO_OUT_MIN1 + (v))

#define SERVO_OUT_PULSE() {\
    TCCR0A = 0b11000000; /* Normal mode(0) , SET OC0A on compare match   */ \
    TCCR0B = 0b10000000; /* TIMER STOP/ force compare match OC0A */ \
    TCCR0A = 0b10000000; /* Normal mode(0), CLEAR OC0A on compare match */ \
    OCR0A = SERVO_CONV_OCR0(aOutValue[0]); \
    TCNT0 = 0; /* clear counter */ \
    TCCR0B = 0b00000100; /* Normal mode(0) 1/256 clock / START Timer*/ \
    \
    TCCR1 = 0b00000000;		/* Timer STOP */ \
    GTCCR = 0b00111000;		/* set OC1B on compare match, force reset OC1B  */ \
    GTCCR = 0b00100000;		/* clear OC1B on compare match */ \
    OCR1B = SERVO_CONV_OCR1(aOutValue[1]); \
    TCNT1 = 0;			/* clear timer */ \
    TCCR1 = 0b00001000;		/* Prescaler 1/128 / START timer */ \
  }

void portInit(uchar cf) {
  /* setup PORTB */
  DDRB &= ~portConfig[cf].ddrbIn;
  DDRB |= portConfig[cf].ddrbOut;
  PORTB |= portConfig[cf].ddrbIn;  /* pull-up all input ports */
  PORTB &= ~portConfig[cf].ddrbOut; /* PUTPUT 0 for all output pin */
  if (conf == 4) {
    PORTB &= 0b11010111;	/* for analog in (PB3, PB5) */
  } else if (conf == 6) {
    PORTB &= 0b11000111;	/* for analog in (PB3, PB4, PB5) */
  }

  /* PWMの設定をする */
  if (cf == 2) {		/* for TONE setup */
    /* TC0, TC1を初期化 */
    TCCR0A = 0b01000010;	/* CTC mode, Toggle OC0A on Compare Match, OC0B Disconnected  */
    TCCR0B = 0b00000100;	/* 1/256 clock */
    OCR0A = 0;			/* for channel 0 */
    /* disable timer1 */
    TCCR1 = 0x00;		
    GTCCR = 0x00;
  } else if ((cf == 4) || (cf == 6)) { /* for PWM setup */
    /* OC0Aを初期化 */
    TCCR0A = 0x83;		/* clear OC0A (non inverting mode), unuse OC0B */
    TCCR0B = 0x01;		/* set 0x03 for 1/64 clock , set 0x01 for 1/1 clock */
    OCR0A = 0;			/* for channel 0 */
    if (cf == 4) {
      /* OC1Bを使う */
      TCCR1  = 0x01;		/* OC1A disable set 0x07 for CK/64 clock, set 0x01 for 1/1 clock */
      GTCCR  = 0x60;		/* OC1B enable , set 0x00 for disable */
      OCR1B = 0;
    }
  } else if (cf == 8) {		/* for servo setup */
    /* TC0を初期化する */
    TCCR0A = 0b10100010;	/* CTC mode (2), clear OC0A/OC0B on Compare match */
    TCCR0B = 0b00000011;	/* CTC mode (2) 1/64 clock */
    OCR0A = 0;
    OCR0B = 0;
    /* TC0を初期化する */
    TCCR1 = 0b00100111;		/* CTC mode disable,pwm disable,clear OC1A on Compare match, 1/64 clock */
    GTCCR = 0b00100000;		/* clear OC1B on Compare match */
    OCR1B = 0;
  } else{			/* DISABLE timer, anyway */
    /* disable timer0 */
    TCCR0A = 0b00000010;	/* OC0A/OC0B DISCONNECTDC0B  */
    TCCR0B = 0b00000100;	/* 1/256 clock */
    /* disable timer1 */
    TCCR1 = 0x00;
    GTCCR = 0x00;
  }

  /* キースキャンテーブルの初期化 */
  initializeKeyStatus();

  /* テーブル参照用 */
  polyOutMin = 0;
  polyOutMax = ((conf == 1) ? 4 : 2); /* 64: MODE=1, 62 : MODE=2,3 */

  scanKeyMin = (conf == 5) ? 0 : 3;
  scanKeyMax = 4;

  polyOutInit();
}

/* ------------------------------------------------------------------------- */
/* ------------------------ Oscillator Calibration ------------------------- */
/* ------------------------------------------------------------------------- */

/* Calibrate the RC oscillator to 8.25 MHz. The core clock of 16.5 MHz is
 * derived from the 66 MHz peripheral clock by dividing. Our timing reference
 * is the Start Of Frame signal (a single SE0 bit) available immediately after
 * a USB RESET. We first do a binary search for the OSCCAL value and then
 * optimize this value with a neighboorhod search.
 * This algorithm may also be used to calibrate the RC oscillator directly to
 * 12 MHz (no PLL involved, can therefore be used on almost ALL AVRs), but this
 * is wide outside the spec for the OSCCAL value and the required precision for
 * the 12 MHz clock! Use the RC oscillator calibrated to 12 MHz for
 * experimental purposes only!
 */
static void calibrateOscillator(void) {
  uchar step = 128;
  uchar trialValue = 0, optimumValue;
  int   x, optimumDev, targetValue = (unsigned)(1499 * (double)F_CPU / 10.5e6 + 0.5);

  /* do a binary search: */
  do{
    OSCCAL = trialValue + step;
    x = usbMeasureFrameLength();    /* proportional to current real frequency */
    if(x < targetValue)             /* frequency still too low */
      trialValue += step;
    step >>= 1;
  }while(step > 0);
  /* We have a precision of +/- 1 for optimum OSCCAL here */
  /* now do a neighborhood search for optimum value */
  optimumValue = trialValue;
  optimumDev = x; /* this is certainly far away from optimum */
  for(OSCCAL = trialValue - 1; OSCCAL <= trialValue + 1; OSCCAL++){
    x = usbMeasureFrameLength() - targetValue;
    if(x < 0)
      x = -x;
    if(x < optimumDev){
      optimumDev = x;
      optimumValue = OSCCAL;
    }
  }
  OSCCAL = optimumValue;
}
/*
Note: This calibration algorithm may try OSCCAL values of up to 192 even if
the optimum value is far below 192. It may therefore exceed the allowed clock
frequency of the CPU in low voltage designs!
You may replace this search algorithm with any other algorithm you like if
you have additional constraints such as a maximum CPU clock.
For version 5.x RC oscillators (those with a split range of 2x128 steps, e.g.
ATTiny25, ATTiny45, ATTiny85), it may be useful to search for the optimum in
both regions.
*/

static uchar __attribute__((section(".eeprom"))) calVal;

void    usbEventResetReady(void) {
  calibrateOscillator();
  eeprom_write_byte(&calVal, OSCCAL);   /* store the calibrated value in EEPROM */
}

void calibrate() {
  uchar i;
  uchar calibrationValue;

  calibrationValue = eeprom_read_byte(&calVal); /* calibration value from last time */
  if(calibrationValue != 0xff){
    OSCCAL = calibrationValue;
  }
  odDebugInit();
  usbDeviceDisconnect();
  for(i=0;i<20;i++){  /* 300 ms disconnect */
    _delay_ms(15);
  }
}

/* EOF */
