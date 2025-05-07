// TEAM 8

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

// Arithmetic& Logical (Instruction Code)
#define ADD 0x1E
#define SUB 0x1D
#define MUL 0x1B
#define AND 0x1A
#define OR 0x19
#define NOT 0x18
#define XOR 0x17
#define SHL 0x16
#define SHR 0x15

// Data Movement (Instruction code)
#define WM 0x01
#define RM 0x02
#define RIO 0x04
#define WIO 0x05
#define WB 0x06
#define WIB 0x07
#define WACC 0x09
#define RACC 0x0B
#define SWAP 0x0E

// Program Control (Instruction Code)
#define BR 0x03
#define BRE 0x14
#define BRNE 0x13
#define BRGT 0x12
#define BRLT 0x11
#define EOP 0x1F

// Global variables

// Flags
unsigned char FLAGS = 0x00;

/* declaration of IO memory (latches and buffers) */
unsigned char iOData[32]; // 8-bit data x 32 (16 latches and 16 buffers)

// Memory Chip
long A1[32], A2[32], A3[32], A4[32], A5[32], A6[32], A7[32], A8[32];
long B1[32], B2[32], B3[32], B4[32], B5[32], B6[32], B7[32], B8[32];

// External Control Signal
unsigned char CONTROL = 0x00;
int IOM = 0;
int RW = 0;
int OE = 0;

// Buses
unsigned int ADDR = 0x000;
unsigned char BUS = 0x00;

// Function prototypes

// Arithmetic & Logic Unit
int ALU();
unsigned char twosComp(unsigned char data);
unsigned char setFlags(unsigned int ACC);

// Arithmetic functions
unsigned char arithmeticAddition(unsigned char operand1, unsigned char operand2);
unsigned char arithmeticSubtraction(unsigned char operand1, unsigned char operand2);
unsigned char arithmeticMultiplication(unsigned char operand1, unsigned char operand2);

// Logic functions
unsigned char logicalAND(unsigned char operand1, unsigned char operand2);
unsigned char logicalOR(unsigned char operand1, unsigned char operand2);
unsigned char logicalNOT(unsigned char operand1);
unsigned char logicalXOR(unsigned char operand1, unsigned char operand2);
unsigned char logicalShiftRight(unsigned char operand1, int numberOfBits);
unsigned char logicalShiftLeft(unsigned char operand1, int numberOfBits);

unsigned char getLSB_8bit(unsigned char data);
unsigned char getMSB_8bit(unsigned char data);
void printBin(int data, unsigned char data_width);
void printBoothsTable(unsigned char A, unsigned char Q, unsigned char Qneg1, unsigned char M);
void printACC(unsigned int ACC, int numberOfBits);

// Control Unit functions
int CU();
void mainMemory();
void IOMemory();
void initMemory();

// I/O Connection functions
void InputSim();

/*============================================================================================================*/
int main()
{
  initMemory(); // populate memory

  if (CU() == 1)
    printf("Program run successfully!");
  else
    printf("Error encountered, program terminated!");

  return 0;
}

