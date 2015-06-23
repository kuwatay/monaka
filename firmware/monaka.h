/*
  monaka.h
*/

/* チャッタリング防止用にキーを読み込む待ち時間 */
#define KEY_COUNT_TH 200

/* トリガモードのパルス幅(カウント数) */
#define TRIG_WIDTH 100

/* トリガ用のカウンタのリセット用 */
#define TRIG_RESET_INTERVAL 10000

/* LED,KEYスキャンのカウント値 */
#define SCAN_COUNT_TH 10

/* MIDI CLOCK 分周比の初期値 */
#define INITIAL_MIDI_CLOCK_RATIO 4

/* MIDI CLOCK をSTART/STOP制御するかどうか 1=制御あり*/
#define INITIAL_MIDI_CLOCK_START_STOP_CONTROL 0

#define INITIAL_MIDI_CLOCK_CONTROL (INITIAL_MIDI_CLOCK_RATIO || (INITIAL_MIDI_CLOCK_START_STOP_CONTROL << 6));

/* ANALOG_INの間隔を決める定数 */
#define ANALOG_POLLING_WAIT_LOOP 150
/* NOTE
. @RadioJunkBox
MIDIクロックは1拍（4分音符の場合）が24クロックだから16分音符は6クロックですね
 */

/* SERVOのパルス間隔を決める定数 =~ 50Hz = 20mS */
#ifdef PEPPER
/* PEPPER with 16.5MHz clock */
#define SERVO_POLLING_WAIT_LOOP 1200
#else
#ifdef ARDUINO
/* ARDUINO with 16MHz clock */
#define SERVO_POLLING_WAIT_LOOP 570
#else
/* GINGER with 12MHz clock */
#define SERVO_POLLING_WAIT_LOOP 430
#endif
#endif

struct portInfo {
  volatile uchar *port;
  uchar bit;
};

#define setPortBit0(port, bit) *(port) &= ~_BV(bit);
#define setPortBit1(port, bit) *(port) |= _BV(bit);

#define setPortBit(port, bit, val) { \
    if ((val) == 0) { setPortBit0((port), (bit)); } else	\
      { setPortBit1((port), (bit)); }				\
}
#define getPortBit(port, bit) ((*(port) & _BV(bit)) >> (bit))

#define flipPortBit(port, bit) (*(port) ^= _BV(bit))

extern void keyEventAppend(uchar, uchar, uchar);
extern void parseMidiMessage(uchar *, uchar);
extern void initializeKeyStatus(void);
extern void polyOutInit();

extern uint16_t getTp(uchar);

const prog_uint16_t freq_table[] ;
