#include"../header/SP.hpp" 
#include<iostream>
#include <algorithm>
#include <set>



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


bool overlapIndim(std::vector<int>&Dimpos, std::vector<int>&DimWeight,int id1,int id2)
{


    int d_pos1s = Dimpos.at(id1);
    int d_pos1e = Dimpos.at(id1) + DimWeight.at(id1);
    int d_pos2s = Dimpos.at(id2);
    int d_pos2e = Dimpos.at(id2) + DimWeight.at(id2);
    if(d_pos1s>=d_pos2s&&d_pos1e<=d_pos2e)return true;
    if(d_pos1s<=d_pos2s&&d_pos1e>=d_pos2e)return true;
    if(d_pos1s < d_pos2s && d_pos1e > d_pos2s)return true;
    if(d_pos2s < d_pos1s && d_pos2e > d_pos1s)return true;
    return false;
}


//find left,right,top,bottom
std::vector<bool> findSide(SequencePair&sp,int block,int s1_dir,int s2_dir){

    int s1_idx = sp.S1_idx.at(block);
    int s2_idx = sp.S2_idx.at(block);
    int n = sp.S1.size();

    std::vector<bool>mark(n,false);

    for(int i = s1_idx+s1_dir;(i>=0&&i<n);i+=s1_dir)
        mark.at(sp.S1.at(i)) = true;

    std::vector<bool>targetSide(n,false);
    for(int i = s2_idx+s2_dir;(i>=0&&i<n);i+=s2_dir){
        if(mark.at(sp.S2.at(i)))
            targetSide.at(sp.S2.at(i)) = true;
    }
    return targetSide;
}



void shiftSp(std::vector<int>&sequence,int idxNow,int targetIdx){

    if(idxNow==targetIdx)return ;
    int dir = idxNow > targetIdx ? -1 : 1;
    int id = sequence.at(idxNow);
    for(int i = idxNow + dir;i!=targetIdx + dir;i+=dir){
        sequence.at(i-dir) = sequence.at(i);
    }
    sequence.at(targetIdx) = id;
}

//Hcstr
// first : x+w
// second : blockid
// sorted by x+w 
void compact_left_block(int block,int &xpos,SequencePair&sp,std::vector<int>&y_pos,std::vector<int>&blockHeight,std::vector<std::pair<int,int>>&Hcstr)
{
    //get left side
    std::vector<bool>left = findSide(sp,block,-1,-1);
    int final_s1_idx = sp.S1_idx.at(block);
    int final_s2_idx = sp.S2_idx.at(block);

    bool move = false;
    int newXpos = 0;
    for(int i = 0;i < Hcstr.size();i++){
        int id = Hcstr.at(i).second;
        if(left.at(id)){ // is target side
            if(overlapIndim(y_pos,blockHeight,block,id)) // overlap in y region
            {
                newXpos = Hcstr.at(i).first;
                if(newXpos!=xpos)move=true;
                break;
            }  
            move = true;
            int target_s1_idx = sp.S1_idx.at(id);
            int target_s2_idx = sp.S2_idx.at(id);
            if(y_pos.at(id) >= y_pos.at(block) + blockHeight.at(block)){ //id is higher than block , then put it bellow of the target.
                if(final_s2_idx > target_s2_idx){//仍為右側
                    final_s2_idx = target_s2_idx;
                }
            }
            else{
                if(final_s1_idx > target_s1_idx){//仍為右側
                    final_s1_idx = target_s1_idx;
                }
            }
        }
    }
    if(move){
        //shift SP
        shiftSp(sp.S1,sp.S1_idx.at(block),final_s1_idx);
        shiftSp(sp.S2,sp.S2_idx.at(block),final_s2_idx);
        sp.setIdx();
        //need update the Hcstr? sure! 
        int idx; //find Hcstr's idx
        for(int i = 0;i<Hcstr.size();i++)
            if(Hcstr.at(i).second==block)
                idx = i;
        int width = Hcstr.at(idx).first - xpos;
        xpos = newXpos;
        int newHcstr = xpos + width;//update
        //insertion sort
        int i = idx+1;
        for(;i<Hcstr.size()&&Hcstr.at(i).first>newHcstr;i++){
            Hcstr.at(i-1) = Hcstr.at(i);
        }
        Hcstr.at(i-1).first = newHcstr;
        Hcstr.at(i-1).second = block;
    }
}

