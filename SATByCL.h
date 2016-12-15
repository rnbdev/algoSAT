#ifndef SATBYCL_H
#define SATBYCL_H

#include "SATSolver.h"
#include <cstdio>
#include <vector>
#include <random>
#include <ctime>

class SATByCL: public SATSolver{
private:
    default_random_engine generator{(u_int) time(0)};
public:
    vector<u_int> * l;
    u_int d;
    u_int m;


    void init(){
        // i1 - initialize
        d = 0;
        l = new vector<u_int>(num_literal);
        for(int i=0;i<num_literal;i++){
            l->at(i) = i << 1;
        }
        m = num_clause;
    }

    void solve(){
        u_int cx, cx_n;
        i2:{
            // printf("i2\n");
            // advance
            if(d == num_literal){
                s = SAT;
                return;
            }else{
                get_another_literal(); // TODO: code : DONE
            }

        }

        i3:{
            // printf("i3\n");
            // find falsified c_i
            cx = get_falsified_clause(); // TODO: code : DONE
            if(cx >= m){
                if(s == SAT) return;
                goto i2; // necessary? can just say SAT
            }

        }

        i4:{
            // printf("i4\n");
            // find falsified c_j
            l->at(d - 1) = l->at(d - 1) ^ 1;
            cx_n = get_falsified_clause();
            if(cx_n >= m){ // found none
                if(s == SAT) return;
                goto i2;
            }
        }

        i5:{
            // printf("i5\n");
            // resolve
            resolve(cx, cx_n); // TODO : code : DONE
            if(clauses->back()->empty()){
                s = UNSAT;
                return;
            }else{
                d = get_highest_d(); // TODO: get the highest index from resolved clause
                cx = m - 1;
                goto i4;
            }
        }
    }

    u_int get_highest_d(){ // TODO: refine
        auto c = clauses->back();
        for(u_int i = d;i > 0;--i){
            for(auto e_l: *c){
                if((e_l ^ 1) == l->at(i - 1)){
                    return i;
                }
            }
        }
        assert(0);
    }

    void resolve(u_int c1, u_int c2){
        vector<bool> l_f(num_literal << 1, 0);
        for(auto e_l: *clauses->at(c1)){
            l_f[e_l] = 1;
        }
        for(auto e_l: *clauses->at(c2)){
            if(l_f[e_l ^ 1]){
                l_f[e_l ^ 1] = 0;
            }else{
                l_f[e_l] = 1;
            }
        }
        deque<u_int> * n_c = new deque<u_int>;

        for(auto e_l: *clauses->at(c1)){
            if(l_f[e_l]){
                n_c->push_back(e_l);
                l_f[e_l] = 0;
                literal_in->at(e_l)->push_back(m);
            }
        }
        for(auto e_l: *clauses->at(c2)){
            if(l_f[e_l]){
                n_c->push_back(e_l);
                literal_in->at(e_l)->push_back(m);
            }
        }
        clauses->push_back(n_c);
        ++m;
    }

    u_int get_another_literal(){
        u_int a = uniform_int_distribution<u_int>{d, num_literal-1}(generator);
        // printf("-- (%u, %u), %u\n", d, num_literal-1, a);
        u_int b = l->at(a);
        l->at(a) = l->at(d);
        l->at(d) = b;
        ++d;
    }

    u_int get_falsified_clause(){
        vector<bool> c_m(m, 1);
        u_int n_unsat = m;
        for(u_int i=0;i<d;i++){
            for(auto c: *literal_in->at(l->at(i))){
                if(c_m[c]){
                    c_m[c] = 0;
                    --n_unsat;
                }
            }
        }
        if(n_unsat == 0){
            s = SAT;
            return m;
        }
        for(u_int i=d;i<num_literal;i++){
            for(auto c: *literal_in->at(l->at(i))){
                c_m[c] = 0;
            }
            for(auto c: *literal_in->at(l->at(i) ^ 1)){
                c_m[c] = 0;
            }
        }
        for(u_int i=0;i<m;i++){
            if(c_m[i]) return i;
        }
        return m;
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
            for(u_int i = 0;i<d;i++){
                if(i%10 == 0){
                    if(i > 0) fprintf(fp, " 0");
                    fprintf(fp, "\nv ");
                }
                if((*l)[i] & 1){
                    fprintf(fp, "-");
                }
                fprintf(fp, "%u ", ((*l)[i] >> 1) + 1);
            }
            fprintf(fp, " 0\nv 0");
        }
        fprintf(fp, "\n");
    }

    ~SATByCL(){
        delete l;
    }
};

#endif // SATBYCL_H