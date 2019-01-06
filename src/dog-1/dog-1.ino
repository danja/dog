#include <EEPROM.h>
// #include <unwind-cxx.h>
// #include <StandardCplusplus.h>
// #include <system_configuration.h>
//#include <utility.h>
#include <ArduinoSTL.h>

#include <TM1638lite.h>

/**
     Comms with TM1638
     https://github.com/danja/TM1638lite
     arguments are the ports on the Uno
     (I've opted for the ones without extra functions
*/
TM1638lite tm(4, 7, 8);

const int speakerPin = 9;

// TM1638lite tm = TM1638lite::TM1638lite(4, 7, 8);


#define MAX_PROG_SIZE 128 // probably need to move prog elsewhere, is greedy
#define PC_STACK_SIZE 64
#define X_STACK_SIZE 64

#define PROG_MODE false
#define RUN_MODE true
#define STEP false
#define RUN true

/**
   ###################### Flags ##############################
*/
#define NEGATIVE 0
#define OVERFLOW 1
#define ZERO 2
#define CARRY 3

/**
   ###################### Instruction Set ##############################
*/
// START OPCODES # leave this line in place, used by ass.py

// system
#define NOP 0x00 // no operation

// accumulator A load/store
#define LDAi 0x10 // load accumulator A, immediate value
#define LDAa 0x11 // load accumulator A, absolute ref'd address
#define LDAx 0x12 // load accumulator A, indexed 

#define STAa 0x14 // store accumulator A, absolute ref'd address
#define STAx 0x15 // store accumulator A, indexed 


// accumulator B
#define LDBi 0x18 // load accumulator B, immediate value
#define LDBa 0x19 // load accumulator B, absolute ref'd address
#define LDBx 0x1A // load accumulator B, indexed 

#define STBa 0x1C // store accumulator B, absolute ref'd address
#define STBx 0x1D // store accumulator B, indexed 


// logic ops
#define AND 0x20 // bitwise AND of accumulators A & B, result in A
#define OR 0x21 // bitwise OR of accumulators A & B, result in A
#define XOR 0x22 // bitwise XOR of accumulators A & B, result in A
#define COMA 0x23 // accumulator A bitwise complement
#define COMB 0x24 // accumulator A bitwise complement
#define ROLA 0x25 // rotate accumulator A bits left
#define RORA 0x26 // rotate accumulator A bits right
#define ROLB 0x27 // rotate accumulator B bits left
#define RORB 0x28 // rotate accumulator B bits right
#define SWAP 0x29  // swap values between accumulators A & B
#define LSL 0x2A // logical shift left through both accumulators
#define LSR 0x2B // logical shift right through both accumulators

// status register - flag ops
#define CLRS 0x30 // clear status
#define SETS 0x31 // set status
#define SETC 0x32 // set carry
#define CLC 0x33 // clear carry 
#define CLV 0x34 // clear overflow

#define BITAi 0x35 // memory contents AND acc A, immediate, only status affected
#define BITAa 0x36 // memory contents AND acc A, absolute, only status affected
#define BITAx 0x37 // memory contents AND acc A, indexed, only status affected

#define BITBi 0x39 // memory contents AND acc B, immediate, only status affected
#define BITBa 0x3A // memory contents AND acc B, absolute, only status affected
#define BITBx 0x3B // memory contents AND acc B, indexed, only status affected


// Auxiliary Stack
#define PUSHXA 0x40 // push accumulator A onto Auxiliary Stack
#define POPXA 0x41 // pop accumulator A from Auxiliary Stack
#define PUSHXB 0x42 // push accumulator B onto Auxiliary Stack
#define POPXB 0x43 // pop accumulator B from Auxiliary Stack
// see https://www.forth.com/starting-forth/2-stack-manipulation-operators-arithmetic/
#define SWAPS 0x44 // swap top 2 values of Auxiliary Stack a b c => b a c
#define DUP 0x45 // duplicate top value of Auxiliary Stack  a b c => a a b c
#define OVER 0x46 // push copy of 2nd value of Auxiliary Stack on top // a b c => b a b c
#define ROT 0x47 // rotate top 3 values of Auxiliary Stack a b c => c a b
#define DROP 0x48 // delete top valueof Auxiliary Stack a b c => b c
#define TUCK 0x49 // tuck copy of top value of Auxiliary Stack below 2nd value a b c => a b a c

// IMPLEMENTED TO HERE

// Pc-related, jumps etc

// PC stack
#define LSPi 0x50 // load Stack Pointer, immediate
#define LSPa 0x51 // load Stack Pointer, absolute
#define LSPx 0x52 // load Stack Pointer, indexed


#define LDXi 0x54 // load Index Register, immediate
#define LDXa 0x55 // load Index Register, absolute
#define LDXx 0x56 // load Index Register, indexed

#define SPCa 0x58 // store stack pointer, absolute
#define SPCx 0x59 // store stack pointer, indexed

