#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

#define NUM_REGS 32
#define MEM_SIZE 512*1024


union reg {
    __int64_t _sInt;
    __uint64_t _uInt;
    __uint64_t _boolArr;
    double _double;
};


int validAddress (int wanted){
    if (wanted > MEM_SIZE || wanted < 0){
        return 0;
    }
    return 1;
}

// int overflow (int64_t num){
//     if (num < 0 || regArr[rd]._sInt + L >= MEM_SIZE || (regArr[rs]._sInt + L )%4 != 0){
//         return 1;
//     }
//     return 0;
// }

int64_t makeNegative (int num){
    num = (num) | 0xF000;
    return num;
}


int main(int argc, char** argv){

    // check if there is no argument filepath 
    if (argc != 2 || argv[1] == NULL){
        fprintf(stderr, "Invalid tinker filepath\n");
        return -1;
    }

    // check if you can't open the .tk file
    char *lastDot = strrchr(argv[1], '.');
    if (lastDot == NULL || strcmp(lastDot, ".tko") != 0){
        fprintf(stderr, "Invalid tinker filepath\n");
        return -1;
    }

    // open tko file to read it in
    FILE *tkoFile = fopen(argv[1], "r");
    if (tkoFile == NULL){
        fprintf(stderr, "Invalid tinker filepath\n");
        return -1;
    }


    uint8_t mem[MEM_SIZE];
    memset(mem, 0, sizeof(int8_t) * MEM_SIZE);
    // uint8_t binLine;
    // for (int i = 0; fread(&binLine, sizeof(int8_t), 1, tkoFile) == 1; i++){
    //     mem[i] = binLine;
    // }

    fread(mem, sizeof(uint8_t), MEM_SIZE, tkoFile);

    union reg regArr[NUM_REGS];
    memset(&regArr, 0, sizeof(union reg) * 32);

    int PC = 0;

    uint32_t opCode = 0;

    uint32_t line;
    uint64_t negMask = 0xFFFFF000;
    uint64_t masked = 0b1111111111111111111111111111111111111111111111111111000000000000;
    uint64_t rdMask;
    regArr[31]._uInt = MEM_SIZE;
    while (PC < MEM_SIZE){

        if (PC < 0 || PC >= MEM_SIZE || PC %4 != 0){
            goto exit;
        }

        uint32_t instruction = 0b0ull;    
        for (int i = 0; i < 4; i++){
            instruction |= (mem[PC+i] << (8*i));
        }

        opCode = (instruction >> 27) & 0x1F;
        uint32_t rd = (instruction >> 22) & 0x1F;
        uint32_t rs = (instruction >> 17) & 0x1F;
        uint32_t rt = (instruction >> 12) & 0x1F;
        short L = (instruction) & 0xFFF;


        // line = *(uint32_t*)&mem[PC];
        
        // opCode = (line >> 27) & 0x1F;
        // uint32_t rd = (line >> 22) & 0x1F;
        // uint32_t rs = (line >> 17) & 0x1F;
        // uint32_t rt = (line >> 12) & 0x1F;
        // short L = (line) & 0xFFF;
    


        // if (L >= 2048){
        //     L = (line) | 0xF000;
        // }

        //printf("PC: %d\n", PC);
        //printf("%d:    %d %d %d %d %d\n", PC, opCode, rd, rs, rt, L);
        int branch = 0;

        switch (opCode){
            case 0:
                regArr[rd]._sInt = regArr[rs]._sInt + regArr[rt]._sInt;
                break;
            case 1:
                regArr[rd]._uInt = regArr[rd]._uInt + L;
                break;
            case 2:
                regArr[rd]._sInt = regArr[rs]._sInt - regArr[rt]._sInt;
                break;
            case 3:
                regArr[rd]._sInt = regArr[rd]._sInt - L;
                break;
            case 4:
                regArr[rd]._sInt = regArr[rs]._sInt * regArr[rt]._sInt;
                break;
            case 5:
                if(regArr[rt]._sInt == 0){
                    fprintf(stderr, "Simulation error\n");
                    return -1;
                }
                regArr[rd]._sInt = regArr[rs]._sInt / regArr[rt]._sInt;
                break;
            case 6:
                regArr[rd]._uInt = regArr[rs]._uInt & regArr[rt]._uInt;
                break;
            case 7:
                regArr[rd]._uInt = regArr[rs]._uInt | regArr[rt]._uInt;
                break;
            case 8:
                regArr[rd]._uInt = regArr[rs]._uInt ^ regArr[rt]._uInt;
                break;
            case 9:
                regArr[rd]._uInt = ~regArr[rs]._uInt;
                break;
            case 10:
                regArr[rd]._uInt= regArr[rs]._uInt >> regArr[rt]._uInt;
                break;
            case 11:
                regArr[rd]._uInt = regArr[rd]._uInt >> L;
                break;
            case 12:
                regArr[rd]._uInt = regArr[rs]._uInt << regArr[rt]._uInt;
                PC += 4;
                break;
            case 13:
                regArr[rd]._uInt = regArr[rd]._uInt << L;
                break;
            case 14:
                branch = 1;
                PC = regArr[rd]._uInt;
                break;
            case 15: 
                branch = 1;
                PC += regArr[rd]._uInt;
                break;
            case 16:
                // brr
                branch = 1;
                int16_t shift = L << 4;
                shift = shift >> 4;
                PC += shift;
                break;
            case 17:
                if (regArr[rs]._uInt != 0){
                    branch = 1;
                    PC = regArr[rd]._uInt;
                }
                break;
            case 18:
                // call
                // todo: check overlfow for regs31

                if ((regArr[31]._sInt - 8) < 0 || (regArr[31]._sInt - 8) >= MEM_SIZE || (regArr[31]._sInt-8)%4 != 0){
                    goto exit;
                }

                //regArr[31]._sInt -= 8;
                *(uint64_t*)(&mem[regArr[31]._uInt - 8]) = PC + 4;
                // for (int t = 0; t < 8; t++){
                //     mem[regArr[31]._sInt - 8 + t] = ((PC+4) >> (8 * t));
                // }

                //mem[regArr[31]._sInt - 8] = PC + 4;
                PC = regArr[rd]._uInt;
                branch = 1;

                // regArr[regArr[31]._uInt - 8]._uInt = PC + 4;
                // PC = rd;
                break;
            case 19:
                // return
                branch = 1;
                if ((regArr[31]._sInt - 8) < 0 || (regArr[31]._sInt - 8) >= MEM_SIZE || (regArr[31]._sInt-8)%4 != 0){
                    goto exit;
                }
                PC = *(uint64_t*)(&mem[regArr[31]._uInt]-8);
                //regArr[31]._sInt += 8;

                //PC = mem[regArr[31]._sInt - 8];
                // PC = regArr[regArr[31]._uInt - 8]._uInt;
                break;
            case 20:
                if (regArr[rs]._sInt > regArr[rt]._sInt){
                    branch = 1;
                    PC = regArr[rd]._uInt;
                } 
                break;
            case 21:
                // mov rd, (rs)(L)
                // add overflow issue (if regArr[rs]._uInt + L >= MEM size or target address < regArr[rd]) then throw error

                // if (L >= 2048){
                //     L = (uint64_t)(L | negMask);
                // }
                L = L << 4;
                L = L >> 4;
                if ((regArr[rs]._uInt + L) < 0 || (regArr[rs]._uInt + L) >= MEM_SIZE || (regArr[rs]._uInt + L )%4 != 0){
                    goto exit;
                }
                regArr[rd]._uInt = *(uint64_t*)(mem + regArr[rs]._uInt + L);
                //regArr[rd]._sInt = (int64_t)mem[regArr[rs]._uInt + L];
                // regArr[rd] = regArr[regArr[rs]._uInt + L];
                break;
            case 22:
                regArr[rd] = regArr[rs];
                break;
            case 23:
                rdMask = regArr[rd]._uInt & masked;
                regArr[rd]._uInt = rdMask  | ((uint64_t)L);
                // regArr[rd]._sInt &= ~0x0000000000000FFF;
                // regArr[rd]._sInt |= ((uint64_t)L);
                break;
            case 24:
                // mov4
                // if (L >= 2048){
                //     L = (uint64_t)(L | negMask);
                // }
                L = L << 4;
                L = L >> 4;
                if (regArr[rd]._uInt + L < 0 || regArr[rd]._uInt + L >= MEM_SIZE || (regArr[rd]._uInt + L )%4 != 0){
                    goto exit;
                }
                *(uint64_t*)(mem + regArr[rd]._uInt + L) = regArr[rs]._uInt;
                //mem[regArr[rd]._sInt + L] = regArr[rs]._sInt;
                // regArr[regArr[rd]._uInt+L] = regArr[rs];
                break;
            case 25:
                // addf
                regArr[rd]._double = regArr[rs]._double + regArr[rt]._double;
                break;
            case 26:
                regArr[rd]._double = regArr[rs]._double - regArr[rt]._double;
                break;
            case 27:
                regArr[rd]._double = regArr[rs]._double * regArr[rt]._double;
                break;
            case 28:
                if (regArr[rt]._double == 0.0){
                    fprintf(stderr, "Simulation error\n");
                    return -1;
                }
                regArr[rd]._double = regArr[rs]._double / regArr[rt]._double;
                break;
            case 29:
                if (regArr[rs]._uInt == 0){
                    //regArr[rd]._sInt = scanf("%d", regArr[rs]._sInt);
                    scanf("%lu", &regArr[rd]._uInt);
                }
                break;
            case 30:
                if (regArr[rd]._sInt == 1){
                    //printf("%ld\n", regArr[rs]._uInt);
                    printf("%lu\n", regArr[rs]._uInt);
                } 
                break;
            case 31:
                // todo comment out
                // for (int i = 0; i < 32; i++){
                //     printf("%d: %ld\n", i, regArr[i]._sInt);
                // }
                return 0;
            default:
                break;
        }

        if (branch == 0){
            PC = PC + 4;
        }
        // if (!validAddress(PC)){
        //     break;
        // }  
        //fprintf(stderr, "one pass through switch case\n");

    }
    fprintf(stderr, "Simulation error\n");
    return -1;

    exit: 
    fprintf(stderr, "Simulation error\n");
    return -1;
}