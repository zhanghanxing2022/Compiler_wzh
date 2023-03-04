#include <stdio.h>
#include <assert.h>
#include "util.h"
#include "slp.h"

FILE *stream=NULL;
int begin_id=1;

void putint(iTable_ value){
    if(value->isid){
        fprintf(stream, "    call void @putint(i32 %%%d)\n",value->value);
    }
    else{
        fprintf(stream, "    call void @putint(i32 %d)\n",value->value);
    }
    fprintf(stream, "    call void @putch(i32 10)\n");
}
//定义解释函数，使用函数式风格，不使用赋值语句，在定义时就对变量进行初始化
Table_ interpExpList(A_expList, Table_);
Table_ interpStm(A_stm, Table_);
iTable_ interpExp(A_exp, Table_);
//ID_VALID变量判断当前的id名是否有效,这个变量是否声明过
int ID_VALID =1;

iTable_ p_binop(iTable_ L,iTable_ R,A_binop oper,Table_ t){
     fprintf(stream, "    %%%d = ",begin_id);
     switch(oper){
        case A_plus:
            fprintf(stream, "add i32 ");
            break;
        case A_minus:
            fprintf(stream, "sub i32 ");
            break;
        case A_times:
            fprintf(stream, "mul i32 ");
            break;
        case A_div:
            fprintf(stream, "sdiv i32 ");
            break;
        default:
        ;
     }
     if(L->isid&&R->isid){
        fprintf(stream, "%%%d, %%%d\n",L->value,R->value);
     }
     else if(L->isid){
        fprintf(stream, "%%%d, %d\n",L->value,R->value);
     }
     else if(R->isid){
        fprintf(stream, "%d, %%%d\n",L->value,R->value);
     }
     else{
        fprintf(stream, "%d, %d\n",L->value,R->value);
     }
     return iTable(begin_id++,TRUE,t);
}
//lookup函数实现在链表中查找优先级最高的node
int lookup(Table_ t, string id){
     //当t不为空且id名符合时结束
    while ((t != NULL) && (t->id != id)) {
        t = t->tail;
    }
     //t不为空时,返回t的值
    if (t != NULL){
        return t-> value;
    
    }else{
         //如果t为空证明表中不存在这个变量,原语句出错
        ID_VALID = 0;
        return -1;
   
    }
}
//update函数实现链表的更新,在表头插入新的节点,打印赋值语句
Table_ update(Table_ t, string id,iTable_ value){
    
    if(value->isid){
      fprintf(stream, "    %%%d = add i32 %%%d, 0\n",begin_id,value->value);
    }
    else{
      fprintf(stream, "    %%%d = add i32 %d, 0\n",begin_id,value->value);
    }
  
    return Table(id, begin_id++, t);
}
//interpStm函数对语句解释
Table_ interpStm(A_stm s, Table_ t){
    switch (s->kind) {
        //对复合语句递归调用interpStm
        case A_compoundStm:{
            t = interpStm(s->u.compound.stm1, t);
            t = interpStm(s->u.compound.stm2, t);
            return t;
        }
        //新建一个表达式node并初始化
        case A_assignStm:{
            iTable_ it = interpExp(s->u.assign.exp, t);
            //赋值
            t = update(it->t, s->u.assign.id,it);
      
            return t;
        }
        case A_printStm:{
            t = interpExpList(s->u.print.exps, t);
            return t;
        }
        default:
            return t;
    }
}
//interExp对表达式进行解释
iTable_ interpExp(A_exp e, Table_ t){
    switch (e->kind){
        case A_idExp:{
            int value = lookup(t, e->u.id);
             //如果未在链表中找到变量,报错
            if (!ID_VALID){
                printf("In statement A_idExp\n");
                printf("error: Identifier %s does not exist!\n", e->u.id);
                ID_VALID = 1;
       
            }
             //如果找到变量则返回一个表达式node
            return iTable(value,TRUE, t);
     
        }
        case A_numExp:{
            iTable_ it = iTable(e->u.num,FALSE, t);
            return it;
        }
        case A_opExp:{
            iTable_ left = interpExp(e->u.op.left, t);
            iTable_ right = interpExp(e->u.op.right, left->t);
           
            return p_binop(left,right,e->u.op.oper,t);
             
            
        }
        case A_eseqExp:{
            t = interpStm(e->u.eseq.stm, t);
            iTable_ it = interpExp(e->u.eseq.exp, t);
            return it;
        }
        default:
            return NULL;
    }
}

//interpExpList处理ExpList的情况
Table_ interpExpList(A_expList e, Table_ t){
  //只有print语句中会有explist型参数,所以我们要打印所有参数
    switch (e->kind) {
         //对当前表达式求值
        case A_pairExpList:{
            iTable_ it = interpExp(e->u.pair.head, t);
        //打印结果
            putint(it);
        //继续对剩下的explist递归调用interExpList
            t = interpExpList(e->u.pair.tail, it->t);
      
            return t;
        }
        case A_lastExpList:{
            iTable_ it = interpExp(e->u.last, t);
            putint(it);
            return it->t;
        }
        default:
            return t;
    }
}


void generate_ll(FILE *my_stream, A_stm stm){
    stream=my_stream;
    assert(stream!=NULL);  //文件正确打开
    interpStm(stm,NULL);
  
}