void Floorplan::compact_left(){
    std::vector<std::pair<int,int>>Hcstr(blockHeight.size(),{0,0});
    for(int i = 0;i<blockHeight.size();i++){
        Hcstr.at(i).first = x_pos.at(i) + blockWidth.at(i);
        Hcstr.at(i).second = i;
    }
    std::sort(Hcstr.begin(),Hcstr.end(),[](std::pair<int,int>&p1,std::pair<int,int>&p2){
        return p1.first > p2.first;
    });
    int n = x_pos.size();
    for(int i = 0;i<n;i++){
        compact_left_block(i,x_pos.at(i),sp,y_pos,blockHeight,Hcstr);
    }
}





void compact_process(int block,int&pos,int newpos,int s1_idx,int s2_idx,compact_args cmp_args){
    shiftSp(cmp_args.sp->S1,cmp_args.sp->S1_idx.at(block),s1_idx);
    shiftSp(cmp_args.sp->S2,cmp_args.sp->S2_idx.at(block),s2_idx);
    cmp_args.sp->setIdx();

    //update the cstr
    int idx; //find cstr's idx
    for(int i = 0;i<cmp_args.Movcstr->size();i++)
        if(cmp_args.Movcstr->at(i).second==block)
            idx = i;

    int len = cmp_args.Movcstr->at(idx).first - pos;
    pos = newpos;//update pos

    int newCstr = pos + len;//update cstr
    //insertion sort
    int i = idx+1;
    for(;i<cmp_args.Movcstr->size()&&cmp_args.Movcstr->at(i).first>newCstr;i++){
        cmp_args.Movcstr->at(i-1) = cmp_args.Movcstr->at(i);
    }
    cmp_args.Movcstr->at(i-1).first = newCstr;
    cmp_args.Movcstr->at(i-1).second = block;


}

void compact_custom(int block,int &pos,compact_args cmp_args)
{
    //get corresbonding side blocks.
    // return size : num of blocks
    // side.at(i) = true iff block i is in the target side.
    std::vector<bool>side = findSide(*cmp_args.sp,block,cmp_args.sideArg.first,cmp_args.sideArg.second);

    int final_s1_idx = cmp_args.sp->S1_idx.at(block);
    int final_s2_idx = cmp_args.sp->S2_idx.at(block);

    bool move = false;
    int newpos=0;//initial to 0 to avoid run out boundary.

    bool overlap = false;

    /*compact searching*/
    //scan Movcstr and adjust the finalidx in sp.
    for(int i = 0;i < cmp_args.Movcstr->size();i++){
        int id = cmp_args.Movcstr->at(i).second;
        if(side.at(id)){ // is target side
            if(overlapIndim(*cmp_args.d2_pos,*cmp_args.d2_weight,block,id)) {//stop
                newpos = cmp_args.Movcstr->at(i).first;
                overlap = true;
                if(newpos!=pos)move=true;

                if(block==14)
                {
                    std::cout<<"stuck by"<<id<<"\n";
                    std::cout<<final_s1_idx<<" "<<final_s2_idx<<"\n";
                    std::cout<<cmp_args.sp->S1_idx.at(block)<<" "<<cmp_args.sp->S2_idx.at(block)<<"\n";
                }


                break;
            }  

            if(block==14){
                std::cout<<"check"<<id<<"\n";
            }

            move = true;//not being block in first check means move happend.
            int target_s1_idx = cmp_args.sp->S1_idx.at(id);
            int target_s2_idx = cmp_args.sp->S2_idx.at(id);

            // choose side to insert. for left compact, may choose move block to  top or bottom of block id.

            // put block to bottom or left of id 
            if(cmp_args.d2_pos->at(id) >= cmp_args.d2_pos->at(block) + cmp_args.d2_weight->at(block)){
                if(cmp_args.s2_cmp(final_s2_idx ,target_s2_idx))
                    final_s2_idx = target_s2_idx;
            }
            else{
                if(cmp_args.s1_cmp(final_s2_idx ,target_s2_idx))
                    final_s1_idx = target_s1_idx;
            }
        }
    }


    /*compact searching*/
    if(move){
        compact_process(block,pos,newpos,final_s1_idx,final_s2_idx,cmp_args);
    }
}
//


