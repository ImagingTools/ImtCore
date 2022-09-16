#include "tablenodedocx.h"
#include "textnodedocx.h"
#include <tableignorcellstruct.h>
#include "export_global.h"

TableNodeDocx::TableNodeDocx(){

}

//triple -- (строчка, столбец, сколько проигнорить)
QDomElement TableNodeDocx::addTable(int row, int column, QList<QDomElement> cellList, Empty emptyCell){
    QListIterator<QDomElement> cell(cellList);
    QDomDocument doc;

    //pre-
    QDomElement wp;
    QDomElement wTbl;
    QDomElement wTblPr;
        QDomElement tblStyle;
        QDomElement wTblW;
        QDomElement wTblInd;
    QDomElement wTblGrid;
        QDomElement wgridCol;
    QDomElement wTr;
        QDomElement wTrPr;
    QDomElement wTc;
        QDomElement wtcPr;
            QDomElement wtcW;
        QDomElement wtcBorder;
        QDomElement wShd;
    //init
    //Atributes

    //create
    wTbl = doc.createElement("w:tbl");
    //w:tblPr
    wTblPr = doc.createElement("w:tblPr");
    wTblW = doc.createElement("w:tblW");
    wTblW.setAttribute("w:w",width); //width
    wTblW.setAttribute("w:type","auto"); //pct
    tblStyle = doc.createElement("w:tblStyle");
    tblStyle.setAttribute("w:val", "a3");
    wTblInd = doc.createElement("w:tblInd");
        wTblInd.setAttribute("w:w", tblInd);
        wTblInd.setAttribute("w:type","dxa");
    wTblPr.appendChild(tblStyle);
    wTblPr.appendChild(wTblInd);
    wTblPr.appendChild(wTblW);

    wTbl.appendChild(wTblPr);
    //w:tblGrid
    wTblGrid = doc.createElement("w:tblGrid");
    for(int iColumn =0; iColumn < column; iColumn++){
        QString strGridCol = QString::number( gridColList.at(iColumn)* CM_TO_TWIP );
        wgridCol = doc.createElement("w:gridCol");
        wgridCol.setAttribute("w:w",strGridCol);//3213
        wTblGrid.appendChild(wgridCol);
    }
    wTbl.appendChild(wTblGrid);
    for(int iRow = 0; iRow < row; iRow++){
        wTr = doc.createElement("w:tr");
        for(int iColumn=0; iColumn < column; ++iColumn){
            if( !emptyCell.column.contains(iColumn) || !emptyCell.row.contains(iRow)){
                wTc = cell.peekNext();
                wTr.appendChild(wTc);
                cell.next();
            }
        }
        wTbl.appendChild(wTr);
    }

    return wTbl;
}

QDomElement TableNodeDocx::addTablePlus(int row, int column, QList<QDomElement> cellList, int styles[]){
    QListIterator<QDomElement> cell(cellList);

    QDomDocument doc;
    QDomElement wTbl;
    QDomElement wTr;
    QDomElement wTc;
    //QDomElement wTblPr;
    QDomElement wr;
    QDomElement wp;

    //tbl:pr

    //FIXED!!!!!!!!!!!!!
    wTbl = doc.createElement("w:tbl");
    for(int iRow = 1; iRow <= row; iRow++){
        wTr = doc.createElement("w:tr");
        for(int iColumn = 1; iColumn <= column; iColumn++){
            wTc = doc.createElement("w:tc");
            wp = doc.createElement("w:p");
            wr = doc.createElement("w:r");

            TextNodeDocx txt = TextNodeDocx();
            //wr = txt.addText(cell.peekNext());

            cell.next();
            wp.appendChild(wr);
            wTc.appendChild(wp);
            wTr.appendChild(wTc);
        }
        wTbl.appendChild(wTr);
    }

    return wTbl;
}
