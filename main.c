#include <stdio.h>
/* 8 bit virtual machine
 * similar to 6502
 * 64k of memory
 * Matt Roelle
 */

/* OPCODE TABLE
 *  0x00 load accumulator
 *  0x01 load x register
 *  0x02 load y register
 *  0x03 store accumulator
 *  0x04 store x register
 *  0x05 store y register
 *  0x06 add with carry
 */

typedef unsigned short int word;

unsigned char REG_A; //accumulator
unsigned char REG_X; //general purpose
unsigned char REG_Y; //general purpose

unsigned char FLAGS = 0x00;
unsigned char FLAGS_buffer = FLAGS;

char MEM[0xFFFF]; //64k of memory

int read_params(char params[2], int num_of_args, FILE *input) {
  char mode;
  int ret;
  int i;
  for ( i = 0; i < num_of_args; ++i ) {
    mode = fgetc(input);
    if (mode == 0x00) {
      params[i] = fgetc(input);
      ret = 1; //immediate mode
    }
    else if (mode == 0x01) {
      char high_byte;
      char low_byte;
      high_byte = fgetc(input);
      low_byte = fgetc(input);
      params[i] = high_byte;
      params[i+1] = low_byte;
      i += 1;
      ret = 2; //reference mode
    }
    else {
      ret = -1; //No mode set
    }
  }
  return(ret);
}

int main(int argc, char *argv[]) {
  FILE *input;
  input = fopen(argv[1], "r");
  
  char c;
  int mode;
  char params[2];

  while(c != EOF) {
    c = fgetc(input);
    //read in bytes
    if (c != EOF) {
      switch(c) {
        //when reading params if the first byte is 0x00 then mode is immediate
        //                    if the first byte is 0x01 then the mode is reference
        case 0x00:
          printf("LDA\n"); 
          mode = read_params(params, 1, input);
          if (mode == 1) {
            REG_A = params[0];
          }
          else if (mode == 2) {
            REG_A = MEM[(params[0]*256)+params[1]];
          }
          break;
        case 0x01:
          printf("LDX\n");
          mode = read_params(params, 1, input);
          if (mode == 1) {
            REG_X = params[0];
          }
          else if (mode == 2) {
            REG_X = MEM[(params[0]*256)+params[1]];
          }
          break;
        case 0x02:
          printf("LDY\n");
          mode = read_params(params, 1, input);
          if (mode == 1) {
            REG_Y = params[0];
          }
          else if (mode == 2) {
            REG_Y = MEM[(params[0]*256)+params[1]];
          }
          break;
        case 0x03:
          printf("STA\n");
          read_params(params, 1, input);
          MEM[(params[0]*256)+params[1]] = REG_A;
          break;
        case 0x04:
          printf("STX\n");
          read_params(params, 1, input);
          MEM[(params[0]*256)+params[1]] = REG_X;
          break;
        case 0x05:
          printf("STY\n");
          read_params(params, 1, input);
          MEM[(params[0]*256)+params[1]] = REG_Y;
          break;
        case 0x06:
          printf("ADD\n");
          
          break;
        default:
          printf("illegal instruction\n");
      }
    }
  }
  printf("A REGISTER: %d,\nX REGISTER: %d,\nY REGISTER: %d\n",REG_A, REG_X, REG_Y);
  return(1);
}
