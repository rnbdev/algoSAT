#ifndef SATBYGA_H
#define SATBYGA_H

#include "SATSolver.h"
#include <vector>
#include <random>
#include <ctime>
#include <algorithm>

using namespace std;

class Person;
class SATByGA;


class Person{
public:
    vector<bool> * dna;
    u_int fitness;

    Person(vector<bool> * d): dna(d){}

    Person(){
        dna = new vector<bool>;
    }

    void set_fitness(u_int f){
        fitness = f;
    }

    bool at(u_int i){
        return dna->at(i);
    }

    void push_back(bool b){
        dna->push_back(b);
    }

    ~Person(){
        delete dna;
    }
};



class SATByGA: public SATSolver{
public:
    u_int n_population;
    float mutation_rate = 0.02;
    vector<Person *> * population;
    default_random_engine generator{(u_int) time(0)};

    struct fitness_compare{
        bool operator() (Person * p1, Person * p2){
            return (p1->fitness) < (p2->fitness);
        }
    } fit_comp;

    SATByGA(u_int n){
        n_population = n;
        population = new vector<Person *>;
    }

    void init(){
        s = UNKNOWN;
        printf("higest fitness %u\n", num_clause);
        initialize_population();
    }

    void initialize_population(){
        for(auto p: *population){
            delete p;
        }
        population->clear();
        for(u_int i = 0;i < n_population; i++){
            Person * p = new Person;
            for(u_int i = 0;i < num_literal;i++){
                p->push_back(uniform_int_distribution<u_int>{0, 1}(generator));
            }
            compute_fitness(p);
            population->push_back(p);
        }
    }

    Person * make_child(Person * p1, Person * p2){
        Person * c = new Person;
        for(u_int i = 0;i < num_literal;i++){
            bool r = uniform_int_distribution<u_int>{0, 1}(generator);
            if(r == 0){
                c->push_back(p1->at(i));
            }else{
                c->push_back(p2->at(i));
            }
        }
        mutate(c);
        compute_fitness(c);
        return c;
    }

    void compute_fitness(Person * p){
        u_int count = 0;
        for(auto c: *clauses){
            bool flag = 0;
            for(auto l: *c){
                if(p->at(l>>1) ^ (l & 1)){
                    flag = 1;
                    break;
                }
            }
            if(flag) count++;
        }
        p->set_fitness(count);
    }

    void mutate(Person * p){
        float r;
        for(u_int i = 0; i < num_literal; i++){
            r = uniform_real_distribution<float>{0, 1}(generator);
            if(r < mutation_rate){
                bool b = p->dna->at(i);
                if((*literal_in)[i << 1 | b]->size() > (*literal_in)[i << 1 | (b ^ 1)]->size()){
                    p->dna->at(i) = p->dna->at(i) ^ 1;
                }
            }
        }
    }

    Person * get_parent(u_int size){
        vector<Person *> t;
        for(u_int i = 0; i < size; i++){
            u_int ix = uniform_int_distribution<u_int>{0, n_population-1}(generator);
            t.push_back(population->at(ix));
        }
        return *max_element(t.begin(), t.end(), fit_comp);
    }

    void solve(u_int iter){
        for(u_int i = 0; i < iter; i++){

            sort(population->begin(), population->end(), fit_comp);

            if(i % 100 == 0){
                printf("c evolved no. %u\n", i);

                for(u_int i = 0; i < 10; i++){
                    printf("%u\t", population->at(n_population - 1 - i)->fitness);
                }
                printf("\n");
            }

            if(population->back()->fitness >= num_clause){
                s = SAT;
                return;
            }

            vector<Person *> * next_population = new vector<Person *>;
            for(u_int i = 0; i < n_population; i++){
                Person * p1, * p2;
                p1 = get_parent(n_population * .2);
                p2 = get_parent(n_population * .2);
                Person * c = make_child(p1, p2);
                next_population->push_back(c);
            }
            sort(next_population->rbegin(), next_population->rend(), fit_comp);

            for(u_int i = 0; i < n_population * .1; i++){
                delete next_population->back();
                next_population->pop_back();
            }

            while(next_population->size() < n_population){
                next_population->push_back(population->back());
                population->pop_back();
            }

            for(auto p: *population){
                delete p;
            }
            delete population;

            population = next_population;
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
                if(! population->back()->at(i))
                    fprintf(fp, "-");
                fprintf(fp, "%d ", i+1);
            }
            fprintf(fp, " 0\nv 0");
        }
        fprintf(fp, "\n");
    }

    ~SATByGA(){
        for(auto p: *population){
            delete p;
        }
        delete population;
    }
};

#endif // SATBYGA_H