#ifndef BACKTRACK_H
#define BACKTRACK_H

#include <cstdio>
#include <cstring>

#include "ds/LinkedList.h"

typedef unsigned int u_int;

using namespace std;

class Backtrack{
    protected:
        enum status{ SAT, UNSAT, UNKNOWN };

        u_int * size_clause;
        u_int ** clauses;
        u_int num_literal, num_clause;
        status s;

    public:
        void readDimacs(FILE * fp){
            char type, cnf[4];
            fscanf(fp, "%c", &type);
            while(type == 'c'){
                fscanf(fp, "%*[^\n]");
                fscanf(fp, "\n%c", &type);
            }
            if(type != 'p') return;
            fscanf(fp, "%s", cnf);
            fscanf(fp, "%u", &num_literal);
            fscanf(fp, "%u", &num_clause);

            if(strcmp(cnf, "cnf") != 0) return;

            LinkedList<u_int> * clauses_with_lit[num_literal<<1];
            for(u_int i=0;i<num_literal<<1;i++){
                clauses_with_lit[i] = NULL;
            }

            size_clause = new u_int[num_clause];

            for(u_int i=0; i<num_clause; i++){
                int lit;
                size_clause[i] = 0;
                while(1){
                    fscanf(fp, "%d", &lit);
                    if(lit == 0) break;
                    size_clause[i]++;
                    if(0 < lit){
                        lit--;
                        LinkedList<u_int> * a = new LinkedList<u_int>(i);
                        a->setNext(clauses_with_lit[lit<<1]);
                        clauses_with_lit[lit<<1] = a;
                    }else if(0 > lit){
                        lit = -lit; lit--;
                        LinkedList<u_int> * a = new LinkedList<u_int>(i);
                        a->setNext(clauses_with_lit[lit<<1|1]);
                        clauses_with_lit[lit<<1|1] = a;
                    }
                }
            }

            clauses = new u_int * [num_clause];
            for(u_int i=0;i<num_clause;i++){
                clauses[i] = new u_int[size_clause[i]];
            }
            u_int temp_ix[num_clause];
            memset(temp_ix, 0, sizeof(*temp_ix)*num_clause);
            for(u_int i=0;i<num_literal<<1;i++){
                LinkedList<u_int> * curr = clauses_with_lit[i];
                while(curr){
                    u_int c = curr->getValue();
                    clauses[c][temp_ix[c]++] = i;
                    LinkedList<u_int> * last = curr;
                    curr = last->next();
                    delete last;
                }
            }

            // for(u_int i=0;i<num_clause;i++){
            //     for(u_int j=0;j<size_clause[i];j++){
            //         if(clauses[i][j]&1) printf("-");
            //         printf("%u ", (clauses[i][j]>>1)+1);
            //     }
            //     printf("0\n");
            // }
        }

        bool is_sat(){
            return s == SAT;
        }

        // void printDimacs(FILE * fp) = 0;

        ~Backtrack(){
            for(u_int i=0;i<num_clause;i++){
                delete[] clauses[i];
            }
            delete[] clauses;
            delete[] size_clause;
        }
};

#endif // BACKTRACK_H