#define PUSHA 0x5A // push accumulator A onto PC stack
#define POPA 0x5B  // pop accumulator A onto PC stack
#define PUSHB 0x5C // push accumulator B onto PC stack
#define POPB 0x5D  // pop accumulator B onto PC stack

// unconditional jumps
#define JMPi 0x60 // immediate jump 
#define JMPa 0x61 // absolute jump
#define BRA 0x63 // relative jump

// subroutine jumps
#define JSRa 0x64 // jump to subroutine absolute
#define JSRr 0x65 // jump to subroutine relative (BSR)
#define RTS 0x66 // return from subroutine

// conditional, relative branches
#define BZS 0x68 // branch if zero set (6800 BEQ)
#define BZC 0x69 // branch if zero clear(BNE)
#define BCS 0x6A // branch if carry set 
#define BCC 0x6B // branch if carry clear
#define BNS 0x6C // branch if negative set (BMI)
#define BNS 0x6D // branch if negative clear (BMI)
#define BVS 0x6E // branch if overflow set 
#define BVC 0x6F // branch if overflow clear 
#define BGE 0x70 // branch if greater than or equal to 0
#define BGT 0x71 // branch if greater than 0
#define BLT 0x72 // branch if less than 0

// arithmetic
#define ABA 0x80 // add B to A

#define ADDAi 0x81 // add to accumulator A, immediate
#define ADDAa 0x82 // add to accumulator A, absolute
#define ADDAx 0x83 // add to accumulator A, indexed

#define ADDBi 0x85 // add to accumulator B, immediate
#define ADDBa 0x86 // add to accumulator B, absolute
#define ADDBx 0x87 // add to accumulator B, indexed
#define ADDBx 0x88 // to accumulator B, doubly-indexed

#define ADCAi 0x89 // add with carry, accumulator A, immediate
#define ADCAa 0x8A // add with carry, accumulator A, absolute
#define ADCAx 0x8B // add with carry, accumulator A, indexed


#define ADCBi 0x8D // add with carry, accumulator B, immediate
#define ADCBa 0x8E // add with carry, accumulator B, absolute
#define ADCBx 0x8F // add with carry, accumulator B, indexed
#define ADCBx 0x90 // add with carry, accumulator B, doubly-indexed

#define SUBAi 0x91 // add to accumulator A, immediate
#define SUBAa 0x92 // add to accumulator A, absolute
#define SUBAx 0x93 // add to accumulator A, indexed


#define SUBBi 0x95 // add to accumulator B, immediate
#define SUBBa 0x96 // add to accumulator B, absolute
#define SUBBx 0x97 // add to accumulator B, indexed
#define SUBBx 0x98 // to accumulator B, doubly-indexed

// logical
#define ANDAi 0xA0 // AND immediate memory contents with accumulator A, result in A
#define ANDAa 0xA1 // AND absolute memory contents with accumulator A, result in A
#define ANDAx 0xA2 // AND indexed memory contents with accumulator A, result in A

#define ANDBi 0xA4 // AND immediate memory contents with accumulator B, result in B
#define ANDBa 0xA5 // AND absolute memory contents with accumulator B, result in B
#define ANDBx 0xA6 // AND indexed memory contents with accumulator B, result in B

#define ORAi 0xA8 // OR immediate memory contents with accumulator A, result in A
#define ORAa 0xA9 // OR absolute memory contents with accumulator A, result in A
#define ORAx 0xAA // OR indexed memory contents with accumulator A, result in A

#define ORBi 0xAC // OR immediate memory contents with accumulator B, result in B
#define ORBa 0xAD // OR absolute memory contents with accumulator B, result in B
#define ORBx 0xAE // OR indexed memory contents with accumulator B, result in B

#define EORAi 0xB0 // EXOR immediate memory contents with accumulator A, result in A
#define EORAa 0xB1 // EXOR absolute memory contents with accumulator A, result in A
#define EORAx 0xB2 // EXOR absolute memory contents with accumulator A, result in 

#define EORBi 0xB4 // EXOR immediate memory contents with accumulator B, result in B
#define EORBa 0xB5 // EXOR absolute memory contents with accumulator B, result in B
#define EORBx 0xB6 // EXOR indexed memory contents with accumulator B, result in B

#define CAB 0xB8 // compare A and B, only status flags affected

#define CMPAi 0xB9 // compare immediate memory and accumulator A, only status flags affected
#define CMPAa 0xBA // compare absolute memory and accumulator A, only status flags affected
#define CMPAx 0xBB // compare indexed memory and accumulator A, only status flags affected

#define CMPBi 0xBD // compare immediate memory and accumulator B, only status flags affected
#define CMPBa 0xBE // compare absolute memory and accumulator B, only status flags affected
#define CMPBx 0xBF // compare indexed memory and accumulator B, only status flags affected


#define CLRA 0xC1 // clear value of accumulator A
#define CLRB 0xC2 // clear value of accumulator B
#define CLRa 0xC3 // clear absolute memory
#define CLRx 0xC4 // clear indexed memory

// increment/decrement registers

