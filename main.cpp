#include<iostream>
#include <algorithm>
#include<vector>
#include "header/SP.hpp"
#include <string>




float alpha;

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

    std::cout<<"outline"<<fp.outline.first<<" "<<fp.outline.second<<"\n";


    SA_outline(fp,1000000,0.15,100,1000);
    // initial packing using greedy approach
    //show the width , height
    auto packing = fp.getPacking();
    std::cout<<"width : "<<packing.first<<" height "<<packing.second<<"\n";



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
    return (packingR-outlineR)*(packingR-outlineR);
}

int slack_rotate_x(Floorplan&fp,int &id1){
    id1 = findsmallSlack(fp.x_slack);
    fp.rotate(id1);
    return 0;
}
int slack_rotate_y(Floorplan&fp,int &id1){
    id1 = findsmallSlack(fp.y_slack);
    fp.rotate(id1);
    return 0;
}

//op 4 ,5 
int slack_move_x(Floorplan&fp,int &id1,int &id2){
    int b1 = findsmallSlack(fp.x_slack);
    int b2 = findbiggestSlack(fp.x_slack,b1);
    if(b2==-1||b1==-1)
    {
        std::cerr<<"error\n";
        exit(1);
    }
    int leftOrRight = rand()%2;
    fp.moveto(b1,b2,leftOrRight,&id1,&id2);
    return 4 + leftOrRight;
}
// op 6 7 
int slack_move_y(Floorplan&fp,int &id1,int &id2){
    int b1 = findsmallSlack(fp.y_slack);
    int b2 = findbiggestSlack(fp.y_slack,b1);
    if(b2==-1||b1==-1)
    {
        std::cerr<<"error\n";
        exit(1);
    }
    int TopOrBot = rand()%2;
    fp.moveto(b1,b2,TopOrBot,&id1,&id2);
    return 6 + TopOrBot;
}
int slack_move(Floorplan&fp,int hint,int&id1,int&id2){
    //operation 0 : rotate
    //operation 4,5 : S1 move left , move right
    //operation 6,7 : S2 move top , move bottom
    fp.updateSlack();
    int rotate = 0; 
    int op;
    if(hint==0){//both ok
        int chooseSeq = rand()%2;
        if(chooseSeq==0){//find small x_slack
            if(!rotate)
                op = slack_move_x(fp,id1,id2);
            else 
                op = slack_rotate_x(fp,id1);
        }
        else{
            if(!rotate)
                op = slack_move_y(fp,id1,id2);
            else 
                op = slack_rotate_y(fp,id1);
        }
    }
    else if(hint==1){//packing.first > outline.first , find x_slack = 0
        if(!rotate)
            op = slack_move_x(fp,id1,id2);
        else 
            op = slack_rotate_x(fp,id1);
    }
    else{    
        if(!rotate)
            op = slack_move_y(fp,id1,id2);
        else 
            op = slack_rotate_y(fp,id1);
    }
    return op;
}


int pick_move(Floorplan&fp,int hint,int&id1,int&id2){
    int slack = 0;
    //int slack = rand()%2;
    if(slack)
        return slack_move(fp,hint,id1,id2);
    int blockNum = fp.sp.S1.size();
    id1 = rand()%blockNum;
    int op = rand()%3;
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
    else if(op < 4) 
        fp.swapBlock(op-1,id1,id2);
    else{
        if(op==4||op==5)
            shiftSp(fp.sp.S1,id1,id2);
        if(op==6||op==7)
            shiftSp(fp.sp.S2,id1,id2);
    }
}

#include<random>
bool climb(float initTemp,float temp,float cost1,float cost2){
    float chance = rand() / RAND_MAX;
    int delta_cost = cost2 - cost1;
    return chance < std::exp(-delta_cost * initTemp/temp);//t越小,exp值越小,越不可能跳,delta越大,越不可能跳
}


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
