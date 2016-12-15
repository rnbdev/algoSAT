#include "SATByBacktrack.h"
#include "SATByWatchee.h"
#include "SATByCDPLL.h"
#include "SATByCL.h"
#include "SATByRandomWalk.h"

#include <cassert>

int main(int argc, char * argv[]){
    assert(argc > 1);

    FILE * fp1, * fp2;
    if(argc > 2){
        fp1 = fopen(argv[2], "r");
        if(argc > 3){
            fp2 = fopen(argv[3], "w");
        }else{
            fp2 = stdout;
        }
    }else{
        fp1 = stdin;
        fp2 = stdout;
    }

    if(argv[1][0] == 'b'){
            printf("-------- backtrack --------\n");
            SATByBacktrack b;
            b.readDimacs(fp1);
            b.init();
            b.solve();
            b.printDimacs(fp2);
    }else if(argv[1][0] == 'w'){
            printf("-------- watchee --------\n");
            SATByWatchee b;
            b.readDimacs(fp1);
            b.init();
            b.solve();
            b.printDimacs(fp2);
    }else if(argv[1][0] == 'd'){
            printf("-------- cyclic DPLL --------\n");
            SATByCDPLL b;
            b.readDimacs(fp1);
            b.init();
            b.solve();
            b.printDimacs(fp2);
    }else if(argv[1][0] == 'c'){
            printf("-------- clause learning --------\n");
            SATByCL b;
            b.readDimacs(fp1);
            b.init();
            b.solve();
            b.printDimacs(fp2);
    }else if(argv[1][0] == 'r'){
            printf("-------- random walk --------\n");
            SATByRandomWalk b(.5);
            b.readDimacs(fp1);
            b.init();
            b.solve(1000, 100);
            b.printDimacs(fp2);
    }else{
        printf("invalid choice\n");
    }

    fclose(fp1);
    fclose(fp2);
}