#define INCA 0xD0 // increment accumulator A, affects ZNO
#define INCB 0xD1 // increment accumulator B
#define INCa 0xD2 // increment absolute address
#define INCx 0xD3 // increment indexed address
// #define INCS 0xD4  // increment PC Stack pointer
#define INXS 0xD5  // increment Auxiliary Stack pointer
#define INCX 0xD6 // increment Index Register 

#define DECA 0xD7 // decrement accumulator A
#define DECB 0xD8 // decrement accumulator B
#define DECa 0xD9 // decrement absolute address
#define DECx 0xDA // decrement indexed address
// #define DECS 0xDB  // decrement PC Stack pointer
#define DEXS 0xDC  // decrement Auxiliary Stack pointer
#define DECX 0xDD // decrement Index Register 

// hardware-related
#define USE 0xE0 // capture hardware 
#define UNUSE 0xE1 // release hardware 


#define TEMPO 0xF0 // set tempo
#define REST 0xF1 // musical rest
#define TONE 0xF2 // play a tone, immediate
#define TONEAB 0xF3 // play a tone, values from accumulators
#define TONEx 0xF4 // play a tone, value from address in index reg

//debugging/testing
#define RESET 0xF7 // hard reset
#define DUMP 0xF8 // send register contents to serial
#define TEST 0xF9 // run test routine
#define RND 0xFA // load accumulators A & B with random values
#define PAUSE 0xFB // wait for keypress
#define DEBUG 0xFC // sets/reset debugOn
#define OK 0xFD // display ok
#define ERR 0xFE // display err

// finally...
#define HALT 0xFF // terminates

// END OPCODES # leave this line in place, used by ass.py

uint8_t tempo = 120;

// correspond to MIDI notes starting at B0
static const unsigned int notes[] = {31, 33, 35, 37, 39, 41, 44, 46, 49, 52, 55, 58, 62, 65, 69, 73, 78, 82, 87, 93, 98,
                                     104, 110, 117, 123, 131, 139, 147, 156, 165, 175, 185, 196, 208, 220, 233, 247, 262,
                                     277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494, 523, 554, 587, 622, 659, 698,
                                     740, 784, 831, 880, 932, 988, 1047, 1109, 1175, 1245, 1319, 1397, 1480, 1568, 1661,
                                     1760, 1865, 1976, 2093, 2217, 2349, 2489, 2637, 2794, 2960, 3136, 3322, 3520, 3729,
                                     3951, 4186, 4435, 4699, 4978
                                    };
boolean debugOn = false;

boolean mode = PROG_MODE;
boolean runMode = STEP;
boolean pause = false;

uint8_t ledStep = 0;

/**
    ################# Storage ##################
*/
uint8_t program[MAX_PROG_SIZE]; // the code
uint8_t pcStack[PC_STACK_SIZE]; // PC/subroutine-oriented stack
uint8_t xStack[X_STACK_SIZE]; // experimental stack-oriented programming/maths stack

/**
    ################# The Registers ##################
*/
unsigned int pc; // program counter
unsigned int xReg; // index register, 16 bits, 0 to 65535  (or rather, MAX_PROG_SIZE?)
unsigned int pcStackP; // stack pointer, 16 bits

uint8_t acc[2]; // accumulators A & , 8 - bits, -128 to 127
uint8_t xStackP; // stack pointer, 8 bits, 0 to 255

uint8_t status; // status register (flags), initialised with a vaguely helpful test pattern, LEDs over system displays only (4 & 5)

/**
    ################ Init Registers ###################
*/
void initRegisters() {

  resetRegisters();

  for (unsigned int i = 0; i < MAX_PROG_SIZE; i++) { // wipe all instructions
    program[i] = 0; // NOP
  }

  for (unsigned int i = 0; i < PC_STACK_SIZE; i++) { // wipe contents of PC stack
    pcStack[i] = 0;
  }

  for (unsigned int i = 0; i < X_STACK_SIZE; i++) { // wipe contents of ALU stack
    xStack[i] = 0;
  }

  // misc resetting
}

void resetSystem() {
  mode = PROG_MODE;
  ledStep = 0;
  pause = false;
}

void resetRegisters() {
  pc = 0; // program counter
  acc[0] = 0; // accumulator A, 8-bits
  acc[1] = 0; // accumulator B, 8-bits
  xReg = 0; // index register, 16 bits
  pcStackP = 0; // PC stack pointer, 16 bits
  xStackP = 0; // Auxiliary stack pointer, 8 bits
  status = 0x30; // status register (flags), initialised with a vaguely helpful test pattern, LEDs over system displays only (4 & 5)
}

void welcome() {
  tm.displayText("DOG-1");
  delay(1000);
}


/**
   ###############################################################
   ######################## SETUP ################################
   ###############################################################
*/
void setup() {
  welcome(); // display welcome message
  tm.reset(); // reset display hardware
  initRegisters();
  resetSystem();
  Serial.begin(57600); // 9600
}

/**
   ################################################################
   ########################  LOOP #################################
   ################################################################
*/
static boolean recvInProgress = false;
boolean newData = false;
boolean readyToReceive = true;

