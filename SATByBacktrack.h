#ifndef SATBYBACKTRACK_H
#define SATBYBACKTRACK_H

#include "Backtrack.h"

class SATByBacktrack: public Backtrack{
    struct cell{
        u_int l; u_int f; u_int b; u_int c;
    };
    cell * tab;
    u_int d, a, l;
    u_int * mv;
    u_int * size;
    u_int * start;
    u_int formulaSize;
    u_int totalSize;

    u_int guess(){
        l = (d<<1);
        if(tab[l].c < tab[l | 1].c)
            l |= 1;

        // printf("guess %d\n", d);
        mv[d] = (l&1) + 4 * (tab[l^1].c == 0);

        if(tab[l].c == a)
            return 1;
        else
            return 0;
    }

    u_int valid_guess(){
        if(!remove_lit(l^1)){
            return 0;
        }
        suppress_lit(l);
        d++;
        return 1;
    }

    u_int dual_guess(){
        if(mv[d]<2){
            mv[d] ^= 3;
            l ^= 1;
            // printf("dual guess d = %d , l = %d, mv[d] = %d\n", d, l, mv[d]);
            return 1;
        }else{
            return 0;
        }
    }

    u_int backtrack(){
        if(d<=0) return 0;
        // printf("Backtracking %d\n", l);
        d--;
        l = d<<1 | (mv[d]&1);

        unremove_lit(l^1);
        unsuppress_lit(l);

        return 1;
    }

    u_int remove_lit(u_int l){
        // printf("removing %d\n", l);
        u_int ix = tab[l].f;
        while(ix >= (num_literal<<1)){
            u_int c = tab[ix].c;
            if(size[c]==1){
                ix = tab[ix].b;
                while(ix >= (num_literal<<1)){
                    c = tab[ix].c;
                    size[c]++;
                    ix = tab[ix].b;
                }
                return 0;
            }
            size[c]--;
            ix = tab[ix].f;
        }
        return 1;
    }

    void unremove_lit(u_int l){
        // printf("unremoving %d\n", l);
        u_int ix = tab[l].b;
        while(ix >= (num_literal<<1)){
            u_int c = tab[ix].c;
            size[c]++;
            ix = tab[ix].b;
        }
    }

    void suppress_lit(u_int l){
        // printf("suppressing for %d\n", l);
        u_int ix = tab[l].f;
        while(ix >= (num_literal<<1)){
            u_int c = tab[ix].c;
            for(u_int i=start[c];i<start[c]+size[c]-1;i++){
                tab[tab[i].b].f = tab[i].f;
                tab[tab[i].f].b = tab[i].b;
                tab[tab[i].l].c--;
            }
            a--; ix = tab[ix].f;
        }
    }

    void unsuppress_lit(u_int l){
        // printf("unsuppressing for %d\n", l);
        u_int ix = tab[l].b;
        while(ix >= (num_literal<<1)){
            u_int c = tab[ix].c;
            for(u_int i=start[c];i<start[c]+size[c]-1;i++){
                tab[tab[i].b].f = i;
                tab[tab[i].f].b = i;
                tab[tab[i].l].c++;
            }
            a++; ix = tab[ix].b;
        }
    }

    public:
        void init(){
            d = 0; a = num_clause;
            mv = new u_int[num_literal];
            size = new u_int[num_clause];
            start = new u_int[num_clause];
            totalSize = (num_literal<<1);

            for(u_int i=0;i<num_clause;i++){
                start[i] = totalSize;
                size[i] = size_clause[i];
                totalSize += size[i];
            }
            formulaSize = totalSize - (num_literal<<1);
            tab = new cell[totalSize];
            for(u_int i=0;i<num_literal<<1;i++){
                tab[i].l = i;
                tab[i].c = 0;

            }
            u_int last_clause_of_lit[num_literal<<1];
            for(int i=0;i<num_literal<<1;i++)
                last_clause_of_lit[i] = i;

            for(u_int i=0;i<num_clause;i++){
                for(u_int j=0;j<size_clause[i];j++){
                    u_int li = clauses[i][j];
                    u_int ix = start[i]+size[i]-j-1;

                    tab[ix].l = li;
                    tab[ix].b = last_clause_of_lit[li];
                    tab[tab[ix].b].f = ix;
                    tab[ix].c = i;
                    tab[li].c++;

                    last_clause_of_lit[li] = ix;
                }
            }
            for(int i=0;i<num_literal<<1;i++){
                tab[i].b = last_clause_of_lit[i];
                tab[tab[i].b].f = i;
            }

            // printTable();
        }

        void printTable(){
            for(u_int i=0;i<totalSize;i++){
                printf("%3d", i);
                printf(i < totalSize-1 ? " " : "\n");
            }

            for(u_int i=0;i<totalSize;i++){
                printf("%3d", tab[i].l);
                printf(i < totalSize-1 ? " " : "\n");
            }

            for(u_int i=0;i<totalSize;i++){
                printf("%3d", tab[i].f);
                printf(i < totalSize-1 ? " " : "\n");
            }

            for(u_int i=0;i<totalSize;i++){
                printf("%3d", tab[i].b);
                printf(i < totalSize-1 ? " " : "\n");
            }

            for(u_int i=0;i<totalSize;i++){
                printf("%3d", tab[i].c);
                printf(i < totalSize-1 ? " " : "\n");
            }

            printf("\n");
        }

        void solve(){
            if(guess()){
                s = SAT;
                return;
            };
            while(1){
                while(1){
                    if(valid_guess()){
                        if(guess()){
                            s = SAT;
                            return;
                        }
                    }else{
                        if(!dual_guess())
                            break;
                    }
                }
                while(1){
                    if(backtrack()){
                        if(dual_guess())
                            break;
                    }else{
                        s = UNSAT;
                        return;
                    }
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
            delete[] size;
            delete[] start;
            delete[] tab;
        }
};

#endif