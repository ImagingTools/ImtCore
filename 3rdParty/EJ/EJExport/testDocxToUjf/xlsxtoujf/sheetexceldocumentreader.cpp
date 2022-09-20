#include "sheetexceldocumentreaders.h"

SheetExcelDocumentReader::~SheetExcelDocumentReader()
{

}

bool DimensionExcelDocumentReader::read(XlsxToUjfDocument &document, StylesExcel& styles, SharedStrings& strings, QDomNode node,
                                        QMap<QString, SheetExcelDocumentReader *> *editorsSheet){
    QString value = node.attributes().namedItem("ref").nodeValue();
    QString alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    document.startCoord = value.left(value.indexOf(':'));
    document.endCoord = value.right(value.size() - value.indexOf(':') - 1);
    int indexChar1 = alphabet.indexOf(value.at(0));
    int indexChar2 = alphabet.indexOf(value.at(value.indexOf(':') + 1));
    int indexDigit1 = value.mid(1, value.indexOf(':') - 1).toInt();
    int indexDigit2 = value.mid(value.indexOf(':') + 2).toInt();
    int row = indexDigit2 - indexDigit1 + 1;
    int col = indexChar2 - indexChar1 + 1;
    document.createTable(row, col);
    document.removeWidthCells(row, col);
    return true;
}

bool SheetDataExcelDocumentReader::read(XlsxToUjfDocument &document, StylesExcel& styles, SharedStrings& strings, QDomNode node,
                                        QMap<QString, SheetExcelDocumentReader *> *editorsSheet){
    bool result = true;
    node = node.firstChild();
      while (!node.isNull()) {
//         qDebug() << node.toElement().tagName();
         SheetExcelDocumentReader *editor = editorsSheet->value(node.toElement().tagName());
         if(editor != nullptr){
             result &= editor->read(document, styles, strings, node, editorsSheet);
         }
         node = node.nextSibling();
      }
    return result;
}

bool RowExcelDocumentReader::read(XlsxToUjfDocument &document, StylesExcel& styles, SharedStrings& strings, QDomNode node,
                                        QMap<QString, SheetExcelDocumentReader *> *editorsSheet){
    bool result = true;
    document.indexRowIncr();
    node = node.firstChild();
      while (!node.isNull()) {
//         qDebug() << node.toElement().tagName();
         SheetExcelDocumentReader *editor = editorsSheet->value(node.toElement().tagName());
         if(editor != nullptr){
             result &= editor->read(document, styles, strings, node, editorsSheet);
         }
         node = node.nextSibling();
      }
      document.resetIndexCol();
    return result;
}

bool CellExcelDocumentReader::read(XlsxToUjfDocument &document, StylesExcel& styles, SharedStrings& strings, QDomNode node,
                                        QMap<QString, SheetExcelDocumentReader *> *editorsSheet){

    QString nameCell = node.attributes().namedItem("r").nodeValue();
    int i, j;
    document.calcCoord(nameCell, i, j);
    document.setIndexRow(i);
    document.setIndexCol(j);
    document.createCell();
    int idStyle = node.attributes().namedItem("s").nodeValue().toInt();
    CellXfs xfs = styles.getCellXfsByIndex(idStyle);

    if(!xfs.getHAlign().isNull()){
        document.setHAlign(xfs.getHAlign());
    }

    if(!xfs.getVAlign().isNull()){
        document.setVAlign(xfs.getVAlign());
    }

    Font font = xfs.getFont();
    document.setSizeText(int(font.getSize() * 0.56));
    QColor color = font.getColor();
    QString str = font.getColor().name().mid(1);
    document.setColorText(color);


    if(font.isBold()){
        document.setBold();
    }

    if(font.isItalic()){
        document.setItalic();
    }

    if(font.isUnderline()){
        document.setUnderline();
    }

    if(font.isStrikeOut()){
        document.setStrikeOut();
    }

    Borders border = xfs.getBorders();
    document.setWidthBottomBorderCell(border.getBottom());
    document.setWidthTopBorderCell(border.getTop());
    document.setWidthLeftBorderCell(border.getLeft());
    document.setWidthRightBorderCell(border.getRight());
    document.applyCellStyles();

    QDomNode attrt = node.attributes().namedItem("t");
    QDomNode f = node.toElement().firstChildElement("f");
    if(!f.isNull()){
        QString formula = f.firstChild().nodeValue();
        document.setFormula(formula);
    }
    QDomNode v = node.toElement().firstChildElement("v");
    if (!v.isNull()){
        QString text = v.firstChild().nodeValue();
        if(!attrt.isNull()){
            int indexText = text.toInt();
            text = strings.getStringByIndex(indexText);
        }
        document.setText(text);
    }


    return true;
}

bool VExcelDocumentReader::read(XlsxToUjfDocument &document, StylesExcel& styles, SharedStrings& strings, QDomNode node,
                                        QMap<QString, SheetExcelDocumentReader *> *editorsSheet){

    return true;
}

bool MergeCellsExcelDocumentReader::read(XlsxToUjfDocument &document, StylesExcel& styles, SharedStrings& strings, QDomNode node,
                                        QMap<QString, SheetExcelDocumentReader *> *editorsSheet){

    int i, j, k, l;
    for(int index = 0; index < node.childNodes().size(); index++) {
       QDomNode mergeCell = node.childNodes().at(index);
       QString value = mergeCell.attributes().namedItem("ref").nodeValue();
       document.calcCoord(value.left(value.indexOf(':')), i, j);
       document.calcCoord(value.mid(value.indexOf(':') + 1), k, l);
       document.setCursorOnCell(i, j);
       int row = k - i + 1;
       int col = l - j + 1;
       document.merge(row, col);
    }
       document.setCursorOnCell(document.getIndexRow(), document.getIndexCol());
    return true;
}