void loop() {
  if (!recvInProgress) {
    handleButtons();

    // trap overflows
    if (pc >= MAX_PROG_SIZE) pc = 0;
    if (pc < 0) pc = MAX_PROG_SIZE - 1;

    showStatus();

    if (!pause) {
      display();

      if (mode == RUN_MODE) {
        if (runMode == STEP) { // RUN-STEP FIX ME!
          //     waitForButton();

        }
        doOperation();
        pc++;
      }
    }
  }
  if (mode != RUN_MODE && !newData && readyToReceive) {
    receiveProg();
  }
  if (newData) {
    translateProg();
  }
}

/**
   ########################################################################################
   ############################# Serial Loading of Programs ###############################
   ########################################################################################
*/

// Current problem appears to be that thing keeps loading even after it's loaded a prog and before it's run it.
//   readyToReceive seems like !newData..

// note stream.readBytesUntil(character, buffer, length)

uint8_t buffer [2 * MAX_PROG_SIZE]; // maybe buffer isn't needed now I've improved the serial..?
static uint8_t rawSize;
boolean loadToEEPROM;
uint16_t end;

void receiveProg() {
  loadToEEPROM = false;
  static uint8_t ndx = 0;
  char startMarker = '<';
  char endMarker = '>';
  char rc;

  while (Serial.available() > 0 && newData == false) {
    rc = Serial.read();

    if (recvInProgress == true) {
      if (rc != endMarker) {
        buffer[ndx] = rc;
        ndx++;
        if (ndx >= MAX_PROG_SIZE) {
          ndx = MAX_PROG_SIZE - 1;
        }
      } else { // got end marker
        flashMessage("Loaded");
        recvInProgress = false;
        newData = true;
        rawSize = ndx;
        readyToReceive = false;
        if (readLong(0) == 0xFF && readLong(2) == 0xFF) {
          //    loadToEEPROM();
          loadToEEPROM = true;
        }
      }
    } else if (rc == startMarker) {
      mode = PROG_MODE;
      readyToReceive = false;
      recvInProgress = true;
      flashMessage("Loading");
      // pc = 0;
    }
  }
}

void loadFromEEPROM() {
  flashMessage("EEPROM");
  for (uint16_t i = 0; i < end; i++) {
    program[i] = EEPROM.read(i);
  }
}

uint16_t readLong(uint16_t startPos) {
  uint8_t hi = hexCharToValue(buffer[startPos]);
  uint8_t lo = hexCharToValue(buffer[startPos + 1]);
  return hi * 16 + lo;
}
/*
   Move data from buffer into program array
*/
void translateProg() {

  uint16_t start = 0;

  // first two bytes specify program location
  if (!loadToEEPROM) {
    start = readLong(0) * 256 + readLong(2);
  }
  /*
         uint8_t hi = hexCharToValue(buffer[0]);
      uint8_t lo = hexCharToValue(buffer[1]);
      tm.displayHex(4, hi);
      tm.displayHex(5, lo);
      start = readLong(0);
      hi = hexCharToValue(buffer[2]);
      lo = hexCharToValue(buffer[3]);
  */
  pc = start;

  for (uint8_t pos = 4; pos < rawSize - 1; pos = pos + 2) {
    // Serial.write(hi);
    // Serial.write(lo);
    uint8_t hi = hexCharToValue(buffer[pos]); // USE READLONG
    uint8_t lo = hexCharToValue(buffer[pos + 1]);
    uint8_t code = hi * 16 + lo;
    program[pc++] = code;
    delay(10);
    stepLED();
  }
  end = pc;
  newData = false;
  flashMessage("Loaded.");

  pc = start;
  display();
  showStatus();

  if (program[0] == TEST) {
    flashMessage("testing");
    mode = RUN_MODE;
  }

  if (loadToEEPROM) {
    flashMessage("EEPROM");
    for (uint16_t i = 0; i < end; i++) {
      EEPROM[i] = program[i];
    }
  }
}

