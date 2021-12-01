#include"../header/SP.hpp" 
#include<iostream>
#include <algorithm>

void SequencePair::setIdx()
{
    for(int i = 0;i < S1.size();i++)
        S1_idx.at(S1.at(i)) = i;
    for(int i = 0;i < S1.size();i++)
        S2_idx.at(S2.at(i)) = i;
}

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
    for(int i = 0;i<n/2;i++)
        sp.S1.at(i) = l1.at(i); 
    for(int i = n/2;i<n;i++)
        sp.S1.at(i) = l2.at(i-n/2); 
    for(int i = 0;i<n-n/2;i++)
        sp.S2.at(i) = l2.at(i);
    for(int i = n-n/2;i<n;i++)
        sp.S2.at(i) = l1.at(i-(n-n/2));
    
    sp.setIdx();
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


int Floorplan::getHPWL(){
    int length = 0;
    for(int i = 0;i< Netlist.size();i++){
        auto &blk = Netlist.at(i).blocks;
        auto &term = Netlist.at(i).terminals;
        //empty net
        if(blk.empty()&&term.empty())continue;
        double l = 2147483647,d = 2147483647;
        double r = 0, t = 0;
        for(auto ptr = blk.begin();ptr!=blk.end();++ptr){
            int blkid =  *ptr;
            double x = x_pos.at(blkid) + blockWidth.at(blkid)  / 2; 
            double y = y_pos.at(blkid) + blockHeight.at(blkid) / 2;
            if (x <  l)l = x;
            if (x >  r)r = x;
            if (y <  d)d = y;
            if (y <  t)t = y;
        }
        for(auto ptr = term.begin();ptr!=term.end();++ptr){
            int tid = *ptr;
            double x = terminals.at(tid).x; 
            double y = terminals.at(tid).y; 
            if (x <  l)l = x;
            if (x >  r)r = x;
            if (y <  d)d = y;
            if (y <  t)t = y;
        }
        int x_l = r-l;
        int y_l = t-d;
        length += (x_l + y_l);
    }
    return length;
}



void Floorplan::moveto(int moveBlock,int targetBlock,int relation) {
    int target_s1_idx = sp.S1_idx.at(targetBlock);
    int target_s2_idx = sp.S2_idx.at(targetBlock);
    int s1_idx = sp.S1_idx.at(moveBlock);   
    int s2_idx = sp.S2_idx.at(moveBlock);   


    //如果目標本來就在右邊,則扣1
    //如果目標在序列中為左邊,則目標當前的index為此moveblock將來的index
    if(relation==0) { //put left
        if(s1_idx < target_s1_idx)target_s1_idx -=1; 
        if(s2_idx < target_s2_idx)target_s2_idx -=1; 
    }
    else if(relation==1) { //put right 
        if(s1_idx > target_s1_idx)target_s1_idx +=1; 
        if(s2_idx > target_s2_idx)target_s2_idx +=1; 
    }
    else if(relation==2){// put top
        if(s1_idx < target_s1_idx)target_s1_idx -=1; 
        if(s2_idx > target_s2_idx)target_s2_idx +=1; 
    }
    else if(relation==3){// put down 
        if(s1_idx > target_s1_idx)target_s1_idx +=1; 
        if(s2_idx < target_s2_idx)target_s2_idx -=1; 
    }

    if(s1_idx < target_s1_idx){
        for(int i = s1_idx+1;i <=target_s1_idx;i++)
            sp.S1.at(i-1) = sp.S1.at(i);
    }else if(s1_idx > target_s1_idx){
        for(int i = s1_idx-1;i >=target_s1_idx;i--)
            sp.S1.at(i+1) = sp.S1.at(i);
    }
    if(s2_idx < target_s2_idx){
        for(int i = s2_idx+1;i <=target_s2_idx;i++)
            sp.S2.at(i-1) = sp.S2.at(i);
    }else if(s2_idx > target_s2_idx){
        for(int i = s2_idx-1;i >=target_s2_idx;i--)
            sp.S2.at(i+1) = sp.S2.at(i);
    }
    sp.S1.at(target_s1_idx) = moveBlock;
    sp.S2.at(target_s2_idx) = moveBlock;
    sp.setIdx();
}

void Floorplan::updateSlack(){
    x_slack.clear();x_slack.resize(x_pos.size(),0);
    y_slack.clear();y_slack.resize(x_pos.size(),0);
    std::vector<int>rev_x_pos(x_pos.size(),0);
    std::vector<int>rev_y_pos(x_pos.size(),0);
    reverse(sp.S1);
    reverse(sp.S2);
    int xSize =  LCS(sp.S1,sp.S2,blockWidth,rev_x_pos);// from right
    reverse(sp.S1);
    int ySize =  LCS(sp.S1,sp.S2,blockWidth,rev_y_pos);//from top
    //recover S2
    reverse(sp.S2);
    for(int i = 0;i<sp.S1.size();i++){
        x_slack.at(i) = xSize - rev_x_pos.at(i) - blockWidth.at(i) - x_pos.at(i);
        y_slack.at(i) = ySize - rev_y_pos.at(i) - blockHeight.at(i) - y_pos.at(i);
    }
}
void Floorplan::fixed_outline_based(){

}
