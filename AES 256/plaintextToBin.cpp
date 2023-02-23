#include <iostream>
#include <fstream>

using namespace std;

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
            printf("Invalid key\n");
            return -1;                                                                                                               }
   }
   return num[0]*16+num[1];
}

int main(){
    string input;
    ofstream file;
    file.open("input.txt",std::ios::binary);
    cin >>input;
    int len = input.length();
    unsigned char output[len/2];
    for(int i=0;i<len-1;i+=2){
        output[i/2]=string_to_byte(input[i],input[i+1]);
    }
    file.write(reinterpret_cast<char*>(&output), sizeof(output));
    file.close();
    return 0;
}




