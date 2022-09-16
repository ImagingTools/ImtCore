#include "stylesexceldocumentreaders.h"
#include <QDebug>

StylesExcelDocumentReader::~StylesExcelDocumentReader()
{

}

bool FontsStyleDocumentReader::read(StylesExcel& document, QDomNode node)
{
    QDomNodeList listFonts = node.childNodes();
    for(int i = 0; i < listFonts.size(); i++){
        QDomNode font = listFonts.at(i);
        QDomNodeList childs =  font.childNodes();
        Font m_font;
        for(int j = 0; j < childs.size(); j++){
            QDomNode childsNode = childs.at(j);
            qDebug() << childsNode.nodeName();
            if (childsNode.nodeName() == "sz") {
                m_font.setSize(childsNode.attributes().namedItem("val").nodeValue().toInt());
            } else if(childsNode.nodeName() == "color"){
                QString rgb = childsNode.attributes().namedItem("rgb").nodeValue();
                if(!rgb.isNull()){
                    m_font.setColor(QColor("#" + rgb.right(6)));
                }
            } else if(childsNode.nodeName() == "b") {
                m_font.setBold();
            } else if(childsNode.nodeName() == "i"){
                m_font.setItalic();
            } else if(childsNode.nodeName() == "u"){
                m_font.setUnderline();
            } else if(childsNode.nodeName() == "s"){
                m_font.setStrikeOut();
            }
        }
        document.addFont(m_font);
    }

    return true;
}

bool FillsStyleDocumentReader::read(StylesExcel& document, QDomNode node)
{
//    QDomNodeList listFills = node.childNodes();
//    for(int i = 0; i < listFills.size(); i++){
//        QDomNode fill = listFills.at(i);
//        QDomNodeList childs =  fill.childNodes();

//        document.addFill(fill);
//    }

    return true;
}

bool BordersStyleDocumentReader::read(StylesExcel& document, QDomNode node)
{
    QDomNodeList listBorders = node.childNodes();

    for(int i = 0; i < listBorders.size(); i++){
        QDomNode border = listBorders.at(i);
        QDomNodeList childs =  border.childNodes();
        Borders m_border;
        for(int j = 0; j < childs.size(); j++){
            QDomNode childsNode = childs.at(j);
            qDebug() << childsNode.nodeName();
            int width = document.getWidthByString(childsNode.attributes().namedItem("style").nodeValue());
            if(childsNode.nodeName() == "left") {
                m_border.setLeft(width);
            } else if(childsNode.nodeName() == "right"){
                m_border.setRight(width);
            } else if(childsNode.nodeName() == "top"){
                m_border.setTop(width);
            } else if(childsNode.nodeName() == "bottom"){
                m_border.setBottom(width);
            }
        }
        document.addBorder(m_border);
    }
    return true;
}

bool CellXfsStyleDocumentReader::read(StylesExcel& document, QDomNode node)
{
    QDomNodeList listCellXfs = node.childNodes();
    for(int i = 0; i < listCellXfs.size(); i++){
        QDomNode cellXfsNode = listCellXfs.at(i);
        CellXfs cellXfs;
        for(int j = 0; j < cellXfsNode.attributes().count(); j++){
            QDomNode attr = cellXfsNode.attributes().item(j);
            int id = attr.nodeValue().toInt();
            if(attr.nodeName() == "fontId"){
               cellXfs.setFont(document.getFontByIndex(id));
            }else if(attr.nodeName() == "fillId"){
               // cellXfs.setFill(document.getFillByIndex(0));
            }else if(attr.nodeName() == "borderId"){
                cellXfs.setBorder(document.getBorderByIndex(id));
            }
            QDomNode align = cellXfsNode.firstChildElement("alignment");
            if(!align.isNull()){
                QDomNode horizont = align.attributes().namedItem("horizontal");
                QDomNode vertical = align.attributes().namedItem("vertical");
                if(!horizont.isNull()){
                    cellXfs.setHAlign(horizont.nodeValue());
                }
                if(!vertical.isNull()){
                    cellXfs.setVAlign(vertical.nodeValue());
                }
            }
        }
        document.addCellXfs(cellXfs);
    }
    return true;
}
