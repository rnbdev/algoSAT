#ifndef SATBYBACKTRACKVEC_H
#define SATBYBACKTRACKVec_H

#include <vector>

#include "BacktrackVec.h"

class SATByBacktrackVec: public BacktrackVec{
public:
    u_int a, d, l;
    vector<u_int> * C;
    vector<u_int> * m;
    vector<deque<u_int> *> * literal_in;

    void init(){
        // a1
        // initialize
        a = num_clause;
        d = 0;
        C = new vector<u_int>(num_literal << 1);
        m = new vector<u_int>(num_clause);
        literal_in = new vector<vector<u_int> *>(num_literal<<1);
        for(u_int i=0;i<num_literal<<1;i++){
            (*literal_in)[i] = new vector<u_int>;
        }

        for(u_int i=0;i<num_clause;i++){
            for(auto p: (*clauses)[i]){
                (*literal_in)[p]->push_back(i);
            }
        }
    }

    void solver(){
        a2:{
            // choose
            l = d << 2;
            if((*C)[l] <= (*C)[l | 1]) l ^= 1;
            (*m)[d] = (l & 1) + (((*C)[l ^ 1] == 0) << 2);
            if((*C)[l] == a){
                s = SAT;
                return;
            }
        }

        a3:{
            // remove -l
            if(! remove_literal(l ^ 1)){ // TODO : 
                goto a5;
            }
        }

        a4:{
            // deactivate l's clauses
            suppress_clause(l); // TODO
            a = a - (*C)[l];
            d = d + 1;
            goto a2;
        }

        a5:{
            // try again
            if((*m)[d] < 2){
                (*m)[d] = 3 - (*m)[d];
                l = (d << 1) + ((*m)[d] & 1);
                goto a3;
            }
        }

        a6:{
            // backtrack
            if(d == 0){
                s = UNSAT;
                return;
            }else{
                d = d - 1;
                l = (d << 1) + ((*m)[d] & 1);
            }
        }

        a7:{
            // reactivate l's clauses
            a = a + (*C)[l];
            unsuppress_clause(l); // TODO
        }

        a8:{
            // unremove -l
            unremove_literal(l ^ 1) // TODO
            goto a5;
        }
    }

    bool remove_literal(u_int lit){
        // lit remembers which clauses it was in
        for(auto c: *(*literal_in)[lit]){
            if((*clauses)[c]->size() == 1){
                return 0;
            }
        }
        for(auto c: *(*literal_in)[lit]){
            auto cx = (*clauses)[c];
            for(u_int i=0;i<cx->size();i++){
                if(cx->front() == lit){
                    cx->pop_front();
                    break; // assumes no repetition
                }else{
                    cx->push_back(cx->front());
                    cx->pop_front();
                }
            }
        }
    }

    void unremove_literal(u_int lit){
        for()
    }

    void suppress_clause(u_int lit){

    }

    void unsuppress_clause(u_int lit){

    }

    void 

    void printDimacs(FILE * fp){
        fprintf(fp, "s ");
        switch(s){
            case SAT:
                fprintf(fp, "SATISFIABLE"); break;
            case UNSAT:
                fprintf(fp, "UNSATISFIABLE"); break;
            default:
                fprintf(fp, "UNKNOWN"); break;
        }
        if(s == SAT){
            for(u_int i=0;i<=d;i++){
                if(i%10 == 0){
                    if(i > 0) fprintf(fp, " 0");
                    fprintf(fp, "\nv ");
                }
                if(mv[i]&1)
                    fprintf(fp, "-");
                fprintf(fp, "%d ", i+1);
            }
            fprintf(fp, " 0\nv 0");
        }
        fprintf(fp, "\n");
    }

    ~SATByBacktrack(){
        // delete[] mv;
        delete C;
        delete m;
    }
};

#endif