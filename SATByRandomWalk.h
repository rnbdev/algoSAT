#ifndef SATBYRANDOMWALK_H
#define SATBYRANDOMWALK_H

#include "SATSolver.h"
#include <random>
#include <ctime>
#include <vector>
#include <cassert>

using namespace std;

/*
input : C_1 ^ C_2 ^ ... ^ C_m on n variables
        greed avoidence parameter p
algo:
init:
    assign some arbitrary values
    r, t = 0
    c_1, .. , c_n = 0
    k_j = number of true literals in C_j
    if k_j = 0, f_r = j, w_j = r and ++r
    if k_j = 1, if l_i was only true literal, then ++c_i
    ** r : number of unsat clauses
       f and w : lists them
       c_i : cost of changing - number of sat clause will become
             unsat if flipped

next:
    if r = 0; found solution. STOP!
    else
        uniformly random select j st. C_j is unsat clause
        smallest cost in C_j is c
        if c = 0 or 
           U > p (U is random variable in [0, 1)):
                choose a random l_i with c_i = c
        else:
            choose a random l_i from C_j
    flip l_i and update k, r, c, f, w
    t++;
    if t > N: break
*/

class SATByRandomWalk: public SATSolver{
private:
    default_random_engine generator{(u_int) time(0)};
public:
    u_int r;
    vector<bool> * assignments;
    vector<u_int> * c;
    vector<u_int> * k;
    vector<u_int> * f;
    vector<u_int> * w;
    vector<vector<u_int> *> * appeared_in;
    double p;

    SATByRandomWalk(double p): p(p){
        s = UNKNOWN;
    }

    void init(){
        assignments = new vector<bool>(num_literal);
        c = new vector<u_int>(num_literal);
        k = new vector<u_int>(num_clause);
        f = new vector<u_int>(num_clause);
        w = new vector<u_int>(num_clause);
        appeared_in = new vector<vector<u_int> *>(num_literal<<1);
        for(int i=0;i<num_literal<<1;i++){
            (*appeared_in)[i] = new vector<u_int>;
        }
        for(u_int i=0;i<clauses->size();i++){
            for(u_int lx=0;lx<(*clauses)[i]->size();lx++){
                (*appeared_in)[(*(*clauses)[i])[lx]]->push_back(i); 
            }
        }
    }

    void random_guess(){
        for(u_int i=0;i<num_literal;i++){
            (*assignments)[i] = uniform_int_distribution<int>{0, 1}(generator);
        }
    }

    void compute_vars(){
        r = 0;
        for(u_int i=0;i<c->size();i++) c->at(i) = 0;
        for(u_int j=0;j<clauses->size();j++){
            (*k)[j] = 0;
            u_int m_lit;
            for(int lx=0;lx<(*clauses)[j]->size();lx++){
                u_int lit = (*(*clauses)[j])[lx];
                // (*appeared_in)[lit]->push_back(j); // DANGER
                if((lit & 1) ^ (*assignments)[lit >> 1]){
                    (*k)[j]++;
                    m_lit = lit >> 1;
                }

            }
            if((*k)[j] == 0){
                (*f)[r] = j;
                (*w)[j] = r;
                ++r;
            }else if((*k)[j] == 1){
                ++(*c)[m_lit];
            }
            assert(r < num_clause);
            assert((*c)[m_lit] <= num_clause);
        }
        if(r == 0){
            s = SAT;
        }

    }

