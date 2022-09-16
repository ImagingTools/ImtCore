#ifndef POSITIONNODE_H
#define POSITIONNODE_H

#include <QDomDocument>



class PositionNode //создание файла разметки информации
{
public:
    PositionNode();
    QDomDocument gluePosition(QList<QDomElement> nodesList, int tCount);  //склейка
    QDomElement addOne(QString node, int number, int style); //добавили одну ячейку текста в массив текста
    QList<QDomElement> addTable(QString node, int firstNumber, QMap <int, int> style); //добавили таблицу (разметка)

};

#endif // POSITIONNODE_H