/*==========================================Arithmetic & Logic Unit==========================================*/
int ALU()
{
  static int ACC = 0x000;

  printf("\n*****************************************");
  printf("\nFetching operands...");
  printf("\nOP1 = ");
  printBin(ACC, 8);
  printf("\nOP2 = ");
  printBin(BUS, 8);

  switch (CONTROL)
  {
  case ADD: // Addition
    printf("\nOperation = ADD");
    printf("\nAdding OP1 & OP2");
    ACC = arithmeticAddition(ACC, BUS);
    printACC(ACC, 16);
    break;
  case SUB: // Subtraction
    printf("\nOperation = SUB");
    printf("\nSubtracting OP1 & OP2");
    ACC = arithmeticSubtraction(ACC, BUS);
    printACC(ACC, 16);
    break;
  case MUL: // Multiplication
    printf("\nOperation = MUL");
    ACC = arithmeticMultiplication(ACC, BUS);
    printACC(ACC, 16);
    break;
  case AND: // AND
    printf("\nOperation = AND");
    ACC = logicalAND(ACC, BUS);
    printACC(ACC, 8);
    break;
  case OR: // OR
    printf("\nOperation = OR");
    ACC = logicalOR(ACC, BUS);
    printACC(ACC, 8);
    break;
  case NOT: // NOT
    printf("\nOperation = NOT");
    ACC = logicalNOT(ACC);
    printACC(ACC, 8);
    break;
  case XOR: // XOR
    printf("\nOperation = XOR");
    ACC = logicalXOR(ACC, BUS);
    printACC(ACC, 8);
    break;
  case SHR: // Shift Right
    printf("\nOperation = Shift Right");
    ACC = logicalShiftRight(ACC, BUS);
    printACC(ACC, 8);
    break;
  case SHL: // Shift Left
    printf("\nOperation = Shift Left");
    ACC = logicalShiftLeft(ACC, BUS);
    printACC(ACC, 8);
    break;
  case WACC:
    printf("\nWACC");
    ACC = (unsigned char)BUS;
    printACC(ACC, 16);
    break;
  case RACC:
    printf("\nRACC");
    BUS = ACC;
    printACC(ACC, 16);
    break;
  case BRE:
    printf("\nBRE");
    ACC = (ACC + twosComp(BUS)) & 0x00FF;
    printACC(ACC, 16);
    break;
  case BRNE:
    printf("\nBRNE");
    ACC = (ACC + twosComp(BUS)) & 0x00FF;
    printACC(ACC, 16);
    break;
  case BRGT:
    printf("\nBRGT");
    ACC = (ACC + twosComp(BUS)) & 0x00FF;
    printACC(ACC, 16);
    break;
  case BRLT:
    printf("\nBRLT");
    ACC = (ACC + twosComp(BUS)) & 0x00FF;
    printACC(ACC, 16);
    break;
  default:
    printf("Out of range!\n");
  }

  setFlags(ACC);
  printf("\nZF=%d, CF=%d, SF=%d, OF=%d\n", FLAGS & 0x01, (FLAGS >> 1) & 0x01, (FLAGS >> 2) & 0x01, (FLAGS >> 7) & 0x01);
}

/*============================================================================================================*/
// Function to set the zero, overflow, sign and carry flags
unsigned char setFlags(unsigned int ACC)
{
  FLAGS ^= FLAGS;

  if (CONTROL == 0x17 || CONTROL == 0x18 || CONTROL == 0x19 || CONTROL == 0x1A)
    FLAGS &= 0x01;

  if (CONTROL == 0x15 || CONTROL == 0x16)
    FLAGS &= 0x02;

  if (ACC == 0x0000)
    FLAGS |= 0x01;

  if (ACC > 0x7F)
    FLAGS |= 0x80;

  if ((ACC & 0x0080) == 0x0080)
    FLAGS |= 0x04;

  if (ACC > 0xFF)
    FLAGS |= 0x02;
}

// Function to take the 2’s complement of a number
unsigned char twosComp(unsigned char data)
{
  return ~data + 1;
}

// Function to print ACC in binary characters of data
void printACC(unsigned int ACC, int numberOfBits)
{
  printf("\nACC = ");
  printBin(ACC, numberOfBits);
  printf("\n");
}

// Function to print the Booth's multiplication table
void printBoothsTable(unsigned char A, unsigned char Q, unsigned char Qneg1, unsigned char M)
{
  printBin(A, 8);
  printf(" ");
  printBin(Q, 8);
  printf("   %d", (int)Qneg1);
  printf("   ");
  printBin(M, 8);
  printf("\n");
}

// Function to print binary characters of data
void printBin(int data, unsigned char data_width)
{
  for (int i = data_width - 1; i >= 0; i--)
  {
    putchar((data & (1 << i)) ? '1' : '0');
  }
}

// Function to get the LSB of an 8-bit number
unsigned char getLSB_8bit(unsigned char data)
{
  return data & 0x01;
}

// Function to get the MSB of an 8-bit number
unsigned char getMSB_8bit(unsigned char data)
{
  return (data >> 7) & 0x01;
}

