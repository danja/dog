#include <TM1638lite.h>

/**  Comms with TM1638
    arguments are the ports on the Uno
    (I've gone for the ones without extra functions
*/
TM1638lite tm(4, 7, 8);

#define MAX_PROG_SIZE 1024 // probably need to move prog elsewhere, is greedy
#define STACK_SIZE 128

#define PROG_MODE false
#define RUN_MODE true
#define STEP false
#define RUN true

/*
   Instruction Set
*/
#define NOP 0x00 // no operation
#define CLRS 0x01 // clear status flags

#define LDAi 0x10 // load accumulator A, immediate value
#define LDAa 0x11 // load accumulator A, absolute ref'd address
#define STAa 0x12 // store accumulator A, absolute ref'd address

#define LDBi 0x18
#define LDBa 0x19
#define STBa 0x1A

#define SPCa 0x28
#define JMPa 0x19
#define JNZa 0x1A

#define AND 0x38
#define OR  0x39
#define XOR 0x3A
#define NEGA 0x3B
#define NEGB 0x3C
#define ROLA 0x3D
#define RORA 0x3E
#define ROLB 0x3F
#define RORB 0x40
#define SWAP 0x41

#define ADD 0x48
#define SUB 0x49
#define CMP 0x4A

#define PUSHA 0x50
#define POPA 0x51
#define PUSHB 0x52
#define POPB 0x53

#define USE 0x80

#define HALT 0xFF

boolean mode = PROG_MODE;
boolean runmode = STEP;


/**
    ################# Storage ##################
*/
uint8_t program[MAX_PROG_SIZE]; // the code
uint8_t stack[STACK_SIZE];

/**
    ################# The Registers ##################
*/
unsigned int pc = 0; // program counter
char accA = 0; // accumulator A, 8-bits, -128 to 127
char accB = 0; // accumulator B, 8-bits, -128 to 127
unsigned int xReg = 0; // index register, 16 bits, 0 to 65535
uint8_t status = 48; // status register (flags), initialised with a vaguely helpful test pattern, LEDs over system displays only (4 & 5)

// SETUP ########################################################
void setup() {
  // Serial.begin(9600);

  // initialise registers
  for (unsigned long i = 0; i < MAX_PROG_SIZE; i++) {
    program[i] = 0; // NOP
  }
}

long previousPC = 0;

// LOOP ########################################################
void loop() {
  tm.displayText("DOG-1");
  delay(1000);
  tm.reset();
  while (1) {

    handleButtons();
    if (pc >= MAX_PROG_SIZE) pc = 0;
    if (pc < 0) pc = MAX_PROG_SIZE - 1;
    displayMode();
    displayPC();
    displayCode();
    showStatus();

    if (mode == RUN_MODE) {
      if (runmode == STEP) { // RUN-STEP

        doOperation();
      } else {

      }

    }

    delay(100);
  }
}

void doOperation() {
  uint8_t op = program[pc];

  switch (op) {

    case NOP: // no operation
      return;

    case CLRS: // clear status flags
      status = 0;
      return;

    case LDAi:
      pc++;
      accA = program[pc++];
      return;

    case LDAa:
      pc++;
      accA = program[readAbsoluteAddr()];
      return;

    case STAa:
      pc++;
      program[readAbsoluteAddr()] = accA;
      return;



    case HALT:
      waitForButton();
      mode = PROG_MODE;
      pc = 0;
      return;

    default:
      showError("noPE");
  }
}

unsigned int readAbsoluteAddr() {
  uint8_t lo = program[pc++];
  uint8_t hi = program[pc++];
  return (hi << 8) + lo;
}

void showError(String message) {
  tm.displayText("    " + message);
  displayPC();
  waitForButton();
  mode = PROG_MODE;
}

void waitForButton() {
  delay(500); // dirty, allow time for button release
  while (tm.readButtons() == 0) {
    delay(100);
  }
}

/*
   LEDs
*/
void showStatus() {
  uint8_t shifty = status;

  for (uint8_t i = 0; i < 8; i++) {
    tm.setLED(i, shifty & 1);
    shifty = shifty >> 1;
  }
}

/*
   Buttons
*/

unsigned long buttonMillis = 0; // time since last button press
unsigned long buttonDelay = 200;
uint8_t previousButtons = 0;

uint8_t incdec = true; // true for increment, false for decrement

void handleButtons() {
  uint8_t buttons = tm.readButtons();

  // puts a delay on the buttons if they haven't changed so they don't run away
  unsigned long currentMillis = millis();
  if ((currentMillis - buttonMillis > buttonDelay) || (buttons != previousButtons)) {
    buttonMillis = currentMillis;
    previousButtons = buttons;

    // do system buttons (4 & 5)

    // both together - reset pc
    if ( (buttons & (1 << 4)) && (buttons & (1 << 5))) {
      pc = 0;
      return;
    }


    if (buttons & (1 << 4)) {
      mode = !mode;
      return;
    }

    if (mode == PROG_MODE) {
      if (buttons & (1 << 5)) {
        incdec = !incdec;
        return;
      }
    }

    // do PC buttons (0-3) - inc/dec value as appropriate, have removed wrap below zero, it was getting too confusing
    if (buttons & (1 << 3)) {
      if (incdec) {
        pc++;
      } else {
        if (pc > 0) pc--;
      }
      return;
    }

    if (buttons & (1 << 2)) {
      if (incdec) {
        pc += 16;
      } else {
        if (pc > 16) pc -= 16;
      }
      return;
    }

    if (buttons & (1 << 1)) {
      if (incdec) {
        pc += 256 ;
      } else {
        if (pc > 256) pc -= 256;
      }
      return;
    }

    if (buttons & (1 << 0)) {
      if (incdec) {
        pc += 4096;
      } else {
        if (pc > 4096) pc -= 4096;
      }
      return;
    }

    // do code buttons (6 & 7)- inc/dec value as appropriate, note there's no carry or wrap
    uint8_t codeLow = program[pc] & 0xF; // mask
    uint8_t codeHigh = (program[pc] & 0xF0) >> 4; // mask & shift
    if (buttons & (1 << 6)) {
      if (incdec) {
        codeHigh++;
        if (codeHigh >= 16) codeHigh = 0;
      } else {
        if (codeHigh > 0) {
          codeHigh--;
        }
      }
    }

    if (buttons & (1 << 7)) {
      if (incdec) {
        codeLow++;
        if (codeLow >= 16) codeLow = 0;
      } else {
        if (codeLow > 0) {
          codeLow--;
        }
      }
    }
    program[pc] = (codeHigh << 4) + codeLow;
  }
}



void displayPC() {
  long pc_cut = pc;
  for (uint8_t i = 0; i < 4; i++) { // I tried decrementing, but got errors every time

    if (pc_cut > 0) {
      tm.displayHex(3 - i, pc_cut % 16);
      pc_cut = pc_cut / 16;
    } else {
      tm.displayHex(3 - i, 0);
    }
  }
}

void displayCode() {
  uint8_t codeLow = program[pc] & 0xF; // mask
  uint8_t codeHigh = (program[pc] & 0xF0) >> 4; // mask & shift
  tm.displayHex(6, codeHigh);
  tm.displayHex(7, codeLow);
}

void displayMode() {
  if (mode == PROG_MODE) {
    tm.displayASCII(4, 'P');
    if (incdec) {
      tm.displaySS(5, 1); // just the top segment
    } else {
      tm.displaySS(5, 8); // just the top segment
    }
  } else {
    tm.displayASCII(4, 'R');
  }
}

