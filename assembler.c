#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <regex.h>
#include <ctype.h>
#include <math.h>
#include <stdint.h>

struct label {
    char* _name;
    int _address;
    int _nameLength;
};

int validLabelName(char labelName[], int length){
    for (int i = 1; i < length; i++){
        char letter = labelName[i];
        
        if ( !(isalnum(letter) || letter == '-' || letter == '_')){
            return 0;
        }   
    }
    return 1; 
}

int duplicateLabels(struct label labelArr[], int arrSize){
    for (int i = 0; i < arrSize; i++){
        for (int j = i+1; j < arrSize; j++){
            if (strcmp(labelArr[i]._name,labelArr[j]._name) == 0){
                return 1;
            }
        }
    }
    return 0;
}

int indexOf(char instruction[], char character){
    for (int i = 0; i < strlen(instruction); i++){
        if (instruction[i] == character) return i;
    }
    return -1;
}

int numDigits (int num){
    int count = 0;
    do {
        num /= 10;
        ++count;
    } while (num != 0);
    return count;
}

char* replaceLabels(char instruction[], struct label labelArr[], int arrSize){
    char* startIndex = instruction;
    char* indexChar = strstr(instruction, ":");
    if (indexChar == NULL){
        return instruction;
    }
    int indexColon = indexChar - startIndex;

    struct label labelFound;
    
    for (int i = 0; i < arrSize; i++){
        // TODO: possible bug if one label name has another label name inside of it ex: L1 vs LL1
        if(strstr(instruction, labelArr[i]._name) != NULL){
            labelFound = labelArr[i];
            break;
        }
    }
    
    int endIndex = indexColon + labelFound._nameLength;

    int labelAdd = labelFound._address;
    int addressInts[(int)(numDigits(labelFound._address))*sizeof(char)];
    for (int i = sizeof(addressInts)/sizeof(int) - 1; i >= 0; i--){
        addressInts[i] = labelAdd%10;
        labelAdd /= 10;
    }
    
    int newInstructionLength = strlen(instruction) - labelFound._nameLength/sizeof(char) + numDigits(labelFound._address);
    char* newInstruction = (char*) malloc((newInstructionLength) * sizeof(char) + 1);
    
    int index = 0;

    for (int i = 0; i < indexColon; i++){
        newInstruction[index] = instruction[i];
        index++;
    }

    for (int i = 0; i < sizeof(addressInts)/sizeof(int); i++){
        newInstruction[index] = addressInts[i] + '0';
        index++;
    }
    
    for (int i = endIndex+1; i < strlen(instruction); i++){
        newInstruction[index] = instruction[i];
        index++;
    }

    return newInstruction;

}

struct instruction{
    uint32_t _opCode;
    uint32_t _rd;
    uint32_t _rs;
    uint32_t _rt;
    int64_t _L;

};

