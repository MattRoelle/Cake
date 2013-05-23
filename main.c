#include <stdio.h>
#include <stdlib.h>
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
 *  0x07 subtract with carry
 */

typedef unsigned short int word;

unsigned char REG_A; //accumulator
unsigned char REG_X; //general purpose
unsigned char REG_Y; //general purpose

unsigned char FLAGS = 0x00;
unsigned char FLAGS_buffer = 0x00;

char MEM[0xFFFF]; //64k of memory

int read_params(unsigned char params[2], int num_of_args, char *prog, int *pp) {
  unsigned char mode;
  int ret;
  int i;
  for ( i = 0; i < num_of_args; ++i ) {
    mode = prog[*pp];
    *pp = *pp + 1;
    if (mode == 0x00) {
      params[i] = prog[*pp];
      *pp = *pp + 1;
      ret = 1; //immediate mode
    }
    else if (mode == 0x01) {
      char high_byte;
      char low_byte;
      high_byte = prog[*pp];
      *pp = *pp + 1;
      low_byte = prog[*pp];
      *pp = *pp + 1;
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
  
  int prog_size = 0;
  unsigned char *prog_buffer = malloc(sizeof(char));

  unsigned char c;

  while(c != EOF) {
    prog_size++;
    c = fgetc(input);
    //read in bytes
    if (c != EOF) {
      realloc(prog_buffer, prog_size*sizeof(char));
      prog_buffer[prog_size-1] = c;
    }
  }

  int mode;
  unsigned char params[2];
  int i = 0;

  printf("prog_size: %d\n", prog_size);

  while (i < prog_size-1) {
    c = prog_buffer[i];
    i++;
    printf("pp: %d\n",i);
    switch(c) {
      //when reading params if the first byte is 0x00 then mode is immediate
      //                    if the first byte is 0x01 then the mode is reference
      case 0x00:
        printf("LDA\n"); 
        mode = read_params(params, 1, prog_buffer, &i);
        if (mode == 1) {
          REG_A = params[0];
        }
        else if (mode == 2) {
          REG_A = MEM[(params[0]*256)+params[1]];
        }
        break;
      case 0x01:
        printf("LDX\n");
        mode = read_params(params, 1, prog_buffer, &i);
        if (mode == 1) {
          REG_X = params[0];
        }
        else if (mode == 2) {
          REG_X = MEM[(params[0]*256)+params[1]];
        }
        break;
      case 0x02:
        printf("LDY\n");
        mode = read_params(params, 1, prog_buffer, &i);
        if (mode == 1) {
          REG_Y = params[0];
        }
        else if (mode == 2) {
          REG_Y = MEM[(params[0]*256)+params[1]];
        }
        break;
      case 0x03:
        printf("STA\n");
        read_params(params, 1, prog_buffer, &i);
        MEM[(params[0]*256)+params[1]] = REG_A;
        break;
      case 0x04:
        printf("STX\n");
        read_params(params, 1, prog_buffer, &i);
        MEM[(params[0]*256)+params[1]] = REG_X;
        break;
      case 0x05:
        printf("STY\n");
        read_params(params, 1, prog_buffer, &i);
        MEM[(params[0]*256)+params[1]] = REG_Y;
        break;
      case 0x06:
        printf("ADD\n");
        mode = read_params(params,  1, prog_buffer, &i);
        if (mode == 1) {
          REG_A += params[0];
        }
        else if (mode == 2) {
          REG_A += MEM[(params[0]*256)+params[1]];
        }
        if (REG_A > 0xFF) {
          char difference;
          difference = REG_A - 0xFF;
          REG_A = difference; //accumulator becomes the sum - 255
          FLAGS |= 1 << 0; //set carry flag
        }
        break;
      case 0x07:
        printf("SUB\n");
        mode = read_params(params,  1, prog_buffer, &i);
        if (mode == 1) {
          REG_A -= params[0];
        }
        else if (mode == 2) {
          REG_A -= MEM[(params[0]*256)+params[1]];
        }
        if (REG_A < 0x00) {
          char difference;
          REG_A = 0xFF + REG_A;
          FLAGS |= 1 << 0; //set carry flag
        } 
        break;
      default:
        printf("illegal instruction\n");
    }
  }

  printf("A REGISTER: %d,\nX REGISTER: %d,\nY REGISTER: %d\n",REG_A, REG_X, REG_Y);
  return(1);
}
