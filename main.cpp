#include<iostream>
#include<vector>
#include "header/SP.hpp"



int LCS(std::vector<int>&S1,std::vector<int>&S2,std::vector<int>&Weight,std::vector<int>&Position);
void reverse(std::vector<int>&vec);




int main(int argc,char * argv[]){

    if(argc!=4){
        std::cerr<<"please enter correct format!\n";
        exit(1);
    }

    Floorplan fp(std::atof(argv[1]),argv[2],argv[3]);


    return 0;
}


void reverse(std::vector<int>&vec){
    for(int i = 0;i< vec.size()/2;i++){
        int j = vec.size() - i - 1;
        int temp = vec.at(i);
        vec.at(i) = vec.at(j);
        vec.at(j) = temp;
    }
}
int LCS(std::vector<int>&S1,std::vector<int>&S2,std::vector<int>&Weight,std::vector<int>&Position){
    std::vector<int>Indx_s2(S1.size(),0);
    std::vector<int>LengthRecord(S1.size(),0);

    //saving Indx_s2 info.  
    for(int i = 0;i<S1.size();i++)
        Indx_s2.at(S2.at(i)) = i;

    for(int i = 0;i<S1.size();i++){
        int blockId = S1.at(i);
        int P = Indx_s2.at(blockId); //get blockId in S2's index.

        Position.at(blockId) = LengthRecord.at(P);
        int t = Position.at(blockId) + Weight.at(blockId);
        for(int j = P;j < S2.size();j++)
        {
            if(t > LengthRecord[j])
                LengthRecord[j] = t;
            else 
                break;
        }
    }
    return *LengthRecord.rbegin();
}
