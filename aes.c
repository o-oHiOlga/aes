#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "aes.h"

word rotWord(word key){
  word rot = key << 8;
  key>>=24;
  rot= rot|key;
  //printf("KeyRot: %x\n",key&0xffffffff);
  return rot;
}

void cipher (byte in[4*Nb], byte out[4*Nb],word w[Nb*(Nr+1)]){
  byte state[4*Nb];
  //state=in;
}


int main (int argc,char *argv[]){
  byte input[4*Nb]={0x32,0x43,0xf6,0xa8,0x88,0x5a,0x30,0x8d,0x31,0x31,0x98,0xa2,0xe0,0x37,0x07,0x34};
  word key[4] = {0x2b7e1516,0x28aed2a6,0xabf71588,0x09cf4f3c};

  printf("%x\n",key[0]&0xffffffff);

  word rotation = rotWord(key[0]);
  printf("Rotation: %x\n",rotation&0xffffffff);

  byte in[4][4];
  int i,j,k=0;
  for(i=0;i<4;i++){
    for(j=0;j<4;j++){
      in[j][i]=input[k];
      k++;
    }
  }
  //char ch = 0xC0;
//printf("%x", ch & 0xff);

  for(i=0;i<4;i++){
    for(j=0;j<4;j++)
      printf("%x ",in[i][j]&0xff);
    printf("\n" );
  }



return 0;
}
