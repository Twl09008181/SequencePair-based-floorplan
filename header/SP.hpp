#ifndef SP_HPP
#define SP_HPP
#include<vector>
#include<utility>
#include <map>
#include<string>
#include<fstream>
#include <list>

struct SequencePair{
    std::vector<int>S1;
    std::vector<int>S2;
    std::vector<int>S1_idx;//record the index of block id in S1. 
    std::vector<int>S2_idx;
    void swapBlock(bool s1,int id1,int id2); // if type = 0 , then swap only in sp1, type = 1 , swap only in sp2  , type = 2, swap both.  
    void setIdx();
    void showSequence();
    void showSequenceIdx();
};


struct coordinate{
    int x,y;
};
struct Net{
    std::list<int> blocks;//may move
    std::list<int>terminals;//fixed
};

class Floorplan{
public:
    Floorplan(float alpha,const std::string&blockfile,const std::string&netfile);
    Floorplan() = default;
    //File information
    std::pair<int,int>outline;
    std::vector<int>blockWidth;
    std::vector<int>blockHeight;
    std::vector<coordinate>terminals;
    std::vector<Net>Netlist;
    float alpha;
    std::pair<int,int> getPacking();
    int getHPWL();//need call getPacking first....
    void updateSlack();//need call getPacking first
    /*
    operations
    */
    void rotate(int blockId);
    void swapBlock(int swapType,int id1,int id2);// if type = 0 , then swap only in sp1, type = 1 , swap only in sp2  , type = 2, swap both.  

    //slack-based move
    void moveto(int moveBlock,int targetBlock,int relation,int *OriginIdx1=nullptr,int *OriginIdx2=nullptr);
    void fixed_outline_based();
//for debug
//private:
    std::map<std::string,int>blockMap;//key : name , val : id 
    std::map<std::string,int>TerminalMap;//key : name , val : id 
    SequencePair sp;
    std::vector<int>x_pos;
    std::vector<int>y_pos;
// to provide the guid of operation moving.
    std::vector<int>x_slack;
    std::vector<int>y_slack;


    void compact_left();
    void compact_bottom();

    void compact_lb();//compact left bottom

};

void compact_left_block(int block,int &xpos,SequencePair&sp,std::vector<int>&y_pos,std::vector<int>&blockHeight,std::vector<std::pair<int,int>>&Hcstr);


inline bool smaller(int a,int b){return a<b;}
inline bool bigger(int a,int b){return a>b;}
std::vector<bool> findSide(SequencePair&sp,int block,int s1_dir,int s2_dir);
void shiftSp(std::vector<int>&sequence,int idxNow,int targetIdx);
struct compact_args{
    

    /*   candidate find     */
    // find left / right / top /bottom side 
    // <s1 dir ,s2 dir>
    // left : -1,-1 , right : 1,1  , top : -1,1  bottom : 1,-1
    std::pair<int,int>sideArg;
    SequencePair* sp;

    /* overlap checking */
    std::vector<int>*d2_pos;// the other dimension's position for  overlap check.
    std::vector<int>*d2_weight;// the other dimension's length for overlap check.

    // use to find constraint candidate
    // if doing left-compact, d2cstr is sorted by x + w. (same dimension)
    // the block with biggest value of x + w  must in front of this vector   
    // so we can check whether can move left or stop(overlap) 
    // first : cstr value : x+w (for left compact) , second:correspond block id 
    std::vector<std::pair<int,int>>*Movcstr;

    //sequence cmp function
    // idx change only when  cmp is true
    // left compact may change block relation from right(block)/left(other block) to top/bottom(both can happen).    
    // begin : s1 : <others,block> s2:<others,block>
    // if change to some block's top , then s1 become : <block,others> ,s2 is unchange.     
    // so s1_idx need move to -1 dir.

    // if left compact, then both s1,s2 index need move left , so use max compare function (only when final > target, then move)

    // if bottom compact, then s1 need from idx small to idx big , so use min compare function
    // s2 need from big to small , use max compare function.

    //final,target is corresponding index in sequence.
    bool(*s1_cmp)(int final,int target);
    bool(*s2_cmp)(int final,int target);
};
void compact_custom(int block,int &pos,compact_args cmp_args);
int findbiggestSlack(std::vector<int>&slack,int origin);
int findsmallSlack(std::vector<int>&slack);
#endif
