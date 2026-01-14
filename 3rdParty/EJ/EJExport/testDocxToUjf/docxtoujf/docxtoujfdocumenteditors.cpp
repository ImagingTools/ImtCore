#include "docxtoujfdocumenteditors.h"

WordDocumentReader::~WordDocumentReader()
{

}

bool ParagraphDocumentReader::edit(EjDocxToUjfDocument &document, QDomNode node, QMap<QString, WordDocumentReader*> *editors)
{
    bool result = true;
    node = node.firstChild();
    while (!node.isNull())
    {
//       qDebug() << node.toElement().tagName();
       WordDocumentReader *editor = editors->value(node.toElement().tagName());
       if(editor != nullptr)
       {
           result &= editor->edit(document, node, editors);
       }
       node = node.nextSibling();
    }

    document.setDefaultAlign();
    if(!document.tableIsActive())
    {
        document.newString();
    }

    return result;
}

bool TableDocumentReader::edit(EjDocxToUjfDocument &document, QDomNode node, QMap<QString, WordDocumentReader*> *editors)
{
    bool result = true;
    int rows = node.toElement().elementsByTagName("w:tr").size();
    int cols = node.toElement().firstChildElement("w:tblGrid").elementsByTagName("w:gridCol").size();
    document.createTable(rows, cols);
    node = node.firstChild();
    while (!node.isNull())
    {
//       qDebug() << node.toElement().tagName();
       WordDocumentReader *editor = editors->value(node.toElement().tagName());
       if(editor != nullptr)
       {
           result &= editor->edit(document, node, editors);
       }
       node = node.nextSibling();
    }
    document.resetIndexesTable();
    document.moveCursor();
    document.newString();
    return result;
}

bool pPrDocumentReader::edit(EjDocxToUjfDocument &document, QDomNode node, QMap<QString, WordDocumentReader*> *editors)
{
    QDomNode jc = node.firstChildElement("w:jc");
    if(!jc.isNull())
    {
        QDomNode attr = jc.attributes().item(0);
        QString align = attr.nodeValue();
        document.setHAlign(align);
    }else
    {
        document.setHAlign("left");
    }

    QDomNode pStyle = node.firstChildElement("w:pStyle ");
    if(!pStyle.isNull())
    {
        QDomNode attr = pStyle.attributes().namedItem("w:val");
        int value = attr.nodeValue().toInt();
        document.setHeadingStyle(value);
    }
    return true;
}

bool tblGridDocumentReader::edit(EjDocxToUjfDocument &document, QDomNode node, QMap<QString, WordDocumentReader*> *editors)
{
    return true;
}

bool TableRowDocumentReader::edit(EjDocxToUjfDocument &document, QDomNode node, QMap<QString, WordDocumentReader*> *editors)
{
    bool result = true;
    document.indexRowIncr();
    node = node.firstChild();
    while (!node.isNull())
    {
//       qDebug() << node.toElement().tagName();
       WordDocumentReader *editor = editors->value(node.toElement().tagName());
       if(editor != nullptr){
           result &= editor->edit(document, node, editors);
       }
       node = node.nextSibling();
    }
    document.resetIndexCol();
    return result;
}

bool trPrDocumentReader::edit(EjDocxToUjfDocument &document, QDomNode node, QMap<QString, WordDocumentReader*> *editors)
{
    return true;
}

bool TableCellDocumentReader::edit(EjDocxToUjfDocument &document, QDomNode node, QMap<QString, WordDocumentReader*> *editors)
{
    bool result = true;
    document.indexColIncr();
    document.createCell();
    node = node.firstChild();
    while (!node.isNull())
    {
//       qDebug() << node.toElement().tagName();
       WordDocumentReader *editor = editors->value(node.toElement().tagName());
       if(editor != nullptr)
       {
           result &= editor->edit(document, node, editors);
       }
       node = node.nextSibling();
    }
    return result;
}