/*============================================================================================================*/
// Arithmetic functions
unsigned char arithmeticAddition(unsigned char operand1, unsigned char operand2)
{
  return operand1 + operand2;
}

unsigned char arithmeticSubtraction(unsigned char operand1, unsigned char operand2)
{
  operand2 = twosComp(operand2);
  return arithmeticAddition(operand1, operand2);
}

unsigned char arithmeticMultiplication(unsigned char operand1, unsigned char operand2)
{
  // uses Booth's Algorithm

  unsigned char A_LSB = 0x00;
  unsigned char A_MSB = 0x00;
  unsigned char ACC_local = 0x0000;

  unsigned char A = 0x00;
  unsigned char Q = operand2;
  unsigned char Qneg1 = 0x00;
  unsigned char Qsub0 = getLSB_8bit(Q);
  unsigned char M = operand1;

  printf("\n    A        Q     Q_-1     M\n");
  printBoothsTable(A, Q, Qneg1, M);

  for (int i = 0; i < 8; i++)
  {
    if ((int)Qsub0 == 1 && (int)Qneg1 == 0)
    {
      A = arithmeticSubtraction(A, M);
      printBoothsTable(A, Q, Qneg1, M);
    }
    else if ((int)Qsub0 == 0 && (int)Qneg1 == 1)
    {
      A = arithmeticAddition(A, M);
      printBoothsTable(A, Q, Qneg1, M);
    }
    A_MSB = getMSB_8bit(A);
    A_LSB = getLSB_8bit(A);
    Qneg1 = getLSB_8bit(Q);

    // Arithmetic Shift Right
    A = logicalOR((A >> 1), (A_MSB << 7));
    Q = logicalOR((Q >> 1), (A_LSB << 7));
    Qsub0 = getLSB_8bit(Q);

    printBoothsTable(A, Q, Qneg1, M);
  }

  ACC_local = A;
  ACC_local = ACC_local << 8;
  return logicalOR(ACC_local, Q);
}

/*============================================================================================================*/
// Logic functions
unsigned char logicalAND(unsigned char operand1, unsigned char operand2)
{
  return operand1 & operand2;
}

unsigned char logicalOR(unsigned char operand1, unsigned char operand2)
{
  return operand1 | operand2;
}

unsigned char logicalNOT(unsigned char operand1)
{
  return ~(operand1);
}

unsigned char logicalXOR(unsigned char operand1, unsigned char operand2)
{
  return operand1 ^ operand2;
}

unsigned char logicalShiftRight(unsigned char operand1, int numberOfBits)
{
  return operand1 >> numberOfBits;
}

unsigned char logicalShiftLeft(unsigned char operand1, int numberOfBits)
{
  return operand1 << numberOfBits;
}

