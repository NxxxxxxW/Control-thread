#include "ass.h"
#include <stdlib.h>
int main(int argc, char* argv[]){
    string op;
    int operator_count = kDefaultOperatorCount;
    if(argc > 1){
        operator_count = atoi(argv[1]);
        if(operator_count <= 0 || operator_count > 10){
            operator_count = kDefaultOperatorCount;
        }
    }
    Ass *pAss = new Ass();
    pAss->create_generators();
    pAss->create_operators(operator_count);

    while(true){

        cin >> op;
        if(op.size() == 1 && op[0] == 'p'){
            pAss->thread_pause();
        }else if(op.size() == 1 && op[0] == 'r'){
            pAss->thread_resume();
        }
    }
    return 0;
}
