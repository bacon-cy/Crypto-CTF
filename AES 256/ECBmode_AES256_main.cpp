#include <iostream>
#include <string>
#include <fstream>


#define KEY_LEN 32 //bytes
#define KEY_HEX 64 //2*key_len
#define BLOCK_SIZE 16 //bytes
#define Nk 8
#define Nb 4
#define Nr 14
typedef uint8_t byte;
typedef uint8_t word[4];
using namespace std;

// uint8_t : 0~255
static const byte sbox[256] = {
            //0    1     2     3     4    5     6     7      8    9     A      B    C     D     E     F
     /*0*/ 0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
     /*1*/ 0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
     /*2*/ 0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
     /*3*/ 0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
     /*4*/ 0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
     /*5*/ 0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
     /*6*/ 0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
     /*7*/ 0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
     /*8*/ 0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
     /*9*/ 0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
     /*A*/ 0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
     /*B*/ 0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
     /*C*/ 0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
     /*D*/ 0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
     /*E*/ 0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
     /*F*/ 0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16 };

static const byte inverse_sbox[256] = {
    //0    1     2     3     4    5     6     7      8    9     A      B    C     D     E     F
    0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
    0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
    0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
    0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
    0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
    0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
    0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
    0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
    0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
    0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
    0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
    0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
    0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
    0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
    0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
    0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d };

static const byte Rcon[11] = {
        0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36 };


byte state[4*Nb]; // hex1 * 16 + hex2 = hex1hex2
/* State Block
* 0 4 8  12
* 1 5 9  13
* 2 6 10 14
* 3 7 11 15
*/
word roundKey[Nb*(Nr+1)]; //each round + init round

//key generation
void keyGeneration(const byte* key);
void rotWord(word _word);
void subWord(word _word);
//encrypt
void encrypt();
void subBytes(byte* _state);
void shiftRows(byte* _state);
byte mult(byte a, byte b);
void mixColumns(byte* _state);
void addRoundKey(int round, byte* _state);
//decrypt
void decrypt();
void invAddRoundKey(int round, byte* _state);
void invShiftRows(byte* _state);
void invSubBytes(byte* _state);
void invMixColumns(byte* _state);
//debug
void printState();
void printKey(int _round);

//file io
int string_to_byte(char first, char second);
bool valid_path(char in);
void padding(int start);
int unpadding();

// ./main
int main() {
    string input;
    char path[1001];
    char outpath[1001];
    bool encr = true;
    byte *_key = (byte *) malloc(sizeof(byte) * KEY_LEN);

    getline(cin,input);
    unsigned long long len = input.length();
    if(len > 1000){
        cerr<<"ERROR: the input line is too long, please adjust the line length smaller than 1000.\n";
        exit(0);
    }

    int inputindex = 0;
    while(inputindex+1 < len) {
        if (input[inputindex] != '-'){
            inputindex++;
            continue;
        }
        switch (input[inputindex+1]) {
            case 'h' : { // -h : help
                printf("Use console line input {-e/-d} -k <key> <file address>.\n");
                printf("-e : encrypt\n -d : decrypt\n -k <key> : Input the key in 64 hex words, 256bits.\n");
                printf("-f <file> : Input the relative path or absolute path of the file, which is going to be en/decrpypted. The path must contain English and '/' only.\n");
                break;
            }
            case 'e' : { // -e : encrypt
                encr = true;
                break;
            }
            case 'd' : { // -d : decrypt
                encr = false;
                break;
            }
            case 'k' : {
                //-k <key> : input key (AES256, input the key in 64 hex words ( = 32 Bytes = 256 bits))
                // Capital or not is fine
                char keystr[KEY_HEX];
                while (inputindex < len &&
                       !((input[inputindex] <= '9' && input[inputindex] >= '0')
                         || (input[inputindex] <= 'F' && input[inputindex] >= 'A')
                         || (input[inputindex] <= 'f' && input[inputindex] >= 'a')))
                    inputindex++;
                for (int i = 0; i < KEY_HEX; inputindex++) {
                    keystr[i++] = input[inputindex];
                }
                int _temp = 0;
                for (int i = 0; i < KEY_HEX; i += 2) {
                    _temp = string_to_byte(keystr[i], keystr[i + 1]);
                    if (_temp == -1) return 0;
                    _key[i/2] = _temp;
                }
                break;
            }
            case 'i': { //-i <inputfile> : input the file address
                int path_len = 0;
        		inputindex+=2;
                while (inputindex < len && (input[inputindex] == ' ' || input[inputindex] == '\t')) inputindex++;
                while (inputindex < len && valid_path(input[inputindex])) path[path_len++] = input[inputindex++];
                path[path_len] = '\0';
                break;
            }
            case 'o': { //-o <outputfile>
                int outpath_len = 0;
                inputindex+=2;
                while (inputindex < len && (input[inputindex] == ' ' || input[inputindex] == '\t')) inputindex++;
                while (inputindex < len && valid_path(input[inputindex])) outpath[outpath_len++] = input[inputindex++];
                outpath[path_len] = '\0';
                break;
            }
            default : {
                cerr<<"ERROR: wrong input,'"<<input[inputindex+1] <<"', please enter -h for help.\n";
                continue;
            }
        }
        inputindex++;
    }
    //key generation function
    keyGeneration(_key);

    //input file, block size is 16 Bytes

    //check istream and fstream is working
    ifstream file;
    ofstream output;
    file.open(path, std::ios::in | std::ios::binary);
    if(file.fail()){
        cerr<<"ERROR: File input failed.\n";
        exit(0);
    }
    output.open(outpath, std::ios::app | std::ios::binary);
    if(output.fail()){
        cerr<<"ERROR: Can not create or open the output file.\n";
        exit(0);
    }
    
    //get input file size
    file.seekg(0, ios::end);
    unsigned long long file_size = file.tellg(); // size in bytes
    file.seekg(0,ios::beg);
    
    //in decryption, file size should be multiple of 16
    if(!encr && file_size%BLOCK_SIZE){
        cerr<<"ERROR: The input file for decreption must be a multiple of 16 bytes.\n";
        exit(0);
    }
    
    //start input and processing
    for(int i=0;i<file_size;i+=BLOCK_SIZE){
        file.read((char*)&state, sizeof(state));
        if(i+BLOCK_SIZE>file_size) padding(file_size-i);
        
        int unpad = 0;
        if(encr)
            encrypt();
        else{
            decrypt();
            if(i+BLOCK_SIZE>=file_size) unpad = unpadding();
        }
        
        output.write((char*)&state, sizeof(state)-unpad);
    }

    return 0;
}


