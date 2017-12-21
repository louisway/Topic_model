#include <iostream>
#include <fstream>
#include <string>

int main() {
    std::ifstream myReadFile;
    myReadFile.open("/home/louis/Desktop/content.txt");
    //char output[10000];
    std::string output;
    int num = 0;
    /*for( int i = 0; i < 100; ++i) {
    if (i > 2) break;
    if ( i == 2 ) {
    myReadFile >> num;
    std::cout << num + 1 << std::endl;
    }
    else {
    myReadFile >> output;
    std::cout << output << std::endl;
    }
    }*/
    if (myReadFile.is_open()) {
        while (!myReadFile.eof()) {
            myReadFile >> output;
            std::cout << output << std::endl;
            myReadFile >> output;
            std::cout << output << std::endl;
            myReadFile >> num;
            std::cout << num << std::endl;
            for( int i = 0; i < num; ++i){
                myReadFile >> output;
            }
        }
    }
    myReadFile.close(); 
    return 1;
}