/*===============================================Control Unit===============================================*/
int CU(void)
{
  unsigned int PC = 0x000;
  unsigned int IR = 0x0000;
  unsigned int MAR = 0x000;
  unsigned char MBR = 0x00;
  unsigned int IOAR = 0x000;
  unsigned char IOBR = 0x00;

  unsigned char inst_code = 0x0000;
  unsigned int operand = 0x000;
  int exitCode = 0;

  // Internal control signals
  int _FETCH = 0;
  int _IO = 0;
  int _MEMORY = 0;

  while (!exitCode)
  {
    printf("****************************\n");
    printf("PC\t\t\t: 0x%03X\n", PC);
    printf("Fetching instruction...\n");

    // Setting control signals, main memory access, read operation, allow data movement
    CONTROL = inst_code;
    IOM = 1;
    RW = 0;
    OE = 1;

    // set internal control signal to signify fetch operation
    _FETCH = 1;
    _IO = 0;
    _MEMORY = 0;

    // fetch upper byte
    ADDR = PC;
    IOM = 1;
    RW = 0;
    OE = 1;
    mainMemory();
    IR = (unsigned int)BUS << 8;
    PC++;

    // fetch lower byte
    ADDR = PC;
    mainMemory();
    IR |= BUS;
    PC++;

    printf("IR\t\t\t: 0x%04X\n", IR);

    // Decode instruction
    inst_code = (IR >> 11) & 0x1F;
    operand = IR & 0x07FF;

    printf("Instruction Code\t: 0x%02X\n", inst_code);
    printf("Operand\t\t\t: 0x%03X\n", operand);

    printf("Instruction\t\t: ");

    if (inst_code == WM)
    {
      printf("WM\n");
      MAR = operand;

      // set internal control signals, accessing memory
      _FETCH = 0;
      _MEMORY = 1;
      _IO = 0;

      // set external control signals, accessing main memory, write operation, allow data movement
      CONTROL = inst_code;
      IOM = 1;
      RW = 1;
      OE = 1;

      ADDR = MAR;

      if (_MEMORY)
        BUS = MBR;

      mainMemory();
      printf("Writing data to memory...\n");
    }
    else if (inst_code == RM)
    {
      printf("RM\n");
      MAR = operand;

      // set internal control signals, accessing memory
      _FETCH = 0;
      _MEMORY = 1;
      _IO = 0;

      // set external control signals, accessing main memory, read operation, allow data movement
      CONTROL = inst_code;
      IOM = 1;
      RW = 0;
      OE = 1;

      printf("Reading data to memory...\n");
      ADDR = MAR;
      mainMemory();

      if (_MEMORY)
        MBR = BUS;

      printf("MBR\t\t\t: 0x%02X\n", MBR);
    }
    else if (inst_code == BR)
    {
      printf("BR\n");
      PC = operand;

      // set external control signals
      CONTROL = inst_code;
      OE = 1;
      printf("Branch to 0x%03X on next cycle.\n", PC);
    }
    else if (inst_code == RIO)
    {
      printf("RIO\n");
      IOAR = operand;

      // set internal control signals, accessing I/O
      _FETCH = 0;
      _MEMORY = 0;
      _IO = 1;

      // set external control signals, accessing I/O, read operation, allow data movement
      CONTROL = inst_code;
      IOM = 0;
      RW = 0;
      OE = 1;

      printf("Reading to IO buffer...\n");
      ADDR = IOAR;
      IOMemory();

      if (_IO)
        IOBR = BUS;

      printf("IOBR\t\t\t: 0x%02X\n", IOBR);
    }
    else if (inst_code == WIO)
    {
      printf("WIO\n");
      IOAR = operand;

      // set internal control signals, accessing I/O
      _FETCH = 0;
      _MEMORY = 0;
      _IO = 1;

      // set external control signals, accessing I/O, write operation, allow data movement
      CONTROL = inst_code;
      IOM = 0;
      RW = 1;
      OE = 1;

      printf("Writing to IO buffer...\n");
      ADDR = IOAR;

      if (_IO)
        BUS = IOBR;

      IOMemory();
      printf("IOBR\t\t\t: 0x%02X\n", IOBR);
    }
    else if (inst_code == WB)
    {
      printf("WB\n");

      // set internal control signals, accessing memory
      _FETCH = 0;
      _MEMORY = 1;
      _IO = 0;

      // set external control signals, accessing main memory, write operation, allow data movement
      CONTROL = inst_code;
      IOM = 1;
      RW = 1;
      OE = 1;

      printf("Loading data to MBR...\n");
      ADDR = PC - 1;
      mainMemory();

      if (_MEMORY)
        MBR = BUS;

      printf("MBR\t\t\t: 0x%02X\n", MBR);
    }
    else if (inst_code == WIB)
    {
      printf("WIB\n");

      // set internal control signals, accessing I/O
      _FETCH = 0;
      _MEMORY = 0;
      _IO = 1;

      // set external control signals, accessing I/O, write operation, allow data movement
      CONTROL = inst_code;
      IOM = 0;
      RW = 1;
      OE = 1;

      printf("Loading data to IOBR...\n");
      ADDR = PC - 1;

      if (_IO)
        IOBR = BUS;

      IOMemory();
      printf("IOBR\t\t\t: 0x%02X\n", IOBR);
    }
    else if (inst_code == EOP)
    {
      printf("EOP\n");
      exitCode = 1;
      printf("End of program.\n\n");
      return 1;
    }
    else if (inst_code == ADD || inst_code == SUB || inst_code == MUL || inst_code == AND || inst_code == OR || inst_code == NOT || inst_code == XOR || inst_code == SHL || inst_code == SHR || inst_code == WACC)
    {
      switch (inst_code)
      {
      case ADD:
        printf("ADD\n");
        break;
      case SUB:
        printf("SUB\n");
        break;
      case MUL:
        printf("MUL\n");
        break;
      case AND:
        printf("AND\n");
        break;
      case OR:
        printf("OR\n");
        break;
      case NOT:
        printf("NOT\n");
        break;
      case XOR:
        printf("XOR\n");
        break;
      case SHL:
        printf("SHL\n");
        break;
      case SHR:
        printf("SHR\n");
        break;
      case WACC:
        printf("WACC\n");
        break;
      }

      // set internal control signals
      _FETCH = 0;
      _MEMORY = 1;
      _IO = 0;

      // set external control signals
      CONTROL = inst_code;
      IOM = 0;
      RW = 0;
      OE = 0;

      if (_MEMORY)
        BUS = MBR;

      ALU();
      printf("MBR\t\t\t: 0x%02X\n", MBR);
    }
    else if (inst_code == RACC)
    {
      printf("RACC\n");

      // set internal control signals
      _FETCH = 0;
      _MEMORY = 1;
      _IO = 0;

      // set external control signals
      CONTROL = inst_code;
      IOM = 0;
      RW = 0;
      OE = 0;

      ALU();
      if (_MEMORY)
        MBR = BUS;

      printf("Reading ACC to BUS...\n");
      printf("MBR\t\t\t: 0x%02X\n", MBR);
    }
    else if (inst_code == SWAP)
    {
      unsigned char temp;
      printf("SWAP\n");

      // set internal control signals
      _FETCH = 0;
      _MEMORY = 0;
      _IO = 0;

      // set external control signals
      CONTROL = inst_code;
      IOM = 0;
      RW = 0;
      OE = 0;

      printf("Swapping MBR and IOBR...\n");
      MBR = MBR ^ IOBR;
      IOBR = MBR ^ IOBR;
      MBR = MBR ^ IOBR;

      printf("MBR\t\t\t: 0x%02X\n", MBR);
      printf("IOBR\t\t\t: 0x%02X\n", IOBR);
    }
    else if (inst_code == BRE || inst_code == BRNE || inst_code == BRGT || inst_code == BRLT)
    {
      switch (inst_code)
      {
      case BRE:
        printf("BRE\n");
        break;
      case BRNE:
        printf("BRNE\n");
        break;
      case BRGT:
        printf("BRGT\n");
        break;
      case BRLT:
        printf("BRLT\n");
        break;
      }

      // set internal control signals
      _FETCH = 0;
      _MEMORY = 1;
      _IO = 0;

      // set external control signals
      CONTROL = inst_code;
      IOM = 0;
      RW = 0;
      OE = 0;

      ADDR = operand;

      if (_MEMORY)
        BUS = MBR;

      printf("Comparing ACC and BUS...\n");
      ALU();

      switch (inst_code)
      {
      case BRE:
        if (FLAGS & 0x01) // Z
        {
          PC = ADDR;
          OE = 1;
          printf("Branch tp 0x%03X on next cycle\n", PC);
        }
        break;
      case BRNE:
        if (!(FLAGS & 0x01)) // !Z
        {
          PC = ADDR;
          OE = 1;
          printf("Branch tp 0x%03X on next cycle\n", PC);
        }
        break;
      case BRGT:
        if (!((FLAGS >> 2) & 0x01)) // !SF
        {
          PC = ADDR;
          OE = 1;
          printf("Branch tp 0x%03X on next cycle\n", PC);
        }
        break;
      case BRLT:
        if ((FLAGS >> 2) & 0x01) // SF
        {
          PC = ADDR;
          OE = 1;
          printf("Branch tp 0x%03X on next cycle\n", PC);
        }
        break;
      }
    }
    else
    {
      printf("Unknown instruction\n");
      return 0;
    }

    printf("BUS\t\t: 0x%02X\n", BUS);
    printf("ADDR\t\t: 0x%03X\n", ADDR);
    printf("PC\t\t: 0x%03X\n", PC);
    printf("MAR\t\t: 0x%02X\n", MAR);
    printf("IOAR\t\t: 0x%02X\n", IOAR);
    printf("IOBR\t\t: 0x%02X\n", IOBR);
    printf("CONTROL\t\t: 0x%02X\n", CONTROL);
    getch();
  }

  return 1;
}

