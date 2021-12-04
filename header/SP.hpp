#ifndef SP_HPP
#define SP_HPP
#include<vector>
#include<utility>
#include <map>
#include<string>
#include<fstream>
#include <list>

#define INT_MAX_RANGE 2147483647
struct SequencePair{
    std::vector<int>S1;
    std::vector<int>S2;
    std::vector<int>S1_idx;//record the index of block id in S1. 
    std::vector<int>S2_idx;
    void swapBlock(bool s1,int id1,int id2); // if type = 0 , then swap only in sp1, type = 1 , swap only in sp2  , type = 2, swap both.  
    void setIdx();
    void showSequence();
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
    /*
    operations
    */
    void rotate(int blockId);
    void swapBlock(int swapType,int id1,int id2);// if type = 0 , then swap only in sp1, type = 1 , swap only in sp2  , type = 2, swap both.  

//for debug
//private:
    std::map<std::string,int>blockMap;//key : name , val : id 
    std::map<std::string,int>TerminalMap;//key : name , val : id 
    SequencePair sp;
    std::vector<int>x_pos;
    std::vector<int>y_pos;

};



#endif
