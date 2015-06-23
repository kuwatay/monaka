/* ginger.c */
/* ginger-monaka specific routines */

#define USE_16BIT_FTABLE
#ifndef INITIAL_CONFIG
#define INITIAL_CONFIG 8
#endif

struct portConfig_t {
  uchar ddrbIn;			/* DDRB 入力ピンのビット */
  uchar ddrbOut;		/* DDRB 出力ピンのビット */
  uchar ddrcIn;			/* DDRC 入力ピンのビット */
  uchar ddrcOut;		/* DDRC 出力ピンのビット */
  uchar ddrdIn;			/* DDRD 入力ピンのビット */
  uchar ddrdOut;		/* DDRD 出力ピンのビット */
} portConfig [] = {
  { 0x10, 0x2f, 0x00, 0x3f, 0x00, 0xfa}, /* Config 0 */
  { 0x10, 0x2f, 0x00, 0x3f, 0x00, 0xfa}, /* Config 1 */
  { 0x10, 0x2f, 0x00, 0x3f, 0x00, 0xfa}, /* Config 2 */
  { 0x1c, 0x23, 0x00, 0x3f, 0x98, 0x62}, /* Config 3 */
  { 0x11, 0x2e, 0x3f, 0x00, 0x02, 0xf8}, /* Config 4 */
  { 0x1d, 0x22, 0x3f, 0x00, 0xfa, 0x00}, /* Config 5 */
  { 0x1f, 0x20, 0x3f, 0x00, 0xfa, 0x00}, /* Config 6 */
  { 0x1f, 0x20, 0x3f, 0x00, 0xfa, 0x00}, /* Config 7 */
  { 0x10, 0x2f, 0x00, 0x3f, 0x90, 0x6a}, /* Config 8 */
};

struct portInfo_t {
  uchar tone_no;
  volatile uchar *port;
  uchar bit;
  uchar status;
};

/* mapping table for Output port and note */
struct portInfo_t portInfoOut []= {
  {60, &PORTC, 0, 0},		/* IX:00 C3 */
  {61, &PORTC, 1, 0},		/* IX:01 C#3 */
  {62, &PORTC, 2, 0},		/* IX:02 D3 */
  {63, &PORTC, 3, 0},		/* IX:03 D#3 */
  {64, &PORTC, 4, 0},		/* IX:04 E3 */
  {65, &PORTC, 5, 0},		/* IX:05 F3 */
  {66, &PORTB, 0, 0},		/* IX:06 F#3 */
  {67, &PORTD, 1, 0},		/* IX:07 G3 */
  {68, &PORTD, 6, 0},		/* IX:08 G#3 */
  {69, &PORTD, 5, 0},		/* IX:09 A3 */
  {70, &PORTB, 2, 0},		/* IX:10 A#3 */
  {71, &PORTB, 3, 0},		/* IX:11 B3 */
  {72, &PORTD, 3, 0},		/* IX:12 C4 */
  {73, &PORTD, 4, 0},		/* IX:13 C#4 */
  {74, &PORTD, 7, 0}		/* IX:14 D4 */
};

/* mapping table for input port and note (Config 3, 4, 5, 6) */
struct portInfo_t portInfoIn []= {
  {59, &PINB, 4, 0},		/* IX:00 B2   (MODE            8) built-in button */
  {73, &PIND, 4, 0},		/* IX:01 C#4  (MODE 3,    5,  ,8) */
  {74, &PIND, 7, 0},		/* IX:02 D4   (MODE 3,    5,  ,8) */
  {72, &PIND, 3, 0},		/* IX:03 C4   (MODE 3,    5, 6  ) */
  {70, &PINB, 2, 0},		/* IX:04 A#3  (MODE 3,    5, 6  ) */
  {71, &PINB, 3, 0},		/* IX:05 B3   (MODE 3,    5, 6  ) */
  {59, &PINB, 4, 0},		/* IX:06 B2   (MODE 3, 4, 5, 6) built-in button */
  {64, &PINC, 4, 0},		/* IX:07 E3   (MODE    4, 5, 6  ) */
  {65, &PINC, 5, 0},		/* IX:08 F3   (MODE    4, 5, 6  ) */
  {66, &PINB, 0, 0},		/* IX:09 F#3  (MODE    4, 5, 6  ) */
  {67, &PIND, 1, 0},		/* IX:10 G3   (MODE    4, 5, 6  ) */
  {69, &PIND, 5, 0},		/* IX:11 A3   (MODE       5, 6  ) */
  {60, &PINC, 0, 0},		/* IX:12 C3   (MODE       5, 6  ) */
  {61, &PINC, 1, 0},		/* IX:13 C#3  (MODE       5, 6  ) */
  {62, &PINC, 2, 0},		/* IX:14 D3   (MODE       5, 6  ) */
  {63, &PINC, 3, 0},		/* IX:15 D#3  (MODE       5, 6  ) */
  {68, &PIND, 6, 0},		/* IX:16 G#3  (MODE       5, 6  ) */
};


