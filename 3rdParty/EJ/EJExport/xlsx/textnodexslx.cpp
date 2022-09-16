#include "textnodexslx.h"

TextNodeXslx::TextNodeXslx(){

}


QDomElement TextNodeXslx::addTextWithStyle(QString text, int tumbler[]){
// возможно не надо, ибо стили вообще отдельно
    return QDomElement();
}

QDomElement TextNodeXslx::addText(QString textI){

    //init nodes
    QDomDocument doc;
    QDomElement wr;
    QDomElement wt;
    QDomText txt;

    QDomElement wrPr;
    QDomElement sizeNode;
    QDomElement sizeCsNode;
    wrPr = doc.createElement("w:rPr");
    int sizeFont = 24;
    QString szFont = QString::number(sizeFont);
    sizeNode = doc.createElement("w:sz");
    sizeNode.setAttribute("w:val",szFont);
    sizeCsNode=doc.createElement("w:szCs");
    sizeCsNode.setAttribute("w:val", szFont);

    //glue

    wrPr.appendChild(sizeNode);
    wrPr.appendChild(sizeCsNode);

    //add in xml-doc
    wr = doc.createElement("w:r");
    wt = doc.createElement("w:t");
    txt = doc.createTextNode(textI);
    //STYLE

    //FINAL
    wt.appendChild(txt);
    wr.appendChild(wrPr);
    wr.appendChild(wt);


    return wr;
}
