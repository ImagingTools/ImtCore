#include "positionnode.h"

#include <QListIterator>
#include <QList>
#include <QMap>

PositionNode::PositionNode()
{

}

QDomDocument PositionNode::gluePosition(QList<QDomElement> nodesList, int tCount){
    QDomDocument doc;
    QListIterator<QDomElement> iterator(nodesList);
    //iter
    QDomElement worksheet = doc.createElement("worksheet");

    QDomElement sheetFormat = doc.createElement("sheetFormatPr");
    sheetFormat.setAttribute("defaultRowHeight", "15"); //по умолчанию

    QDomElement sheetData = doc.createElement("sheetData");
     //Циклом проходим и склеиваем все в один
    while (iterator.hasNext()) {
        sheetData.appendChild( iterator.peekNext() );
        iterator.next();
    }

     QDomElement pageMargins = doc.createElement("pageMargins");
     pageMargins.setAttribute("left", "0.7");
     pageMargins.setAttribute("right", "0.7");
     pageMargins.setAttribute("top", "0.75");
     pageMargins.setAttribute("bottom", "0.75");
     pageMargins.setAttribute("header", "0.3");
     pageMargins.setAttribute("footer", "0.3");

     QDomElement pageSize = doc.createElement("pageSetup");
     pageSize.setAttribute("r:id", "rId1");
     pageSize.setAttribute("verticalDpi", "180");
     pageSize.setAttribute("horizontalDpi", "180");
     pageSize.setAttribute("paperSize", "9");
     pageSize.setAttribute("orientation", "portrait");
     pageSize.setAttribute("paperSize", "9");

     QDomElement tableParts;
     if(tCount!=0){
       tableParts = doc.createElement("tableParts");
       tableParts.setAttribute("count", tCount);

       for(int i=0; i<tCount; i++){
          QDomElement tablePart = doc.createElement("tablePart");
          tablePart.setAttribute("r:id",QString::number(i+2));
          tableParts.appendChild(tablePart);
       }
     }

    worksheet.appendChild(sheetFormat);
    worksheet.appendChild(sheetData);
    worksheet.appendChild(pageMargins);
    worksheet.appendChild(pageSize);
    worksheet.appendChild(tableParts);

    doc.appendChild(worksheet);
    return doc;

}

QDomElement PositionNode::addOne(QString node, int number, int style){
    QDomDocument doc;
    QDomElement row = doc.createElement("row");
    row.setAttribute("r", QString::number(number+1));
    
    QDomElement c= doc.createElement("c");
    c.setAttribute("r", node);
    if(style>0){c.setAttribute("s", QString::number(style));}
    c.setAttribute("t", "s");
    
    QDomElement v= doc.createElement("v");
    v.appendChild(doc.createTextNode(QString::number(number+1)));
    c.appendChild(v);
    
    row.appendChild(c);    
    return row;   
}

QList<QDomElement> PositionNode::addTable(QString node, int firstNumber, QMap<int, int> style){
    QString alphabet = "ABCDEFGHIGKLMNOPQRSTUVWXYZ";

    QStringList address = node.split(" ");
    int pozI = alphabet.indexOf(address[0]);
    int pozJ = address[1].toInt();
    int endI = alphabet.indexOf(address[2]);
    int endJ = address[3].toInt();
    int styleNum=1;
    QDomElement elemCell;
    QList<QDomElement> cellList;

    for (int i=pozI; i<endI;i++){
        for(int j=pozJ; j<endJ; j++){
            QString nodeCell = alphabet[i]+QString::number(j);
            if (style.value(styleNum)>0){ elemCell = addOne(nodeCell, firstNumber, style.value(styleNum));
            }else{ elemCell = addOne(nodeCell, firstNumber, 0);}
            styleNum++;
            firstNumber++;
            cellList.append(elemCell);
        }
    }
    return cellList;
}
