#include <TM1638lite.h>

/**
     Comms with TM1638
     https://github.com/danja/TM1638lite
     arguments are the ports on the Uno
     (I've opted for the ones without extra functions
*/
TM1638lite tm(4, 7, 8);

#define MAX_PROG_SIZE 512 // probably need to move prog elsewhere, is greedy
#define PC_STACK_SIZE 64
#define ALU_STACK_SIZE 64

#define PROG_MODE false
#define RUN_MODE true
#define STEP false
#define RUN true

/**
   ###################### Instruction Set ##############################
*/
// system stuff
#define NOP 0x00 // no operation
#define CLRS 0x01 // clear status flags

// accumulator A
#define LDAi 0x10 // load accumulator A, immediate value
#define LDAa 0x11 // load accumulator A, absolute ref'd address
#define LDAx 0x12 // load accumulator A, indexed 
#define LDAxx 0x13 // load accumulator A, doubly-index
#define STAa 0x14 // store accumulator A, absolute ref'd address
#define STAx 0x15 // store accumulator A, indexed 
#define STAxx 0x16 // store accumulator A, doubly-indexed

// accumulator B
#define LDBi 0x18 // load accumulator B, immediate value
#define LDBa 0x19 // load accumulator B, absolute ref'd address
#define LDBx 0x1A // load accumulator B, indexed 
#define LDBxx 0x1B // load accumulator B, doubly-index
#define STBa 0x1C // store accumulator B, absolute ref'd address
#define STBx 0x1D // store accumulator B, indexed 
#define STBxx 0x1E // store accumulator B, doubly-indexed

// IMPLEMENTED TO HERE

// Pc-related, jumps etc
#define SPCa 0x20
#define JMPa 0x21
#define JNZa 0x22
#define SKIPX 0x23
// PC stack
#define PUSH 0x24
#define POP 0x25

// logic ops
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

// Accumulator arithmetic ops
#define ADD 0x48
#define SUB 0x49
#define CMP 0x4A

// ALU stack-related
#define PUSHA 0x50
#define POPA 0x51
#define PUSHB 0x52
#define POPB 0x53
// see https://www.forth.com/starting-forth/2-stack-manipulation-operators-arithmetic/
#define SWAP 0x54
#define DUP 0x55
#define OVER 0x56
#define ROT 0x57
#define DROP 0x58

// hardware-related
#define USE 0x80
#define UNUSE 0x81

#define HALT 0xFF

boolean mode = PROG_MODE;
boolean runMode = STEP;

uint8_t ledStep = 0;

/**
    ################# Storage ##################
*/
uint8_t program[MAX_PROG_SIZE]; // the code
uint8_t pcStack[PC_STACK_SIZE]; // PC/subroutine-oriented stack
uint8_t aluStack[ALU_STACK_SIZE]; // experimental stack-oriented programming/maths stack

/**
    ################# The Registers ##################
*/
unsigned int pc = 0; // program counter
unsigned int xReg = 0; // index register, 16 bits, 0 to 65535  (or rather, MAX_PROG_SIZE?)
unsigned int pcStackP = 0; // stack pointer, 16 bits

char acc[2] = {0, 0}; // accumulators A & , 8 - bits, -128 to 127
uint8_t aluStackP = 0; // stack pointer, 8 bits, 0 to 255

uint8_t status = 48; // status register (flags), initialised with a vaguely helpful test pattern, LEDs over system displays only (4 & 5)

/**
    ################ Init Registers ###################
*/
void initRegisters() {

  pc = 0; // program counter
  acc[0] = 0x12; // accumulator A, 8-bits, -128 to 127
  acc[1] = 0xEF; // accumulator B, 8-bits, -128 to 127
  xReg = 0; // index register, 16 bits, 0 to 65535 (or rather, MAX_PROG_SIZE?)
  pcStackP = 0; // PC stack pointer, 16 bits
  aluStackP = 0; // ALU stack pointer, 8 bits
  status = 48; // status register (flags), initialised with a vaguely helpful test pattern, LEDs over system displays only (4 & 5)

  for (unsigned long i = 0; i < MAX_PROG_SIZE; i++) { // wipe all instructions
    program[i] = 0; // NOP
  }

  for (unsigned long i = 0; i < PC_STACK_SIZE; i++) { // wipe contents of PC stack
    pcStack[i] = 0;
  }

  for (unsigned long i = 0; i < ALU_STACK_SIZE; i++) { // wipe contents of ALU stack
    aluStack[i] = 0;
  }

  // misc resetting
  mode = PROG_MODE;
  ledStep = 0;
}


// SETUP ########################################################
void setup() {
  Serial.begin(9600);
  welcome(); // display welcome message
  initRegisters();
}

