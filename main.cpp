#include<iostream>
#include <algorithm>
#include<vector>
#include "header/SP.hpp"
#include <string>




float alpha;
float fitinRatio = 1;

int SA_Fit(Floorplan&fp,float temp,float decade,float frozen,int k);
float SA_Opt(Floorplan&fp,float temp,float decade,float frozen,int k);
void output(const std::string&filename,Floorplan&fp);



bool isFitIn(int w,int h,int o_w,int o_h){return w<o_w && h<o_h;}

int main(int argc,char * argv[]){

    if(argc!=5){
        std::cerr<<"please enter correct format!  $ ./Lab3.exe <alpha> <input.block name> <input.net name> <output name>  \n";
        exit(1);
    }

    srand(0);
    alpha = std::atof(argv[1]);
    Floorplan fp(alpha,argv[2],argv[3]);


    //packing relative , need SP and W,H.
    std::vector<int>S1 = fp.sp.S1;
    std::vector<int>S2 = fp.sp.S2;
    std::vector<int>Width = fp.blockWidth;
    std::vector<int>Height = fp.blockHeight;

    //first step do fit in  
    int minArea = INT_MAX_RANGE;
    for(int i = 0;i<10;i++){
        int Area = SA_Fit(fp,1000000,0.15,100,1000);
        if(Area!=0 && Area < minArea){
            S1 = fp.sp.S1;
            S2 = fp.sp.S2;
            Width = fp.blockWidth;
            Height = fp.blockHeight;
            minArea = Area;
        
            #ifdef DEBUG
            auto packing = fp.getPacking();
            std::cout<<"width : "<<packing.first<<" height "<<packing.second<<"\n";
            std::cout<<"Area : "<<packing.first*packing.second<<"\n";
            #endif
        }
        fitinRatio-=0.01;
    }
    fp.sp.S1 = S1;
    fp.sp.S2 = S2;
    fp.blockHeight = Height;
    fp.blockWidth = Width;

   #ifdef DEBUG
    std::cout<<"After Fit in process :\n";
    std::cout<<"outline "<<fp.outline.first<<" "<<fp.outline.second<<"\n";
    //show the width , height
    auto packing = fp.getPacking();
    std::cout<<"width : "<<packing.first<<" height "<<packing.second<<"\n";
    std::cout<<"Area : "<<packing.first*packing.second<<"\n";
    int wl = fp.getHPWL();
    std::cout<<"wl: "<<wl<<"\n";
    std::cout<<"cost "<<alpha*packing.first*packing.second+(1-alpha)*wl<<"\n";
    #endif


    float minCost = INT_MAX_RANGE;
    for(int i = 0;i<3;i++){
        float cost = SA_Opt(fp,100000,0.15,100,1000);
        if(cost <= minCost){
            S1 = fp.sp.S1;
            S2 = fp.sp.S2;
            Width = fp.blockWidth;
            Height = fp.blockHeight;
            minCost = cost;
            #ifdef DEBUG
            packing = fp.getPacking();
            std::cout<<"width : "<<packing.first<<" height "<<packing.second<<"\n";
            std::cout<<"Area : "<<packing.first*packing.second<<"\n";
            std::cout<<"wl : "<<fp.getHPWL();
            std::cout<<"cost : "<<minCost<<"\n";
            #endif
        }
    }
    fp.sp.S1 = S1;
    fp.sp.S2 = S2;
    fp.blockHeight = Height;
    fp.blockWidth = Width;

    #ifdef DEBUG
    std::cout<<"After Optimization process :\n";
    std::cout<<"outline "<<fp.outline.first<<" "<<fp.outline.second<<"\n";
    //show the width , height
    packing = fp.getPacking();
    std::cout<<"width : "<<packing.first<<" height "<<packing.second<<"\n";
    std::cout<<"Area : "<<packing.first*packing.second<<"\n";
    wl = fp.getHPWL();
    std::cout<<"wl: "<<wl<<"\n";
    std::cout<<"cost "<<alpha*packing.first*packing.second+(1-alpha)*wl<<"\n";
    #endif

    output(argv[4],fp);



    return 0;
}

void output(const std::string&filename,Floorplan&fp){
    auto packing = fp.getPacking();
    std::ofstream out {filename};
    if(!out){
        std::cerr<<"file can't open!\n";
        exit(1);
    }
    int hpwl = fp.getHPWL();

    int area = packing.first * packing.second;
    out << int(fp.alpha * area +  (1-alpha) * hpwl)<<"\n";
    out << hpwl<<"\n";
    out << area<<"\n";
    out << packing.first <<" "<<packing.second<<"\n";
    out << 0.1<<"\n";

    for(auto ptr:fp.blockMap){
        int i = ptr.second;
        out <<ptr.first<<" "<< fp.x_pos[i]<<" "<<fp.y_pos[i]<<" "<<fp.x_pos[i]+fp.blockWidth[i]<<" "<<fp.y_pos[i]+fp.blockHeight[i]<<" \n";
    }

   out.close();
}