/*
   ###########################################################
   ####################### Operations ########################
   ###########################################################
*/
void doOperation() {
  uint8_t op = program[pc];
  uint8_t val8; // temporary 8 bit value
  uint8_t val16; // temporary 16 bit value

  switch (op) {

    // ############### system ###############

    case NOP: // no operation
      return;

    //accumulator A load and store

    case LDAi:               // Load accumulator A immediate
      LDi(0);
      debug("LDAi");
      return;

    case LDAa:                            // Load accumulator A absolute
      LDa(0);
      return;

    case LDAx:                            // Load accumulator A indexed (6502 calls it Indexed Indirect)
      LDx(0);
      return;

    case STAa:                            // Store accumulator A absolute
      STa(0);
      return;

    case STAx:                            // Store accumulator A indexed (6502 calls it Indexed Indirect)
      STx(0);
      return;

    // ############### accumulator B load and store

    case LDBi:               // Load accumulator B immediate
      LDi(1);
      return;

    case LDBa:                            // Load accumulator B absolute
      LDa(1);
      return;

    case LDBx:                            // Load accumulator B indexed (6502 calls it Indexed Indirect)
      LDx(1);
      return;

    case STBa:                            // Store accumulator B absolute
      STa(1);
      return;

    case STBx:                            // Store accumulator B indexed (6502 calls it Indexed Indirect)
      STx(1);
      return;

    // ############### Branches

    case BRA:
      pc++;
      pc += (int8_t)program[pc];
      return;

    case BZS:                           // branch if zero set
      pc++;
      if (getFlag(ZERO)) {
        pc += (int8_t)program[pc]; // TODO overflow check, 2's comp
      }
      return;

    case BZC:                           // branch if zero clear
      pc++;
      if (~getFlag(ZERO)) {
        pc += (int8_t)program[pc]; // TODO overflow check, 2's comp
      }
      return;

    //
    case BCS:                           // branch if carry set
      pc++;
      if (getFlag(CARRY)) {
        pc += (int8_t)program[pc]; // TODO overflow check, 2's comp
      }
      return;

    case BCC:                           // branch if carry clear
      pc++;
      if (~getFlag(CARRY)) {
        pc += (int8_t)program[pc]; // TODO overflow check, 2's comp
      }
      return;

    // ############### acc logical operators ###############

    case CAB:                             // compare B with A, only NVZ status flags affected
      if (acc[0] == acc[1]) {
        setFlag(ZERO, true);
        setFlag(OVERFLOW, false);
        setFlag(NEGATIVE, false);
        return;
      } else {
        setFlag(ZERO, false);
      }
      val8 = acc[0] + (~acc[1] + 1);
      setFlag(CARRY, val8 & 128);
      // C bit set if subtraction would require a borrow in the most significant bit of result, otherwise cleared. V flag????

      return;

    case AND: // bitwise AND of accumulators A & B, result in A
      acc[0] = acc[0] & acc[1];
      return;

    case OR: // bitwise OR of accumulators A & B, result in A
      acc[0] = acc[0] | acc[1];
      return;

    case XOR: // bitwise XOR of accumulators A & B, result in A
      acc[0] = acc[0] ^ acc[1];
      return;

    case COMA: // bitwise complement, accumulator A
      acc[0] = ~acc[0];
      return;

    case COMB: // bitwise complement, accumulator B
      acc[1] = ~acc[1];
      return;

    case ROLA: // rotate left accumulator A
      ROL(0);
      return;

    case RORA: // rotate right accumulator A
      ROR(0);
      return;

    case ROLB:  // rotate left accumulator B
      ROL(1);
      return;

    case RORB: // rotate right accumulator B
      ROR(1);
      return;

    case LSL: // logical shift left
      setFlag(CARRY, acc[0] & 0x80);
      acc[0] = acc[0] << 1;
      acc[0] = acc[0] | (acc[1]  & 0x80);
      acc[1] = acc[1] << 1;
      return;

    case LSR: // shift right through both accumulators 0 -> A -> B -> CARRY

      setFlag(CARRY, acc[1] & 1);
      // uint8_t val8 = acc[1] & 1; // get 0th bit of acc B
      acc[1] = acc[1] >> 1; // shift right
      acc[1] = ((acc[0] & 1) << 7) | acc[1]; // move lsb of acc A across
      acc[0] = acc[0] >> 1;
      setFlag(ZERO, acc[0] == 0);
      return;

    case EORAi:
      EORi(0);
      return;

    case EORBi:
      EORi(1);
      return;

    case SWAP: // swap values between accumulator A & B
      val8 = acc[0];
      acc[0] = acc[1];
      acc[1] = val8;
      return;

    // ############# status flag ops #############

    // 76543210
    // ----CZVN

    case CLRS: // clear status flags
      status = 0;
      return;

    case SETS: // set status (immediate)
      status = program[++pc];
      return;

    case SETC: // set carry
      setFlag(CARRY, true);
      return;

    case CLC: // clear carry
      setFlag(CARRY, false);
      return;

    case CLV: // clear overflow
      setFlag(OVERFLOW, false);
      return;

    // Increment operators : affect ZNO

    /*
      #define DECA 0xD7 // increment accumulator A
      #define DECB 0xD8 // increment accumulator B
      #define DECa 0xD9 // increment absolute address
      #define DECx 0xDA // increment indexed address
      // #define DECS 0xDB  // increment PC Stack pointer
      #define DEXS 0xDC  // increment Auxiliary Stack pointer
      #define DECX 0xDD // increment Index Register
    */

    case INCA:
      if (acc[0] == 0xFF) setFlag(OVERFLOW, true);
      acc[0]++;
      doFlags(acc[0]);
      return;

    case DECA:
      if (acc[0] == 0xFF) setFlag(OVERFLOW, true);
      acc[0]++;
      doFlags(acc[0]);
      return;

    case INCB:
      if (acc[1] == 0xFF) setFlag(OVERFLOW, true);
      acc[1]++;
      doFlags(acc[1]);
      return;

    case INCa:
      val16 = readAbsoluteAddr();
      if (program[val16] == 0xFF) {
        setFlag(OVERFLOW, true);
      }
      program[val16]++;
      doFlags(program[val16]);
      return;

    case INCx:
      val16 = xReg;          // start with the index register value
      val16 += program[++pc];              // add the next byte in the program
      val8 = program[val16];               // look up the value at the total
      if (program[val16] == 0xFF) {
        setFlag(OVERFLOW, true);
      }
      program[val16]++;
      doFlags(program[val16]);
      return;

    case INXS:
      if (xStackP == 0xFF) {
        setFlag(OVERFLOW, true);
      }
      xStackP++;
      doFlags(xStackP);
      return;

    case INCX:
      if (xReg == 0xFFFF) {
        setFlag(OVERFLOW, true);
      }
      xReg++;
      doFlags(xReg);
      return;

    /*
         bits 7 and 6 of operand are transfered to bit 7 and 6 of SR (N,V);
         the zeroflag is set to the result of operand AND accumulator.
    */
    case BITAi: // memory contents AND acc, immediate, only status affected
      BITi(0);
      return;

    case BITAa: // memory contents AND acc, absolute, only status affected
     BITa(0);
      return;

    case BITAx: // memory contents AND acc, indexed, only status affected
    BITx(0);
      return;

    case BITBi: // memory contents AND acc, immediate, only status affected
    BITi(1);
      return;

    case BITBa: // memory contents AND acc, absolute, only status affected
      BITa(1);
      return;

    case BITBx: // memory contents AND acc, indexed, only status affected
      BITx(0);
      return;

    // ############# Auxiliary Stack-related #############

    case PUSHXA: // push value in accumulator A to top of ALU Stack
      pushX(0);
      return;

    case POPXA: // pop value from top of ALU Stack into accumulator A
      popX(0);
      return;

    case PUSHXB: // push value in accumulator B to top of X Stack
      pushX(1);
      return;

    case POPXB: // pop value from top of ALU Stack into accumulator B
      popX(1);
      return;

    case SWAPS: // a b c => b a c
      swapX();
      return;

    case DUP: // a b c => a a b c
      dupX();
      return;

    case OVER: // a b c => b a b c
      if (xStackP >= X_STACK_SIZE - 1) {
        showError("OvEr");
      }
      xStackP++;
      xStack[xStackP] = xStack[xStackP - 2];
      return;

    case ROT: //  a b c => c a b
      val8 = xStack[xStackP];
      xStack[xStackP] = xStack[xStackP - 2];
      xStack[xStackP - 2] = xStack[xStackP - 1];
      xStack[xStackP - 1] = val8;
      showError("tESt");
      return;

    case DROP: // a b c => b c
      if (xStackP <= 0) {
        showError("Undr");
      }
      xStackP--;
      showError("tESt");
      return;

    case TUCK: // a b c => a b a c
      swapX();
      dupX();
      showError("tESt");
      return;

/*
 * 
case LSPi: // load Stack Pointer, immediate
 // xStackP =
return;

case LSPa: // load Stack Pointer, absolute
return;

case LSPx: // load Stack Pointer, indexed
return;

case LDXi: // load Index Register, immediate
return;

case LDXai: // load Index Register, absolute
return;

case LDXx: // load Index Register, indexed
return;

 */
    // PC Stack
    // unconditional jumps
    // subroutine jumps
    // conditional, relative branches
    // arithmetic
    // logical
    // increment/decrement registers
    // hardware-related

    // ################# fun
    case TEMPO:
      tempo = program[++pc];
      return;

    case TONE:
      doToneI();
      return;

    case TONEAB:
      doToneAB();
      return;

    case TONEx:
      doToneX();
      return;

    case REST:
      doRest();
      return;

    // ################# debugging/testing

    case RESET:
      flashMessage("reset");
      doReset();
      return;

    case TEST:
      //   resetRegisters();
      // testFlags();
      return;

    case DUMP:
      flashMessage("dump");
      Serial.begin(9600);
      while (!Serial) {
        // wait for serial port to connect. Needed for native USB port only
      }
      delay(1);
      Serial.println("<");
      Serial.println("{");
      Serial.print("\"pc\": ");
      Serial.println(pc);
      Serial.print(",\n\"status\": ");
      Serial.println(status & 0x0F); // ignore extra flags for now
      Serial.print(",\n\"accA\": ");
      Serial.println(acc[0]);
      Serial.print(",\n\"accB\": ");
      Serial.println(acc[1]);
      Serial.print(",\n\"xReg\": ");
      Serial.println(xReg);
      Serial.print(",\n\"pcStackP\": ");
      Serial.println(pcStackP);
      Serial.print(",\n\"xStackP\": ");
      Serial.println(xStackP);
      Serial.println("}");
      Serial.println(">");
      // delay(1000); // wait a sec
      // Serial.end();
      // resetRegisters();
      mode = PROG_MODE;
      flashMessage("dumped");
      newData = false;
      resetRegisters();
      readyToReceive = true;
      // pc = 0;
      return;

    case DEBUG:// flips the state of debug
      debugOn = !debugOn;
      return;

    case RND: // load accumulators A & B with random values
      pause = true;
      return;

    case PAUSE: //
      pause = true;
      displayHex(4, 2, acc[0]);
      displayHex(6, 2, acc[1]);
      waitForButton();
      return;

    case OK:
      tm.displayText("- OK -");
      waitForButton();
      return;

    case ERR:
      showError("Err.");
      return;

    // ## and finally... ##

    case HALT:
      if (!pause) {
        tm.displayASCII (4, 'r') ; // 'run done'
        tm.displayASCII (5, 'd') ;
      }
      waitForButton(); // pause would be better here
      mode = PROG_MODE;
      runMode = STEP;
      pc = 0;
      tm.displayASCII (0, 'S') ;
      waitForButton();
      return;

    default:
      showError("noPE");
  }
}

