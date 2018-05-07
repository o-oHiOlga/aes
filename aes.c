#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "aes.h"

word w[Nb*(Nr+1)];


word subWord(word temp){
  int r,c,i;
  word postSbox;

  for(i=24;i>=0;){
    r=((temp>>i)&0x000000f0)>>4;
    c =((temp>>i)&0x0000000f);
    postSbox=postSbox^(Sbox[r][c]<<i);
    i=i-8;
  }
  return postSbox;
}

void keyExpansion(byte key[4*Nk]){
  word temp;
  int r=0;
  int i=0,j=0;
  while (i<Nk){
    w[i]=joinWord(key[4*i],key[4*i+1],key[4*i+2],key[4*i+3]);
    i=i+1;
  }
  i=Nk;
  while(i<Nb*(Nr+1)){
    temp = w[i-1];
    printf("i:%d temp: %x\n",i,temp );
    if(i%Nk==0){
      temp=(subWord(rotWord(temp)))^rcon[i/Nk];
    }else if(Nk>6 && i%Nk==4){
      temp=subWord(temp);
    }
    w[i]=w[i-Nk]^temp;
    i=i+1;
  }
}



int main (int argc,char *argv[]){
  byte input[4*Nb]={0x32,0x43,0xf6,0xa8,0x88,0x5a,0x30,0x8d,0x31,0x31,0x98,0xa2,0xe0,0x37,0x07,0x34};
  //byte key[4][4] = {{0x2b,0x7e,0x15,0x16},{0x28,0xae,0xd2,0xa6},{0xab,0xf7,0x15,0x88},{0x09,0xcf,0x4f,0x3c}};
  byte key[16]={0x2b,0x7e,0x15,0x16,0x28,0xae,0xd2,0xa6,0xab,0xf7,0x15,0x88,0x09,0xcf,0x4f,0x3c};
  keyExpansion(key);


return 0;
}
