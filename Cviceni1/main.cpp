#include <iostream>
#include <string>
#include "field.h"
#include "vector3d.h"

int main()
{
    Field<double> myField = Field<double>(5, 4);

    myField(0,0) = 1;
    myField(2,0) = 1;
    myField(3,0) = 1;
    myField(4,0) = 1;
    myField(0,1) = 1;
    myField(4,3) = 1;

    //Field<double> myNewField = Field<double>(myField.getSizeI(), myField.getSizeJ());

    Field<double> myNewField (myField);

    myNewField = myField * 2.0;
    myNewField = myNewField + 3.0;
    myField = myField  + myNewField;

    double a = myNewField.mean();

    return 0;
}