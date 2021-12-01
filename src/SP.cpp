#include"../header/SP.hpp" 
#include<iostream>
#include <algorithm>

void init_SP(const std::vector<int>&weight,SequencePair&sp){

    std::vector<std::pair<int,int>>weight_map;
    int n = weight.size();
    weight_map.resize(n);
    for(int i = 0; i < n; i++){
        weight_map.at(i).first = i; // block id 
        weight_map.at(i).second = weight.at(i);//correspond weight
    }
    // sort by weight
    std::sort(weight_map.begin(),weight_map.end(),
        [](const std::pair<int,int>&p1,const std::pair<int,int>&p2){
            return p1.second < p2.second;
        }
    );
    sp.S1.resize(n);
    sp.S2.resize(n);
    sp.S1_idx.resize(n);
    sp.S2_idx.resize(n);

    //do swap 
    //let sum of first half weight  approximate equal to sum of other half weight.
    for(int i = n/4 ;i < n/2;i++){
        std::pair<int,int>temp = weight_map.at(i);
        weight_map.at(i) = weight_map.at(i + n/2);
        weight_map.at(i + n/2) = temp; 
    }

    //S1 = <l1,l2>
    //S2 = <l2,l1>
    std::vector<int>l1;l1.resize(n/2);
    std::vector<int>l2;l2.resize(n-n/2);
    for(int i = 0;i<n/2;i++)
        l1.at(i) = weight_map.at(i).first;
    for(int i = n/2;i<n;i++)
        l2.at(i-n/2) = weight_map.at(i).first;

    //build sequence    
    for(int i = 0;i<n/2;i++){
        sp.S1.at(i) = l1.at(i); 
        sp.S1_idx.at(sp.S1.at(i)) = i;
    }
    for(int i = n/2;i<n;i++){
        sp.S1.at(i) = l2.at(i-n/2); 
        sp.S1_idx.at(sp.S1.at(i)) = i;
    }
    for(int i = 0;i<n-n/2;i++){
        sp.S2.at(i) = l2.at(i);
        sp.S2_idx.at(sp.S2.at(i)) = i;
    }
    for(int i = n-n/2;i<n;i++){
        sp.S2.at(i) = l1.at(i-(n-n/2));
        sp.S2_idx.at(sp.S2.at(i)) = i;
    }
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

std::pair<int,int> Floorplan::getPacking(){
    int width =  LCS(sp.S1,sp.S2,blockWidth,x_pos);
    reverse(sp.S1);
    int height =  LCS(sp.S1,sp.S2,blockHeight,y_pos);
    reverse(sp.S1);
    return {width,height};
}

void Floorplan::rotate(int blockId){
       int temp =  blockWidth.at(blockId);
       blockWidth.at(blockId) = blockHeight.at(blockId);
       blockHeight.at(blockId) = temp;
}

void SequencePair::showSequence(){


    for(int i = 0;i<S1.size();i++)
        std::cout<<S1.at(i)<<" ";
    std::cout<<"\n";
    for(int i = 0;i<S2.size();i++)
        std::cout<<S2.at(i)<<" ";
    std::cout<<"\n";

}
void SequencePair:: showSequenceIdx(){

    for(int i = 0;i<S1.size();i++)
        std::cout<<S1_idx.at(i)<<" ";
    std::cout<<"\n";
    for(int i = 0;i<S2.size();i++)
        std::cout<<S2_idx.at(i)<<" ";
    std::cout<<"\n";

}
void SequencePair:: swapBlock(bool s1,int id1,int id2) 
{
    auto & s = (s1) ? this->S1 : this->S2;
    auto & s_idx = (s1) ? this->S1_idx : this->S2_idx;
    int idx1 = s_idx.at(id1);
    int idx2 = s_idx.at(id2);
    std::swap(s.at(idx1),s.at(idx2));
    std::swap(s_idx.at(id1),s_idx.at(id2));
}
void Floorplan::swapBlock(int swapType,int id1,int id2){// if type = 0 , then swap only in sp1, type = 1 , swap only in sp2  , type = 2, swap both.  
    if(swapType==0)
        sp.swapBlock(true,id1,id2);
    else if(swapType==1)
        sp.swapBlock(false,id1,id2);
    else{
        sp.swapBlock(true,id1,id2);
        sp.swapBlock(false,id1,id2);
    }
}
Floorplan::Floorplan(float alpha,const std::string&blockfile,const std::string&netfile){
    this->alpha = alpha;
    //std::cout<<"alpha : "<<alpha<<"\n";
    std::ifstream block{blockfile};
    if(!block){std::cerr<<"open"<<blockfile<<" failed\n";exit(1);}    


    std::string fileGarbageToken;
/*
 get Block info 
*/
    int blockNum,terminalNum;
    // get outline
    block >> fileGarbageToken >> outline.first >> outline.second;
    //std::cout<<"outline:"<<outline.first<<" "<<outline.second<<"\n";
    block >> fileGarbageToken >> blockNum ;
    block >> fileGarbageToken >> terminalNum;
    //std::cout<<"block:"<<blockNum<<" terminal : "<<terminalNum<<"\n";


    blockHeight.resize(blockNum);
    blockWidth.resize(blockNum);
    terminals.resize(terminalNum);

    for(int i = 0;i < blockNum; i++){
        std::string blockName;
        int w,h;
        block >> blockName >> w >> h;
     //   std::cout<<  blockName <<" "<< w <<" " <<h<<" \n";
        blockWidth.at(i) = w;
        blockHeight.at(i) = h;
        blockMap.insert({blockName,i});
    }

    for(int i = 0; i < terminalNum ; i++){
        std::string terminalName;
        int x,y;
        block >> terminalName >> fileGarbageToken >> x >> y;
    //    std::cout<<  terminalName <<" " <<x <<" "<< y<<" "<<"\n";
        TerminalMap.insert({terminalName,i});
        terminals.at(i) = {x,y};
    }
    block.close();

    std::ifstream net{netfile};
    if(!net){std::cerr<<"open"<<netfile<<" failed\n";exit(1);}    
    int netNum;
    net >> fileGarbageToken >> netNum;
    Netlist.resize(netNum);
    for(int i = 0;i < netNum; i ++){
        int degree;
        net >> fileGarbageToken >> degree;
        for(int j = 0;j < degree;j++){
            std::string tName;
            net >> tName;
            auto blockIter = blockMap.find(tName);
            // is a block name
            if(blockIter!= blockMap.end()){
                Netlist.at(i).blocks.push_front(blockIter->second);
            }
            // is a terminal name
            else{
                auto terminalIter = TerminalMap.find(tName);
                if(terminalIter == TerminalMap.end()){
                    std::cerr<<"error in read net terminal!\n";
                    exit(1);
                }
                Netlist.at(i).terminals.push_front(terminalIter->second);
            }
        }
    }

    net.close();
    //init Sequence pair
    init_SP(blockHeight,sp);

   /* 
    for(int i = 0 ;i<sp.S1.size();i++)
        std::cout<<sp.S1.at(i)<<" ";
    std::cout<<"\n";
    for(int i = 0 ;i<sp.S2.size();i++)
        std::cout<<sp.S2.at(i)<<" ";
        */

    /*
   // trivial init , but not good enough. 
    sp.S1.resize(blockHeight.size());
    sp.S2.resize(blockHeight.size());
    for(int i = 0;i<blockHeight.size();i++)
    {
        sp.S1.at(i) = i;
        sp.S2.at(i) = i;
    }
    */
    x_pos.resize(blockWidth.size());
    y_pos.resize(blockWidth.size());
    
}
