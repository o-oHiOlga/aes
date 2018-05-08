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
byte * subBytes(byte state[]){
  int r,c,i;
  byte result[16];
  for(i=0;i<16;i++){
    r=(state[i]&0xf0)>>4;
    c=(state[i]&0x0f);
    result[i]=Sbox[r][c];
  }
  return result;
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
    if(i%Nk==0){
      temp=(subWord(rotWord(temp)))^rcon[i/Nk];
    }else if(Nk>6 && i%Nk==4){
      temp=subWord(temp);
    }
    w[i]=w[i-Nk]^temp;
    i=i+1;
  }
}

byte * addRoundKey(byte state[]){
  byte result[16];
  int i,j=0,k;
  for (i=0;i<16;) {
    result[i]=(((w[j]&0xff000000)^(state[i]<<24))>>24)&0xff; //esto es un byte no un word por eso da cero
    result[i+1]=(((w[j]&0x00ff0000)^(state[i+1]<<16))>>16)&0xff;
    result[i+2]=(((w[j]&0x0000ff00)^(state[i+2]<<8))>>8)&0xff;
    result[i+3]=(((w[j]&0x000000ff)^(state[i+3]<<0))>>0)&0xff;
    j=j+1;
    i=i+4;
  }
  return result;
}

int main (int argc,char *argv[]){
  byte input[4*Nb]={0x32,0x43,0xf6,0xa8,0x88,0x5a,0x30,0x8d,0x31,0x31,0x98,0xa2,0xe0,0x37,0x07,0x34};
  byte key[16]={0x2b,0x7e,0x15,0x16,0x28,0xae,0xd2,0xa6,0xab,0xf7,0x15,0x88,0x09,0xcf,0x4f,0x3c};
  byte state[16]={0x32,0x43,0xf6,0xa8,0x88,0x5a,0x30,0x8d,0x31,0x31,0x98,0xa2,0xe0,0x37,0x07,0x34};
  byte *pstate,*pstate1;
  int i;

  keyExpansion(key);
  pstate=addRoundKey(state);
  pstate1=subBytes(pstate);


return 0;
}
