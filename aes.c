#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "aes.h"

void imprimeStatus(byte* state){
  int i;
  for(i=0;i<16;i++)
    printf("%02x",state[i]);
  printf("\n");
}

word subWord(word temp){
  int r,c,i;
  word postSbox=0;
  for(i=24;i>=0;i-=8){
    r=((temp>>i)&0x000000f0)>>4;
    c =((temp>>i)&0x0000000f);
    postSbox=postSbox^(Sbox[r][c]<<i);
  }
  return postSbox;
}
byte * subBytes(byte state[]){
  int r,c,i;
  byte* result=(byte *)calloc(16,sizeof(byte));
  for(i=0;i<16;i++){
    r=(state[i]&0xf0)>>4;
    c=(state[i]&0x0f);
    result[i]=Sbox[r][c];
  }
  return result;
}
byte * invSubBytes(byte state[]){
  int r,c,i;
  byte* result=(byte *)calloc(16,sizeof(byte));
  for(i=0;i<16;i++){
    r=(state[i]&0xf0)>>4;
    c=(state[i]&0x0f);
    result[i]=invSbox[r][c];
  }
  return result;
}
byte * shiftRows(byte state[]){
  byte* result=(byte *)malloc(16*sizeof(byte));
  word aux;
  int i=1,j;
  for(j=0;j<16;j++)
    result[j]=state[j];

  for(i=1;i<4;i++){
    aux=joinWord(result[i],result[i+4],result[i+8],result[i+12]);
    if(i==1){
      aux=(aux>>24)|aux<<8;
      result[i]=(aux>>24)&0xff;
      result[i+4]=(aux>>16)&0xff;
      result[i+8]=(aux>>8)&0xff;
      result[i+12]=aux&0xff;
    }else if(i==2){
      aux=(aux>>16)|(aux<<16);
      result[i]=(aux>>24)&0xff;
      result[i+4]=(aux>>16)&0xff;
      result[i+8]=(aux>>8)&0xff;
      result[i+12]=aux&0xff;
    }else if(i==3){
      aux=(aux>>8)|(aux<<24);
      result[i]=(aux>>24)&0xff;
      result[i+4]=(aux>>16)&0xff;
      result[i+8]=(aux>>8)&0xff;
      result[i+12]=aux&0xff;
    }
  }
  return result;
}

void keyExpansion(byte* key,word* w){
  word temp;
  int i=0;
  while (i<Nk){
    w[i] = joinWord(key[4*i],key[4*i+1],key[4*i+2],key[4*i+3]);
    i=i+1;
  }
  i=Nk;
  while(i < Nb * (Nr + 1)){
    temp = w[i-1];
    if(i % Nk == 0){
      temp=(subWord(rotWord(temp))) ^ rcon[i/Nk];
    } else if(Nk > 6 && i % Nk == 4){
      temp=subWord(temp);
    }
    w[i]=w[i-Nk] ^ temp;
    //printf("key1: %x\n",w[i]);
    i=i+1;
  }
}

byte xtime(byte x){
  byte r;
  r=((x << 1) ^ (x & 0x80 ? 0x1b : 0))& 0xff;
  return r;
}
byte * mixColumns(byte state[]){
  byte* result=(byte *)calloc(16,sizeof(byte));
  int i;
  for(i=0;i<16;i+=4){
    result[i]=(xtime(state[i]))^(state[i+1]^xtime(state[i+1]))^state[i+2]^state[i+3];
    result[i+1]=state[i]^(xtime(state[i+1]))^(state[i+2]^xtime(state[i+2]))^(state[i+3]);
    result[i+2]=state[i]^state[i+1]^(xtime(state[i+2]))^(state[i+3]^xtime(state[i+3]));
    result[i+3]=(state[i]^xtime(state[i]))^state[i+1]^state[i+2]^(xtime(state[i+3]));
  }
  return result;
}
byte * InvMixColumns(byte state[]){
  byte* result=(byte *)malloc(16*sizeof(byte));
  byte xt8, xt4, xt2;
  byte x1t8, x1t4, x1t2;
  byte x2t8, x2t4, x2t2;
  byte x3t8, x3t4, x3t2;
  int i;
  for(i=0;i<16;i+=4){
    xt2=xtime(state[i]);
    xt4=xtime(xt2);
    xt8=xtime(xt4);
    x1t2=xtime(state[i+1]);
    x1t4=xtime(x1t2);
    x1t8=xtime(x1t4);
    x2t2=xtime(state[i+2]);
    x2t4=xtime(x2t2);
    x2t8=xtime(x2t4);
    x3t2=xtime(state[i+3]);
    x3t4=xtime(x3t2);
    x3t8=xtime(x3t4);
    result[i]=(xt8^xt4^xt2)^(x1t8^x1t2^state[i+1])^(x2t8^x2t4^state[i+2])^(x3t8^state[i+3]);
    result[i+1]=(xt8^state[i])^(x1t8^x1t4^x1t2)^(x2t8^x2t2^state[i+2])^(x3t8^x3t4^state[i+3]);
    result[i+2]=(xt8^xt4^state[i])^(x1t8^state[i+1])^(x2t8^x2t4^x2t2)^(x3t8^x3t2^state[i+3]);
    result[i+3]=(xt8^xt2^state[i])^(x1t8^x1t4^state[i+1])^(x2t8^state[i+2])^(x3t8^x3t4^x3t2);
    //printf("%02x %02x %02x %02x\n", result[i],result[i+1],result[i+2],result[i+3] );
  }
  return result;
}

