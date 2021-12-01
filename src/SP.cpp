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
}