/**
   #############################################################
   ##################### Operation Helpers #####################
   #############################################################
*/

void doReset() {
  asm volatile ("  jmp 0");
}

void doTone(uint8_t rawNote, uint8_t rawDuration) {
  while (rawNote > 88) {
    rawNote = rawNote - 88;
  }
  unsigned int note = notes[rawNote];
  unsigned long duration = rawDuration * 6000 / tempo;
  tone(speakerPin, note, duration);
  delay(duration);
}

void doToneI() {
  uint8_t rawNote = program[++pc];
  uint8_t rawDuration = program[++pc];
  doTone(rawNote, rawDuration);
}

void doToneAB() {
  doTone(acc[0], acc[1]);
}

void doToneX() {
  unsigned int addr = readAbsoluteAddr();
  doTone(program[addr], program[addr + 1]);
}

void doRest() {
  unsigned long duration = program[++pc] * 6000 / tempo;
  delay(duration);
}
/**
   read two bytes from program (updating PC 2), combine & return
*/
uint16_t readAbsoluteAddr() {
  uint8_t lo = program[++pc];
  uint8_t hi = program[++pc];
  return (hi << 8) + lo;
}

// 0 through 127 are, in hex, $00 through $7F, respectively.
// -128 through -1 are, in hex, $80 through $FF, respectively.

