#include<iostream>
#include<vector>
#include "header/SP.hpp"
#include <string>




void output(const std::string&filename);

int main(int argc,char * argv[]){

    if(argc!=5){
        std::cerr<<"please enter correct format!  $ ./Lab3.exe <alpha> <input.block name> <input.net name> <output name>  \n";
        exit(1);
    }

    Floorplan fp(std::atof(argv[1]),argv[2],argv[3]);

    // initial packing using greedy approach
    //show the width , height
    auto packing = fp.getPacking();
    std::cout<<"width : "<<packing.first<<" height "<<packing.second<<"\n";


    /*
    
    do SA

    */


    output(argv[4]);

    return 0;
}

void output(const std::string&filename){
    /*
        not done
    */
}
