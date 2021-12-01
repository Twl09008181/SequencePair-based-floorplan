#include"../header/SP.hpp" 
#include<iostream>

/*
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
private:
    std::map<std::string,int>blockMap;//key : name , val : id 
    std::map<std::string,int>TerminalMap;//key : name , val : id 
    SequencePair sp;
};
*/



Floorplan::Floorplan(float alpha,const std::string&blockfile,const std::string&netfile){
    this->alpha = alpha;

    std::ifstream block{blockfile};
    if(!block){std::cerr<<"open"<<blockfile<<" failed\n";exit(1);}    
    
    block >> outline.first >> outline.second;


    block.close();




    
}