void keyGeneration(const byte* key){
    //key includes 32 bytes, each word include 4 bytes, there are 8 words init
    for(int i=0; i<32; i++){
        roundKey[i/4][i%4] = key[i];
    }
    word temp;
    //generating round keys, there are Nb(Nr+1) words = 4*15 = 60
    for(int i=8;i<60;i++){ //for each words
        for(int _=0; _<4; _++) temp[_] = roundKey[i-1][_];
        if(i % Nk == 0){
            rotWord(temp);
            subWord(temp);
            temp[0] ^= Rcon[i/Nk];
        }else if(i % Nk == 4){
            subWord(temp);
        }
        for(int _=0; _<4; _++) roundKey[i][_] = roundKey[i-Nk][_] ^ temp[_];
    }
}

void rotWord(word _word){
    byte _temp = _word[0];
    for(int i=0;i<3;i++) _word[i] = _word[i+1];
    _word[3] = _temp;
}

void subWord(word _word){
    for(int i=0;i<4;i++) _word[i] = sbox[_word[i]];
}


void encrypt(){
    // init round
    //    printf("round[0].input  "); printState();
    //    printf("round[0].k_sch  "); printKey(0);
    addRoundKey(0,state);
    
    //Nr-1 main rounds
    for(int round = 1; round<Nr ; round++){
        //    printf("round[%d].start  ",round); printState();
        subBytes(state);
        //    printf("round[%d].s_box  ",round); printState();
        shiftRows(state);
        //    printf("round[%d].s_row  ",round); printState();
        mixColumns(state);
        //    printf("round[%d].m_col  ",round); printState();
        addRoundKey(round,state);
        //    printf("round[%d].k_sch  ",round); printKey(round);
    }

    //final round
    subBytes(state);
    shiftRows(state);
    addRoundKey(Nr, state);
}

void subBytes(byte* _state){
    for(int i=0;i<16;i++)  _state[i] = sbox[_state[i]];
}

void shiftRows(byte* _state){
    byte _temp[4];
    //s [r' ,c] = s [r,(c+ shift (r, Nb))] mod Nb, by FIPS 197 page 17
    for(int c=1;c<Nb;c++){ // c: column
        for(int k=0;k<Nb;k++) _temp[k] = _state[k*Nb + c];
        for(int k=0;k<Nb;k++){
            _state[k*Nb + c] = _temp[(c+k)%Nb];
        }
    }
}

byte mult(byte a, byte b){ //exponentiating by squaring
    byte ans = 0;
    byte _a = a, _b = b;
    while(b){
        if(b & 0x01) ans ^= a;

        // a *= 2
        if(a & 0x80) a = (a<<1) ^ 0x1b;
        else a<<=1;
        
        b>>=1;
    }
    return ans;
}

void mixColumns(byte* _state){
    byte _temp[4];
    for(int c=0;c<Nb;c++){
        for(int k=0;k<Nb;k++) _temp[k] = _state[c*Nb + k];
        // by FIPS 197 page 18
        _state[c*Nb    ] = mult(_temp[1],3) ^ mult(_temp[0],2) ^ mult(_temp[3],1) ^ mult(_temp[2],1) ;
        _state[c*Nb + 1] = mult(_temp[2],3) ^ mult(_temp[1],2) ^ (_temp[0]) ^ (_temp[3]) ;
        _state[c*Nb + 2] = mult(_temp[3],3) ^ mult(_temp[2],2) ^ (_temp[1]) ^ (_temp[0]) ;
        _state[c*Nb + 3] = mult(_temp[0],3) ^ mult(_temp[3],2) ^ (_temp[2]) ^ (_temp[1]) ;
    }
}

