#include<iostream>
#include<vector>




int LCS(std::vector<int>&S1,std::vector<int>&S2,std::vector<int>&Weight,std::vector<int>&Position);
void reverse(std::vector<int>&vec);




int main(){


    std::vector<int>S1{3,2,0,5,1,4};
    std::vector<int>S2{5,2,4,3,0,1};
    std::vector<int>Width{4,3,3,2,4,6};
    std::vector<int>Height{6,7,3,3,3,4};
    
    std::vector<int>Hpos(6,0);
    std::vector<int>Vpos(6,0);
    int HorizontalLength = LCS(S1,S2,Width,Hpos);
    reverse(S1);
    int VerticalLength = LCS(S1,S2,Height,Vpos);


    std::cout<<"Horizontal L :"<<HorizontalLength<<"\n";
    for(int i = 0;i<6;i++)
        std::cout<<"block i x:"<<Hpos.at(i)<<" ";
    std::cout<<"\n";

    std::cout<<"Vertical L :"<<VerticalLength<<"\n";
    for(int i = 0;i<6;i++)
        std::cout<<"block i y:"<<Vpos.at(i)<<" ";
    std::cout<<"\n";
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
