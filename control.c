
#include "control.h"

// extern pTAC* funcLink;// 一个装函数�? TAC里面声明的全局变量

extern int FuncNum;// 有多少个函数 TAC里面声明的全局变量的数�?

// void optimiztion(pTAC TAC_node, pTACHeadList TAC_block)
// {
//     //printfTAC(TAC_node);
//     // toSSA(TAC_node, TAC_block);
//     // addFuncfai(TAC_node, TAC_block);
//     preinitOptimize();
//     int i = 0;
//     do
//     {
//         initOptimize(TAC_node,TAC_block);

//         optimizePropagationConstant(TAC_node,TAC_block);
//         printf("\n\n\n\n ---- after optimizePropagationConstant ---- \n\n\n\n");
//         // printfTAC(TAC_node);

//         optimizeVariableTransmission(TAC_node,TAC_block);
//         printf("\n\n\n\n ---- after optimizeVariableTransmission ---- \n\n\n\n");
//         // printfTAC(TAC_node);

//         optimizeConstFloding(TAC_node,TAC_block);
//         printf("\n\n\n\n ---- after ConstFloding ---- \n\n\n\n");
//         // printfTAC(TAC_node);

//         initOptimize(TAC_node,TAC_block);

//         deletUseless(TAC_node,TAC_block);
//         printf("\n\n\n\n ---- after deletUseless ---- \n\n\n\n");
//         // printfTAC(TAC_node);

//         optimizeCommonSubexpression(TAC_node,TAC_block);
//         printf("\n\n\n\n ---- after optimizeCommonSubexpression ---- \n\n\n\n");
//         // printfTAC(TAC_node);

//         i++;
//     } while (i < 5);
    
    

//     printf("end");
// }

extern int opt_flag;
void control(past node ,char output_file[])
{
    // 传入的是AST
    printf("--ast--\n");
    showAst(node, -1);
    // TAC生成
    // initSignal();
    // pTAC node_TAC = buildTAC(node,-1);
    // buildGlobalList();
    // printfgloabl();
    // printfTAC(node_TAC);
    // blockVarReName(node_TAC);
    // printfTAC(node_TAC);
    // printfgloabl();
    // get_num(node_TAC);
    // 将TAC的不同函数放�? funcLink 的pTAC数组中去
    // Scanner(node_TAC);  //  遍历生成
    //printf("-----TAC-----\n");
    // if (NULL != node_TAC)
    {
        // //build_Label_TAC(node_TAC);
        // relinkTAC(node_TAC);
    }
    //printf("----TAC gloabl----\n");


    // pTAC tempTAC;
    // initArm(output_file);
    // for (size_t i = 0; i < FuncNum; i++)
    // {
    //     tempTAC = funcLink[i];
    //     pTACHeadList TAC_block = NULL;
    //     if (NULL != node_TAC)
    //     {
    //         //printfTAC(tempTAC);
    //         build_Label_TAC(tempTAC);// ?????
    //         initTACBlock();
    //         TAC_block = blockTAC(tempTAC);
    //         //showblock(TAC_block);// 输出应将递归改为遍历，但结构是正确的
    //         if (opt_flag == 1)
    //         {
    //             optimiztion(tempTAC, TAC_block);
    //         }
    //         buildliveness(tempTAC, TAC_block);
    //         // printf("------%d liveness------\n",i);

    //         // showAllLiveness(tempTAC, TAC_block);
    //         // printf("------%d liveness------\n",i);
    //         // printf("\n\n\n\n\n\n");
            
    //         reg_control_block(tempTAC,TAC_block,0);
            
    //         buildARM(tempTAC,TAC_block);    
    //     }
    // }
    // finishArm();
    // printfgloabl();
}