#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "aes.h"



void cipher (byte in[4*Nb], byte out[4*Nb],word w[Nb*(Nr+1)]){

}
byte subWord(word temp){
  int row,column;
  byte postSbox;
  row = temp & 0xffff0000;
  column = temp & 0x0000ffff;
  postSbox=Sbox[row][column];
  return postSbox;
}

void keyExpansion(byte key[4*Nk], word *w[Nb*(Nr+1)]){
  word temp;
  int i=0;
  while (i<Nk){
    w[i]=word(key[4*i],key[4*i+1],key[4*i+2],key[4*i+3]);
    i=i+1;
  }
  i=Nk;
  while(i<Nb*(Nr+1)){
    temp = w[i-1];
    if(i%Nk==0){
      temp=subWord(rotWord(temp))^rcon[i/Nk];
    }else if(Nk>6 && i%Nk==4){
      temp=subWord(temp);
    }
    w[i]=w[i-Nk]^temp;
    i=i+1;
  }
}



int main (int argc,char *argv[]){
  byte input[4*Nb]={0x32,0x43,0xf6,0xa8,0x88,0x5a,0x30,0x8d,0x31,0x31,0x98,0xa2,0xe0,0x37,0x07,0x34};
  word key[4] = {0x2b7e1516,0x28aed2a6,0xabf71588,0x09cf4f3c};
  word keyExp[Nb*(Nr+1)];

  keyExpansion(key[0],&keyExp);

return 0;
}