void mainMemory(void)
{
  unsigned int col, row, cs;
  unsigned int temp = 0x00;

  if (OE)
  {
    // Decoding address data
    col = ADDR & 0x001F;
    row = (ADDR >> 5) & 0x001F;
    cs = ADDR >> 10;

    if (IOM == 1)
    {
      if (RW) // Memory Write
      {
        if (cs)
        {
          B1[row] = (BUS & 0x01) ? (B1[row] | (0x01 << col)) : (B1[row] & ~(0x01 << col));
          B2[row] = (BUS & 0x02) ? (B2[row] | (0x01 << col)) : (B2[row] & ~(0x01 << col));
          B3[row] = (BUS & 0x04) ? (B3[row] | (0x01 << col)) : (B3[row] & ~(0x01 << col));
          B4[row] = (BUS & 0x08) ? (B4[row] | (0x01 << col)) : (B4[row] & ~(0x01 << col));
          B5[row] = (BUS & 0x10) ? (B5[row] | (0x01 << col)) : (B5[row] & ~(0x01 << col));
          B6[row] = (BUS & 0x20) ? (B6[row] | (0x01 << col)) : (B6[row] & ~(0x01 << col));
          B7[row] = (BUS & 0x40) ? (B7[row] | (0x01 << col)) : (B7[row] & ~(0x01 << col));
          B8[row] = (BUS & 0x80) ? (B8[row] | (0x01 << col)) : (B8[row] & ~(0x01 << col));
        }
        else
        {
          A1[row] = (BUS & 0x01) ? (A1[row] | (0x01 << col)) : (A1[row] & ~(0x01 << col));
          A2[row] = (BUS & 0x02) ? (A2[row] | (0x01 << col)) : (A2[row] & ~(0x01 << col));
          A3[row] = (BUS & 0x04) ? (A3[row] | (0x01 << col)) : (A3[row] & ~(0x01 << col));
          A4[row] = (BUS & 0x08) ? (A4[row] | (0x01 << col)) : (A4[row] & ~(0x01 << col));
          A5[row] = (BUS & 0x10) ? (A5[row] | (0x01 << col)) : (A5[row] & ~(0x01 << col));
          A6[row] = (BUS & 0x20) ? (A6[row] | (0x01 << col)) : (A6[row] & ~(0x01 << col));
          A7[row] = (BUS & 0x40) ? (A7[row] | (0x01 << col)) : (A7[row] & ~(0x01 << col));
          A8[row] = (BUS & 0x80) ? (A8[row] | (0x01 << col)) : (A8[row] & ~(0x01 << col));
        }
      }
      else // Memory Read
      {
        if (cs)
        {
          BUS = ((B1[row] >> col) & 0x01) ? (BUS | 0x01) : (BUS & ~0x01);
          BUS = ((B2[row] >> col) & 0x01) ? (BUS | 0x02) : (BUS & ~0x02);
          BUS = ((B3[row] >> col) & 0x01) ? (BUS | 0x04) : (BUS & ~0x04);
          BUS = ((B4[row] >> col) & 0x01) ? (BUS | 0x08) : (BUS & ~0x08);
          BUS = ((B5[row] >> col) & 0x01) ? (BUS | 0x10) : (BUS & ~0x10);
          BUS = ((B6[row] >> col) & 0x01) ? (BUS | 0x20) : (BUS & ~0x20);
          BUS = ((B7[row] >> col) & 0x01) ? (BUS | 0x40) : (BUS & ~0x40);
          BUS = ((B8[row] >> col) & 0x01) ? (BUS | 0x80) : (BUS & ~0x80);
        }
        else
        {
          BUS = ((A1[row] >> col) & 0x01) ? (BUS | 0x01) : (BUS & ~0x01);
          BUS = ((A2[row] >> col) & 0x01) ? (BUS | 0x02) : (BUS & ~0x02);
          BUS = ((A3[row] >> col) & 0x01) ? (BUS | 0x04) : (BUS & ~0x04);
          BUS = ((A4[row] >> col) & 0x01) ? (BUS | 0x08) : (BUS & ~0x08);
          BUS = ((A5[row] >> col) & 0x01) ? (BUS | 0x10) : (BUS & ~0x10);
          BUS = ((A6[row] >> col) & 0x01) ? (BUS | 0x20) : (BUS & ~0x20);
          BUS = ((A7[row] >> col) & 0x01) ? (BUS | 0x40) : (BUS & ~0x40);
          BUS = ((A8[row] >> col) & 0x01) ? (BUS | 0x80) : (BUS & ~0x80);
        }
      }
    }
  }
}

