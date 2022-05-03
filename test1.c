#include<stdio.h>
#include<stdlib.h>
#include<string.h>

void Decode();

void Execute();

char sstr[42];
//A function to pick a specific part of the string
char* substring(char string[42],int s,int l){
  int c = 0;
  while (c < l)
   {
      sstr[c] = string[s+c-1];
      c++;
   }
   sstr[c] = '\0';
   return sstr;
}

typedef unsigned char byte;
//function to convert given binary string into a decimal
int strbin_to_dec(char * str) {
    uint result = 0;
    for (int i = strlen(str) - 1, j = 0; i >= 0; i--, j++) {
        byte k = str[i] - '0'; //ASCII coding
        k <<= j;
        result += k;
    }
    return result;
}

//A function to convert given decimal into a binary string
char *decimal_to_binary(int n)
    {
    int c, d, count;
    char *pointer;
    count = 0;
    pointer = (char*)malloc(12+1);
    if ( pointer == NULL )
    exit(EXIT_FAILURE);
    for ( c = 11 ; c >= 0 ; c-- )
    {
    d = n >> c;
    if ( d & 1 )
    *(pointer+count) = 1 + '0';
    else
    *(pointer+count) = 0 + '0';
    count++;
    }
    *(pointer+count) = '\0';
    return pointer;
    }

//Registers required
char AC[42] = "0000000000000000000000000000000000000000";
char MQ[42];
char MBR[42];
char IBR[22] = "00000000000000000000";
char IR[10];
char PC[] = "000000000000";
int pc;
char MAR[14];
int signal;
int w;
//Memory
char M[][42] ={"0000000100000000111100000110000000000101",
               "0000111100000000001100000001000000001111",
               "--------------------00000101000000000101",
               "0010000100000000000000010100000000000000",
               "0000000000000000000000000000000000000000"};


//opcode for HALT is 00000000
void Fetch(){
  //Fetch until there is a Halt
  while(strcmp(substring(M[strbin_to_dec(PC)],1,8),"00000000") != 0){
    //next instruction is not in IBR
    if(strcmp(IBR,"00000000000000000000") == 0)
    {
      strcpy(MAR,PC);//MAR = PC
      strcpy(MBR,M[strbin_to_dec(MAR)]);//MBR=M[MAR]
      //no LHS
      if(strcmp(substring(MBR,1,20),"--------------------") == 0){
        strcpy(IBR,"00000000000000000000");//Store nothing in IBR
        strcpy(IR,substring(MBR,21,8));//IR = MBR(21:28)
        strcpy(MAR,substring(MBR,29,12));//MAR = MBR(29:40)
        pc = strbin_to_dec(PC)+1;
        strcpy(PC,decimal_to_binary(pc));//PC = PC + 1
      }
      //has LHS and RHS
      else{
        strcpy(IBR,substring(MBR,21,20));//IBR = MBR(21:40)
        strcpy(IR,substring(MBR,1,8));//IR = MBR(1:8)
        strcpy(MAR,substring(MBR,9,12));//MAR = MBR(9:20)
      }
    }
    //next instruction in IBR
    else{
      strcpy(IR,substring(IBR,1,8));//IR = MBR(1:8)
      strcpy(MAR,substring(IBR,9,12));//MAR = MBR(9:20)
      strcpy(IBR,"00000000000000000000");//Empty the IBR after fetching the instruction in it
      pc = strbin_to_dec(PC)+1;
      strcpy(PC,decimal_to_binary(pc));//PC = PC + 1
    }
  Decode();//Calling the Decode function to detect the opcodes
  }
}

//Instructions with their respective Opcodes are detected
void Decode(){
  if(strcmp(IR,"00100001") == 0){
    signal = 1;//STOR M(X)
    Execute();
  }
  else if(strcmp(IR,"00000001") == 0){
    signal = 2;//LOAD M(X)
    Execute();
  }
  else if(strcmp(IR,"00001101") == 0){
    signal = 3;//JUMP M(X,0:19)
    Execute();
  }
  else if(strcmp(IR,"00001111") == 0){
    signal = 4;//JUMP+M(X,0:19)
    Execute();
  }
  else if(strcmp(IR,"00000101") == 0){
    signal = 5;//ADD M(X)
    Execute();
  }
  else if(strcmp(IR,"00000110") == 0){
    signal = 6;//SUB M(X)
    Execute();
  }
  else if(strcmp(IR,"00010100") == 0){
    signal = 7;//LSH
    Execute();
  }
  else{
    signal = 0;//NONE
    Execute();
  }
}

//Necessary operations are done after the opcodes are detected
void Execute(){
  switch(signal)
  {
    case 1: strcpy(MBR,AC);
            printf("%s\n",MBR);//Printing 'c' value
            memcpy(M[strbin_to_dec(PC)]+8,MBR,12);
            break;
    case 2: strcpy(MBR,MAR);
            strcpy(AC,MBR);
            break;
    case 3: pc = strbin_to_dec(MAR);
            strcpy(PC,decimal_to_binary(pc));
            break;
    case 4: if(strcmp(substring(AC,1,1),"0") == 0)//Checking the first bit of AC to check whether it's +ve or -ve
              {
                strcpy(PC,MAR);
                strcpy(IBR,"00000000000000000000");
              }
            break;
    case 5: strcpy(MBR,MAR);
            strcpy(AC,decimal_to_binary(strbin_to_dec(AC)+strbin_to_dec(MBR)));
            break;
    case 6: strcpy(MBR,MAR);
            w = strbin_to_dec(AC)-strbin_to_dec(MBR);
            if(w > 0)
              strcpy(AC,decimal_to_binary(w));
            else{
              strcpy(AC,decimal_to_binary(-1*w));
              memcpy(AC,"1",1);
            }
            break;
    case 7: strcpy(MBR,AC);
            if(strcmp(substring(MBR,1,1),"0") == 0)
              strcpy(AC,decimal_to_binary(strbin_to_dec(MBR)*2));
            else{
              strcpy(AC,decimal_to_binary(strbin_to_dec(MBR)*2*-1));
              memcpy(AC,"1",1);
            }
            break;
    case 0: break;
  }

}

int main()
{
  Fetch();
  printf("%s\n",M[3]);//Printing the word(3)
  printf("%s\n",AC);//Printing AC
  return 0;
}
