#include <iostream>
#include <fstream>

using namespace std;

typedef uint8_t b;
b state[16];

int main( ){
//    fstream file;
  //  file.open("input.txt",ios::binary);
    //file.seekp(0, ios::end);
//    unsigned long long file_size = file.tellp();
//    file.seekp(0, ios::beg);
//    file.read(( char *)&inpt[3], sizeof( inpt));
//    cout<<inpt[2];
    
//    return 0;
    fstream file;
    string path = "new.txt";
    file.open("new.txt",std::ios::out | std::ios::in);
    if( file.fail( )){
        cerr<<" ERROR: file input failed.\n" ;
        exit( 0);
    }

    file.seekp( 0, ios::end);
    unsigned long long file_size = file.tellp( ); // size in bytes
    file.seekp( 0,ios::beg);
    //turn the input file into blocks
    //    for( int i=0;i<file_size;i+=BLOCK_SIZE){
    // state: current block
    file.read( ( char*)&state, sizeof( state));
    for( unsigned char k : state) cout<<k<<"  " ;
    
}
