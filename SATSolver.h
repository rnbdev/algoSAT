#ifndef SATSOLVER_H
#define SATSOLVER_H

#include <cstdio>
#include <vector>
#include <deque>
#include <cstring>

typedef unsigned int u_int;

using namespace std;

class SATSolver{
    protected:
        enum status{ SAT, UNSAT, UNKNOWN };

        vector<deque<u_int> *> * clauses;
        vector<vector<u_int> *> * literal_in;
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

            clauses = new vector<deque<u_int> *>(num_clause);
            literal_in = new vector<vector<u_int> *>(num_literal<<1);

            for(u_int i=0;i<num_literal<<1;i++){
                literal_in->at(i) = new vector<u_int>;
            }

            for(u_int i=0; i<num_clause; i++){
                clauses->at(i) = new deque<u_int>;
                int lit;
                while(1){
                    fscanf(fp, "%d", &lit);
                    if(lit == 0) break;
                    if(0 < lit){
                        lit--;
                        clauses->at(i)->push_back(lit << 1);
                        literal_in->at(lit << 1)->push_back(i);
                    }else if(0 > lit){
                        lit = -lit; lit--;
                        clauses->at(i)->push_back(lit << 1 | 1);
                        literal_in->at(lit << 1 | 1)->push_back(i);
                    }
                }
                // may be sort clauses
            }

            // print_cnf();
        }

        void print_cnf(){
            for(auto clause: *clauses){
                for(auto e_lit: *clause){
                    printf("%d ", get_orig_literal(e_lit, 1));
                    // if(e_lit&1) printf("-");
                    // printf("%u ", (e_lit >> 1) + 1);
                }
                printf("0\n");
            }
        }

        void add_empty_clause(){
            clauses->push_back(new deque<u_int>);
            num_clause++;
        }

        void add_literal_in(u_int lit, u_int cl){
            (*clauses)[cl]->push_back(lit);
        }

        void rotate_clause(u_int cl){
            (*clauses)[cl]->push_back((*clauses)[cl]->front());
            (*clauses)[cl]->pop_front();
        }

        int get_orig_literal(u_int lit, int offset){
            int o_lit = (lit >> 1) + offset;
            if(lit & 1) o_lit = - o_lit;
            return o_lit;
        }

        bool is_sat(){
            return s == SAT;
        }

        // void printDimacs(FILE * fp) = 0;

        ~SATSolver(){
            for(auto p: *clauses){
                delete p;
            }
            delete clauses;
            for(auto p: *literal_in){
                delete p;
            }
            delete literal_in;
        }
};

#endif // SATSOLVER_H