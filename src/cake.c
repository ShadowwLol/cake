#include "../include/cake.h"

int main(int argc, char * argv[]){
    if (argc < 2){
        return EX_F;
    }

    parse_file(argv[1]);

    return EX_S;
}
