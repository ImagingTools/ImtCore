#include "tablenodexlsx.h"
//#include "textnodexslx.h"

TableNodeXlsx::TableNodeXlsx(){

}

QString TableNodeXlsx :: getRefValue(int tPosition, int column, int row, bool type){
    // tposition - положение, col/row -столбцы строки
    // type true - адрес для записи в файл таблицы
    // type false - адрес для записи в файл разметки
    //запись вида AI:XJ (левый верхний:правый нижний)--
    //а неподвижно, I - позиция табл под текстом, X - соответст буква под столбцом, J - кол-во строк+позиция (правый нижний угол)
    // проверить точно ли -1 в tPosition,

    QString alphabet = "ABCDEFGHIGKLMNOPQRSTUVWXYZ"; //пока считаем, что дольше z не зайдем
    QString result;
    if (type){ result ="A"+QString::number(tPosition)+":"+alphabet[column]+QString::number(row+tPosition);}
    else {
        result ="A "+QString::number(tPosition)+" "+alphabet[column]+" "+QString::number(row+tPosition);}
    return result;
}

QDomDocument TableNodeXlsx::glueTable(int column, int row, QList<QString> tableInfo){
   QListIterator<QString> tInfo(tableInfo);
   QDomDocument doc;
   int id= tInfo.next().toInt();
   QString refValue = getRefValue(tInfo.next().toInt()-1, column, row, true);

   QDomElement table = doc.createElement("table");
        table.setAttribute("id", id);
        table.setAttribute("name", "table"+QString::number(id));
        table.setAttribute("displayName", "table"+QString::number(id));
        table.setAttribute("ref", refValue);


    QDomElement autoFilter = doc.createElement("autoFilter");
         autoFilter.setAttribute("ref", refValue);

    QDomElement tableColums = doc.createElement("tableColumns");
        tableColums.setAttribute("count", QString::number(column));

        QList<QDomElement> colums;
        QDomElement tableColumn= doc.createElement("tableColumn");
        for(int i=0; i<column;i++){
            tableColumn.setAttribute("id", i+1);
            tableColumn.setAttribute("name",tInfo.next());
            colums.append(tableColumn);
            tableColumn.removeAttribute("id");
            tableColumn.removeAttribute("name");
        }

         QListIterator<QDomElement> tColums(colums);
        for(int i=0; i<column;i++){
            tableColums.appendChild(tColums.next());
        }

//пока забиваем стандартно
    QDomElement tableStyleInfo = doc.createElement("tableStyleInfo");
        tableStyleInfo.setAttribute("name", "TableStyleMedium9");
        tableStyleInfo.setAttribute("showFirstColumn", "0");
        tableStyleInfo.setAttribute("showLastColumn","0");
        tableStyleInfo.setAttribute("showRowStripes", "1");
        tableStyleInfo.setAttribute("showColumnStripes", "0");

    table.appendChild(tableColums);
    table.appendChild(tableStyleInfo);

    doc.appendChild(table);
    return doc;
}

QDomElement TableNodeXlsx :: addTable(int row, int column, QList<QString> cellList){ return QDomElement(); }




