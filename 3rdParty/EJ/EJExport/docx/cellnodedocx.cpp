#include "cellnodedocx.h"
#include <QDebug>

CellNodeDocx::CellNodeDocx()
{
    isMerge = false;
    restart = true;
}

//listText -- wp; styleTumbler -- параметры ячейки
QDomElement CellNodeDocx::addCellWithStyle(QDomElement wp, QMap<QString, int> style_tumbler){
    //INIT
    //QListIterator<QDomElement> cell(listText);
    QDomDocument doc;
    QDomElement wTc;
    QDomElement wtcW;
    QDomElement wtcPr;
    QDomElement wgridSpan;
    QDomElement wMerge;
    QDomElement wShd;
    //QDomElement wp;
    QDomElement wtcBorder;
    QDomElement wTop;
    QDomElement wBottom;
    QDomElement wLeft;
    QDomElement wRight;
    //Merge
    QString restOrCont;
    QString gridSpan;
    //width
    QString width = QString::number( style_tumbler.value("width"));
    //Setting color
    QString strRed;
    if(style_tumbler.value("RedBrushCell") == 0){
        strRed = "00";
    }else{
        strRed = QString("%1").arg(style_tumbler.value("RedBrushCell"), 0,16).toUpper();
    }
    QString strGreen;
    if(style_tumbler.value("GreenBrushCell") == 0){
        strGreen = "00";
    }else{
        strGreen = QString("%1").arg(style_tumbler.value("GreenBrushCell"), 0,16).toUpper();
    }
    QString strBlue;
    if(style_tumbler.value("BlueBrushCell") == 0){
        strBlue = "00";
    }else{
        strBlue = QString("%1").arg(style_tumbler.value("BlueBrushCell"), 0,16).toUpper();
    }
    QString fill;
    fill= strRed + strGreen +strBlue;
    if( fill == "000000" || fill.isEmpty()){
        fill ="auto";
    }

    //CREATE AND ADD ATTRIBUTES
    wTc = doc.createElement("w:tc");

    wtcPr = doc.createElement("w:tcPr");
        wtcW = doc.createElement("w:tcW");
            wtcW.setAttribute("w:type","dxa");
            wtcW.setAttribute("w:w", width); //3213 354
        if(isMerge){
            gridSpan = QString::number(style_tumbler.value("gridSpan"));
            wgridSpan = doc.createElement("w:gridSpan");
                wgridSpan.setAttribute("w:val", gridSpan);
            if(restart){ restOrCont = "restart";}else{
                restOrCont = "continue";
            }
            wMerge = doc.createElement("w:vMerge");
                wMerge.setAttribute("w:val", restOrCont);
        }

        wtcBorder= doc.createElement("w:tcBorders");
            wTop= doc.createElement("w:top");
                wTop.setAttribute("w:val", "single"); //nil интересный вариант
                //wTop.setAttribute("w:sz", 2); //-1
                wTop.setAttribute("w:sz", style_tumbler.value("topBorderSize"));
                wTop.setAttribute("w:space","0");
                wTop.setAttribute("w:color","000001");
            wBottom = doc.createElement("w:bottom");
                wBottom.setAttribute("w:val", "single");
                wBottom.setAttribute("w:sz", style_tumbler.value("bottomBorderSize"));
                //wBottom.setAttribute("w:sz", 2); //1
                wBottom.setAttribute("w:space","0");
                wBottom.setAttribute("w:color","000001");
            wLeft = doc.createElement("w:left");
                wLeft.setAttribute("w:val", "single");
                wLeft.setAttribute("w:sz", style_tumbler.value("leftBorderSize"));
                //wLeft.setAttribute("w:sz", 9); //2
                wLeft.setAttribute("w:space","0");
                wLeft.setAttribute("w:color","000001");
            wRight = doc.createElement("w:right");
                wRight.setAttribute("w:val", "single");
                wRight.setAttribute("w:sz", style_tumbler.value("rightBorderSize"));
                //wRight.setAttribute("w:sz", 12); //3
                wRight.setAttribute("w:space","0");
                wRight.setAttribute("w:color","000001");
        wShd = doc.createElement("w:shd");
        wShd.setAttribute("w:fill", fill); // or FF-FF-FF
        wShd.setAttribute("w:val","clear");



    //p
    //wp = cell.peekNext();

    //GLUE
    wTc.appendChild(wtcPr);
        wtcPr.appendChild(wtcW);
        wtcPr.appendChild(wtcBorder);
        if(isMerge){
        wtcPr.appendChild(wgridSpan);
        wtcPr.appendChild(wMerge);}
            wtcBorder.appendChild(wTop);
            wtcBorder.appendChild(wLeft);
            wtcBorder.appendChild(wBottom);
            wtcBorder.appendChild(wRight);
        wtcPr.appendChild(wShd);
    wTc.appendChild(wp);

    return wTc;
}

