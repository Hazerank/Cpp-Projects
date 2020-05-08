#include <iostream>
#include <vector>
#include <fstream>
#include <map>
#include <string.h>

using namespace std;

/**
 * global variables ( memory, registers and flags )
 */
unsigned char memory[2<<15] ;    // 64K memory
unsigned short ax = 0 ;
unsigned short bx = 0 ;
unsigned short cx = 0 ;
unsigned short dx = 0 ;
unsigned short di = 0 ;
unsigned short si = 0 ;
unsigned short bp = 0 ;
unsigned short sp = (2<<15)-2 ;

bool zf = 0;              // zero flag
bool sf = 0;              // sign flag
bool cf = 0;              // cary flag
bool af = 0;              // auxillary flag
bool of = 0;              // overflow flag

int global = 0;

/**
 *  The maps and Vectors that stores particular datas ( lines, variables, labels )
 */
vector<string> lines;                       // The vector that stores parsed lines
map<string, pair<char, int>> vars;          // The map that stores variable names and positions at the memory
map<string, int> labels;                    // The map that stores label names and positions at the memory
vector<string> varLines;                    // The vector that stores variable names and positions at the memory

/**
 *  Endian store is done here for 8 bit registers.
 *  They are defined via pointers.
 */

unsigned char *pah = (unsigned char *) ( ( (unsigned char *) &ax) + 1) ;
unsigned char *pal = (unsigned char *) &ax  ;
unsigned char &al = *pal;
unsigned char &ah = *pah;

unsigned char *pbh = (unsigned char *) ( ( (unsigned char *) &bx) + 1) ;
unsigned char *pbl = (unsigned char *) &bx  ;
unsigned char &bl = *pbl;
unsigned char &bh = *pbh;

unsigned char *pch = (unsigned char *) ( ( (unsigned char *) &cx) + 1) ;
unsigned char *pcl = (unsigned char *) &cx  ;
unsigned char &cl = *pcl;
unsigned char &ch = *pch;

unsigned char *pdh = (unsigned char *) ( ( (unsigned char *) &dx) + 1) ;
unsigned char *pdl = (unsigned char *) &dx  ;
unsigned char &dl = *pdl;
unsigned char &dh = *pdh;

map<string, unsigned short* > regs = {      // The map that stores register names and pointers
        {"ax" , &ax},{"bx", &bx},{"cx",&cx},{"dx",&dx},{"di",&di},{"si",&si},{"bp",&bp},{"sp",&sp}
};
map<string, unsigned char* > regs8 = {      // The map that stores 8 bit register names and pointers
        {"al" , pal},{"ah", pah},
        {"bl" , pbl},{"bh", pbh},
        {"cl" , pcl},{"ch", pch},
        {"dl" , pdl},{"dh", pdh},
};



/*
 * This function takes file name as parameter. Writes lines to lines vector after processing with tokenizer.
 * Writes var defining lines to varLines vector. Writes labels to labels vector.
 * */
void readFile(const string& fileName){
    ifstream inFile(fileName);
    string temp;
    getline(inFile, temp);
    if(temp[temp.size()-1] == '\r')temp= temp.substr(0, temp.size()-1);
    char* token = strtok(&temp[0], " ");
    if(strcmp(token, "code")) throw "Error";
    token = strtok(nullptr, " ");
    if(strcmp(token, "segment")) throw "Error";
    token = strtok(nullptr, " ");
    if(token != nullptr && strcmp(token, "\r") ) throw "Error";
    bool finished = false;
    while(inFile.peek() != EOF){
        getline(inFile, temp);
        if(temp[temp.size()-1] == '\r')temp= temp.substr(0, temp.size()-1);
        string t = temp;
        char* token = strtok(&t[0], " ");
        if(strcmp(token, "code") == 0){
            token = strtok(nullptr, " ");
            if(strcmp(token, "ends") == 0){
                return;
            }
            token = strtok(nullptr, " ");
            if(token != nullptr) throw "Error";

        }

        if(!finished){
            char* token = strtok(&temp[0], " ");
            if(token[strlen(token)-1] == ':'){
                string label = token;
                token = strtok(nullptr, " ");
                label.resize(label.size()-1);
                if(token == nullptr){
                    labels.insert(make_pair(label, lines.size()-1));
                }
                else{
                    throw "Wrong label definition.";
                }
            }
            else{
                global+=6;
                if(strcmp(token, "int") == 0){
                    token = strtok(nullptr, " ");
                    if(token == nullptr) throw "Error";
                    else if(strcmp(token, "20h") == 0){
                        finished = true;
                        lines.emplace_back("int20h");
                    }
                    else if(strcmp(token, "21h") == 0){
                        lines.emplace_back("int21h");
                    }
                    else throw "Error";
                }
                else{
                    string line;
                    while(token!= nullptr){
                        line += token;
                        line += " ";
                        token = strtok(nullptr, " ");
                    }
                    lines.push_back(line);
                }
            }
        }
        else{
            varLines.emplace_back(temp);
            char* token = strtok(&temp[0], " ,");

        }
    }
    throw "Error";
}
/*
 * This function takes a char as parameter and returns if it is a digit in 16 base.
 * */