void Floorplan::compact_lb(){
    //sorted by left_bottom 
    std::vector<int>compact_order(x_pos.size(),0);
    for(int i = 0;i<compact_order.size();i++)compact_order.at(i) = i;
    std::sort(compact_order.begin(),compact_order.end(),
        [this](int a,int b)
        {
            if(this->y_pos.at(a) < this->y_pos.at(b))return true;
            else if(this->y_pos.at(a) > this->y_pos.at(b))return false;
            else{
                return this->x_pos.at(a) < this->x_pos.at(b);
            }
        }
    );
    std::vector<std::pair<int,int>>Hcstr(blockWidth.size(),{0,0});
    std::vector<std::pair<int,int>>Vcstr(blockHeight.size(),{0,0});
    for(int i = 0;i<blockHeight.size();i++){
        Hcstr.at(i).first = x_pos.at(i) + blockWidth.at(i);
        Hcstr.at(i).second = i;
        Vcstr.at(i).first = y_pos.at(i) + blockHeight.at(i);
        Vcstr.at(i).second = i;
    }
    std::sort(Hcstr.begin(),Hcstr.end(),[](std::pair<int,int>&p1,std::pair<int,int>&p2){
        return p1.first > p2.first;
    });
    std::sort(Vcstr.begin(),Vcstr.end(),[](std::pair<int,int>&p1,std::pair<int,int>&p2){
        return p1.first > p2.first;
    });
    int n = x_pos.size();

    compact_args leftArgs{
        {-1,-1},
        &sp,
        &y_pos,
        &blockHeight,
        &Hcstr,
        bigger,
        bigger
    };
    compact_args bottomArgs{
        {1,-1},
        &sp,
        &x_pos,
        &blockWidth,
        &Vcstr,
        smaller,
        bigger
    };
    for(int i = 0;i<n;i++){
        int id = compact_order.at(i);
        //compact_custom(id,x_pos.at(id),leftArgs);
        if(id==3)
        {
            std::cout<<"befor compact\n";
            sp.showSequence();
        }
        compact_custom(id,y_pos.at(id),bottomArgs);
        if(id==3)
        {
            std::cout<<"after compact\n";
            sp.showSequence();
            exit(1);
        }
    }
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
            if (y >  t)t = y;
        }
        for(auto ptr = term.begin();ptr!=term.end();++ptr){
            int tid = *ptr;
            double x = terminals.at(tid).x; 
            double y = terminals.at(tid).y; 
            if (x <  l)l = x;
            if (x >  r)r = x;
            if (y <  d)d = y;
            if (y >  t)t = y;
        }
        int x_l = int(r)-int(l);
        int y_l = int(t)-int(d);
        length += (x_l + y_l);
    }
    return length;
}



void Floorplan::moveto(int moveBlock,int targetBlock,int relation,int *OriginIdx1,int *OriginIdx2) {
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

    if(OriginIdx1)*OriginIdx1 = s1_idx;
    if(OriginIdx2)*OriginIdx2 = s2_idx;
    shiftSp(sp.S1,s1_idx,target_s1_idx);
    shiftSp(sp.S2,s2_idx,target_s2_idx);

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
    int ySize =  LCS(sp.S1,sp.S2,blockHeight,rev_y_pos);//from top
    //recover S2
    reverse(sp.S2);
    for(int i = 0;i<sp.S1.size();i++){
        x_slack.at(i) = xSize - rev_x_pos.at(i) - blockWidth.at(i) - x_pos.at(i);
        y_slack.at(i) = ySize - rev_y_pos.at(i) - blockHeight.at(i) - y_pos.at(i);
        if(x_slack.at(i)<0)
        {
            std::cerr<<"x_slack <0\n";
            exit(1);
        }
        if(y_slack.at(i)<0)
        {
            std::cerr<<"y_slack <0\n";
            exit(1);
        }
    }
}