// NEGATIVE 0
// OVERFLOW 1
// ZERO 2
// CARRY 3

void setFlag(uint8_t flag, boolean value) {
  uint8_t mask = (1 << flag);

  if (value) {
    status = status | mask;
  } else {
    status = status & ~mask;
  }
  showStatus();
}

boolean getFlag(uint8_t flag) {
  uint8_t mask = (1 << flag);
  return status & mask;
}

void testFlags() {

  for (uint8_t index = 0; index < 8; ++index) {
    setFlag(index, true);
    delay(200);
  }
  for (uint8_t index = 0; index < 8; ++index) {
    setFlag(index, false);
    delay(200);
  }

  setFlag(ZERO, true);
  setFlag(NEGATIVE, getFlag(ZERO));
  delay(1000);
}

void doFlags(uint16_t val) {
  setFlag(ZERO, val == 0); /////////////////////////// acc[id] == 0
  setFlag(NEGATIVE, val & 128);
}

void LDi(uint8_t id) {             // Load accumulator <id> immediate
  acc[id] = program[++pc]; // move to next position in program, load into acc
  setFlag(OVERFLOW, false);
  doFlags(acc[id]);
}

void LDa(uint8_t id) { // Load accumulator <id> absolute
  acc[id] = program[readAbsoluteAddr()]; // read next 2 bytes, lookup the value at that address
}

void LDx(uint8_t id) { // Load accumulator A indexed (6502 calls it Indexed Indirect)
  unsigned long addr = xReg;          // start with the index register value
  addr += program[++pc];              // add the next byte in the program
  acc[id] = program[addr];               // look up the value at the total
}

void STa(uint8_t id) {  // Store accumulator <id> absolute
  program[readAbsoluteAddr()] = acc[id]; // read next 2 bytes, store acc value at that address
}

void STx(uint8_t id) { // Store accumulator <id> indexed (6502 calls it Indexed Indirect)
  unsigned long addr = xReg;          // start with the index register value
  addr += program[++pc];              // add the next byte in the program
  program[addr] = acc[id];               // store acc value at the total
}

void ROL(uint8_t id) { // rotate left accumulator <id>
  uint8_t temp = acc[id] & 128; // get 7th bit
  acc[id] = acc[id] << 1; // shift left
  acc[id] = acc[id] + (status & CARRY); // load carry flag to bit 0
  status = status | temp; // put previous 7th bit in carry flag
}

void ROR(uint8_t id) { // rotate left accumulator <id>
  uint8_t temp = acc[id] & 1; // get 0th bit
  acc[id] = acc[id] >> 1; // shift right
  acc[id] = acc[id] + 128 * (status & CARRY); // load carry flag to bit 7
  status = status | temp; // put previous 7th bit in carry flag
}

void EORi(uint8_t id) { // EXOR
  acc[id] = acc[id] ^ program[++pc]; // move to next position in program, exor with acc acc
  setFlag(OVERFLOW, false);
  doFlags(acc[id]);
}

