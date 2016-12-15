#ifndef SATBYWATCHEE_H
#define SATBYWATCHEE_H

#include "SATSolver.h"
#include <cassert>

class SATByWatchee: public SATSolver{
public:
    u_int d;
    u_int l;
    vector<u_int> * m;
    vector<vector<u_int> *> * W; // W[l] = {clauses has l at beginning}

    void init(){
        // b1
        // initialize

        m = new vector<u_int>(num_literal);

        W = new vector<vector<u_int> *>(num_literal<<1);
        for(u_int i=0;i<num_literal<<1;i++){
            W->at(i) = new vector<u_int>;
        }

        for(u_int i=0;i<num_clause;i++){
            deque<u_int> * c = clauses->at(i);
            if(! c->empty()){
                W->at(c->front())->push_back(i); // literals indexed from 1
            }
        }
        
        d = 0;
    }

    void solve(){
        b2:{
            // rejoice or choose
            // printf("b2\n");
            if(d >= num_literal){
                s = SAT;
                return;
            }else{
                (*m)[d] = is_watchee((d << 1) | 1) || (! is_watchee(d << 1));
                l = (d << 1)  + (*m)[d];
            }
        }

        b3:{
            // printf("b3\n");
            // remove -l if possible
            if(!remove_watch(l ^ 1)){
                goto b5;
            }
        }

        b4:{
            // printf("b4\n");
            // advance
            d = d + 1;
            goto b2;
        }

        b5:{
            // printf("b5\n");
            // try again
            if((*m)[d] < 2){
                (*m)[d] = 3 - (*m)[d];
                l = (d << 1) + ((*m)[d] & 1);
                goto b3;
            }
        }

        b6:{
            // printf("b6\n");
            // backtrack
            if(d == 0){
                s = UNSAT;
            }else{
                d = d - 1;
                goto b5;
            }
        }
    }

    bool is_watchee(u_int lit){
        return ! W->at(lit)->empty();
    }

    bool is_true(u_int lit){
        if(! is_decided(lit >> 1)) return 1;
        return ((*m)[lit>>1] & 1) == (lit & 1);
    }

    bool is_decided(u_int lit){
        // printf("checking if %u is decided : %d\n", lit, x->at(lit));
        return d > lit;
    }

    bool remove_watch(u_int lit){ // TODO: refine this - but looks same type of iterations
        // printf("removing lit for %u, %u\n", lit>>1, lit&1);
        for(u_int j = W->at(lit)->size(); j-->0;){
            // printf("clauses %u\n", cx);
            u_int cx = W->at(lit)->at(j);
            deque<u_int> * c = clauses->at(cx);
            assert(clauses->at(cx)->front() == lit);
            clauses->at(cx)->push_back(clauses->at(cx)->front());
            clauses->at(cx)->pop_front();
            u_int i;
            for(i=0;i<clauses->at(cx)->size()-1;i++){
                u_int new_w = clauses->at(cx)->front();
                if(is_true(new_w)){
                    W->at(new_w)->push_back(cx);
                    W->at(lit)->pop_back();
                    break;
                }
                clauses->at(cx)->push_back(clauses->at(cx)->front());
                clauses->at(cx)->pop_front();
            }
            if(i == clauses->at(cx)->size() - 1){
                return 0; 
            }
        }
        return 1;
        // W->at(lit)->clear();
        // printf("size after clear %lu\n", W->at(lit)->size());
    }

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
            for(u_int i=0;i<d;i++){
                if(i%10 == 0){
                    if(i > 0) fprintf(fp, " 0");
                    fprintf(fp, "\nv ");
                }
                if((*m)[i]&1)
                    fprintf(fp, "-");
                fprintf(fp, "%d ", i+1);
            }
            fprintf(fp, " 0\nv 0");
        }
        fprintf(fp, " \n");
    }


    ~SATByWatchee(){
        delete m;
        for(auto p: *W)
            delete p;
        delete W;
    }
};

#endif // SATBYWATCHEE_H