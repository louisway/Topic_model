#include <iostream>
#include <fstream>
#include <string>
#include <time.h>
#include <stdlib.h>
//#include <stdio.h>

int main() {
    std::ifstream myReadFile;
    myReadFile.open("/home/louis/Desktop/content.txt");
    //char output[10000];
    int repear = 0;
    int max = -1;
    int min = 1000;
    for ( int lj = 0; lj < 1000000; lj++) {
        srand(time(NULL)* (lj));
        int ui_random = rand() % (100+1);
        if (repear != ui_random) {
            repear = ui_random;
            if (repear < min) {
                min = repear;
            }
            if (repear > max) {
               max = repear;
            }
            std::cout << repear << std::endl;
        }
    } 
    std::cout << max << std::endl;

    std::cout << min << std::endl;
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
    //std::cout << output << std::endl;
    }
    }*/
    int a=0,b=0;
    if (myReadFile.is_open()) {
        while (!myReadFile.eof()) {
            myReadFile >> output;
            //std::cout << output << std::endl;
            myReadFile >> output;
            //std::cout << output << std::endl;
            myReadFile >> num;
            //std::cout << num << std::endl;
            for( int i = 0; i < num; ++i){
                myReadFile >> output;
            }
            srand(time(NULL));
            double ui_random = ((double)rand()) / RAND_MAX;
            if(ui_random > 0.2) a++;
            else b++;
        }
        myReadFile.clear();
        myReadFile.seekg(0,std::ios::beg);
        while (!myReadFile.eof()) {
            myReadFile >> output;
            //std::cout << output << std::endl;
            myReadFile >> output;
            //std::cout << output << std::endl;
            myReadFile >> num;
            //std::cout << num << std::endl;
            for( int i = 0; i < num; ++i){
                myReadFile >> output;
            }
            srand(time(NULL)* (a + b));
            //double ui_random = ((double)rand()) % 100;
            //std::cout << rand() % 100 << std::endl;
            //if(ui_random > 0.9) a++;
            //else b++;
        }

    }
    myReadFile.close(); 
    std::cout<<  "a:" << a << "b:" << b << std::endl;
    return 1;
}