    void recompute_vars(u_int a_l){ // TODO : check this function
        bool v = (*assignments)[a_l];
        for(int x=0;x<2;x++){
            for(int l_c=0;l_c<(*appeared_in)[(a_l<<1) | x]->size();l_c++){
                u_int a_c = (*(*appeared_in)[(a_l<<1) | x])[l_c];
                if(v ^ x){
                    if((*k)[a_c] == 0){
                        r--;
                        assert((*f)[(*w)[a_c]] == a_c);
                        assert((*w)[(*f)[r]] == r);
                        (*f)[(*w)[a_c]] = (*f)[r];
                        (*w)[(*f)[r]] = (*w)[a_c];
                    }else if((*k)[a_c] == 1){
                        for(int lx=0;lx<(*clauses)[a_c]->size();lx++){
                            u_int lit = (*(*clauses)[a_c])[lx];
                            if(lit == ((a_l<<1) | x)){
                                continue;
                            }
                            if((lit & 1) ^ (*assignments)[lit >> 1]){
                                --(*c)[lit >> 1];
                                break;
                            }
                        }
                    }
                    ++(*k)[a_c];
                }else{
                    if((*k)[a_c] == 1){
                        --(*c)[a_l];
                    }
                    --(*k)[a_c];
                }
                assert(0 <= (*k)[a_c]);
                assert((*k)[a_c] <= num_literal);
                if((*k)[a_c] == 0){
                    (*f)[r] = a_c;
                    (*w)[a_c] = r;
                    r++;
                }else if((*k)[a_c] == 1){
                    if(v ^ x){
                        ++(*c)[a_l];
                        assert((*c)[a_l] <= num_clause);
                    }else{
                        for(int lx=0;lx<(*clauses)[a_c]->size();lx++){
                            u_int lit = (*(*clauses)[a_c])[lx];
                            if((lit & 1) ^ (*assignments)[lit >> 1]){
                                ++(*c)[lit >> 1];
                                break;
                            }

                        } 
                    }
                }
            }
        }
    }

    u_int random_walk(){
        u_int us_c = uniform_int_distribution<u_int>{0, r-1}(generator);
        us_c = (*f)[us_c];
        u_int min_cost = num_clause + 1;
        vector<u_int> * min_cost_lit = new vector<u_int>;
        for(u_int lx=0;lx<(*clauses)[us_c]->size();lx++){
            u_int a_lit = (*(*clauses)[us_c])[lx] >> 1;
            if((*c)[a_lit] < min_cost){
                min_cost = (*c)[a_lit];
                min_cost_lit->clear();
                min_cost_lit->push_back(a_lit);
            }else if((*c)[a_lit] == min_cost){
                min_cost_lit->push_back(a_lit);
            }
        }

        u_int rt_l;

        assert(min_cost_lit->size() > 0);
        
        if(min_cost == 0){
            u_int flip_lx = uniform_int_distribution<u_int>{0, (u_int) min_cost_lit->size()-1}(generator);
            rt_l = (*min_cost_lit)[flip_lx];

        }else{
            double U = uniform_real_distribution<double>{0., 1.}(generator);
            if(U > p){
                u_int flip_lx = uniform_int_distribution<u_int>{0, (u_int) min_cost_lit->size()-1}(generator);
                rt_l = (*min_cost_lit)[flip_lx];
            }else{
                u_int flip_lx = uniform_int_distribution<u_int>{0, (u_int) (*clauses)[us_c]->size()-1}(generator);
                rt_l = (*(*clauses)[us_c])[flip_lx] >> 1;
            }
        }
        delete min_cost_lit;
        (*assignments)[rt_l] = (*assignments)[rt_l] ^ 1;
        return rt_l;
    }

    void solve(u_int t, u_int iter){
        for(int i=0;i<iter;i++){
            random_guess();
            compute_vars();
            for(int j=0;j<t;j++){
                if(r == 0){
                    printf("c iteration taken %d\n", i);
                    printf("c steps taken %d\n", j);
                    s = SAT;
                    return;
                }
                // random_walk(); compute_vars();
                recompute_vars(random_walk());
            }
        }
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
                if(! (*assignments)[i])
                    fprintf(fp, "-");
                fprintf(fp, "%d ", i+1);
            }
            fprintf(fp, " 0\nv 0");
        }
        fprintf(fp, "\n");
    }

    ~SATByRandomWalk(){
        delete assignments;
        delete c;
        delete k;
        delete f;
        delete w;
        for(auto p: *appeared_in)
            delete p;
        delete appeared_in;
    }
};



#endif // RANDOMWALKSAT_H