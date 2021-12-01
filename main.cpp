#include<iostream>
#include<vector>
#include "header/SP.hpp"
#include <string>





void output(const std::string&filename,Floorplan&fp);
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


    fp.fixed_outline_based();


    /*
    
    do SA

    */

   //test for block swap , rotate function


    output(argv[4],fp);

    return 0;
}

void output(const std::string&filename,Floorplan&fp){
    /*
        not done
    */

   std::ofstream out {filename};
   if(!out){
    std::cerr<<"file can't open!\n";
    exit(1);
   }

   fp.getPacking();
    out << fp.outline.first<<" "<<fp.outline.second<<"\n";
    out << fp.blockHeight.size()<<"\n";
    for(int i = 0;i<fp.blockWidth.size();i++)
        out << fp.x_pos[i]<<" "<<fp.y_pos[i]<<" "<<fp.blockWidth[i]<<" "<<fp.blockHeight[i]<<" \n";
    


   out.close();
}