// LOOP ########################################################
void loop() {

  tm.reset(); // reset display hardware

  while (1) { // loop proper

    handleButtons();

    // trap overflows
    if (pc >= MAX_PROG_SIZE) pc = 0;
    if (pc < 0) pc = MAX_PROG_SIZE - 1;

    display();

    if (mode == RUN_MODE) {
      if (runMode == STEP) { // RUN-STEP FIX ME!
        // waitForButton();
        doOperation();
        pc++;
      }
    }

    delay(100);
    if (Serial.available()) {
      doSerialIn();
    }
  }
}

/*
   ##################################### Operations ########################
*/
void doOperation() {
  uint8_t op = program[pc];

  // 1066129901FF
  // 10 66 12 99 01 FF

  switch (op) {

    case NOP: // no operation
      return;

    case CLRS: // clear status flags
      status = 0;
      return;

    case LDAi:               // Load accumulator A immediate
    acc[0] = program[++pc]; 
      return;

    case LDAa:                            // Load accumulator A absolute
      LDa[0];
      return;

    case LDAx:                            // Load accumulator A indexed (6502 calls it Indexed Indirect)
      LDx(0);
      return;


    case LDAxx:                                      // Load accumulator A doubly-indexed (6502 calls it Indirect Indexed)
      LDxx(0);
      return;

    case STAa:                            // Store accumulator A absolute
      STa(0);
      return;

    case STAx:                            // Store accumulator A indexed (6502 calls it Indexed Indirect)
      STx(0);
      return;

    case STAxx:                                      // Store accumulator A doubly-indexed (6502 calls it Indirect Indexed)
      STxx(0);
      return;

    case LDBi:               // Load accumulator B immediate
      LDi[1];
      return;

    case LDBa:                            // Load accumulator B absolute
      LDa[1];
      return;

    case LDBx:                            // Load accumulator B indexed (6502 calls it Indexed Indirect)
      LDx(1);
      return;

    case LDBxx:                                      // Load accumulator B doubly-indexed (6502 calls it Indirect Indexed)
      LDxx(1);
      return;

    case STBa:                            // Store accumulator B absolute
      STa(1);
      return;

    case STBx:                            // Store accumulator B indexed (6502 calls it Indexed Indirect)
      STx(1);
      return;

    case STBxx:                                      // Store accumulator B doubly-indexed (6502 calls it Indirect Indexed)
      STxx(1);
      return;

    case HALT:
      waitForButton();
      mode = PROG_MODE;
      runMode = STEP;
      pc = 0;
      return;

    default:
      showError("noPE"); 
  }
}

/**
   ##################### Operation Helpers #####################
*/
/**
   read two bytes from program (updating PC 2), combine & retuen
*/
unsigned int readAbsoluteAddr() {
  uint8_t lo = program[++pc];
  uint8_t hi = program[++pc];
  return (hi << 8) + lo;
}

void LDi(uint8_t id) {             // Load accumulator <id> immediate
  acc[id] = program[++pc]; // move to next position in program, load into acc
}

void LDa(uint8_t id) { // Load accumulator <id> absolute
  acc[id] = program[readAbsoluteAddr()]; // read next 2 bytes, lookup the value at that address
}

void LDx(uint8_t id) { // Load accumulator A indexed (6502 calls it Indexed Indirect)
  unsigned long addr = xReg;          // start with the index register value
  addr += program[++pc];              // add the next byte in the program
  acc[id] = program[addr];               // look up the value at the total
}

void LDxx(uint8_t id) { // Load accumulator <id> doubly-indexed (6502 calls it Indirect Indexed)
  unsigned long localIndex = readAbsoluteAddr(); // read next 2 bytes,
  unsigned long addr = program[localIndex];      // lookup the value at that address
  addr += xReg;                                   // add the index reg value
  acc[0] = program[addr];                          // look up the value at the result
}

void STa(uint8_t id) {  // Store accumulator <id> absolute
  program[readAbsoluteAddr()] = acc[id]; // read next 2 bytes, store acc value at that address
}

void STx(uint8_t id) { // Store accumulator <id> indexed (6502 calls it Indexed Indirect)
  unsigned long addr = xReg;          // start with the index register value
  addr += program[++pc];              // add the next byte in the program
  program[addr] = acc[id];               // store acc value at the total
}

void STxx(uint8_t id) {  // Store accumulator A doubly-indexed (6502 calls it Indirect Indexed)
  unsigned long localIndex = readAbsoluteAddr(); // read next 2 bytes,
  unsigned long addr = program[localIndex];      // lookup the value at that address
  addr += xReg;                                   // add the index reg value
  program[addr] = acc[id];                          // store acc value at the result
}

