#include <iostream>
#include <string>
#include "field.h"

int main()
{
    Field<double> myField = Field<double>(4, 3);

    myField(0,0) = 1;
    myField(2,0) = 1;
    myField(1,1) = 1;
    myField(4,3) = 1;

    return 0;
}