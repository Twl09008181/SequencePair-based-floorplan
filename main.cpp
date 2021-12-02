#include<iostream>
#include <algorithm>
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


    //do some op
    fp.fixed_outline_based();

    fp.compact_left();

    std::vector<std::pair<int,int>>Hcstr(fp.blockHeight.size(),{0,0});
    for(int i = 0;i<fp.blockHeight.size();i++){
        Hcstr.at(i).first = fp.x_pos.at(i) + fp.blockWidth.at(i);
        Hcstr.at(i).second = i;
    }
    std::sort(Hcstr.begin(),Hcstr.end(),[](std::pair<int,int>&p1,std::pair<int,int>&p2){
        return p1.first > p2.first;
    });

    /*
    int n = fp.x_pos.size();
    for(int i = 0;i<=7;i++){
        if(i==7){

    fp.sp.showSequence();
        }
        compact_left_block(i,fp.x_pos.at(i),fp.sp,fp.y_pos,fp.blockHeight,Hcstr);
    }
*/
    fp.sp.showSequence();

    /*
    std::vector<std::pair<int,int>>Hcstr(fp.blockHeight.size(),{0,0});
    for(int i = 0;i<fp.blockHeight.size();i++){
        Hcstr.at(i).first = fp.x_pos.at(i) + fp.blockWidth.at(i);
        Hcstr.at(i).second = i;
    }
    std::sort(Hcstr.begin(),Hcstr.end(),[](std::pair<int,int>&p1,std::pair<int,int>&p2){
        return p1.first > p2.first;
    });

    compact_left_block(2,fp.x_pos.at(2),fp.sp,fp.y_pos,fp.blockHeight,Hcstr);
*/

    //fp.sp.showSequence();
    //shiftSp(fp.sp.S1,0,5);
    //fp.sp.showSequence();

    /*
    std::vector<bool> side = findSide(fp.sp,14,-1,-1);

    for(int i = 0;i<side.size();i++){
        if(side.at(i))
        {
            std::cout<<i<<" ";
        }
    }

        std::cout<<"\n";
*/
/*
    compact_left_block(19,fp.x_pos.at(19),fp.sp,fp.y_pos,fp.blockHeight,Hcstr);
    compact_left_block(2,fp.x_pos.at(2),fp.sp,fp.y_pos,fp.blockHeight,Hcstr);
    compact_left_block(24,fp.x_pos.at(24),fp.sp,fp.y_pos,fp.blockHeight,Hcstr);
*/



/*
    fp.rotate(6);
    fp.moveto(6,0,2);
    fp.rotate(5);
    packing = fp.getPacking();
    std::cout<<"width : "<<packing.first<<" height "<<packing.second<<"\n";
*/


    /*
    
    do SA

    */

   //test for block swap , rotate function


    std::cout<<"out\n";
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

 //  fp.getPacking();
    out << fp.outline.first<<" "<<fp.outline.second<<"\n";
    out << fp.blockHeight.size()<<"\n";
    for(int i = 0;i<fp.blockWidth.size();i++)
        out << fp.x_pos[i]<<" "<<fp.y_pos[i]<<" "<<fp.blockWidth[i]<<" "<<fp.blockHeight[i]<<" \n";
    


   out.close();
}