/* MONO出力用のポートのマッピング */
struct monoMap_t {
  volatile uchar *port;
  uchar bit;
} monoMap [] = {
  {&PORTD, 6},			/* BIT0 */
  {&PORTD, 5},			/* BIT1 */
  {&PORTB, 2},			/* BIT2 */
  {&PORTB, 3},			/* BIT3 */
  {&PORTD, 3},			/* BIT4 */
  {&PORTD, 4},			/* BIT5 */
  {&PORTD, 7},			/* BIT6 */
};


struct portInfo aout[] = { /* for config 4, 8 */
  {&OCR0A,0},{&OCR0B,0},{&OCR1AL,0},{&OCR1BL,0}, {&OCR2A,0}, {&OCR2B, 0}};

/* テーブル参照用 */
uchar polyOutMin = 60;
uchar polyOutMax = 67;

uchar scanKeyMin = 0;
uchar scanKeyMax = 4;

/*--------- MONOME MODE KEY SCAN用 ------------- */
struct scanPortInfo_t {
  volatile uchar *port;
  uchar bit;
  volatile uchar *ddr;
  volatile uchar *in;
};

#define SCAN_ROW_SIZE 8
#define SCAN_COL_SIZE 8

struct scanPortInfo_t scanCol []= {
  {&PORTD, 6, &DDRD, &PIND},	/* AOUT-0 */
  {&PORTD, 5, &DDRD, &PIND},	/* AOUT-1 */
  {&PORTB, 1, &DDRB, &PINB},	/* AOUT-2 */
  {&PORTB, 2, &DDRB, &PINB},	/* AOUT-3 */
  {&PORTB, 3, &DDRB, &PINB},	/* DOUT-0 */
  {&PORTD, 3, &DDRD, &PIND},	/* DOUT-1 */
  {&PORTD, 4, &DDRD, &PIND},	/* DOUT-2 */
  {&PORTD, 7, &DDRD, &PIND},	/* DOUT-3 */
};

struct scanPortInfo_t scanRow []= {
  {&PORTC, 0, &DDRC, &PINC},	/* AIN-0 */
  {&PORTC, 1, &DDRC, &PINC},	/* AIN-1 */
  {&PORTC, 2, &DDRC, &PINC},	/* AIN-2 */
  {&PORTC, 3, &DDRC, &PINC},	/* AIN-3 */
  {&PORTC, 4, &DDRC, &PINC},	/* DIN-0 */
  {&PORTC, 5, &DDRC, &PINC},	/* DIN-1 */
  {&PORTB, 0, &DDRB, &PINB},	/* DIN-2 */
  {&PORTD, 1, &DDRD, &PIND},	/* DIN-3 */
};

/*--------- MONOME MODE KEY SCAN用 ------------- */

/* インプリされたモードのチェック */
#define MODE_CHECK(c) ((c) <= 8)

/*----------------------------------------------- */
/* I/O入出力LEDの処理 */

#define SET_LED(val) setPortBit(&PORTB, 5, (val))

/* timing clock support */
#define CLOCK_OUT_CHECK() (conf == 3)
#define CLOCK_OUT_LOW() { setPortBit(&PORTD, 6, 0); }
#define CLOCK_OUT_HI() { setPortBit(&PORTD, 6, 1); }

#define CLOCK_START_LOW() {setPortBit(&PORTD, 5, 0); }
#define CLOCK_START_HI() {setPortBit(&PORTD, 5, 1); }