int regexMatchExpression (char instruction[]){
    char* regexArray[] = {
        //"^\tadd r[0-9], r1, r3",
        "^\tadd r\\([0-9]\\|1[0-9]\\|2[0-9]\\|3[01]\\), r\\([0-9]\\|1[0-9]\\|2[0-9]\\|3[01]\\), r\\([0-9]\\|1[0-9]\\|2[0-9]\\|3[01]\\)$",
        "^\taddi r\\([0-9]\\|1[0-9]\\|2[0-9]\\|3[01]\\), [0-9]\\+$",
        "^\tsub r\\([0-9]\\|1[0-9]\\|2[0-9]\\|3[01]\\), r\\([0-9]\\|1[0-9]\\|2[0-9]\\|3[01]\\), r\\([0-9]\\|1[0-9]\\|2[0-9]\\|3[01]\\)$",
        "^\tsubi r\\([0-9]\\|1[0-9]\\|2[0-9]\\|3[01]\\), [0-9]\\+$",
        "^\tmul r\\([0-9]\\|1[0-9]\\|2[0-9]\\|3[01]\\), r\\([0-9]\\|1[0-9]\\|2[0-9]\\|3[01]\\), r\\([0-9]\\|1[0-9]\\|2[0-9]\\|3[01]\\)$",
        "^\tdiv r\\([0-9]\\|1[0-9]\\|2[0-9]\\|3[01]\\), r\\([0-9]\\|1[0-9]\\|2[0-9]\\|3[01]\\), r\\([0-9]\\|1[0-9]\\|2[0-9]\\|3[01]\\)$",
        "^\tand r\\([0-9]\\|1[0-9]\\|2[0-9]\\|3[01]\\), r\\([0-9]\\|1[0-9]\\|2[0-9]\\|3[01]\\), r\\([0-9]\\|1[0-9]\\|2[0-9]\\|3[01]\\)$",
        "^\tor r\\([0-9]\\|1[0-9]\\|2[0-9]\\|3[01]\\), r\\([0-9]\\|1[0-9]\\|2[0-9]\\|3[01]\\), r\\([0-9]\\|1[0-9]\\|2[0-9]\\|3[01]\\)$",
        "^\txor r\\([0-9]\\|1[0-9]\\|2[0-9]\\|3[01]\\), r\\([0-9]\\|1[0-9]\\|2[0-9]\\|3[01]\\), r\\([0-9]\\|1[0-9]\\|2[0-9]\\|3[01]\\)$",
        "^\tnot r\\([0-9]\\|1[0-9]\\|2[0-9]\\|3[01]\\), r\\([0-9]\\|1[0-9]\\|2[0-9]\\|3[01]\\)$",
        "^\tshftr r\\([0-9]\\|1[0-9]\\|2[0-9]\\|3[01]\\), r\\([0-9]\\|1[0-9]\\|2[0-9]\\|3[01]\\), r\\([0-9]\\|1[0-9]\\|2[0-9]\\|3[01]\\)$",
        "^\tshftri r\\([0-9]\\|1[0-9]\\|2[0-9]\\|3[01]\\), [0-9]\\+$",
        "^\tshftl r\\([0-9]\\|1[0-9]\\|2[0-9]\\|3[01]\\), r\\([0-9]\\|1[0-9]\\|2[0-9]\\|3[01]\\), r\\([0-9]\\|1[0-9]\\|2[0-9]\\|3[01]\\)$",
        "^\tshftli r\\([0-9]\\|1[0-9]\\|2[0-9]\\|3[01]\\), [0-9]\\+$",
        "^\tbr r\\([0-9]\\|1[0-9]\\|2[0-9]\\|3[01]\\)$",
        "^\tbrr r\\([0-9]\\|1[0-9]\\|2[0-9]\\|3[01]\\)$",
        //"^\tbrr -[0-9]\\+$",
        // "^\tbrr [0-9]$",
        "\\(^\tbrr -\\?[0-9]\\+$\\)\\|\\(^\tbrr [0-9]\\+$\\)$",
        "^\tbrnz r\\([0-9]\\|1[0-9]\\|2[0-9]\\|3[01]\\), r\\([0-9]\\|1[0-9]\\|2[0-9]\\|3[01]\\)$",
        "^\tcall r\\([0-9]\\|1[0-9]\\|2[0-9]\\|3[01]\\)$",
        "^\treturn$",
        "^\tbrgt r\\([0-9]\\|1[0-9]\\|2[0-9]\\|3[01]\\), r\\([0-9]\\|1[0-9]\\|2[0-9]\\|3[01]\\), r\\([0-9]\\|1[0-9]\\|2[0-9]\\|3[01]\\)$",
        "\\(^\tmov r\\([0-9]\\|1[0-9]\\|2[0-9]\\|3[01]\\), (r\\([0-9]\\|1[0-9]\\|2[0-9]\\|3[01]\\))([0-9]\\+)$\\)\\|\\(^\tmov r\\([0-9]\\|1[0-9]\\|2[0-9]\\|3[01]\\), (r\\([0-9]\\|1[0-9]\\|2[0-9]\\|3[01]\\))(-\\?[0-9]\\+)$\\)",
        // "^\tmov r\\([0-9]\\|1[0-9]\\|2[0-9]\\|3[0]\\), (r\\([0-9]\\|1[0-9]\\|2[0-9]\\|3[0]\\))([0-9]\\+)$",
        // "^\tmov r\\([0-9]\\|1[0-9]\\|2[0-9]\\|3[0]\\), (r\\([0-9]\\|1[0-9]\\|2[0-9]\\|3[0]\\))(-\\?[0-9]\\+)$",
        // "^\t([0-9]\\+)$",
        "^\tmov r\\([0-9]\\|1[0-9]\\|2[0-9]\\|3[01]\\), r\\([0-9]\\|1[0-9]\\|2[0-9]\\|3[01]\\)$",
        "^\tmov r\\([0-9]\\|1[0-9]\\|2[0-9]\\|3[01]\\), [0-9]\\+$",
        //"^\tmov (r\\([0-9]\\|1[0-9]\\|2[0-9]\\|3[0]\\))(-[0-9]\\+\\|[0-9]\\+), r\\([0-9]\\|1[0-9]\\|2[0-9]\\|3[0]\\)$",
        "\\(^\tmov (r\\([0-9]\\|1[0-9]\\|2[0-9]\\|3[01]\\))([0-9]\\+), r\\([0-9]\\|1[0-9]\\|2[0-9]\\|3[01]\\)$\\)\\|\\(^\tmov (r\\([0-9]\\|1[0-9]\\|2[0-9]\\|3[01]\\))(-\\?[0-9]\\+), r\\([0-9]\\|1[0-9]\\|2[0-9]\\|3[01]\\)$\\)",
        "^\taddf r\\([0-9]\\|1[0-9]\\|2[0-9]\\|3[01]\\), r\\([0-9]\\|1[0-9]\\|2[0-9]\\|3[01]\\), r\\([0-9]\\|1[0-9]\\|2[0-9]\\|3[01]\\)$",
        "^\tsubf r\\([0-9]\\|1[0-9]\\|2[0-9]\\|3[01]\\), r\\([0-9]\\|1[0-9]\\|2[0-9]\\|3[01]\\), r\\([0-9]\\|1[0-9]\\|2[0-9]\\|3[01]\\)$",
        "^\tmulf r\\([0-9]\\|1[0-9]\\|2[0-9]\\|3[01]\\), r\\([0-9]\\|1[0-9]\\|2[0-9]\\|3[01]\\), r\\([0-9]\\|1[0-9]\\|2[0-9]\\|3[01]\\)$",
        "^\tdivf r\\([0-9]\\|1[0-9]\\|2[0-9]\\|3[01]\\), r\\([0-9]\\|1[0-9]\\|2[0-9]\\|3[01]\\), r\\([0-9]\\|1[0-9]\\|2[0-9]\\|3[01]\\)$",
        "^\tin r\\([0-9]\\|1[0-9]\\|2[0-9]\\|3[01]\\), r\\([0-9]\\|1[0-9]\\|2[0-9]\\|3[01]\\)$",
        "^\tout r\\([0-9]\\|1[0-9]\\|2[0-9]\\|3[01]\\), r\\([0-9]\\|1[0-9]\\|2[0-9]\\|3[01]\\)$",
        "^\thalt$",
        "^\tclr r\\([0-9]\\|1[0-9]\\|2[0-9]\\|3[01]\\)$",
        "^\tld r\\([0-9]\\|1[0-9]\\|2[0-9]\\|3[01]\\), [0-9]\\+$",
        "^\tpush r\\([0-9]\\|1[0-9]\\|2[0-9]\\|3[01]\\)$",
        "^\tpop r\\([0-9]\\|1[0-9]\\|2[0-9]\\|3[01]\\)$"
    };

    //-[0-9]\\+\\|[0-9]\\+

    regex_t regex;
    // switch back to 36 after
    for (int i = 0; i < 36; i++){
        if (regcomp(&regex, regexArray[i], 0) == 0){
            if(regexec(&regex,instruction, 0, NULL, 0) == 0){
                return i;
            }
        }
    }

    printf("failing regex\n");
    return -1;
}
    
