#include "data.h"

Data& Data::operator = (Data an){
    for(int i = 0;i < DATA_LEN;++i){
        str[i] = an.str[i];
    }
    return *this;
}

bool operator < (Data bn, Data an){
    for(int i = 0;i < DATA_LEN;++i){
        if(bn.str[i] < an.str[i])
            return true;
        else if(bn.str[i] > an.str[i])
            return false;
    }
    return false;
}

bool operator == (Data bn, Data an){
    for(int i = 0;i < DATA_LEN;++i){
        if(bn.str[i] != an.str[i])
            return false;
    }
    return true;
}
