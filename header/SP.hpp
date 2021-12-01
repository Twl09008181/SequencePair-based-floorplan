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
    //File information
    std::pair<int,int>outline;
    std::vector<int>blockWidth;
    std::vector<int>blockHeight;
    std::vector<coordinate>terminals;
    std::vector<Net>Netlist;
    float alpha;
    std::pair<int,int> getPacking();
    int getHPWL();
//for debug
//private:
    std::map<std::string,int>blockMap;//key : name , val : id 
    std::map<std::string,int>TerminalMap;//key : name , val : id 
    SequencePair sp;
    std::vector<int>x_pos;
    std::vector<int>y_pos;
};



#endif