uint32_t findR (char instruction[], int rCount){
    int numR = 0;
    int rIndex = -1;
    for (int i = 0; i < strlen(instruction)-1; i++){
        if(instruction[i] == 'r' && isdigit(instruction[i+1])){
            numR++;
        }
        if (numR == rCount){
            rIndex = i;
            break;
        }
    }

    int r = instruction[rIndex+1] - '0';
    if (isdigit(instruction[rIndex+2])){
        r *= 10;
        r += instruction[rIndex+2] - '0';
    }
    return r;
}

char* findStringL (char instruction[], int paranthesis){
    int len = strlen(instruction);
    //printf("instruction: %s\n", instruction);
    //instruction: %d\n", len);
    int endDigitIndex = len-1;
    int startDigitIndex = 0;
    if (paranthesis){
        for (int i = len -1; i >= 0; i--){
            if(instruction[i] == ')'){
                endDigitIndex = i-1;
                break;
            }
        }
        for (int i = endDigitIndex; i > 0; i--){
            if (instruction[i-1] == '(' || instruction[i-1] == '-'){
                startDigitIndex = i;
                break;
            }
        }
    } else {
        //printf("len: %d\n", len);
        for (int i = len - 1; i >= 0; i--){
            if (isdigit(instruction[i])){
                endDigitIndex = i;
                break;
            }
        }

        for (int i = endDigitIndex; i > 0; i--){
            if (instruction[i-1] == ' ' || instruction[i-1] == '-'){
                startDigitIndex = i;
                break;
            }
        }
    }

    char* numberString[(endDigitIndex-startDigitIndex+2) * sizeof(char)];

    
    int count = 0;

    if (startDigitIndex != 0 & instruction[startDigitIndex-1] == '-'){
        numberString[count] = "-";
        count++;
    }

    for (int i = startDigitIndex; i <= endDigitIndex; i++){
        numberString[count] = &instruction[i];
        count++;
    }
    numberString[count] = '\0';

    return *numberString;
}