/* polyOutのあるモード */
#define POLY_OUT_CHECK() (conf <= 4)

/* trigOutのあるモード */
#define TRIG_OUT_CHECK() (conf == 3)

void monoOut(uchar note, uchar val) {
  uchar i;
  if (conf == 1) {		/* OLNY MODE1 */
    for (i = 0; i < 7; i++) {
      setPortBit(monoMap[i].port, monoMap[i].bit, (note & _BV(i)));
    }
  }
}

/* for ToneOut */
#define SND_OUT_CHECK() ((conf != 4) && (conf != 6) && (conf != 7) && (conf != 8))
#define SND_ON  {OCR1A=getTp(note); DDRB |=  0b00000010; } /* PB1=out */
#define SND_OFF {DDRB &= ~0b00000010; } /* PB1=in */

/* key input */
#define MAX_KEYS (SCAN_ROW_SIZE * SCAN_COL_SIZE)
static uchar keyCountButton[MAX_KEYS];
static uchar buttonStatus[MAX_KEYS];
/* matrix LED status */
static uchar matrixLedStatus[MAX_KEYS];

#define SCAN_KEY_CHECK() (conf != 7)

/* PB1 == H (COHRD MODE) L (NORMAL MODE) */
#define CHORD_MODE_CHECK(p) (((p) & 0x02) == 0)

/* PD7 == H (NORMAL) L (MINOR CHORD) */
#define CHORD_SHIFT_MINOR(p) (((p) & 0x80) == 0)

/* PD4 == H (NORMAL) L (7th CHORD) */
#define CHORD_SHIFT_7TH(p) (((p) & 0x10) == 0)

#define KEY_MODE_READ(x) {\
    if (conf == 6) {\
      x = x | (PINB & 0x02) | (PIND & 0x80) | (PIND & 0x10); \
    }\
}

#define KEY_EVENT_SEND(note, x) { \
    uchar noteOnOff; \
    noteOnOff = (((x) & 1) == 0) ? 1 : 0; \
    if (conf != 6) { \
      keyEventAppend(EVENT_KEY, (note), noteOnOff);	\
    } else { \
      if (CHORD_MODE_CHECK(x)) { \
	keyEventAppend(EVENT_KEY, (note), noteOnOff);	\
      } else {\
	if (CHORD_SHIFT_MINOR(x)) { \
	  if (CHORD_SHIFT_7TH(x)) {  \
	    keyEventAppend(EVENT_KEY, (note), noteOnOff);	\
	    keyEventAppend(EVENT_KEY, (note) + 3, noteOnOff);	\
	    keyEventAppend(EVENT_KEY, (note) + 7, noteOnOff);	\
	    keyEventAppend(EVENT_KEY, (note) + 10, noteOnOff);	\
	  } else { \
	    keyEventAppend(EVENT_KEY, (note), noteOnOff);	\
	    keyEventAppend(EVENT_KEY, (note) + 3, noteOnOff);	\
	    keyEventAppend(EVENT_KEY, (note) + 7, noteOnOff);	\
	  } \
	} else if (CHORD_SHIFT_7TH(x)) { \
	  keyEventAppend(EVENT_KEY, (note), noteOnOff);	\
	  keyEventAppend(EVENT_KEY, (note) + 4, noteOnOff);	\
	  keyEventAppend(EVENT_KEY, (note) + 7, noteOnOff);	\
	  keyEventAppend(EVENT_KEY, (note) + 10, noteOnOff);	\
	} else { \
	  keyEventAppend(EVENT_KEY, (note), noteOnOff);	\
	  keyEventAppend(EVENT_KEY, (note) + 4, noteOnOff);	\
	  keyEventAppend(EVENT_KEY, (note) + 7, noteOnOff);	\
	}\
      } \
    } \
  }

#define SCAN_MATRIX_CHECK() (conf == 7)

/* ANALOG IN/OUT */

#define AOUT_CHECK() (conf == 4)
#define AOUT_MAX() 4

#define AIN_CHECK() (conf == 4)

#define AIN_CH_NUM() 4

#define AIN_CH(ch) (ch)

#define setupCH(ch) ADMUX = (0x40 | ((ch) & 07))