bool tcPrDocumentReader::edit(EjDocxToUjfDocument &document, QDomNode node, QMap<QString, WordDocumentReader*> *editors)
{
    document.setDefaultCellStyles();
    QDomElement borders = node.firstChildElement("w:tcBorders");
    if(!borders.isNull())
    {
        QDomElement top = borders.firstChildElement("w:top");
        if(!top.isNull())
        {
            QDomNode attr = top.attributes().namedItem("w:sz");
            int sz = attr.nodeValue().toInt();
            document.setWidthTopBorderCell(sz * 25);
        }
        QDomElement left = borders.firstChildElement("w:left");
        if(!left.isNull())
        {
            QDomNode attr = left.attributes().namedItem("w:sz");
            int sz = attr.nodeValue().toInt();
            document.setWidthLeftBorderCell(sz * 25);
        }
        QDomElement bottom = borders.firstChildElement("w:bottom");
        if(!bottom.isNull())
        {
            QDomNode attr = bottom.attributes().namedItem("w:sz");
            int sz = attr.nodeValue().toInt();
            document.setWidthBottomBorderCell(sz * 25);
        }
        QDomElement right = borders.firstChildElement("w:right");
        if(!right.isNull())
        {
            QDomNode attr = right.attributes().namedItem("w:sz");
            int sz = attr.nodeValue().toInt();
            document.setWidthRightBorderCell(sz * 25);
        }
    }

    QDomElement vAlign = node.firstChildElement("w:vAlign");
    if(!vAlign.isNull())
    {
        QDomNode attr = vAlign.attributes().namedItem("w:val");
        QString color = attr.nodeValue();
        document.setVAlign("#" + color);
        document.applyAlignToCell();
    }

    QDomElement shd = node.firstChildElement("w:shd");
    if(!shd.isNull())
    {
        QDomNode attr = shd.attributes().namedItem("w:fill");
        QString color = attr.nodeValue();
        if(color != "auto"){
            document.setFillCell(QColor("#" + color));
        }
    }


    QDomElement grid = node.firstChildElement("w:gridSpan");
    int row = 1, col = 1;
    if(!grid.isNull())
    {
        QDomNode attr = grid.attributes().namedItem("w:val");
        int value = attr.nodeValue().toInt();
        document.setIndexCol(document.getIndexCol() + (value - 1));
        col = value;
    }


    QDomElement vmerge = node.firstChildElement("w:vMerge");
    if(vmerge.isNull() && !grid.isNull())
    {
        document.merge(row, col);
    }

    if(!vmerge.isNull() && vmerge.attributes().count() > 0)
    {
        QDomElement currentTc = node.parentNode().toElement();
        QDomElement tr = currentTc.parentNode().toElement();
        QDomNodeList tcList = tr.elementsByTagName("w:tc");
        int totalWidth = 0;
        for(int i = 0; tcList.at(i).toElement() != currentTc; i++)
        {
            QDomElement tc = tcList.at(i).toElement();
            totalWidth += tc.firstChildElement("w:tcPr").
                    firstChildElement("w:tcW").attributes().namedItem("w:w").nodeValue().toInt();
        }
        QDomElement tbl = tr.parentNode().toElement();
        QDomNodeList trList = tbl.elementsByTagName("w:tr");

        for(int i = document.getIndexRow() + 1; i < trList.size(); i++)
        {
            QDomElement trFromTable = trList.at(i).toElement();
            QDomNodeList tcListFromTr = trFromTable.elementsByTagName("w:tc");
            int width = 0;
            QDomElement tcFromTr;
            for(int i = 0; width <= totalWidth; i++)
            {
                tcFromTr = tcListFromTr.at(i).toElement();
                width += tcFromTr.firstChildElement("w:tcPr").
                                    firstChildElement("w:tcW").attributes().namedItem("w:w").nodeValue().toInt();
            }
            QDomElement vmergeFromTc = tcFromTr.firstChildElement("w:tcPr").firstChildElement("w:vMerge");
            if(!vmergeFromTc.isNull() && vmergeFromTc.attributes().count() == 0)
            {
                row++;
            }else{
                break;
            }
        }
//        qDebug() << "Коорд: " <<  document.getIndexRow() << " " <<  document.getIndexCol();
//        qDebug() << "Merge: " << row << " " << col;
        document.merge(row, col);
    }
    document.applyCellStyles();
    return true;
}

bool RDocumentReader::edit(EjDocxToUjfDocument &document, QDomNode node, QMap<QString, WordDocumentReader*> *editors)
{
    bool result = true;
    node = node.firstChild();
    while (!node.isNull())
    {
//       qDebug() << node.toElement().tagName();
       WordDocumentReader *editor = editors->value(node.toElement().tagName());
       if(editor != nullptr){
           result &= editor->edit(document, node, editors);
       }
       node = node.nextSibling();
    }
    return result;
}

bool TextDocumentReader::edit(EjDocxToUjfDocument &document, QDomNode node, QMap<QString, WordDocumentReader*> *editors)
{
    QString text = node.firstChild().nodeValue();
//    qDebug() << text;
    document.setText(text);
    return true;
}

bool tblPrDocumentReader::edit(EjDocxToUjfDocument &document, QDomNode node, QMap<QString, WordDocumentReader*> *editors)
{
    return true;
}

bool rPrDocumentReader::edit(EjDocxToUjfDocument &document, QDomNode node, QMap<QString, WordDocumentReader *> *editors)
{

    QDomNode color = node.firstChildElement("w:color");
    if(!color.isNull())
    {
        QDomNode attr = color.attributes().namedItem("w:val");
        QString clr = attr.nodeValue();
        document.setColorText(QColor("#" + clr));
    }

    QDomNode sz = node.firstChildElement("w:sz");
    if(!sz.isNull())
    {
        QDomNode attr = sz.attributes().namedItem("w:val");
        int size = attr.nodeValue().toInt();
        document.setSizeText(int(size * 0.56));
    }

    QDomNode b = node.firstChildElement("w:b");
    if(!b.isNull())
    {
        document.setBold();
    }

    QDomNode i = node.firstChildElement("w:i");
    if(!i.isNull())
    {
        document.setItalic();
    }

    QDomNode u = node.firstChildElement("w:u");
    if(!u.isNull())
    {
        document.setUnderline();
    }

    QDomNode hl = node.firstChildElement("w:highlight");
    if(!hl.isNull())
    {
        QDomNode attr = hl.attributes().namedItem("w:val");
        QString clr = attr.nodeValue();
        document.setHighlight(QColor(clr));
    }
    document.applyTextStyles();
    return true;
}

bool TabDocumentReader::edit(EjDocxToUjfDocument &document, QDomNode node, QMap<QString, WordDocumentReader *> *editors)
{
    document.setText("  ");
    return true;
}