int64_t findL(char instruction[], int paranthesis){
    //char *str = (findStringL(instruction, paranthesis));

    int len = strlen(instruction);
    //printf("instruction: %s\n", instruction);
    //instruction: %d\n", len);
    int endDigitIndex = len-1;
    int startDigitIndex = 0;
    if (paranthesis){
        for (int i = len -1; i >= 0; i--){
            if(instruction[i] == ')'){
                endDigitIndex = i-1;
                break;
            }
        }
        for (int i = endDigitIndex; i > 0; i--){
            if (instruction[i-1] == '(' || instruction[i-1] == '-'){
                startDigitIndex = i;
                break;
            }
        }
    } else {
        //printf("len: %d\n", len);
        for (int i = len - 1; i >= 0; i--){
            if (isdigit(instruction[i])){
                endDigitIndex = i;
                break;
            }
        }

        for (int i = endDigitIndex; i > 0; i--){
            if (instruction[i-1] == ' ' || instruction[i-1] == '-'){
                startDigitIndex = i;
                break;
            }
        }
    }

    int64_t num = 0;
    for (int i = startDigitIndex; i <= endDigitIndex; i++){
        // printf("%dth iteration: %ld\n", i, num);
        num *= 10;
        num += (instruction[i] - '0');
    }
    if (startDigitIndex > 0 && instruction[startDigitIndex-1] == '-'){

        num *= -1;
        //printf("num for L: %ld\n", num);
        return num;
    }
    //printf("%ld\n", num);
    return num; 
}

uint64_t findLoadL (char instruction[], int paranthesis){
    int len = strlen(instruction);
    //printf("instruction: %s\n", instruction);
    //instruction: %d\n", len);
    int endDigitIndex = len-1;
    int startDigitIndex = 0;
    for (int i = len - 1; i >= 0; i--){
            if (isdigit(instruction[i])){
                endDigitIndex = i;
                break;
            }
        }

    for (int i = endDigitIndex; i > 0; i--){
        if (instruction[i-1] == ' ' || instruction[i-1] == '-'){
            startDigitIndex = i;
            break;
        }
    }

    int64_t num = 0;
    for (int i = startDigitIndex; i <= endDigitIndex; i++){
        // printf("%dth iteration: %ld\n", i, num);
        num *= 10;
        num += (instruction[i] - '0');
    }

    uint64_t num1 = 0;
    for (int i = startDigitIndex; i <= endDigitIndex; i++){
        // printf("%dth iteration: %ld\n", i, num);
        num1 *= 10;
        num1 += (instruction[i] - '0');
    }

    return num1; 
}

