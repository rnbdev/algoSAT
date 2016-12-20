#ifndef LINKEDLIST_H
#define LINKEDLIST_H

template <class T>
class LinkedList{
    T val;
    LinkedList *nextNode;
    public:
        LinkedList(T v): val(v), nextNode(NULL){}

        void setNext(LinkedList<T> *t){nextNode = t;}

        LinkedList * next(){return nextNode;}
        T getValue(){return val;}
};

#endif