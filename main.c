#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "slp.h"
#include "prog1.h"
#include "print_slp.h"
#include "interp.h"

void generate_main_ll(FILE *stream, A_stm stm) {
    
//declare extern func + function head
    fprintf(stream, "declare void @putint(i32) #1\n");
    fprintf(stream, "declare void @putch(i32) #1\n");
    fprintf(stream, "define dso_local i32 @main() #0 {\n");

//actual code emitter:
//Here is a "hardcoded" one. You need to redo this so it works for all legitimate prog1.c
    generate_ll(stream,stm);
   
//End of the function
    fprintf(stream, "    ret i32 0;\n }\n");

    return;
}

int main(int argc, const char * argv[]) {

    FILE *stream;
    
//  print_slpis testing purpose:
    print_slp_stmt(stdout, prog1());
    fprintf(stdout, ";\n");
    
    stream = fopen(argv[1], "w");

    generate_main_ll(stream, prog1()); //generate main.ll for prog1()
    
    return 0;
}