void display() {
  displayMode();
  displayPC();
  displayCode();
  showStatus();
}

void welcome() {
  tm.displayText("DOG-1");
  delay(1000);
}

/**
   ############################# Serial Loading of Programs ###############################
*/
void doSerialIn() {
  uint8_t hi = Serial.read();
  uint8_t lo = Serial.read();
  hi = hexCharToValue(hi);
  lo = hexCharToValue(lo);
  uint8_t code = hi * 16 + lo;
  program[pc++] = code;
  stepLED();

  if (code == 0xFF) {
    pc = 0;
    display();
  }
}

uint8_t hexCharToValue(uint8_t hexChar) {
  if (hexChar >= 48 && hexChar <= 57) return hexChar - 48; // '0'...'9' -> 0...9
  if (hexChar >= 65 && hexChar <= 70) return hexChar - 55; // 'A'...'F' -> 10...15
  if (hexChar >= 97 && hexChar <= 102) return hexChar - 87; // 'a'...'f' -> 10...15
  showError("Char");
}


/**
   ##################### LEDs #####################################
*/
void showStatus() {
  uint8_t shifty = status;

  for (uint8_t i = 0; i < 8; i++) {
    tm.setLED(i, shifty & 1);
    shifty = shifty >> 1;
  }
}

void stepLED() {
  for (uint8_t i = 0; i < 8; i++) {
    tm.setLED(i, 0); // switch off
    if (i == ledStep) tm.setLED(i, 1); // switch this one on
  }
  ledStep++;
  if (ledStep == 8) ledStep = 0;
}

/**
    ################## Buttons #####################################
*/
void waitForButton() { // freezes display until button pressed
  delay(500); // a bit dirty, allow time for button release
  while (tm.readButtons() == 0) {
    delay(100);
  }
}

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

    // #################### DUAL BUTTONS ######################
    // ################ System Buttons ###################

    // 0 & 1 full-on reset & wipe
    if ( (buttons & (1 << 0)) && (buttons & (1 << 1))) {
      initRegisters();
      return;
    }

    // 4 & 5 - reset pc
    if ( (buttons & (1 << 4)) && (buttons & (1 << 5))) {
      pc = 0;
      return;
    }

    // ################ Display Register Buttons ###################

    // 0 & 4 - display Accumulators A, B
    if ( (buttons & (1 << 0)) && (buttons & (1 << 4))) {
      displayHex(4, 2, acc[0]);
      displayHex(6, 2, acc[1]);
      waitForButton();
      return;
    }

    // 0 & 5 - display Index Register
    if ( (buttons & (1 << 0)) && (buttons & (1 << 5))) {
      displayHex(4, 4, xReg);
      waitForButton();
      return;
    }

    // 0 & 6 - display PC Stack Pointer
    if ( (buttons & (1 << 0)) && (buttons & (1 << 6))) {
      displayHex(4, 4, pcStackP);
      waitForButton();
      return;
    }

    // 0 & 7 - display ALU Stack Pointer & status
    if ( (buttons & (1 << 0)) && (buttons & (1 << 7))) {
      displayHex(4, 2, aluStackP);
      displayHex(6, 2, status);
      waitForButton();
      return;
    }

    // ################ Mode Buttons ###################

    // 0 & 3 - display PC Stack Pointer
    if ( (buttons & (1 << 0)) && (buttons & (1 << 3)) && mode == RUN_MODE) {
      runMode == RUN;
      return;
    }

    // #################### SINGLE BUTTON ######################
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

    // ################ Edit Buttons ###################

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

    if (buttons & (1 << 0)) { // is out of reach of current size
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

/**
   ######################## 7-Segment Display #######################
*/
void showError(String message) {
  tm.displayText("    " + message);
  displayPC();
  waitForButton();
  mode = PROG_MODE;
}

void displayCode() {
  displayHex(6, 2, program[pc]);
}

void displayPC() {
  displayHex(0, 4, pc);
}

/**
    Display hex values
*/
void displayHex(uint8_t startDisplay, uint8_t nBytes, long data) {

  for (uint8_t i = 0; i < nBytes; i++) { // I tried decrementing, but got errors every time
    if (data > 0) {
      tm.displayHex(nBytes - i + startDisplay - 1, data % 16);
      data = data / 16;
    } else {
      tm.displayHex(nBytes - i + startDisplay - 1, 0);
    }
  }
}

/**
   System mode displays, 4 & 5
*/
void displayMode() {
  if (mode == PROG_MODE) {
    tm.displayASCII(4, 'P');
    if (incdec) {
      tm.displaySS(5, 1); // just the top segment
    } else {
      tm.displaySS(5, 8); // just the tbottom segment
    }
  } else {
    tm.displayASCII(4, 'R');
  }
}

