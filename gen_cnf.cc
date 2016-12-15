#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <cassert>

using namespace std;

class CNF{
public:
    vector<vector<int> *> *cnf_form;
    int n_vars;
    CNF(int n_v): n_vars(n_v){
        cnf_form = new vector<vector<int> *>;
    }

    void add_clause(vector<int> *c){
        cnf_form->push_back(c);
    }

    void dump(FILE * fp){
        fprintf(fp, "p cnf %d %lu\n", n_vars, cnf_form->size());
        for(auto c_p : *cnf_form){
            for(auto l: *c_p){
                fprintf(fp, "%d ", l);
            }
            fprintf(fp, "0\n");
        }
    }

    ~CNF(){
        for(auto c_p : *cnf_form){
            delete c_p;
        }
        delete cnf_form;
    }

};

int main(int argc, char *argv[]){
    if(argc < 2){
        printf("Give board size\n");
        return 1;
    }
    int n = atoi(argv[1]);
    FILE * cnf_file;
    if(argc < 3){
        cnf_file = stdout;
    }else{
        cnf_file = fopen(argv[2], "w");
    }

    int n_cells = n * n;
    int n_clauses = 0;

    CNF * cnf = new CNF(n_cells);

    // each row has at least one queen
    for(int r=1;r<=n;r++){
        vector<int> * v = new vector<int>;
        for(int c=1;c<=n;c++){
            int b = (r - 1) * n + c;
            v->push_back(b);
            assert(1 <= b && b <= n_cells);
        }
        cnf->add_clause(v);
    }

    // each row has at max one queen
    for(int r=1;r<=n;r++){
        for(int c1=1;c1<=n;c1++){
            int b1 = (r - 1) * n + c1;
            for(int c2=c1+1;c2<=n;c2++){
                int b2 = (r - 1) * n + c2;
                vector<int> * v = new vector<int>;
                *v = {-b1, -b2};
                assert(1 <= b1 && b1 <= n_cells);
                assert(1 <= b2 && b2 <= n_cells);
                cnf->add_clause(v);
            }
        }
    }

    // each column has at least one queen
    for(int c=1;c<=n;c++){
        vector<int> * v = new vector<int>;
        for(int r=1;r<=n;r++){
            int b = (r - 1) * n + c;
            v->push_back(b);
            assert(1 <= b && b <= n_cells);
        }
        cnf->add_clause(v);
    }

    // each column has at max one queen
    for(int c=1;c<=n;c++){
        for(int r1=1;r1<=n;r1++){
            int b1 = (r1 - 1) * n + c;
            for(int r2=r1+1;r2<=n;r2++){
                int b2 = (r2 - 1) * n + c;
                vector<int> * v = new vector<int>;
                *v = {-b1, -b2};
                assert(1 <= b1 && b1 <= n_cells);
                assert(1 <= b2 && b2 <= n_cells);
                cnf->add_clause(v);
            }
        }
    }

    // each diagonal has at max one queen
    for(int r1=1;r1<=n;r1++){
        for(int c1=1;c1<=n;c1++){
            int b1 = (r1 - 1) * n + c1;
            for(int r2=r1+1;r2<=n;r2++){
                int c2, b2;
                // positive slope
                c2 = r2 - (r1 - c1);
                if(c2 <= n){
                    b2 = (r2 - 1) * n + c2;
                    vector<int> * v = new vector<int>;
                    *v = {-b1, -b2};
                    assert(1 <= b1 && b1 <= n_cells);
                    assert(1 <= b2 && b2 <= n_cells);
                    cnf->add_clause(v);
                }
                // negative slope
                c2 = (r1 + c1) - r2;
                if(c2 >= 1){
                    b2 = (r2 - 1) * n + c2;
                    vector<int> * v = new vector<int>;
                    *v = {-b1, -b2};
                    assert(1 <= b1 && b1 <= n_cells);
                    assert(1 <= b2 && b2 <= n_cells);
                    cnf->add_clause(v);
                }
            }
        }
    }

    // each straight line has at max two queens
    for(int r1=1;r1<=n;r1++){
        for(int r2=r1+1;r2<=n;r2++){
            for(int c1=1;c1<=n;c1++){
                for(int c2=1;c2<=n;c2++){
                    int b1 = (r1 - 1) * n + c1;
                    int b2 = (r2 - 1) * n + c2;
                    for(int r3=r2+1;r3<=n;r3++){
                        float c3_;
                        int c3;
                        c3_ = c2 + (((c2 - c1) * (r3 - r2)) / (float)(r2 - r1));
                        c3 = c3_;
                        if(c3 != c3_)
                            continue;
                        if(c3 < 1 || n < c3)
                            continue;
                        int b3 = (r3 - 1) * n + c3;
                        vector<int> * v = new vector<int>;
                        *v = {-b1, -b2, -b3};
                        assert(1 <= b1 && b1 <= n_cells);
                        assert(1 <= b2 && b2 <= n_cells);
                        assert(1 <= b3 && b3 <= n_cells);
                        cnf->add_clause(v);
                    }
                }
            }
        }
    }

    cnf->dump(cnf_file);

    delete cnf;

    fclose(cnf_file);
    return 0;
}