bool isHexDigit(char c){
    return ( ( c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f')  );
}
/*
 * This function takes a char as parameter and returns if it is a digit in 10 base.
 * */
bool isDigit(char c){
    return ( ( c >= '0' && c <= '9'));
}

/*
 * This function takes a string as parameter and returns if it is a hexadecimal number. It can be like 1f, 0ffh or 12xh
 */
bool isHexadecimal(string arg){
    if(arg.size() == 1){
        return false;
    }
    else if(arg.size() == 2){
        if(!isDigit(arg[0])) return false;
        return isHexDigit(arg[1]) || arg[1] == 'h';
    }
    else{
        if(!isDigit(arg[0])) return false;
        for(int i = 1; i <arg.size()-2; i++){
            if(!isHexDigit(arg[i]))return false;
        }
        if(arg[arg.size()-2] == 'x'){
            return arg[arg.size()-1] == 'h';
        }
        else return isHexDigit(arg[arg.size()-2]) && (arg[arg.size()-1] == 'h' || isHexDigit(arg[arg.size()-1]));
    }
}

/*
 * This function takes a string as parameter and returns if it is a decimal number. It can be like 10, 12d or 12xd
 */
bool isDecimal(string arg){
    if(arg.size() == 1){
        return arg[0] >= '0' && arg[0] <= '9';
    }
    else if(arg.size() == 2){
        if(!isDigit(arg[0])) return false;
        return isDigit(arg[1]) || arg[1] == 'd';
    }
    else if(arg[0] == '0' && arg[arg.size()-1] == 'd')return false;
    else{
        for(int i = 0; i <arg.size()-2; i++){
            if(!isDigit(arg[i]))return false;
        }
        if(arg[arg.size()-2] == 'x'){
            return arg[arg.size()-1] == 'd';
        }
        else return isDecimal(arg.substr(arg.size()-2, 2));
    }
}
bool isChar(string arg){
    return(arg.size() == 3 && arg[0] == '\'' && arg[2] == '\'');

}
/*
 * This function takes a string parameter and return its value if it is a number or space character (This is for handling ' ' character error after tokenizing).
 * If it is not a number or ' ' throws error.
 * */
int interpretValue(const char* s){
    if(strcmp(s, "''") == 0) return ' ';
    if(isDecimal(s)){
        return stoi(s);
    }
    if(isHexadecimal(s)) return stoi(s, nullptr, 16);
    if(isChar(s)) return *(s+1);
    throw "Error";
}

/*
 * This function processes varLines vector. Defines vars with given values. Writes values to memory.
 * Replaces var names with memory values and "offset var" with it's offset in the program lines.
 * */
void defineVars(){
    try{
        for(const string& line: varLines){
            char l[40];
            strcpy(l, line.data());
            char* first = strtok(l, " ");
            char* second = strtok(nullptr, " ");
            char* third = strtok(nullptr, " ");
            if(strtok(nullptr, " ") != nullptr) throw "Error";
            if(vars.find(first) != vars.end()) throw "Error";
            char type;
            if(strcmp(second, "db") == 0) type = 'b';
            else if(strcmp(second, "dw") == 0) type = 'w';
            else throw "Error";
            int value = interpretValue(third);

            int offset = global;
            if(type == 'b'){
                memory[offset] = value;
                global++;
            }
            else{
                memory[offset+1] = value/256;
                memory[offset] = value%256;
                global+=2;
            }
            for(string& line: lines) {
                string off = string("offset ") + string(first);
                size_t  t = line.find(off);
                if(t != string::npos)
                    line.replace(t, off.size(), string("")+to_string(offset));

                string varName = type + string("[") + to_string(offset) + string("]");
                t = line.find(first);
                if(t != string::npos)
                    line.replace(t, strlen(first), varName);
                string varNameWithLetter;
                varNameWithLetter += type;
                varNameWithLetter += '.';
                varNameWithLetter += "first";
                t = line.find(varNameWithLetter);
                if(t != string::npos)
                    line.replace(t, strlen(first), varName);
            }
            vars.insert(make_pair(first, make_pair(type, value)));
        }

    }
    catch(char* c){
        throw c;
    }
}

/**
 * Assembly functions section
 * There always are two function for word and byte size operation
 */

/**
 * Move Functions
 */

/**
 * Places given value to given position (byte size)
 * @param char_pointer the pointer of target position
 * @param value the value that will be placed
 */
void mov_func(unsigned char* char_pointer, unsigned char value){

    *char_pointer = value;

}
/**
 * Places given value to given position (word size)
 * @param short_pointer the pointer of target position
 * @param value the value that will be placed
 */
void mov_func(unsigned short* short_pointer, unsigned short value){

    *short_pointer = value;

}

/**
 * Arithmetic functions
 */

/**
 * Adds given value to given position (byte size)
 * Corresponding flags are sets according to operation.
 * @param char_pointer the pointer of target position
 * @param value the value that will be added
 */
void add_func(unsigned char* char_pointer, unsigned char value){

    bool inOverflow = 0;
    of = 0;
    unsigned char total = *char_pointer + value;
    if( (*char_pointer >> 7) && (value >> 7) ){      // cf condition
        cf = true;
        if(!(total >> 7)) {
            of = 1;
            inOverflow = 1;
        }
    }
    else{
        cf = false;
    }
    if(!inOverflow && !(*char_pointer >> 7) && !(value >> 7) && total >> 7)
        of = 1;

    zf = (total == 0);          // zf condition
    sf = total >> 7;            // sf condition

    af = ( (((*char_pointer) % 16) + (value % 16)) >  16 );     // af condition

    *char_pointer = total;      // Result alignment
}
/**
 * Adds given value to given position (word size)
 * Corresponding flags are sets according to operation.
 * @param short_pointer the pointer of target position
 * @param value the value that will be added
 */
void add_func(unsigned short* short_pointer, unsigned short value){

    unsigned short target = *short_pointer;

    bool inOverflow = false;
    of = 0;
    unsigned short total = target + value;              // cf condition
    if( (*short_pointer >> 15) && (value >> 15)  ){
        cf = 1;
        if(!(total >> 15)) {
            of = 1;
            inOverflow = 1;
        }
    }
    else{
        cf = false;
    }
    if(!inOverflow && !(*short_pointer >> 15) && !(value >> 15) && total >> 15)
         of = 1;

    zf = (total == 0);          // zf condition
    sf = total >> 15;           // sf condition

    af = ( (((*short_pointer) % 16) + (value % 16)) >  16 );     // af condition


    *short_pointer = total;     // result alignment

}

/**
 * Subtracts given value from given position and write there (byte size)
 * Corresponding flags are sets according to operation.
 * @param char_pointer the pointer of target position
 * @param value the value that will be subtracted
 */
void sub_func(unsigned char* char_pointer, unsigned char value){

    unsigned char total = *char_pointer - value;
    cf = ( *char_pointer < value );                                      // cf condition

    of = ( ((*char_pointer >> 7) && !(value >> 7) && !(total >> 7)) ||  // of condition
    (!(*char_pointer >> 7) && (value >> 7) && (total >> 7)));

    zf = (total == 0);                                                  // zf condition

    sf = (total >> 7);                                                  // sf condition

    af = ((((*char_pointer)<<4) >> 7) > ((value << 4) >> 7));           // af condition



    *char_pointer = total;                                              // result alignment



}
/**
 * Subtracts given value from given position and write there (word size)
 * Corresponding flags are sets according to operation.
 * @param short_pointer the pointer of target position
 * @param value the value that will be subtracted
 */
void sub_func(unsigned short* short_pointer, unsigned short value){

    unsigned short total = *short_pointer - value;
    cf = ( *short_pointer < value );                                        // cf condition

    of =( ((*short_pointer >> 7) && !(value >> 7) && !(total >> 7)) ||      // of condition
            (!(*short_pointer >> 7) && (value >> 7) && (total >> 7)));

    zf = (total == 0);                                                      // zf condition

    sf = total >> 7;                                                        // sf condition

    af = ((((*short_pointer)<<12) >> 15) > ((value << 12) >> 15));          // af condition

    *short_pointer = total;                                                 // result alignment

}

/**
 * Increment given position (word size)
 * @param short_pointer the pointer of target position
 */
void inc_func(unsigned short* short_pointer ){

    add_func(short_pointer,1);
    // Changes zero flag or overflow flag
}
/**
 * Increment given position (byte size)
 * @param char_pointer the pointer of target position
 */
void inc_func(unsigned char* char_pointer){

    add_func(char_pointer,1);
    // Changes zero flag or overflow flag

}

/**
 * Decrement given position (word size)
 * @param short_pointer the pointer of target position
 */
void dec_func(unsigned short* short_pointer ){

    sub_func(short_pointer,1);
    // Changes zero flag or overflow flag
}
/**
 * Increment given position (byte size)
 * @param char_pointer the pointer of target position
 */
void dec_func(unsigned char* char_pointer){

    sub_func(char_pointer,1);
    // Changes zero flag or overflow flag

}

/**
 * Multiplies with given value according to assembly rules (byte size)
 * @param value the value that will multiply
 */
void mul_func(unsigned char value){

    unsigned short total = al * value;
    if(total >= (2<<14)){
        of = true;
        cf = true;
    }
    else{
        of = false;
        cf = false;
    }

    ax = total;
}
/**
 * Multiplies with given value according to assembly rules (word size)
 * @param value the value that will multiply
 */
void mul_func(unsigned short value){

    int total = value * ax;

    if(total >= (2<<30)){
        of = true;
        cf = true;
    }
    else{
        of = false;
        cf = false;
    }

    dx = *( (unsigned short *) ( ( (unsigned short *) &total) + 1) );
    ax = *((unsigned short *) &total ) ;

}

/**
 * Divides with given value according to assembly rules (word size)
 * @param value the value that will divide
 */
void div_func(unsigned char value){

    if(ax / value >= (2<<7)){
        of = true;
        throw "Error";
    }
    unsigned char quotient = ax / value;
    unsigned char remainder = ax % value;

    al = quotient;
    ah = remainder;

}
/**
 * Divides with given value according to assembly rules (byte size)
 * @param value the value that will divide
 */
void div_func(unsigned short value){

    int number = dx * (2<<15) + ax;
    if(number / value > (2<<15)){
        of = true;
        throw "Error";
    }
    unsigned short quotient = number / value;
    unsigned short remainder = number % value;

    ax = quotient;
    dx = remainder;
}


/**
 * Basic logic functions
 * They apply corresponding bitwise operations bitween given position and given value.
 * Corresponding flags are sets according to operation.
 */

void xor_func(unsigned char* char_pointer, unsigned char value){

    *char_pointer = *char_pointer ^ value;
    unsigned char &result = *char_pointer;
    if(result >= (2<<6)){
        sf = true;
    }
    else
        sf = false;
    if(result == 0){
        zf = true;
    }
    else
        zf = false;
    cf = 0;
    of = 0;

}
void xor_func(unsigned short* short_pointer, unsigned short value){

    *short_pointer = *short_pointer ^ value;
    unsigned short &result = *short_pointer;
    if(result >= (2<<14)){
        sf = true;
    }
    else
        sf = false;
    if(result == 0){
        zf = true;
    }
    else
        zf = false;
    cf = 0;
    of = 0;


}

void or_func(unsigned char* char_pointer, unsigned char value){

    *char_pointer = *char_pointer | value;
    unsigned char &result = *char_pointer;
    if(result >= (2<<6)){
        sf = true;
    }
    else
        sf = false;
    if(result == 0){
        zf = true;
    }
    else
        zf = false;
    cf = 0;
    of = 0;


}
void or_func(unsigned short* short_pointer, unsigned short value){

    *short_pointer = *short_pointer | value;
    unsigned short &result = *short_pointer;
    if(result >= (2<<14)){
        sf = true;
    }
    else
        sf = false;
    if(result == 0){
        zf = true;
    }
    else
        zf = false;
    cf = 0;
    of = 0;


}

void and_func(unsigned short* short_pointer, unsigned short value){

    *short_pointer = *short_pointer & value;
    unsigned short &result = *short_pointer;
    if(result >= (2<<14)){
        sf = true;
    }
    else
        sf = false;
    if(result == 0){
        zf = true;
    }
    else
        zf = false;
    cf = 0;
    of = 0;


}
void and_func(unsigned char* char_pointer, unsigned char value){

    *char_pointer = *char_pointer & value;
    unsigned char &result = *char_pointer;
    if(result >= (2<<6)){
        sf = true;
    }
    else
        sf = false;
    if(result == 0){
        zf = true;
    }
    else
        zf = false;
    cf = 0;
    of = 0;


}

void not_func(unsigned char* char_pointer){

    *char_pointer = ~(*char_pointer);
    unsigned char &result = *char_pointer;
    if(result >= (2<<6)){
        sf = true;
    }
    else
        sf = false;
    if(result == 0){
        zf = true;
    }
    else
        zf = false;
    cf = 0;
    of = 0;

}
void not_func(unsigned short* short_pointer){

    *short_pointer = ~(*short_pointer);
    unsigned short &result = *short_pointer;
    if(result >= (2<<14)){
        sf = true;
    }
    else
        sf = false;
    if(result == 0){
        zf = true;
    }
    else
        zf = false;
    cf = 0;
    of = 0;


}

/**
 * Shift and Rotate functions
 */

void rcl_func(unsigned char* char_pointer){
    bool temp = cf;
    cf = (*char_pointer) >> 7;
    *char_pointer = *char_pointer << 1;
    *char_pointer += temp;
}
void rcl_func(unsigned short* short_pointer){
    bool temp = cf;
    cf = (*short_pointer) >> 15;
    *short_pointer = *short_pointer << 1;
    *short_pointer += temp;
}
void rcl_func(unsigned char* char_pointer, unsigned short value){
    for(int i = 0; i < value ; i++){
        rcl_func(char_pointer);
    }
    if(value == 1){
        of = cf ^ (*char_pointer >> 7);
    }
}
void rcl_func(unsigned short* short_pointer, unsigned short value){
    for(int i = 0; i < value ; i++){
        rcl_func(short_pointer);
    }
    if(value == 1){
        of = cf ^ (*short_pointer >> 15);
    }
}

void rcr_func(unsigned char* char_pointer){
    bool temp = cf;
    cf = (*char_pointer) % 2 ;
    *char_pointer = *char_pointer >> 1;
    *char_pointer += temp << 7;
}
void rcr_func(unsigned short* short_pointer){
    bool temp = cf;
    cf = (*short_pointer) % 2;
    *short_pointer = *short_pointer >> 1;
    *short_pointer += temp << 15;
}
void rcr_func(unsigned char* char_pointer, unsigned short value){
    for(int i = 0; i < value ; i++){
        rcr_func(char_pointer);
    }
    if(value == 1){
        of = (*char_pointer >> 6) ^ (*char_pointer >> 7);
    }
}
void rcr_func(unsigned short* short_pointer, unsigned short value){
    for(int i = 0; i < value ; i++){
        rcr_func(short_pointer);
    }
    if(value == 1){
        of = (*short_pointer >> 14) ^ (*short_pointer >> 15);
    }
}

void shl_func(unsigned char* char_pointer){
    cf = (*char_pointer) >> 7;
    *char_pointer = *char_pointer << 1;
}
void shl_func(unsigned short* short_pointer){
    cf = (*short_pointer) >> 15;
    *short_pointer = *short_pointer << 1;
}
void shl_func(unsigned char* char_pointer, unsigned short value){
    for(int i = 0; i < value ; i++){
        shl_func(char_pointer);
    }
    if(value == 1){
        of = cf ^ (*char_pointer >> 7);
    }
}
void shl_func(unsigned short* short_pointer, unsigned short value){
    for(int i = 0; i < value ; i++){
        shl_func(short_pointer);
    }
    if(value == 1){
        of = cf ^ (*short_pointer >> 15);
    }
}

void shr_func(unsigned char* char_pointer){
    cf = (*char_pointer) % 2;
    *char_pointer = *char_pointer >> 1;
}
void shr_func(unsigned short* short_pointer){
    cf = (*short_pointer) % 2;
    *short_pointer = *short_pointer >> 1;
}
void shr_func(unsigned char* char_pointer, unsigned short value){
    if(value == 1){
        of = (*char_pointer >> 7);
    }
    for(int i = 0; i < value ; i++){
        shr_func(char_pointer);
    }

}
void shr_func(unsigned short* short_pointer, unsigned short value){
    if(value == 1){
        of = (*short_pointer >> 15);
    }
    for(int i = 0; i < value ; i++){
        shr_func(short_pointer);
    }
}

/**
 * Stack functions
 */

void push_func(unsigned short value){

    memory[sp + 1] = *((unsigned char *)(((unsigned char*) &value) + 1)) ;
    memory[sp] = *((unsigned char *) &value);
    sp -= 2;

}
void pop_func(unsigned short *  short_pointer){
    sp += 2;
    if(sp == 0){
        throw "Error";
    }
    *short_pointer = *(unsigned short *)&(memory[sp]);
}

/**
 * Compare and Jump functions
 */

void cmp_func( unsigned short value1 , unsigned short value2){

    unsigned short * value1p = &value1;
    sub_func(value1p,value2);

}
void cmp_func( unsigned char value1 , unsigned char value2){

    unsigned char * value1p = &value1;
    sub_func(value1p,value2);

}

/**
 * Jumps functions
 * They return bool to let jump or not.
 */

bool jz_func(){
   return zf;
}
bool jnz_func(){
    return !zf;
}
bool je_func(){
    return zf;
}
bool jne_func(){
    return !zf;
}
bool ja_func(){
    return ( !cf && !zf );
}
bool jae_func(){
    return !cf ;
}
bool jb_func(){
    return cf;
}
bool jbe_func(){
    return (cf || zf );
}
bool jnae_func(){
    return cf;
}
bool jnb_func(){
    return !cf;
}
bool jnbe_func(){
    return ( !cf && !zf );
}
bool jnc_func(){
    return !cf;
}
bool jc_func(){
    return cf;
}

/**
 * Int functions
 */

void takeInput();
void display();
void int21h_func(){

    if(ah == 1){
        takeInput();
    }
    else if(ah == 2){
        display();
    }
    else throw "Error";
}
void takeInput(){
    cin >> al;
}
void display(){
    cout << dl;
}

/*
 * This method takes a line string as parameter and returns vector of operands in line.
 * */
vector<string> take_args(string line){
    vector<string> args;
    string nospace("");
    char *temp = (char *)(line.data());
    char* token = strtok(temp, " ");
    token = strtok(nullptr, " ");
    while(token != nullptr){
        nospace += token;
        token = strtok(nullptr, " ");
    }
    temp = (char *)(nospace.data());
    token = strtok(temp, ",");
    while(token != nullptr){
        string arg();

        args.push_back(token);
        token = strtok(nullptr, ",");
    }

    return args;
}
/*
 * This method takes a line string as parameter and returns instruction from line.
 * */
string take_func(string line){
    char *temp = (char *)(line.data());
    char* token = strtok(temp, " ");

    return string(token);
}

/*
 * Returns type of operand as char. 'w' if operand is word 'b' if operand is byte. 1 if it is not clear.
 * */
char getType(string arg){
    try{
        if(regs.find(arg) != regs.end()){
            return 'w';
        }
        else if(regs8.find(arg) != regs8.end()){
            return 'b';
        }
        else if(arg[0] == 'b'){
            return 'b';
        }
        else if(arg[0] == 'w'){
            return 'w';
        }
        else if(arg[0] == '['){
            return 1;
        }
        else if(arg.size() == 3 && arg[0] == '\'' && arg[2] == '\''){
            return 'b';
        }
        else if(interpretValue(arg.data()) > 256){
            return 'w';
        }
        else{
            return 1;
        }
    }
    catch(char* e){
        throw e;
    }
}
/*
 * Returns value of operand as short for word type operands. It is used for operands that do not need to change.
 * */
unsigned short getValue(string arg){
    try{
        if(regs.find(arg) != regs.end()){
            return *regs.find(arg)->second;
        }
        else if(arg[arg.length()-1] == ']'){
            if(arg[0] == '['){
                return *((unsigned short*) &memory[getValue(arg.substr(1, arg.length() - 2).data())]);
            }
            else if(arg[0] == 'w' && arg[1] == '['){
                return *((unsigned short*) &memory[getValue(arg.substr(2, arg.length()-3).data())]);
            }
            else throw "Error";
        }
        else{
            int val = interpretValue(arg.data());
            if(val >= (2<<15))throw"Error";
            return val;
        }
    }
    catch(char* e){
        throw e;
    }
}
/*
 * Returns value of operand as char for byte operands.
 * */
unsigned char getValue8(string arg){
    try {

        if(regs8.find(arg) != regs8.end()){
            return *regs8.find(arg)->second;
        }
        else if(arg[arg.length()-1] == ']'){
            if(arg[0] == '['){
                return memory[getValue(arg.substr(1, arg.length() - 2).data())];
            }
            else if(arg[0] == 'b' && arg[1] == '['){
                return memory[getValue(arg.substr(2, arg.length()-3).data())];
            }
            else throw "Error";
        }
        else if(arg.size() == 3 && arg[0] == '\'' && arg[2] == '\'') {
            return arg[1];
        }
        else if(arg.size() == 1 ){
            if(arg[0] < '0' || arg[0] > '9') throw "Error";
            else return arg[0] - '0';
        }
        else{
            int val = interpretValue(arg.data());
            if(val >= (2<<7))throw"Error";
            return val;        }

    }
    catch (char* e){
        throw e;
    }

}

/*
 * Returns pointer of operand for word type operands. It is used for operands that will be changed in operation.
 * */
unsigned short * getPointer(string arg){
    if(regs.find(arg) != regs.end()){
        return regs.find(arg)->second;
    }
    else if(arg[arg.length()-1] == ']'){
        if(arg[0] == '['){
            return (unsigned short *)(&memory[getValue(arg.substr(1, arg.length() - 2).data())]);
        }
        else if(arg[0] == 'w' && arg[1] == '['){
            return (unsigned short *)(&memory[getValue(arg.substr(2, arg.length()-3).data())]);
        }
        else throw "Error";
    }
    else throw "Error";
}
/*
 * Returns pointer of operand for byte type operands.
 * */
unsigned char * getPointer8(string arg){
    if(regs8.find(arg) != regs8.end()){
        return regs8.find(arg)->second;
    }
    else if(arg[arg.length()-1] == ']'){
        if(arg[0] == '['){
            return &memory[getValue(arg.substr(1, arg.length() - 2).data())];
        }
        else if(arg[0] == 'b' && arg[1] == '['){
            return &memory[getValue(arg.substr(2, arg.length()-3).data())];
        }
        else throw "Error";
    }
    else throw "Error";
}

/**
 * Whole process happens in main method.
 * First file parsed into Lines and varLines vectors
 * Then variables are defined in the memory.
 * After then, we traverse lines vector and each element represents one line and one function.
 * There is a great if statements because c++ doesn't have switch case for string.
 * For every function, corresponding controls are done and functions are calling.
 * When we reach int21h, we stop traversing and quit.
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char* argv[])
{
    try{
        readFile(argv[1]);
        defineVars();

    for(int cursor = 0; cursor < lines.size() ; cursor++){
//      cout << cursor << endl;                               // For Debugging.
        string l = lines.at(cursor);
        string func = take_func(l);
        vector<string> args = take_args(l);
        if(args.size() > 2)throw "Error";
        if(func.compare("mov") == 0) {
            if(args.size() != 2)throw "Error";
            char type1 = getType(args[0]);
            char type2 = getType(args[1]);
            if(type1 == 'w' || type2 == 'w'){
                if(type1 == 'b' || type2 == 'b')throw "Error";

                if((args[0][0] == 'w' || args[0][0] == '[') && (args[1][0] == 'w' || args[1][0] == '['))throw "Error";
                unsigned short* p = getPointer(args[0]);
                unsigned short v = getValue(args[1]);
                mov_func(p, v);

            }
            else{
                if((args[0][0] == 'b' && args[0][1] =='[' || args[0][0] == '[') && (args[1][0] == 'b' && args[1][1] == '[' || args[1][0] == '[')) throw "Error";


                unsigned char * p = getPointer8(args[0]);
                unsigned char v = getValue8(args[1]);

                mov_func(p, v);
            }

        }else if (func.compare("add") == 0) {

            if(args.size() != 2)throw "Error";
            char type1 = getType(args[0]);
            char type2 = getType(args[1]);

            if(type1 == 'w'){
                if((args[0][0] == 'w' || args[0][0] == '[') && (args[1][0] == 'w' || args[1][0] == '['))throw "Error";
                unsigned short* p = getPointer(args[0]);
                unsigned short v;
                if(type2 == 'b')   v = getValue8(args[1]);
                else   v = getValue(args[1]);
                add_func(p,v);
            }
            else if(type1 == 'b'){
                if(type2 == 'w') throw "Error";
                unsigned char * p = getPointer8(args[0]);
                unsigned char v = getValue8(args[1]);
                add_func(p, v);
            }
            else if(type2 == 'w'){
                unsigned short* p = getPointer(args[0]);
                unsigned short v = getValue(args[1]);
                add_func(p,v);
            }
            else if(type2 == 'b'){
                unsigned char * p = getPointer8(args[0]);
                unsigned char v = getValue8(args[1]);
                add_func(p, v);
            }
            else throw "Error";
        }else if (func.compare("sub") == 0){
            if(args.size() != 2)throw "Error";
            char type1 = getType(args[0]), type2 = getType(args[1]);
            if(type1 == 'w'){
                if((args[0][0] == 'w' || args[0][0] == '[') && (args[1][0] == 'w' || args[1][0] == '['))throw "Error";
                unsigned short* p = getPointer(args[0]);
                unsigned short v;
                if(type2 == 'b')   v = getValue8(args[1]);
                else   v = getValue(args[1]);
                sub_func(p,v);
            }
            else if(type1 == 'b'){
                if(type2 == 'w') throw "Error";
                unsigned char * p = getPointer8(args[0]);
                unsigned char v = getValue8(args[1]);
                sub_func(p, v);
            }
            else if(type2 == 'w'){
                unsigned short* p = getPointer(args[0]);
                unsigned short v = getValue(args[1]);
                sub_func(p,v);
            }
            else if(type2 == 'b'){
                unsigned char * p = getPointer8(args[0]);
                unsigned char v = getValue8(args[1]);
                sub_func(p, v);
            }
            else throw "Error";
        }else if (func.compare("inc") == 0){
            if(args.size() != 1)throw "Error";
            char type1 = getType(args[0]);
            if(type1 == 'w'){
                unsigned short* p = getPointer(args[0]);
                inc_func(p);
            }
            else{
                unsigned char * p = getPointer8(args[0]);
                inc_func(p);
            }
        }else if (func.compare("dec") == 0){
            if(args.size() != 1)throw "Error";
            char type1 = getType(args[0]);
            if(type1 == 'w'){
                unsigned short* p = getPointer(args[0]);
                dec_func(p);
            }
            else{
                unsigned char * p = getPointer8(args[0]);
                dec_func(p);
            }
        }else if (func.compare("mul") == 0){
            if(args.size() != 1)throw "Error";
            char type1 = getType(args[0]);
            if(type1 == 'w'){
                unsigned short p = getValue(args[0]);
                mul_func(p);
            }
            else if(type1 == 'b'){
                unsigned char p = getValue8(args[0]);
                mul_func(p);
            }
            else throw "Error";
        }else if (func.compare("div") == 0){
            if(args.size() != 1)throw "Error";
            char type1 = getType(args[0]);
            if(type1 == 'w'){
                unsigned short p = getValue(args[0]);
                div_func(p);
            }
            else if( type1 == 'b'){
                unsigned char p = getValue8(args[0]);
                div_func(p);
            }
            else throw "Error";
        }else if (func.compare("xor") == 0){
            if(args.size() != 2)throw "Error";
            char type1 = getType(args[0]), type2 = getType(args[1]);
            if(type1 == 'w' || type2 == 'w'){
                if(type1 == 'b' || type2 == 'b')throw "Error";
                if((args[0][0] == 'w' || args[0][0] == '[') && (args[1][0] == 'w' || args[1][0] == '['))throw "Error";
                unsigned short* p = getPointer(args[0]);
                unsigned short v = getValue(args[1]);
                xor_func(p, v);

            }
            else{
                if((args[0][0] == 'b' && args[0][1] =='[' || args[0][0] == '[') && (args[1][0] == 'b' && args[1][1] == '[' || args[1][0] == '[')) throw "Error";
                unsigned char * p = getPointer8(args[0]);
                unsigned char v = getValue8(args[1]);
                xor_func(p, v);
            }
        }else if (func.compare("or") == 0){
            if(args.size() != 2)throw "Error";
            char type1 = getType(args[0]), type2 = getType(args[1]);

            if(type1 == 'w' || type2 == 'w'){
                if(type1 == 'b' || type2 == 'b')throw "Error";
                if((args[0][0] == 'w' || args[0][0] == '[') && (args[1][0] == 'w' || args[1][0] == '['))throw "Error";
                unsigned short* p = getPointer(args[0]);
                unsigned short v = getValue(args[1]);
                or_func(p, v);

            }
            else{
                if((args[0][0] == 'b' || args[0][0] == '[') && (args[1][0] == 'b' || args[1][0] == '['))throw "Error";
                unsigned char * p = getPointer8(args[0]);
                unsigned char v = getValue8(args[1]);
                or_func(p, v);
            }
        }else if (func.compare("and") == 0){
            if(args.size() != 2)throw "Error";
            char type1 = getType(args[0]), type2 = getType(args[1]);
            if(type1 == 'w' || type2 == 'w'){
                if(type1 == 'b' || type2 == 'b')throw "Error";
                if((args[0][0] == 'w' || args[0][0] == '[') && (args[1][0] == 'w' || args[1][0] == '['))throw "Error";
                unsigned short* p = getPointer(args[0]);
                unsigned short v = getValue(args[1]);
                and_func(p, v);

            }
            else{
                if((args[0][0] == 'b' && args[0][1] =='[' || args[0][0] == '[') && (args[1][0] == 'b' && args[1][1] == '[' || args[1][0] == '[')) throw "Error";
                unsigned char * p = getPointer8(args[0]);
                unsigned char v = getValue8(args[1]);
                and_func(p, v);
            }
        }else if (func.compare("not") == 0){
            if(args.size() != 1)throw "Error";
            char type1 = getType(args[0]);
            if(type1 == 'w'){
                unsigned short* p = getPointer(args[0]);
                not_func(p);
            }
            else{
                unsigned char * p = getPointer8(args[0]);
                not_func(p);
            }
        }else if (func.compare("rcl") == 0){
            if(args.size() != 2)throw "Error";
            char type1 = getType(args[0]), type2 = getType(args[1]);
            if(type1 == 'w'){
                if((args[0][0] == 'w' || args[0][0] == '[') && (args[1][0] == 'w' || args[1][0] == '['))throw "Error";
                unsigned short* p = getPointer(args[0]);
                unsigned short v;
                if(type2 == 'w')   v = getValue(args[1]);
                else {
                    if(args[1][0] == '[') throw "Error";
                    v = getValue8(args[1]);
                }
                rcl_func(p,v);

            }
            else if(type1 == 'b'){
                if((args[0][0] == 'b' && args[0][1] =='[' || args[0][0] == '[') && (args[1][0] == 'b' && args[1][1] == '[' || args[1][0] == '[')) throw "Error";
                unsigned char* p = getPointer8(args[0]);
                unsigned short v;
                if(type2 == 'w')   v = getValue(args[1]);
                else {
                    if(args[1][0] == '[') throw "Error";
                    v = getValue8(args[1]);
                }
                rcl_func(p,v);
            }
            else throw "Error";

        }else if (func.compare("rcr") == 0){
            if(args.size() != 2)throw "Error";
            char type1 = getType(args[0]), type2 = getType(args[1]);
            if(type1 == 'w'){
                if((args[0][0] == 'w' || args[0][0] == '[') && (args[1][0] == 'w' || args[1][0] == '['))throw "Error";
                unsigned short* p = getPointer(args[0]);
                unsigned short v;
                if(type2 == 'w')   v = getValue(args[1]);
                else {
                    if(args[1][0] == '[') throw "Error";
                    v = getValue8(args[1]);
                }
                rcr_func(p,v);

            }
            else if(type1 == 'b'){
                if((args[0][0] == 'b' && args[0][1] =='[' || args[0][0] == '[') && (args[1][0] == 'b' && args[1][1] == '[' || args[1][0] == '[')) throw "Error";
                unsigned char* p = getPointer8(args[0]);
                unsigned short v;
                if(type2 == 'w')   v = getValue(args[1]);
                else {
                    if(args[1][0] == '[') throw "Error";
                    v = getValue8(args[1]);
                }
                rcr_func(p,v);
            }
            else throw "Error";


        }else if (func.compare("shr") == 0){
            if(args.size() != 2)throw "Error";
            char type1 = getType(args[0]), type2 = getType(args[1]);
            if(type1 == 'w'){
                if((args[0][0] == 'w' || args[0][0] == '[') && (args[1][0] == 'w' || args[1][0] == '['))throw "Error";
                unsigned short* p = getPointer(args[0]);
                unsigned short v;
                if(type2 == 'w')   v = getValue(args[1]);
                else {
                    if(args[1][0] == '[') throw "Error";
                    v = getValue8(args[1]);
                }
                shr_func(p,v);

            }
            else if(type1 == 'b'){
                if((args[0][0] == 'b' && args[0][1] =='[' || args[0][0] == '[') && (args[1][0] == 'b' && args[1][1] == '[' || args[1][0] == '[')) throw "Error";
                unsigned char* p = getPointer8(args[0]);
                unsigned short v;
                if(type2 == 'w')   v = getValue(args[1]);
                else {
                    if(args[1][0] == '[') throw "Error";
                    v = getValue8(args[1]);
                }
                shr_func(p,v);
            }
            else throw "Error";


        }else if (func.compare("shl") == 0){
            if(args.size() != 2)throw "Error";
            char type1 = getType(args[0]), type2 = getType(args[1]);
            if(type1 == 'w'){
                if((args[0][0] == 'w' || args[0][0] == '[') && (args[1][0] == 'w' || args[1][0] == '['))throw "Error";
                unsigned short* p = getPointer(args[0]);
                unsigned short v;
                if(type2 == 'w')   v = getValue(args[1]);
                else {
                    if(args[1][0] == '[') throw "Error";
                    v = getValue8(args[1]);
                }
                shl_func(p,v);

            }
            else if(type1 == 'b'){
                if((args[0][0] == 'b' && args[0][1] =='[' || args[0][0] == '[') && (args[1][0] == 'b' && args[1][1] == '[' || args[1][0] == '[')) throw "Error";
                unsigned char* p = getPointer8(args[0]);
                unsigned short v;
                if(type2 == 'w')   v = getValue(args[1]);
                else {
                    if(args[1][0] == '[') throw "Error";
                    v = getValue8(args[1]);
                }
                shl_func(p,v);
            }
            else throw "Error";


        }else if (func.compare("push") == 0){
            if(args.size() != 1)throw "Error";
            char type1 = getType(args[0]);
            if(type1 == 'w'){
                unsigned short v = getValue(args[0]);
                push_func(v);
            }
            else throw "Error";

        }else if (func.compare("pop") == 0){
            if(args.size() != 1)throw "Error";
            char type1 = getType(args[0]);
            if(type1 == 'w' || type1 == '1'){
                unsigned short* p = getPointer(args[0]);
                pop_func(p);
            }
            else throw "Error";

        }else if (func.compare("cmp") == 0){
            if(args.size() != 2)throw "Error";

            char type1 = getType(args[0]);
            char type2 = getType(args[1]);

            if(type1 == 'w' || type2 == 'w'){
                if(type1 == 'b' || type2 == 'b')throw "Error";
                if((args[0][0] == 'w' || args[0][0] == '[') && (args[1][0] == 'w' || args[1][0] == '['))throw "Error";

                unsigned short p = getValue(args[0]);
                unsigned short v = getValue(args[1]);

                cmp_func(p, v);

            }
            else{
                if((args[0][0] == 'b' && args[0][1] =='[' || args[0][0] == '[') && (args[1][0] == 'b' && args[1][1] == '[' || args[1][0] == '[')) throw "Error";
                unsigned char p = getValue8(args[0]);
                unsigned char v = getValue8(args[1]);
                cmp_func(p, v);
            }

        }else if (func.compare("nop") == 0){
            continue;


        }else if (func.compare("int20h") == 0){
            return 0;
        }else if (func.compare("int21h") == 0){
            int21h_func();
        }else if (func.compare("jz") == 0){
            if(args.size() != 1) throw "Error";
            if(labels.find(args[0]) != labels.end()){
                if(jz_func()) cursor = labels.find(args[0])->second;
            }
            else throw "Error";

        }else if (func.compare("jnz") == 0){
            if(args.size() != 1) throw "Error";
            if(labels.find(args[0]) != labels.end()){
                if(jnz_func()) cursor = labels.find(args[0])->second;
            }
            else throw "Error";

        }else if (func.compare("je") == 0){
            if(args.size() != 1) throw "Error";
            if(labels.find(args[0]) != labels.end()){
                if(je_func()) cursor = labels.find(args[0])->second;
            }
            else throw "Error";

        }else if (func.compare("jne") == 0){
            if(args.size() != 1) throw "Error";
            if(labels.find(args[0]) != labels.end()){
                if(jne_func()) cursor = labels.find(args[0])->second;
            }
            else throw "Error";

        }else if (func.compare("ja") == 0){
            if(args.size() != 1) throw "Error";
            if(labels.find(args[0]) != labels.end()){
                if(ja_func()) cursor = labels.find(args[0])->second;
            }
            else throw "Error";

        }else if (func.compare("jae") == 0){
            if(args.size() != 1) throw "Error";
            if(labels.find(args[0]) != labels.end()){
                if(jae_func()) cursor = labels.find(args[0])->second;
            }
            else throw "Error";

        }else if (func.compare("jb") == 0){
            if(args.size() != 1) throw "Error";
            if(labels.find(args[0]) != labels.end()){
                if(jb_func()) cursor = labels.find(args[0])->second;
            }
            else throw "Error";

        }else if (func.compare("jbe") == 0){
            if(args.size() != 1) throw "Error";
            if(labels.find(args[0]) != labels.end()){
                if(jbe_func()) cursor = labels.find(args[0])->second;
            }
            else throw "Error";

        }else if (func.compare("jnae") == 0){
            if(args.size() != 1) throw "Error";
            if(labels.find(args[0]) != labels.end()){
                if(jnae_func()) cursor = labels.find(args[0])->second;
            }
            else throw "Error";

        }else if (func.compare("jnb") == 0){
            if(args.size() != 1) throw "Error";
            if(labels.find(args[0]) != labels.end()){
                if(jnb_func()) cursor = labels.find(args[0])->second;
            }
            else throw "Error";

        }else if (func.compare("jnbe") == 0){
            if(args.size() != 1) throw "Error";
            if(labels.find(args[0]) != labels.end()){
                if(jnbe_func()) cursor = labels.find(args[0])->second;
            }
            else throw "Error";

        }else if (func.compare("jnc") == 0){
            if(args.size() != 1) throw "Error";
            if(labels.find(args[0]) != labels.end()){
                if(jnc_func()) cursor = labels.find(args[0])->second;
            }
            else throw "Error";

        }
        else if (func.compare("jc") == 0){
            if(args.size() != 1) throw "Error";
            if(labels.find(args[0]) != labels.end()){
                if(jc_func()) cursor = labels.find(args[0])->second;
            }
            else throw "Error";

        }
        else throw "Error";

    }

    }
    catch(const char*  e){
        cout << e;
    }
    return 0;
}