/* SERVO */
#define SERVO_CHECK() (conf == 8)
#define SERVO_MAX 6

uchar aOutValue [SERVO_MAX] = { 1, 127, 1, 127, 1, 127}; /* The values are for tests */
#define SERVO_SET_VALUE(ch, val) { aOutValue[ch] = val; }
#ifdef ARDUINO
#define SERVO_OUT_MIN0 61
#define SERVO_OUT_MIN1 247
#define SERVO_OUT_MIN2 124
#define SERVO_CONV_OCR0(v) (SERVO_OUT_MIN0 + ((v)/2))
#define SERVO_CONV_OCR1(v) (SERVO_OUT_MIN1 + ((v) * 2))
#define SERVO_CONV_OCR2(v) (SERVO_OUT_MIN2 + (v))
#else
#define SERVO_OUT_MIN0 46
#define SERVO_OUT_MIN1 187
#define SERVO_OUT_MIN2 92
#define SERVO_CONV_OCR0(v) (SERVO_OUT_MIN0 + ((v) / 10) * 4)
#define SERVO_CONV_OCR1(v) (SERVO_OUT_MIN1 + (v) + (((v) / 10) * 6))
#define SERVO_CONV_OCR2(v) (SERVO_OUT_MIN2 + ((v) / 10) * 8)
#endif


#define SERVO_OUT_PULSE() {\
    TCCR0A = 0b11110000; /* Normal mode(0) , SET OC0A/OC0B on compare match   */ \
    TCCR0B = 0b11000000; /* TIMER STOP/ force compare match (to set OCR0A/OCR0B high) */ \
    TCCR0A = 0b10100000; /* Normal mode(0), CLEAR OC0A/OC0B on compare match */ \
    OCR0A = SERVO_CONV_OCR0(aOutValue[0]);\
    OCR0B = SERVO_CONV_OCR0(aOutValue[1]);\
    TCNT0 = 0; /* clear counter */ \
    TCCR0B = 0b00000100; /* 1/256 clock / START Timer*/ \
\
    TCCR1A = 0b11110000; /* Normal mode(0), SET OC1A/OC1B on compare match   */ \
    TCCR1B = 0b00000000; /* Normal mode(0) TIMER STOP */ \
    TCCR1C = 0b11000000; /* force compare match (to set OCR1A/OCR1B high) */ \
    TCCR1A = 0b10100000; /* Normal mode(0), CLEAR OC1A/OC1B on compare match */ \
    OCR1A = SERVO_CONV_OCR1(aOutValue[2]);\
    OCR1B = SERVO_CONV_OCR1(aOutValue[3]);\
    TCNT1 = 0; /* clear counter */ \
    TCCR1B = 0b00000011; /* Normal mode(0) 1/64 START Timer */ \
\
    TCCR2A = 0b11110000; /* Normal mode(0) , SET OC2A/OC2B on compare match   */ \
    TCCR2B = 0b11000000; /* TIMER STOP/ force compare match */ \
    TCCR2A = 0b10100000; /* Normal mode(0), CLEAR OC2A/OC2B on compare match */ \
    OCR2A = SERVO_CONV_OCR2(aOutValue[4]);\
    OCR2B = SERVO_CONV_OCR2(aOutValue[5]);\
    TCNT2 = 0; /* clear counter */ \
    TCCR2B = 0b00000101; /* 1/128 clock / START Timer*/ \
  }

