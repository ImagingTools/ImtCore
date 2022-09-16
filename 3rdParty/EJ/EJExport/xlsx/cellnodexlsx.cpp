#include "cellnodexlsx.h"

CellNodeXlsx::CellNodeXlsx(){

}

QDomDocument CellNodeXlsx::glueCell(QList<QDomElement> nodesList){
    QDomDocument doc;
    QListIterator<QDomElement> iterator(nodesList);
    //iter
    QDomElement sst;
    //create
    sst = doc.createElement("sst");
    sst.setAttribute("count", nodesList.size());
    sst.setAttribute("uniqueCount", nodesList.size());

    //Циклом проходим и склеиваем все в один
    while( iterator.hasNext() ){
        sst.appendChild( iterator.peekNext());
        iterator.next();
    }

     doc.appendChild(sst);
    return doc;
}

QDomElement CellNodeXlsx::addCellOne(QString node){
        QDomDocument doc;

        QDomElement si;
        //create
        si = doc.createElement("si");

        QDomElement t;
        t = doc.createElement("t");
        t.appendChild(doc.createTextNode(node));

        si.appendChild(t);
        return si;
}