byte * addRoundKey(byte state[],int r,word* w){
  byte* result=(byte *)calloc(16,sizeof(byte));
  int i,j=r*4;
  for (i=0;i<16;) {
    result[i]=(((w[j]&0xff000000)^(state[i]<<24))>>24)&0xff;
    result[i+1]=(((w[j]&0x00ff0000)^(state[i+1]<<16))>>16)&0xff;
    result[i+2]=(((w[j]&0x0000ff00)^(state[i+2]<<8))>>8)&0xff;
    result[i+3]=(((w[j]&0x000000ff)^(state[i+3]<<0))>>0)&0xff;
    j=j+1;
    i=i+4;
  }
  return result;
}

byte * invShiftRows(byte state[]){
  byte* result=(byte *)malloc(16*sizeof(byte));
  word aux;
  int i=1,j;
  for(j=0;j<16;j++)
    result[j]=state[j];

  for(i=1;i<4;i++){
    aux=joinWord(result[i],result[i+4],result[i+8],result[i+12]);
    if(i==1){
      aux=(aux>>8)|aux<<24;
      result[i]=(aux>>24)&0xff;
      result[i+4]=(aux>>16)&0xff;
      result[i+8]=(aux>>8)&0xff;
      result[i+12]=aux&0xff;
    }else if(i==2){
      aux=(aux>>16)|(aux<<16);
      result[i]=(aux>>24)&0xff;
      result[i+4]=(aux>>16)&0xff;
      result[i+8]=(aux>>8)&0xff;
      result[i+12]=aux&0xff;
    }else if(i==3){
      aux=(aux<<8)|(aux>>24);
      result[i]=(aux>>24)&0xff;
      result[i+4]=(aux>>16)&0xff;
      result[i+8]=(aux>>8)&0xff;
      result[i+12]=aux&0xff;
    }
  }
  return result;
}

byte * cipher(byte in[],word* w){
  byte* out=(byte *)calloc(16,sizeof(byte));
  byte* state=(byte *)calloc(16,sizeof(byte));
  int i,l;
  for(i=0;i<16;i++)
    state[i]=in[i];
  state=addRoundKey(state,0,w);
  for(l=1;l<Nr;l++){
    state=subBytes(state);
    state=shiftRows(state);
    state=mixColumns(state);
    state=addRoundKey(state,l,w);
  }
  state=subBytes(state);
  state=shiftRows(state);
  state=addRoundKey(state,Nr,w);

  out=state;

  return out;
}

byte * invCipher(byte* in,word* w){
  byte* out=(byte *)calloc(16,sizeof(byte));
  byte* state=(byte *)calloc(16,sizeof(byte));
  int i,l;
  for(i=0;i<16;i++)
      state[i]=in[i];
  state=addRoundKey(state,Nr,w);
  for(l=Nr-1;l>0;l--){
    state=invShiftRows(state);
    state=invSubBytes(state);
    state=addRoundKey(state,l,w);
    state=InvMixColumns(state);
    printf("InvMixColumns:\n");
    imprimeStatus(state);
  }
  state=invShiftRows(state);
  state=invSubBytes(state);
  state=addRoundKey(state,0,w);
  out=state;
  return out;
}

int main (int argc,char *argv[]){
  //byte input[4*Nb]={0x32,0x43,0xf6,0xa8,0x88,0x5a,0x30,0x8d,0x31,0x31,0x98,0xa2,0xe0,0x37,0x07,0x34};
  //byte key[16]={0x2b,0x7e,0x15,0x16,0x28,0xae,0xd2,0xa6,0xab,0xf7,0x15,0x88,0x09,0xcf,0x4f,0x3c};
  byte input[16]={0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xaa,0xbb,0xcc,0xdd,0xee,0xff};
  byte key[16]={0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f};
  byte *out=(byte *)calloc(16,sizeof(byte));
  word* w=(word *)calloc((Nb*(Nr+1)),sizeof(word));
  int k;

  keyExpansion(key,w);
  out=cipher(input,w);
  //printf("Cifrado:\n");
  //imprimeStatus(out);
  out=invCipher(out,w);
  printf("Descifrado:\n");
  imprimeStatus(out);

return 0;
}