int compareStringIntL (char instruction[]){
    


    char *numStr = findStringL(instruction, 0);
    printf("numStr: %d\n", *numStr);

    uint64_t numInt = findLoadL(instruction, 0);

    for (int i = strlen(numStr) -1; i >= 0; i--){
        int numStrC = numStr[i] - '0';
        if (numStrC != numInt%10){
            return 0;
        }
        numInt /= 10;
    }


    return 1;

}



void convertBinary (FILE *outputFile, uint32_t opCode, uint32_t rd, uint32_t rs, uint32_t rt, short L){
    uint32_t instr = 0;
    // if (L < 0){
    //     L = L - 4096;
    // }
    L = L & 0xFFF;
    instr |= L;
    instr |= rt << 12;
    instr |= rs << 17;
    instr |= rd << 22;
    instr |= opCode << 27;
    //printf("printing in CB function: %d %d %d %d %d\n", opCode, rd, rs, rt, L);
    fwrite((const void *) &instr, sizeof(instr), 1, outputFile);

}


int main (int argc, char** argv) {

    // check if there is no argument filepath 
    if (argc != 2 || argv[1] == NULL){
        fprintf(stderr, "Invalid tinker filepath\n");
        return -1;
    }

    // check if you can't open the .tk file
    char *lastDot = strrchr(argv[1], '.');
    if (lastDot == NULL || strcmp(lastDot, ".tk") != 0){
        fprintf(stderr, "Invalid tinker filepath\n");
        return -1;
    }

    // check if you can open the file
    FILE *tkFile = fopen(argv[1], "r");
    if (tkFile == NULL){
        fprintf(stderr, "Invalid tinker filepath\n");
        return -1;
    }
    char* outputFileName = (char*) malloc(strlen(argv[1]) + 4);
    strcpy(outputFileName, argv[1]);
    strcat(outputFileName,"o\0");
    FILE *tkoFile = fopen(outputFileName, "wb");
    //printf("%s\n", outputFileName);
    // //strcpy(outputFileName, argv[1]);
    // //outputFileName[strlen(argv[1])] = 'o';
    // int nameCount = 0;
    // for (int i = 0; i < strlen(argv[1]); i++){
    //     outputFileName[nameCount] = argv[1][i];
    //     nameCount++;
    // }
    // outputFileName[nameCount] = 'o';
    //FILE *outputFile = fopen("output.txt", "w");


    //ftell & seek & malloc
    fseek(tkFile, 0L, SEEK_END);
    long int tkFileLength = ftell(tkFile);
    char* fileArr = (char*) malloc(tkFileLength * sizeof(char) + 1);
    rewind(tkFile);
    size_t fileSize = fread(fileArr, 1, tkFileLength, tkFile);
    fileArr[tkFileLength] = '\0';


    // count number of \n's
    int newLineCount = 0;
    for (int i = 0; i < strlen(fileArr); i++){
        if (fileArr[i] == '\n') newLineCount++;
        //fwrite(&fileArr[i], 1, sizeof(char), outputFile);
    }
    newLineCount;
     
    // delimit based of new line tabs 
    char* lineInstructions[newLineCount];
    lineInstructions[0] = strtok(fileArr, "\n");
    for (int i = 1; i < newLineCount; i++){
        lineInstructions[i] = strtok(NULL, "\n");
    }

    // first pass-through: count number of colons
    int colonCount = 0;
    for (int i = 0; i < newLineCount; i++){
        if (lineInstructions[i][0] ==  ':') {
        //if (strchr(lineInstructions[i][0], ':') != NULL){
            colonCount++;
        }
    }

    // create array of label structs 
    struct label labelArr[colonCount];
    int index = 0;
    int byteCount = 0;
    int data = 0;
    int code = 0;
    int codeFound = 0;
    
    // second pass-through: set up data for labelArr
    int validLines[newLineCount];
    for (int i = 0; i < newLineCount; i++){

        validLines[i] = 0;
        if (lineInstructions[i][0] ==  ':') {
            struct label newLabel;

            newLabel._name = (char*)malloc(strlen(lineInstructions[i]) * sizeof(char) +1);
            strcpy(newLabel._name, lineInstructions[i]);
            strcat(newLabel._name, "\0");
            
            newLabel._nameLength = strlen(newLabel._name);

            if (!validLabelName(newLabel._name, newLabel._nameLength)){
                // Q? what should i throw if invalid label name? or should i say error on line
                fprintf(stderr, "Error on line %d\n", i+1);
                return -1;
            }
             
            newLabel._address = byteCount;

            labelArr[index] = newLabel;
            index++;

            if (duplicateLabels(labelArr, index)){
                //Q? what error to throw
                fprintf(stderr, "Error on line %d\n", i+1);
                return -1;
            }
        }

        if (lineInstructions[i][0] == ';' || lineInstructions[i][0] == ':'){
            continue;
        } else if (strcmp(lineInstructions[i],".code") == 0){
            code = 1;
            data = 0;
            codeFound = 1;
            continue;
        } else if (strcmp(lineInstructions[i],".data") == 0){
            code = 0;
            data = 1;
            continue;
        } else if (data){
            printf("found data");
            validLines[i] = 2;
            byteCount += 8;
        } else if (code){
            validLines[i] = 1;
            if (strstr(lineInstructions[i],"ld") != NULL) {
                byteCount += 12 * 4;
            } else if (strstr(lineInstructions[i],"push") != NULL || strstr(lineInstructions[i],"pop") != NULL){
                byteCount += 2 * 4;
            } else {
                byteCount += 4;
            }
        }
    }

    if (!codeFound){
        fprintf(stderr, "Error on line 0\n");
        goto exit;
    }
    
    //checking for duplicate labels
    if (duplicateLabels(labelArr, sizeof(labelArr)/sizeof(struct label))){
        return -1;
    }

    // third-pass through: identify instruction
    for (int i = 0; i < newLineCount; i++){

        if (validLines[i] == 0){
            continue;

        } else if (validLines[i] == 1){
            struct instruction line;
            lineInstructions[i] = replaceLabels(lineInstructions[i], labelArr, sizeof(labelArr)/sizeof(struct label));
            //printf("here x1\n");
            line._opCode = regexMatchExpression(lineInstructions[i]);
            //printf("here x2\n");
            //printf("%s\n", lineInstructions[i]);
            line._rd = 0;
            line._rs = 0;
            line._rt = 0;
            line._L = 0;
            //printf("op Code: %d\n", line._opCode);
            
            switch (line._opCode) {
                case -1:
                    fprintf(stderr, "Error on line %d\n", i+1);
                    goto exit;
                    break;
                case 0: case 2: case 4: case 5: case 6: case 7: case 8: case 10: case 12: case 20: case 25: case 26: case 27: case 28:
                    // add rd, rs, rt
                    line._rd = findR(lineInstructions[i], 1);
                    line._rs = findR(lineInstructions[i], 2);
                    line._rt = findR(lineInstructions[i], 3);
                    break;
                case 1: case 3: case 11: case 13: case 23:
                    // addi rd, L
                    // subi, shiftri, shftli, mov
                    line._rd = findR(lineInstructions[i], 1);
                    line._L = findL(lineInstructions[i], 0);
                    // works!
                    printf("L = %ld\n", line._L);
                    if (line._L < 0 || line._L > 4095){
                       fprintf(stderr, "Error on line %d\n", i+1);
                       goto exit;
                    }
                    break;
                case 9: case 17: case 22: case 29: case 30:
                    // not rd, rs
                    line._rd = findR(lineInstructions[i], 1);
                    line._rs = findR(lineInstructions[i], 2);
                    break;
                case 14: case 15: case 18: 
                    // br rd
                    line._rd = findR(lineInstructions[i], 1);
                    break;
                case 16:
                    // brr L
                    line._L = findL(lineInstructions[i], 0);
                    if (line._L < (-2048) || line._L > 2047){
                       fprintf(stderr, "Error on line %d\n", i+1);
                       goto exit;
                    }
                    break;
                case 19: case 31:
                    // return
                    // halt
                    printf("here in halt\n");
                    break;
                case 21:
                    // mov rd, (rs)(L)
                    line._rd = findR(lineInstructions[i], 1);
                    line._rs = findR(lineInstructions[i], 2);
                    line._L = findL(lineInstructions[i], 1);
                    // works!
                    if (line._L < -2048 || line._L > 2047){
                       fprintf(stderr, "Error on line %d\n", i+1);
                       goto exit;
                    }
                    break;
                case 24: 
                    // mov (rd)(L), rs
                    line._rd = findR(lineInstructions[i], 1);
                    line._rs = findR(lineInstructions[i], 2);
                    line._L = findL(lineInstructions[i], 1);
                    if (line._L < -2048 || line._L > 2047){
                       fprintf(stderr, "Error on line %d\n", i+1);
                       goto exit;
                    }
                    break;
                case 32:
                    // clr
                    line._opCode = 8;
                    line._rd = findR(lineInstructions[i], 1);
                    line._rs = line._rd;
                    line._rt = line._rd;
                    break;
                case 33:
                    // ld rd, L
                    // line._rd = findR(lineInstructions[i], 1);
                    // line._L = findL(lineInstructions[i], 0);
                    // int rd;
                    // u_int64_t loadL = scanf()
                    // //u_int64_t loadL = findLoadL(lineInstructions[i],0);

                    // printf("L: %lu\n", loadL);

                    line._rd = findR(lineInstructions[i], 1);
                    line._L = findL(lineInstructions[i],0);

                    if (!compareStringIntL (lineInstructions[i])){
                        printf("string comparison error — overflow suspected\n");
                        fprintf(stderr, "Error on line %d\n", i+1);
                        goto exit;
                    }

                    // should just fail in regex
                    // if (line._L < 0){
                    //     printf("negative immediate value\n");
                    //     fprintf(stderr, "Error on line %d\n", i+1);
                    //     goto exit;
                    // }

                    //struct instruction first = {8, rd, rd, rd};
                    struct instruction a = {8, line._rd, line._rd, line._rd, line._L};
                    struct instruction b = {1, line._rd, line._rs, line._rt, (line._L >> 52) & 0xFFF};
                    struct instruction c = {13, line._rd, line._rs, line._rt, 12};
                    struct instruction d = {1, line._rd, line._rs, line._rt, (line._L >> 40) & 0xFFF};
                    struct instruction e = {13, line._rd, line._rs, line._rt, 12};
                    struct instruction f = {1, line._rd, line._rs, line._rt, (line._L >> 28) & 0xFFF};
                    struct instruction g = {13, line._rd, line._rs, line._rt, 12};
                    struct instruction h = {1, line._rd, line._rs, line._rt, (line._L >> 16) & 0xFFF};
                    struct instruction k = {13, line._rd, line._rs, line._rt, 12};
                    struct instruction l = {1, line._rd, line._rs, line._rt, (line._L >> 4) & 0xFFF};
                    struct instruction m = {13, line._rd, line._rs, line._rt, 4};
                    struct instruction n = {1, line._rd, line._rs, line._rt, line._L & 0xF};
                    struct instruction lineList[] = {a, b, c, d, e, f, g, h, k, l, m, n};
                    for (int i = 0; i < 12; i++){
                        struct instruction currLine = lineList[i];
                        //printf("printing in ld case: %d %d %d %d %ld\n", currLine._opCode, currLine._rd, currLine._rs, currLine._rt, currLine._L);
                        convertBinary(tkoFile, currLine._opCode, currLine._rd, currLine._rs, currLine._rt, currLine._L);
                    }
                    //printf("within load: %d %d %d %d %ld\n", line._opCode, line._rd, line._rs, line._rt, line._L);
                    break;
                case 34:
                    // push
                    line._rd = findR(lineInstructions[i], 1);
                    struct instruction a1 = {24, 31, line._rd, line._rt, -8};
                    struct instruction b1 = {3, 31, line._rs, line._rt, 8};
                    struct instruction lineList1[] = {a1, b1};
                    for (int i = 0; i < 2; i++){
                        struct instruction currLine1 = lineList1[i];
                        // printf("%d %d %d %d %ld\n", currLine1._opCode, currLine1._rd, currLine1._rs, currLine1._rt, currLine1._L);
                        convertBinary(tkoFile, currLine1._opCode, currLine1._rd, currLine1._rs, currLine1._rt, currLine1._L);
                    }
                    break;
                case 35:
                    // pop
                    line._rd = findR(lineInstructions[i], 1);
                    struct instruction a2 = {21, line._rd, 31, line._rt, line._L};
                    struct instruction b2 = {1, 31, line._rs, line._rt, 8};
                    struct instruction lineList2[] = {a2, b2};
                    for (int i = 0; i < 2; i++){
                        struct instruction currLine2 = lineList2[i];
                        //printf("%d %d %d %d %ld\n", currLine2._opCode, currLine2._rd, currLine2._rs, currLine2._rt, currLine2._L);
                        convertBinary(tkoFile, currLine2._opCode, currLine2._rd, currLine2._rs, currLine2._rt, currLine2._L);
                    }
                    break;
                default:
                    break;
            }
            
            if (line._opCode != 33 && line._opCode != 34 && line._opCode != 35){
                //printf("here so not load, push or pop\n");
                //printf("%d %d %d %d %ld\n", line._opCode, line._rd, line._rs, line._rt, line._L);
                convertBinary(tkoFile, line._opCode, line._rd, line._rs, line._rt, line._L);
            }

        } else if (validLines[i] == 2){
            //
            printf("data line: %s\n", lineInstructions[i]);
            // TODO: fix
            
            // int64_t num = 0;
            // for (int j = 0; j < strlen(lineInstructions[i]); j++){
            //     if(isdigit(lineInstructions[i][j])){
            //         num*= 10;
            //         num += lineInstructions[i][j] - '0';
            //     }
            // }
            // for (int j = strlen(lineInstructions[i]) - 1; j >= 0; j--){
            //     if (isdigit(lineInstructions[i][j])){
            //         if (num == 0 || (num%10) != (lineInstructions[i][j]- '0')){
            //             fprintf(stderr, "Error on line %d\n", i+1);
            //             goto exit;
            //         } else {
            //             num /= 10;
            //         }
            //     } 
            // }
            // if (!compareStringIntL (lineInstructions[i])){
            //     printf("string comparison error — overflow suspected\n");
            //     fprintf(stderr, "Error on line %d\n", i+1);
            //     goto exit;
            // }
            // printf("%d\n", num);
            
            //TODO : FIX LATER - throw error if over 64 bits unsigned
            uint64_t data = strtoull(lineInstructions[i], NULL, 10);
            for (int j = strlen(lineInstructions[i]) - 1; j >= 0; j--){
                if (isdigit(lineInstructions[i][j])){
                    if ((lineInstructions[i][j]-'0') != (data % 10)){
                        fprintf(stderr, "Error on line %d\n", i+1);
                        goto exit;
                    } 
                    data /= 10;
                }
            }

            data = strtoull(lineInstructions[i], NULL, 10);
            fwrite((const void *) &data, sizeof(data), 1, tkoFile);
        }

    }

    return 0;

    exit:
    fclose(tkoFile);
    remove(outputFileName);
    return -1;

    // first pass-through — store data label's addresses & total how much memory needed to store all instructions
    

    // second pass-through
    // while not end of file, continue with instructions & pass through new instruction per loop
}

/**
 * open up file, want to be able to go through all lines
 * some sort of token proceessing to differentiate between directive, data item, instruction
 * look at labels
 * first pass through: go through all labels and set them up
 * test cases to see if function can take in single line and write it to a file - unit testing
 * make sure you can output it to a file & read that file and make sure it matches
 * parse all instructions same way (first 5 bits for op code), next 5 bits for 1st operand, 5 for 2nd, 5 for 3rd, and last 12 for data item
 * based of op code, look at relevant parts (ex: add needs 3 operands and no data item)
 * errorhandling: if instructions malformed 
 * takes string and converts it into an enum ()
 * boolean for code: like tab character
 * one big while loop 
 * multiple passes because first need to know how many labels, then calculate how much memory to allocate and do another pass
 * struct for labels! name of label and address of lable
 * 2 while loops
*/