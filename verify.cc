#include <cstdio>
#include <vector>
#include <cassert>
#include "SATSolver.h"

class VerifySAT: public SATSolver{
public:
    vector<bool> * sol;
    bool is_sat;
    void readSol(FILE * fp){
        char type;
        fscanf(fp, "%c", &type);
        while(type == 'c'){
            fscanf(fp, "%*[^\n]");
            fscanf(fp, "\n%c", &type);
        }
        is_sat = 1;
        assert(type == 's');
        fscanf(fp, " %c", &type);
        if(type == 'S'){
            sol = new vector<bool>(num_literal, 0);
            fscanf(fp, "%*[^\n]\n%c", &type);
            assert(type == 'v');
            bool f;
            while(type == 'v'){
                int l;
                f = 1;
                while(1){
                    fscanf(fp, "%d", &l);
                    if(l == 0){
                        break;
                    }else if(l < 0){
                        assert(-l - 1 < num_literal);
                        (*sol)[-l - 1] = 0;
                    }else{
                        assert(l - 1 < num_literal);
                        (*sol)[l - 1] = 1;
                    }
                    f = 0;
                }
                if(f) break;
                fscanf(fp, "%*[^\n]");
                fscanf(fp, "\n%c", &type);
            }
        }else{
            is_sat = 0;
        }
    }

    void printSol(){
        if(! is_sat) return;
        for(u_int i=0;i<num_literal;i++){
            if(!(*sol)[i]) printf("-");
            printf("%u ", i + 1); 
        }
        printf("\n");
    }

    bool verify(){
        if(! is_sat){
            printf("unknown or unsat\n");
            return 0;
        }
        for(auto c: *clauses){
            for(auto l: *c){
                if((*sol)[l>>1] ^ (l&1)){
                    goto a;
                }
            }
            for(auto l: *c){
                printf("%d ", get_orig_literal(l, 1));
            }
            printf("--- this clause is not satisfied\n");
            return 0;
            a: continue;
        }
        printf("all clause is satisfied\n");
        return 1;
    }
};

int main(int argc, char * argv[]){
    assert(argc > 2);
    // printf("cnf file: \"%s\"; sol file: \"%s\"\n", argv[1], argv[2]);
    FILE * fp1 = fopen(argv[1], "r");
    FILE * fp2 = fopen(argv[2], "r");

    VerifySAT b;
    b.readDimacs(fp1);
    b.readSol(fp2);
    b.verify();

    fclose(fp1);
    fclose(fp2);

    // b.printSol();
}