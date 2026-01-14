#ifndef BORDERS_H
#define BORDERS_H


class Borders
{
    int left;
    int right;
    int top;
    int bottom;
public:
    Borders();

    int getLeft();
    int getRight();
    int getTop();
    int getBottom();

    void setLeft(int);
    void setRight(int);
    void setTop(int);
    void setBottom(int);
};

#endif // BORDERS_H