void addRoundKey(int round, byte* _state){
    //[s' 0, c , s '1, c , s ' 2, c , s ' 3, c ]=[ s 0, c , s1,c , s 2, c , s3,c ] XOR [w [round*Nb+c] ]
    for(int i=0;i<Nb;i++){
        for(int k=0;k<4;k++) {
            _state[i*4+k] ^= roundKey[round*4+i][k];
        }
    }
}

void decrypt(){
    //init round, the key is (Nr)th round key
    
    //    printf("round[0].iinput  ");printState();
    //    printf("round[0].ik_sch  ");printKey(Nr);
    addRoundKey(Nr,state);
    
    for(int i=Nr-1;i>=1;i--){
        //    printf("round[%d].istart  ",Nr-i); printState();
        invShiftRows(state);
        //    printf("round[%d].is_row  ",Nr-i); printState();
        invSubBytes(state);
        //    printf("round[%d].is_box  ",Nr-i); printState();
        addRoundKey(i,state);
        //    printf("round[%d].ik_sch  ",Nr-i); printKey(i);
        //    printf("round[%d].ik_add  ",Nr-i); printState();
        invMixColumns(state);
    }
    
    //final round, key is 0 round key
    invShiftRows(state);
    invSubBytes(state);
    addRoundKey(0,state);
}

void invShiftRows(byte* _state){
    //s [r' ,(c+ shift (r, Nb)) mod Nb] = s [r,c] for 0 < r < 4 and 0 <= c < Nb, by FIPS 197 page 21
    byte _temp[4];
    for(int r=1;r<Nb;r++){
        for(int c=0;c<Nb;c++) _temp[c] = _state[c*Nb + r];
        for(int c=0;c<Nb;c++) _state[c*Nb + r] = _temp[(c-r+Nb)%Nb];
    }
}

void invSubBytes(byte* _state){
    for(int i=0;i<16;i++) _state[i] = inverse_sbox[_state[i]];
}

void invMixColumns(byte* _state){
    byte _temp[4];
    for(int c=0;c<Nb;c++){
        for(int k=0;k<Nb;k++) _temp[k] = _state[c*Nb + k];
        _state[c*Nb    ] = mult(_temp[0],0x0e) ^ mult(_temp[1],0x0b) ^ mult(_temp[2],0x0d) ^ mult(_temp[3],0x09) ;
        _state[c*Nb + 1] = mult(_temp[1],0x0e) ^ mult(_temp[2],0x0b) ^ mult(_temp[3],0x0d) ^ mult(_temp[0],0x09) ;
        _state[c*Nb + 2] = mult(_temp[2],0x0e) ^ mult(_temp[3],0x0b) ^ mult(_temp[0],0x0d) ^ mult(_temp[1],0x09) ;
        _state[c*Nb + 3] = mult(_temp[3],0x0e) ^ mult(_temp[0],0x0b) ^ mult(_temp[1],0x0d) ^ mult(_temp[2],0x09) ;
    }
}

int string_to_byte(char first, char second){
    int num[2];
    char input[2];
    input[0] = first;
    input[1] = second;
    for(int i=0;i<2;i++){
        if(input[i]<='9' && input[i] >='0'){
            num[i] = input[i]-'0';
        }else if(input[i]<='F' && input[i]>='A'){
            num[i] = input[i]-'A'+10;
        }else if(input[i]<='f' && input[i]>='a'){
            num[i] = input[i]-'a'+10;
        }else{
            cerr<<"Invalid key\n";
            return -1;
        }
    }
    return num[0]*16+num[1];
}

bool valid_path(char in){
    //path : exactly only can use these characters : A–Z a–z 0–9 '.' '_' '-' '\' '/'
    // no space !!
    if((in <='Z' && in >='A') || (in <='z' && in >='a') || (in<='9' && in>='0')
        || in == '.' || in == '-' || in == '_' || in == '\\' || in == '/'){
        return true;
    }
    return false;
}

void padding(int start){
    for(int i=start;i<BLOCK_SIZE;i++)state[i] = BLOCK_SIZE - start;
}

int unpadding(){
    byte pad = state[15];
    if(pad > 15) return 0;
    for(int i=0;i<pad;i++){
        if(state[15-i]!=pad) return 0;
    }
    return pad;
}

void printState(){
    for(int i=0; i<BLOCK_SIZE; i++) printf("%.2x", state[i]);
    cout<<"\n";
}

void printKey(int round){
    for(int i=0; i<BLOCK_SIZE; i++) printf("%.2x", roundKey[i/4+round*4][i%4]);
    cout<<"\n";
}








