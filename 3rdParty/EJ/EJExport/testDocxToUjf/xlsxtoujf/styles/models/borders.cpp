#include "borders.h"

Borders::Borders()
{
    left = 0;
    right = 0;
    bottom = 0;
    top = 0;
}

int Borders::getLeft()
{
    return left;
}

int Borders::getRight()
{
    return right;
}

int Borders::getBottom()
{
    return bottom;
}

int Borders::getTop()
{
    return top;
}

void Borders::setLeft(int value){
    left = value;
}

void Borders::setRight(int value){
    right = value;
}

void Borders::setBottom(int value){
    bottom = value;
}

void Borders::setTop(int value){
    top = value;
}