int pick_move(Floorplan&fp,int&id1,int&id2){
    int blockNum = fp.sp.S1.size();
    id1 = rand()%blockNum;
    int op = rand()%4;
    if(op==0){
        fp.rotate(id1);
    }
    else{//op 1 2 3
        id2 = id1;
        while(id1==id2)id2 = rand()%blockNum;
        int type = rand()%3;
        fp.swapBlock(type,id1,id2);
        op = 1 + type;
    }
    return op;
}

void recover(Floorplan&fp,int op,int id1,int id2){
    if(op==0)
        fp.rotate(id1);
    else 
        fp.swapBlock(op-1,id1,id2);
}

#include<random>
bool climb(float initTemp,float temp,float cost1,float cost2){
    float chance = rand() / RAND_MAX;
    int delta_cost = cost2 - cost1;
    return chance < std::exp(-delta_cost * initTemp/temp);//t越小,exp值越小,越不可能跳,delta越大,越不可能跳
}


//分兩階段
//先求fit in 
//再進行優化

//fit in階段
// 還未fit int : cost function設定接近ratio
// 成功fit in : 不可超出outline的前提下降低area
float GetR(int w,int h){return float(w)/h;}
float getCost(int w,int h,int o_w,int o_h,bool needFitIn){


    if( (w > o_w || h>o_h ) && needFitIn)return INT_MAX_RANGE;
    if(w>o_w || h>o_h)//return area * (1+100 * ration penalty)
    {
        float r1 = GetR(w,h);
        float r2 = GetR(o_w,o_h);
        return w*h * (1 + 100 * (r1-r2)*(r1-r2));
    }
    else{//return area
        return w*h;
    }
}
float OptimizationCost(int w,int h,int o_w,int o_h,int wl,float alpha){
    //when optimizie the area , wl , it can't hapen over outline.
    if (w > o_w || h>o_h )return INT_MAX_RANGE;
    return alpha*w*h + (1-alpha)*wl;
}
float  SA_Opt(Floorplan&fp,float temp,float decade,float frozen,int k){

    float initTemp = temp;
    auto packing = fp.getPacking(); 

    while(temp >= frozen){

        float cost1 = OptimizationCost(packing.first,packing.second,fp.outline.first,fp.outline.second,fp.getHPWL(),fp.alpha);
        int movetime = 0;
        //move in same temperature k * n times.
        while(movetime < k * fp.sp.S1.size()){
            auto packing = fp.getPacking(); 
            //random pick move
            int id1=-1,id2=-1,op=-1;
            op = pick_move(fp,id1,id2);
            //caculate new cost
            auto newpacking = fp.getPacking();
            float cost2 = OptimizationCost(newpacking.first,newpacking.second,fp.outline.first,fp.outline.second,fp.getHPWL(),fp.alpha);

            // change state
            bool change_state = false;
            if(cost2 < cost1 || climb(initTemp,temp,cost1,cost2))change_state = true;
            if(change_state)
                cost1 = cost2;
            else
                recover(fp,op,id1,id2);

            movetime++;
        }
        temp*=decade;
    }
    packing = fp.getPacking(); 
    return alpha*packing.first*packing.second + (1-alpha)*fp.getHPWL(); 
}

//only consider Area and outline
int SA_Fit(Floorplan&fp,float temp,float decade,float frozen,int k){

    float initTemp = temp;
    bool fitin = false;
    int targetW = fp.outline.first * fitinRatio;
    int targetH = fp.outline.second * fitinRatio;
    auto packing = fp.getPacking(); 

    while(temp >= frozen){

        fitin = isFitIn(packing.first,packing.second,targetW,targetH);
        float cost1 = getCost(packing.first,packing.second,targetW,targetH,false);

        int movetime = 0;
        //move in same temperature k * n times.
        while(movetime < k * fp.sp.S1.size()){
            packing = fp.getPacking(); 
            fitin = isFitIn(packing.first,packing.second,targetW,targetH);//update fitIn

            //random pick move
            int id1=-1,id2=-1,op=-1;
            op = pick_move(fp,id1,id2);

            auto newpacking = fp.getPacking();
            float cost2 = getCost(newpacking.first,newpacking.second,targetW,targetH,fitin);

            // change state
            bool change_state = false;
            if(cost2 < cost1 || climb(initTemp,temp,cost1,cost2))change_state = true;
            if(change_state)
                cost1 = cost2;
            else
                recover(fp,op,id1,id2);
            movetime++;
        }
        temp*=decade;
    }

    packing = fp.getPacking();


    if(isFitIn(packing.first,packing.second,fp.outline.first,fp.outline.second)){
        return packing.first * packing.second;
    }
    else
        return 0;
}
