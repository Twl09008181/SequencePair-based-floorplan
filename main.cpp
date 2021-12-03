#include<iostream>
#include <algorithm>
#include<vector>
#include "header/SP.hpp"
#include <string>




float alpha;
float fitinRatio = 0.97;

void SA_Fit(Floorplan&fp,float temp,float decade,float frozen,int k);
void SA_outline(Floorplan&fp,float temp,float decade,float frozen,int k);
void output(const std::string&filename,Floorplan&fp);
int main(int argc,char * argv[]){

    if(argc!=5){
        std::cerr<<"please enter correct format!  $ ./Lab3.exe <alpha> <input.block name> <input.net name> <output name>  \n";
        exit(1);
    }

    srand(0);
    alpha = std::atof(argv[1]);
    Floorplan fp(alpha,argv[2],argv[3]);


/*
    int idx1,idx2;
    fp.sp.showSequence();
    fp.moveto(3,10,3,&idx1,&idx2);
    fp.sp.showSequence();

    shiftSp(fp.sp.S1,fp.sp.S1_idx.at(3),idx1);
    shiftSp(fp.sp.S2,fp.sp.S2_idx.at(3),idx2);
    fp.sp.showSequence();
*/

    std::cout<<"outline "<<fp.outline.first<<" "<<fp.outline.second<<"\n";

    SA_Fit(fp,1000000,0.15,100,1000);
    // initial packing using greedy approach
    //show the width , height
    auto packing = fp.getPacking();
    std::cout<<"width : "<<packing.first<<" height "<<packing.second<<"\n";
    std::cout<<"Area : "<<packing.first*packing.second<<"\n";
    int wl = fp.getHPWL();
    std::cout<<"wl: "<<wl<<"\n";
    std::cout<<"cost "<<alpha*packing.first*packing.second+(1-alpha)*wl<<"\n";


    output(argv[4],fp);



    return 0;
}

void output(const std::string&filename,Floorplan&fp){
    /*
        not done
    */

   std::ofstream out {filename};
   if(!out){
    std::cerr<<"file can't open!\n";
    exit(1);
   }

 //  fp.getPacking();
    out << fp.outline.first<<" "<<fp.outline.second<<"\n";
    out << fp.blockHeight.size()<<"\n";
    for(int i = 0;i<fp.blockWidth.size();i++)
        out << fp.x_pos[i]<<" "<<fp.y_pos[i]<<" "<<fp.blockWidth[i]<<" "<<fp.blockHeight[i]<<" \n";
    

   out.close();
}

float outlineCost(std::pair<int,int>packing,std::pair<int,int>outline,int &invalid ){
    float outlineR = float(outline.first) / outline.second;
    float packingR = float(packing.first) / packing.second;

    if(packing.first <= outline.first && packing.second <= outline.second)
        invalid = 0;
    else if(packing.first > outline.first)
        invalid = 1;
    else if(packing.second > outline.second) 
        invalid = 2;
    return invalid ? (packingR-outlineR)*(packingR-outlineR) : 0; // if fit int outline, return 0
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
bool isFitIn(int w,int h,int o_w,int o_h){return w<o_w && h<o_h;}
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
/*
void SA_outline(Floorplan&fp,float temp,float decade,float frozen,int k){

    float initTemp = temp;
    while(temp >= frozen){

        int movetime = 0;

        //move in same temperature k * n times.
        while(movetime < k * fp.sp.S1.size()){
            auto packing = fp.getPacking(); 
            int  invalid = 0; // some hint for fix
            //setting cost = Area * (1 + (R-R*)^2 )
            //when Area is large, cost is large
            //when delta R is large, cost is large too

            float CostOutline1 = outlineCost(packing,fp.outline,invalid);
            float AreaCost1 = (packing.first * packing.second);
            float Wlcost1 = fp.getHPWL();

            //random pick move
            int id1=-1,id2=-1,op=-1;
            op = pick_move(fp,invalid,id1,id2);

            //caculate new cost
            auto newpacking = fp.getPacking();
            int invalid2 = 0;

            float CostOutline2 = outlineCost(newpacking,fp.outline,invalid2);
            float AreaCost2 = newpacking.first * newpacking.second;
            float Wlcost2 = fp.getHPWL();

            // change state
            bool change_state = false;

            // if not in outline 
            if(invalid && invalid2){
                float cost1 = (AreaCost1) * (1+100*CostOutline1);
                float cost2 = (AreaCost2) * (1+100*CostOutline2);
                if(cost2 < cost1 || climb(initTemp,temp,cost1,cost2))change_state = true;
            }
            else if(invalid && !invalid2) // invalid 2 fit in.
            {
                change_state = true;
            }
            else if (!invalid&&!invalid2){ //both fit in 
                float cost1 = alpha * AreaCost1 + (1-alpha) * Wlcost1;
                float cost2 = alpha * AreaCost2 + (1-alpha) * Wlcost2;
                if(cost2 < cost1 || climb(initTemp,temp,cost1,cost2))change_state = true;

            }

            if(!change_state){
                recover(fp,op,id1,id2);
            }
            movetime++;
        }
        temp*=decade;
    }
}

*/
//only consider Area and outline
void SA_Fit(Floorplan&fp,float temp,float decade,float frozen,int k){

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
    if(fitin)
    {
        std::cout<<"fit in in ratio:"<<fitinRatio<<"\n";
    }
}
