#ifndef SATBYCDPLL_H
#define SATBYCDPLL_H

#include "SATSolver.h"
#include <vector>
#include <deque>
#include <cassert>

using namespace std;

class SATByCDPLL: public SATSolver{
    /*
    h : current progress in indices
    m : current "move"s


    */
public:
    vector<u_int> * h_;
    vector<u_int> * m;
    vector<int> * x;
    vector<u_int> * NEXT;
    vector<vector<u_int> *> * W; // W[l] = {clauses has l at beginning}
    u_int d, h, t, k;

    void init(){
        d1();
        s = UNKNOWN;
    }

    void d1(){
        // initialize
        h_ = new vector<u_int>(num_literal + 1, 0);
        m = new vector<u_int>(num_literal + 1, 0);
        x = new vector<int>(num_literal + 1, 0);
        W = new vector<vector<u_int> *>(num_literal<<1 + 2);
        NEXT = new vector<u_int>(num_literal + 1); // literals indexed from 1
        // TODO: init W ** DONE
        for(u_int i=0;i<num_literal<<1 + 2;i++){
            W->at(i) = new vector<u_int>;
        }

        for(u_int i=0;i<num_clause;i++){
            deque<u_int> * c = clauses->at(i);
            if(! c->empty()){
                W->at(c->at(0) + 2)->push_back(i); // literals indexed from 1
            }
        }

        m->at(0) = d = h = t = 0;

        for(k=num_literal;k>0;--k){
            x->at(k) = -1;
            if(is_watchee(k << 1) || is_watchee(k << 1 | 1)){
                NEXT->at(k) = h;
                h = k;
                if(t == 0){
                    t = k;
                }
            }
        }
        if(t != 0){
            NEXT->at(t) = h;
        }
    }

    void solve(){

        d2:{
            // printf("d2\n");
            // success?
            if(t == 0){
                s = SAT;
                return;
            }else{
                k = t;
            }
        }


        d3:{
            // printf("d3\n");
            // look for unit clause
            h = NEXT->at(k);
            u_int f = is_unit(h<<1) | (is_unit(h<<1 | 1) << 1);
            if(f == 3){
                // go to D7
                goto d7;
            }else if(f == 1 || f == 2){
                m->at(d+1) = f + 3;
                t = k;
                // go to D5
                goto d5;
            }else{
                if(h != t){
                    k = h;
                    // repeat d3
                    goto d3;
                }
            }
        }

        d4:{
            // printf("d4\n");
            // two way branch
            h = NEXT->at(t);
            assert(d+1<num_literal+1);
            m->at(d+1) = (! is_watchee(h<<1)) || is_watchee(h<<1 | 1);
        }

        d5:{
            // printf("d5\n");
            // move on
            ++d;
            assert(d<num_literal+1);
            k = h;
            h_->at(d) = k;
            for(int i=0;i<d;i++){
                assert(h_->at(i) != k);
            }
            if(t == k){
                t = 0;
            }else{
                h = NEXT->at(k);
                NEXT->at(t) = h;
            }
        }

        d6:{
            // printf("d6\n");
            // update watches
            bool b = (m->at(d) + 1) & 1;
            x->at(k) = b;
            remove_watch(k<<1 | b);
            goto d2;

        }

        d7:{
            // printf("d7\n");
            // backtrack
            t = k;
            while(m->at(d) >= 2){
                k = h_->at(d);
                x->at(k) = -1;
                if(is_watchee(k<<1) || is_watchee(k<<1 | 1)){
                    NEXT->at(k) = h;
                    h = k;
                    NEXT->at(t) = h;
                }
                --d;
            }
        }

        d8:{
            // printf("d8\n");
            // failure
            if(d > 0){
                m->at(d) = 3 - m->at(d);
                k = h_->at(d);
                // return to d6
                goto d6;
            }else{
                // terminate
                s = UNSAT;
            }
        }
        
    }

    bool is_unit(u_int lit){ // TODO: refine this - but looks same type of iterations
        // printf("checking if %u, %u is unit\n", lit >> 1, lit & 1);
        if(literal_in->at(lit-2)->empty()){
            return 0;
        }
        for(auto c: *(literal_in->at(lit - 2))){ // original literal, clauses indexed from 0
            // printf("lit %u, %u in clause %u\n", lit >> 1, lit & 1, c);
            bool f = 1;
            for(auto l: *(clauses->at(c))){
                if(l + 2 == lit) continue;
                if(is_true(l + 2)){
                    f = 0;
                    // printf("%u, %u is true\n", (l+2)>>1, (l+2)&1);
                }

            }
            if(f){
                return 1;
            }
        }
        return 0;
    }

    bool is_watchee(u_int lit){
        return ! W->at(lit)->empty(); 
    }

    bool is_true(u_int lit){
        if(x->at(lit >> 1) < 0) return 1;
        return (lit & 1) ^ x->at(lit >> 1);
    }

    bool is_decided(u_int lit){
        // printf("checking if %u is decided : %d\n", lit, x->at(lit));
        return x->at(lit) >= 0;
    }

    bool remove_watch(u_int lit){ // TODO: refine this - but looks same type of iterations
        // printf("removing lit for %u, %u\n", lit>>1, lit&1);
        for(auto cx: *(W->at(lit))){
            // printf("clauses %u\n", cx);
            deque<u_int> * c = clauses->at(cx);
            assert(c->at(0) + 2 == lit);
            c->push_back(lit - 2);
            c->pop_front();
            for(int i=0;i<c->size()-1;i++){
                u_int new_w = c->front() + 2;
                if(is_true(new_w)){
                    if(! (is_watchee(new_w) || is_watchee(new_w ^ 1) || is_decided(new_w >> 1))){
                        // printf("new watchee adding to ring %u\n", new_w >> 1);
                        if(t == 0){
                            t = h = new_w >> 1;
                            NEXT->at(t) = h;
                        }else{
                            NEXT->at(new_w >> 1) = h;
                            h = new_w >> 1;
                            NEXT->at(t) = h;
                        }
                    }
                    W->at(new_w)->push_back(cx);
                    break;
                }
                c->push_back(c->front());
                c->pop_front();
            }
        }
        W->at(lit)->clear();
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
            for(u_int i=0;i<num_literal;i++){
                if(i%10 == 0){
                    if(i > 0) fprintf(fp, " 0");
                    fprintf(fp, "\nv ");
                }
                if(! x->at(i+1))
                    fprintf(fp, "-");
                fprintf(fp, "%d ", i+1);
            }
            fprintf(fp, " 0\nv 0");
        }
        fprintf(fp, "\n");
    }

    ~SATByCDPLL(){
        delete h_;
        delete m;
        delete x;
        delete NEXT;
        for(auto p: *W)
            delete p;
        delete W;
    }
};


#endif // SATBYCDPLL_H