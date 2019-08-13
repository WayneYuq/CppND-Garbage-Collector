#include <memory>
#include <list>
#include "gc_pointer.h"
#include "gc_details.h"
#include "LeakTester.h"

namespace custom {
    template< typename T>
    class list {
    private:
        struct node{
            T value;
            Pointer< node > prev;
            Pointer< node > next;
            node(T val, node* _prev, node* _next) : value(val), prev(_prev), next(_next) {}
        };
        node* head;
        node* tail;
    public:

        // We don't have any destructors or freeing of memory
        list(): head(NULL), tail(NULL){}
        void push_back(T);
        bool empty() const { return ( !head || !tail ); }
        void print();
    };

    template <typename T>
    void list<T>::push_back(T val){
        list::tail = new node(val, list::tail, NULL);
        if(list::tail->prev){
            list::tail->prev->next = list::tail;
        }
        if( list::empty()){
            list::head = list::tail;
        }
    }

    template< typename T> 
    void list<T>::print() { 
        node* ptr= list::head;  
        while(ptr != NULL) { 
            std::cout<< ptr->value <<" "; 
            ptr = ptr->next; 
        }
        std::cout << std::endl;
    }
}

int main() {
    custom::list< int > list;

    std::cout << "Add 5 numbers: " << std::endl;
    for(int i = 0; i < 5; i++) {
        int temp;
        std::cout<<"Add "<< i+1<< ". element:"<< std::endl;
        std::cin >>temp;
        list.push_back(temp);
    }

    list.print();
    return 0;
}