void IOMemory(void)
{
  if (OE)
  {
    if (RW && !IOM) // check if memory write and IO Memory access
    {
      if (ADDR >= 0x000 && ADDR <= 0x00F) // check the address if valid
        iOData[ADDR] = BUS;               // write data in BUS to IO Memory
    }
    else
    {
      if (ADDR >= 0x010 && ADDR <= 0x01F) // check the address if valid
        BUS = iOData[ADDR];               // load data to BUS
    }
  }
}

void initMemory(void)
{
  printf("Initializing Main Memory...\n\n");

  // Setting the global control signals
  IOM = 1, RW = 1, OE = 1;

  // Format ADDR=<program memory address>; BUS=<instruction>; MainMemory()
  // Calling MainMemory() writes the instruction to memory

  ADDR = 0x000; BUS = 0x30; mainMemory();
  ADDR = 0x001; BUS = 0x15; mainMemory();
  ADDR = 0x002; BUS = 0x0C; mainMemory();
  ADDR = 0x003; BUS = 0x00; mainMemory();
  ADDR = 0x004; BUS = 0x30; mainMemory();
  ADDR = 0x005; BUS = 0x05; mainMemory();
  ADDR = 0x006; BUS = 0x48; mainMemory();
  ADDR = 0x007; BUS = 0x00; mainMemory();
  ADDR = 0x008; BUS = 0x30; mainMemory();
  ADDR = 0x009; BUS = 0x08; mainMemory();
  ADDR = 0x00A; BUS = 0xF0; mainMemory();
  ADDR = 0x00B; BUS = 0x00; mainMemory();
  ADDR = 0x00C; BUS = 0x14; mainMemory();
  ADDR = 0x00D; BUS = 0x00; mainMemory();
  ADDR = 0x00E; BUS = 0xD8; mainMemory();
  ADDR = 0x00F; BUS = 0x00; mainMemory();
  ADDR = 0x010; BUS = 0x58; mainMemory();
  ADDR = 0x011; BUS = 0x00; mainMemory();
  ADDR = 0x012; BUS = 0x0C; mainMemory();
  ADDR = 0x013; BUS = 0x01; mainMemory();
  ADDR = 0x014; BUS = 0x38; mainMemory();
  ADDR = 0x015; BUS = 0x0B; mainMemory();
  ADDR = 0x016; BUS = 0x28; mainMemory();
  ADDR = 0x017; BUS = 0x00; mainMemory();
  ADDR = 0x018; BUS = 0x30; mainMemory();
  ADDR = 0x019; BUS = 0x10; mainMemory();
  ADDR = 0x01A; BUS = 0xE8; mainMemory();
  ADDR = 0x01B; BUS = 0x00; mainMemory();
  ADDR = 0x01C; BUS = 0x58; mainMemory();
  ADDR = 0x01D; BUS = 0x00; mainMemory();
  ADDR = 0x01E; BUS = 0x28; mainMemory();
  ADDR = 0x01F; BUS = 0x01; mainMemory();
  ADDR = 0x020; BUS = 0xB0; mainMemory();
  ADDR = 0x021; BUS = 0x00; mainMemory();
  ADDR = 0x022; BUS = 0xB0; mainMemory();
  ADDR = 0x023; BUS = 0x00; mainMemory();
  ADDR = 0x024; BUS = 0xA8; mainMemory();
  ADDR = 0x025; BUS = 0x00; mainMemory();
  ADDR = 0x026; BUS = 0x14; mainMemory();
  ADDR = 0x027; BUS = 0x01; mainMemory();
  ADDR = 0x028; BUS = 0xC8; mainMemory();
  ADDR = 0x029; BUS = 0x00; mainMemory();
  ADDR = 0x02A; BUS = 0xC0; mainMemory();
  ADDR = 0x02B; BUS = 0x00; mainMemory();
  ADDR = 0x02C; BUS = 0x20; mainMemory();
  ADDR = 0x02D; BUS = 0x01; mainMemory();
  ADDR = 0x02E; BUS = 0x70; mainMemory();
  ADDR = 0x02F; BUS = 0x00; mainMemory();
  ADDR = 0x030; BUS = 0xB8; mainMemory();
  ADDR = 0x031; BUS = 0x00; mainMemory();
  ADDR = 0x032; BUS = 0x30; mainMemory();
  ADDR = 0x033; BUS = 0xFF; mainMemory();
  ADDR = 0x034; BUS = 0xD0; mainMemory();
  ADDR = 0x035; BUS = 0x00; mainMemory();
  ADDR = 0x036; BUS = 0x14; mainMemory();
  ADDR = 0x037; BUS = 0x01; mainMemory();
  ADDR = 0x038; BUS = 0xA0; mainMemory();
  ADDR = 0x039; BUS = 0x3C; mainMemory();
  ADDR = 0x03A; BUS = 0x30; mainMemory();
  ADDR = 0x03B; BUS = 0x2E; mainMemory();
  ADDR = 0x03C; BUS = 0x90; mainMemory();
  ADDR = 0x03D; BUS = 0x42; mainMemory();
  ADDR = 0x03E; BUS = 0xD0; mainMemory();
  ADDR = 0x03F; BUS = 0x00; mainMemory();
  ADDR = 0x040; BUS = 0x88; mainMemory();
  ADDR = 0x041; BUS = 0x44; mainMemory();
  ADDR = 0x042; BUS = 0x30; mainMemory();
  ADDR = 0x043; BUS = 0x00; mainMemory();
  ADDR = 0x044; BUS = 0x48; mainMemory();
  ADDR = 0x045; BUS = 0x00; mainMemory();
  ADDR = 0x046; BUS = 0x30; mainMemory();
  ADDR = 0x047; BUS = 0x03; mainMemory();
  ADDR = 0x048; BUS = 0x48; mainMemory();
  ADDR = 0x049; BUS = 0x00; mainMemory();
  ADDR = 0x04A; BUS = 0x30; mainMemory();
  ADDR = 0x04B; BUS = 0x00; mainMemory();
  ADDR = 0x04C; BUS = 0xA0; mainMemory();
  ADDR = 0x04D; BUS = 0x52; mainMemory();
  ADDR = 0x04E; BUS = 0x30; mainMemory();
  ADDR = 0x04F; BUS = 0x01; mainMemory();
  ADDR = 0x050; BUS = 0xE8; mainMemory();
  ADDR = 0x051; BUS = 0x00; mainMemory();
  ADDR = 0x052; BUS = 0x18; mainMemory();
  ADDR = 0x053; BUS = 0x48; mainMemory();
  ADDR = 0x054; BUS = 0xF8; mainMemory();
}

/*===============================================I/O Connection===============================================*/
void InputSim()
{
  unsigned char data;

  for (int i = 7; i >= 0; i--)
  {
    /* load source */
    data = iOData[0x001];
    /* shift bit to LSB */
    data = data >> i;
    /* mask upper bit */
    data = data & 0x01;
    /* position bit */
    data = data << (7 - i);
    /* write bit to dest buffer */
    iOData[0x01F] = iOData[0x01F] | data;
  }
}