// ####################################################################################

void BITi(uint8_t id) {
  uint8_t val = program[pc++];
BIT(id, val);
}

void BITa(uint8_t id) {
uint8_t val = program[readAbsoluteAddr()];
  BIT(id, val);
}

void BITx(uint8_t id) {
  uint8_t val = program[xReg];
BIT(id, val);
}

void BIT(uint8_t id, uint8_t val){
  setFlag(NEGATIVE, val & 128);
  setFlag(OVERFLOW, val & 64); // wrong!?
  if (val && acc[id] == 0) {
    setFlag(ZERO, true);
  } else {
    setFlag(ZERO, false);
  }
}

void pushX(uint8_t id) {
  if (xStackP >= X_STACK_SIZE - 1) {
    showError("OvEr");
  }
  xStack[xStackP++] = acc[id];
}

void popX(uint8_t id) {
  if (xStackP <= 0) {
    showError("Undr");
  }
  acc[id] = xStack[--xStackP];
}

void swapX() {
  uint8_t temp = xStack[xStackP - 1];
  xStack[xStackP - 1] = xStack[xStackP];
  xStack[xStackP] = temp;
}

void dupX() {
  if (xStackP >= X_STACK_SIZE - 1) {
    showError("OvEr");
  }
  xStackP++;
  xStack[xStackP] = xStack[xStackP - 1];
}



/**
   ###################
*/
void display() {
  displayMode();
  displayPC();
  displayCode();
}



uint8_t hexCharToValue(uint8_t hexChar) {
  if (hexChar >= 48 && hexChar <= 57) return hexChar - 48; // '0'...'9' -> 0...9
  if (hexChar >= 65 && hexChar <= 70) return hexChar - 55; // 'A'...'F' -> 10...15
  if (hexChar >= 97 && hexChar <= 102) return hexChar - 87; // 'a'...'f' -> 10...15
  showError("Char");
  mode = PROG_MODE;
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
      resetSystem();
      return;
    }

    // 0 & 2 load from EEPROM
    if ( (buttons & (1 << 0)) && (buttons & (1 << 2))) {
      initRegisters();
      loadFromEEPROM();
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
      displayPC();
      displayHex(4, 2, acc[0]);
      displayHex(6, 2, acc[1]);
      waitForButton();
      return;
    }

    // 0 & 5 - display Index Register
    if ( (buttons & (1 << 0)) && (buttons & (1 << 5))) {
      displayPC();
      displayHex(4, 4, xReg);
      waitForButton();
      return;
    }

    // 0 & 6 - display EEPROM
    if ( (buttons & (1 << 0)) && (buttons & (1 << 6))) {
      displayPC();
      displayHex(6, 2, EEPROM[pc]);
      waitForButton();
      return;
    }

    // 0 & 7 - display Auxiliary Stack Pointer & status
    if ( (buttons & (1 << 0)) && (buttons & (1 << 7))) {
      displayPC();
      displayHex(4, 2, xStackP);
      displayHex(6, 2, status);
      waitForButton();
      return;
    }

    // ################ Mode Buttons ###################

    // 0 & 3 - flip between single-step & free run
    if ( (buttons & (1 << 0)) && (buttons & (1 << 3)) && mode == RUN_MODE) {
      runMode == RUN;
      flashMessage("run");
      return;
    }


    if (pause) {
      if (buttons & (1 << 4)) {
        pause = false;
        return;
      }
      return;
    }

    // #################### SINGLE BUTTON ######################
    if (buttons & (1 << 4)) {
      mode = !mode;
      if (mode == PROG_MODE) {
        pc = 0;
      } else {
        flashMessage("run");
      }
      return;
    }

    if (mode == PROG_MODE) {
      if (buttons & (1 << 5)) {
        incdec = !incdec;
        return;
      }
    }

    // if (mode == RUN_MODE) {
    //   return;
    //}

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

    /*
        if (buttons & (1 << 0)) { // is out of reach of current size
          if (incdec) {
            pc += 4096;
          } else {
            if (pc > 4096) pc -= 4096;
          }
          return;
        }
    */

    /**
       ################################# do code buttons ############
       (6 & 7)- inc/dec value as appropriate, note there's no carry or wrap
    */
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

void debug(String message) {
  if (debugOn) flashMessage(message);
}

/*
  void debug(uint8_t value){
  if(debug) flashMessage(value);
  }
*/

void debug(long value) {
  if (debugOn) flashMessage(value);
}

void flashMessage(String message) {
  tm.displayText(message);
  delay(1000);
}

/*
  void flashMessage(uint8_t value){
  displayHex(4,2, value);
  delay(1000);
  }
*/

void flashMessage(long value) {
  displayHex(4, 4, value);
  delay(1000);
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
  } else { // RUN_MODE
    tm.displayASCII(4, 'R');
    if (runMode == RUN) {
      tm.displayASCII(5, 'R'); // Run
    } else {
      tm.displayASCII(5, 'S'); // Step
    }
  }
}