//listText -- wp; styleTumbler -- параметры ячейки
QDomElement CellNodeDocx::addCellWithStyleAndMerge(QDomElement wp, QMap<QString, int> style_tumbler, QString merge, QString restart){
    //INIT
    //QListIterator<QDomElement> cell(listText);
    QDomDocument doc;
    QDomElement wTc;
    QDomElement wtcW;
    QDomElement wtcPr;
    QDomElement wgridSpan;
    QDomElement wMerge;
    QDomElement wShd;
    //QDomElement wp;
    QDomElement wtcBorder;
    QDomElement wTop;
    QDomElement wBottom;
    QDomElement wLeft;
    QDomElement wRight;
    //Setting color
    QString fill;
    QString strRed;
    QString strGreen;
    QString strBlue;
    if(style_tumbler.value("RedBrushCell") == 0){
        strRed = "00";
    }else{
        strRed = QString("%1").arg(style_tumbler.value("RedBrushCell"), 0,16).toUpper();
    }
    if(style_tumbler.value("GreenBrushCell") == 0){
        strGreen = "00";
    }else{
        strGreen = QString("%1").arg(style_tumbler.value("GreenBrushCell"), 0,16).toUpper();
    }
    if(style_tumbler.value("BlueBrushCell") == 0){
        strBlue = "00";
    }else{
        strBlue = QString("%1").arg(style_tumbler.value("BlueBrushCell"), 0,16).toUpper();
    }
    fill= strRed + strGreen +strBlue;
    if( fill == "000000" || fill.isEmpty()){
        fill ="auto";
    }
    //CREATE AND ADD ATTRIBUTES
    wTc = doc.createElement("w:tc");

    wtcPr = doc.createElement("w:tcPr");
        wtcW = doc.createElement("w:tcW");
            wtcW.setAttribute("w:type","dxa");
            wtcW.setAttribute("w:w","9999"); //3213 354
            wgridSpan = doc.createElement("w:gridSpan");
                wgridSpan.setAttribute("w:val", merge);
            wMerge = doc.createElement("w:vMerge");
                wMerge.setAttribute("w:val", restart);

        wtcBorder= doc.createElement("w:tcBorders");
            wTop= doc.createElement("w:top");
                wTop.setAttribute("w:val", "single"); //nil интересный вариант
                //wTop.setAttribute("w:sz", 2); //-1
                wTop.setAttribute("w:sz", style_tumbler.value("topBorderSize"));
                wTop.setAttribute("w:space","0");
                wTop.setAttribute("w:color","000001");
            wBottom = doc.createElement("w:bottom");
                wBottom.setAttribute("w:val", "single");
                wBottom.setAttribute("w:sz", style_tumbler.value("bottomBorderSize"));
                //wBottom.setAttribute("w:sz", 2); //1
                wBottom.setAttribute("w:space","0");
                wBottom.setAttribute("w:color","000001");
            wLeft = doc.createElement("w:left");
                wLeft.setAttribute("w:val", "single");
                wLeft.setAttribute("w:sz", style_tumbler.value("leftBorderSize"));
                //wLeft.setAttribute("w:sz", 9); //2
                wLeft.setAttribute("w:space","0");
                wLeft.setAttribute("w:color","000001");
            wRight = doc.createElement("w:right");
                wRight.setAttribute("w:val", "single");
                wRight.setAttribute("w:sz", style_tumbler.value("rightBorderSize"));
                //wRight.setAttribute("w:sz", 12); //3
                wRight.setAttribute("w:space","0");
                wRight.setAttribute("w:color","000001");
        wShd = doc.createElement("w:shd");
        wShd.setAttribute("w:fill", fill); // or FF-FF-FF
        wShd.setAttribute("w:val","clear");



    //p
    //wp = cell.peekNext();

    //GLUE
    wTc.appendChild(wtcPr);
        wtcPr.appendChild(wtcW);
        wtcPr.appendChild(wtcBorder);
//        if(isMerge){
//        wtcPr.appendChild(wgridSpan);
//        wtcPr.appendChild(wMerge);}
            wtcBorder.appendChild(wTop);
            wtcBorder.appendChild(wLeft);
            wtcBorder.appendChild(wBottom);
            wtcBorder.appendChild(wRight);
        wtcPr.appendChild(wShd);
    wTc.appendChild(wp);

    return wTc;
}