std::list<int> get_criticalList(std::vector<int>&slack){
    std::list<int>critical;
    int s =  2147483647;
    for(int i = 0;i < slack.size();i++){
        if(slack.at(i) < s){
            critical.clear();
            s = slack.at(i);
            critical.push_front(i);//add candidate
        }else if(slack.at(i)==s)
            critical.push_front(i);
    }
    return critical;
}

int try_rotate(Floorplan&fp,std::vector<int>&d1,std::vector<int>&d2,std::vector<int>&d2_slack,std::list<int>&critical){
    int max_slack = 0;
    //some times may happen un-compact due to this candidate. so we need to rotate and move it .
    //this stage do not rotate this candidate but only find it and return. 
    int moveCandidate = -1; 
    //doing rotate or find move candidate
    // if height < width , then need to consider y_slack
    //if y_slack is enough , do rotate directly 
    for(auto c:critical){
        if(d2.at(c) < d1.at(c)){ // rotate can decrease the x-span
            if(d2_slack.at(c) >= d1.at(c))
                fp.rotate(c);
        }
        else if (d2.at(c) > d1.at(c)){
            if(d2_slack.at(c) > max_slack){
                max_slack = d2_slack.at(c);
                moveCandidate = c;
            }
        }
    }
    return moveCandidate;
}

int findsmallSlack(std::vector<int>&slack){
    int small = INT_MAX_RANGE;
    int target = -1; 
    for(int i = 0;i<slack.size();i++)
    {
        if(slack.at(i) < small)
        {
            small = slack.at(i);
            target = i;
        }
    }
    return target;
}
int findbiggestSlack(std::vector<int>&slack,int origin){
    int max = -1;
    int target = -1;
    for(int i = 0;i<slack.size();i++){
        if(slack.at(i) > max&&origin!=i){
            max = slack.at(i);
            target = i;
        }
    }
    return target;
}

std::pair<int,int>avgPos(std::vector<int>&pos_x,std::vector<int>&pos_y){

    int avgx = 0;
    int avgy = 0;
    for(int i = 0;i<pos_x.size();i++)
        avgx+=pos_x.at(i);
    for(int i = 0;i<pos_y.size();i++)
        avgy+=pos_y.at(i);
    
    return {avgx / pos_x.size() , avgy/pos_y.size()};
}

void Floorplan::fixed_outline_based(){
    auto pack = getPacking();
    updateSlack();
    int w = pack.first;
    int h = pack.second;
    int count = 0;
    //not feasible
    while(w > outline.first || h > outline.second){
        auto avg = avgPos(x_pos,y_pos);
        if(w > outline.first){
            auto critical_x = get_criticalList(x_slack);
            int moveCandidate = try_rotate(*this,blockWidth,blockHeight,y_slack,critical_x);
            if(moveCandidate!=-1){
                int target = findbiggestSlack(y_slack,moveCandidate);
                if(target!=-1)
                if(y_slack.at(target) > blockWidth.at(moveCandidate)){//can pack after rotate
                    rotate(moveCandidate);
                    if(y_pos.at(target)>avg.second) // move to bellow 
                        moveto(moveCandidate,target,3);
                    else 
                        moveto(moveCandidate,target,2);
                }
            }
        }
        else{
            auto critical_y = get_criticalList(y_slack);
            int moveCandidate = try_rotate(*this,blockHeight,blockWidth,x_slack,critical_y);
            if(moveCandidate!=-1){
                int target = findbiggestSlack(x_slack,moveCandidate);
                if(target!=-1)
                if(x_slack.at(target) > blockHeight.at(moveCandidate)){
                    rotate(moveCandidate);
                    if(x_pos.at(target)>avg.first) // move to left 
                        moveto(moveCandidate,target,0);
                    else 
                        moveto(moveCandidate,target,1);
                }
            }
        }
        pack = getPacking();
        updateSlack();
        w = pack.first;
        h = pack.second;
        std::cout<<w<<" "<<h<<"\n";
        if(count++>200)
        break;
        //if stuck , enter SA procedure
    }

    //如果上述過程就能進入outline,則再用SA優化Area跟線長....
    //如果不行,要能用SA的概念跳脫local 
}