void portInit(uchar cf) {
  /* setup PORTB */
  DDRB &= ~portConfig[cf].ddrbIn;
  DDRB |= portConfig[cf].ddrbOut;
  PORTB |= portConfig[cf].ddrbIn;  /* pull-up all input ports */
  PORTB &= ~portConfig[cf].ddrbOut; /* PUT 0 for all output pin */

  /* setup PORTC */
  DDRC &= ~portConfig[cf].ddrcIn;
  DDRC |= portConfig[cf].ddrcOut;
  PORTC |= portConfig[cf].ddrcIn;   /* pull-up all input ports*/
  PORTC &= ~portConfig[cf].ddrcOut; /* PUT 0 for all output pin */
  if (conf == 4)  {
    PORTC &= 0b11110000;	    /* don't pull-up AIN-0 to AIN-3 for analog input */
  }

  /* setup PORTD */
  DDRD &= ~portConfig[cf].ddrdIn;
  DDRD |= portConfig[cf].ddrdOut;
  PORTD |= portConfig[cf].ddrdIn;   /* pull-up all input ports */
  PORTD &= ~portConfig[cf].ddrdOut; /* PUTPUT 0 for all output pin */

#ifdef ARDUINO
  SET_LED(1);
#endif

  /* Timerを初期化する */
  if (conf == 4) {		/* PWM setup */
    /* TC0を初期化 */
    TCCR0A = 0b10100011;	/* Fast PWM mode(3) */
    TCCR0B = 0b00000001;	/* 1/1 clock */
    /* TCCR0B = 0b00000011;	*/	/* 1/64 clock */
    OCR0A = 0;			/* for channel 0 */
    OCR0B = 0;			/* for channel 1 */

    /* TC1を初期化 */
    TCCR1A = 0b10100001;	/* Fast PWM mode(5)*/
    TCCR1B = 0b00001001;	/* 1/1 CLOCK */
    /*    TCCR1B = 0b00001011;*/	/* 1/64 CLOCK */
    OCR1A = 0;
    OCR1B = 0;
  } else if (conf == 8) {	/* servo setup */
    /* TC0を初期化する */
    TCCR0A = 0b10100000;	/* Normal mode(0), clear OC0A/OC0B on Compare match */
    TCCR0B = 0b0000000;		/* TIMER STOP */
    OCR0A = 0;
    OCR0B = 0;
    /* TC1を初期化する */
    TCCR1A = 0b10100000;	/* Normal mode(0) clear OC1A/OC1B on compare match */
    TCCR1B = 0b00000000;	/* TIMER STOP */
    OCR1A = 0;
    OCR1B = 0;
    /* TC2を初期化する */
    TCCR2A = 0b10100010;	/* Normal mode(0), clear OC2A/OC2B on Compare match */
    TCCR2B = 0b00000000;	/* TIMER STOP */
    OCR2A = 0;
    OCR2B = 0;
  } else {
    /* STOP TC0 */
    TCCR0A = 0b00000000;	/* NORMAL PORT OPERATION */
    TCCR0B = 0b00000000;	/* NORMAL PORT OPERATION */
    TCCR2A = 0b00000000;	/* NORMAL PORT OPERATION */
    TCCR2B = 0b00000000;	/* NORMAL PORT OPERATION */
    if (conf != 7) {		/* TONE setup */
      TCCR1A = 0b01000000; /* CTC mode toggle OC1A, OC1B no use, mode = 4 */
      TCCR1B = 0b00001011; /* 1/64 clock */
      TCCR1C = 0b00000000;
      OCR1A = 0;
    } else {
      TCCR1A = 0b00000000; /* NORMAL PORT OPERATION*/
      TCCR1B = 0b00000000; /* no clock source */
      TCCR1C = 0b00000000;
      OCR1A = 0;
    }
  }

  /* キースキャンテーブルの初期化 */
  initializeKeyStatus();

  /* テーブル参照用 */
  polyOutMin = 0;				    /* C3 */
  if (conf == 2) {
    polyOutMax = 15;
  } else if ((conf == 1) || (conf == 3) || (conf == 8)) {
    polyOutMax = 9;
  } else if (conf == 4) {
    polyOutMin = 11;		/* B3 */
    polyOutMax = 15;		/* D4 */
  }

  /* reference for portInfoIn */
  if (conf == 3) {
    scanKeyMin = 1;
    scanKeyMax = 7;
  } else if (conf == 4) {
    scanKeyMin = 6;
    scanKeyMax = 11;
  } else if (conf == 5) {
    scanKeyMin = 1;
    scanKeyMax = 17;
  } else if (conf == 6) {
    scanKeyMin = 3;
    scanKeyMax = 17;
  } else if (conf == 8) {
    scanKeyMin = 0;
    scanKeyMax = 3;
  } else {			/* CONFIG 1, 2, 7 */
    scanKeyMin = 0;
    scanKeyMax = 1;
  }
  polyOutInit();
}

/* EOF */
