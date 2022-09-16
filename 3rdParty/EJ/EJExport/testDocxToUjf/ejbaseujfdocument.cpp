#include "ejbaseujfdocument.h"

EjBaseUjfDocument::EjBaseUjfDocument()
{

}

EjBaseUjfDocument::~EjBaseUjfDocument()
{
}

int EjBaseUjfDocument::getIndexRow()
{
    return currentIndexRow;
}

int EjBaseUjfDocument::getIndexCol()
{
    return currentIndexCol;
}

void EjBaseUjfDocument::setIndexCol(int value)
{
    this->currentIndexCol = value;
}

void EjBaseUjfDocument::setIndexRow(int value)
{
    this->currentIndexRow = value;
}

void EjBaseUjfDocument::controlSetDocument(EjDocument* doc)
{
    control.setDocument(doc);
    control.clear();
    control.setStatusMode(EDIT_TEXT);
    this->doc = doc;
}

void EjBaseUjfDocument::setBold()
{
    curTextStyle.setFontBold(true);
}

void EjBaseUjfDocument::setItalic()
{
    curTextStyle.setFontItalic(true);
}

void EjBaseUjfDocument::setUnderline()
{
    curTextStyle.setFontUnderline(true);
}

void EjBaseUjfDocument::setStrikeOut()
{
    curTextStyle.setFontStrikeOut(true);
}

void EjBaseUjfDocument::setSizeText(int size)
{
    curTextStyle.m_font.setPixelSize(size);
}

void EjBaseUjfDocument::setColorText(QColor color)
{
    curTextStyle.m_fontColor = color;
}

void EjBaseUjfDocument::setHAlign(QString align)
{
    curParagraph.setAlign(hAligns.value(align));
    control.setParagraphStyle(&curParagraph);
}

void EjBaseUjfDocument::setVAlign(QString align)
{
    curParagraph.m_align |= vAligns.value(align);
     control.setParagraphStyle(&curParagraph);
}

void EjBaseUjfDocument::setText(QString text)
{
    showCurrentStyles();
    control.setTextStyle(&curTextStyle);
    control.setText(text);
    curTextStyle.setDefault();
}


void EjBaseUjfDocument::createTable(int str, int cols)
{
    activeTable = true;
    curTable = new EjTableBlock(str, cols, doc, control.activeIndex);
}

void EjBaseUjfDocument::createCell()
{
    control.activeIndex = curTable->cellIndex(currentIndexRow, currentIndexCol, doc->lBlocks);
    curCell = (EjCellBlock*)doc->lBlocks->at(control.activeIndex);
}

void EjBaseUjfDocument::applyCellStyles()
{
    curTable->setCellStyle(curCell, &curCellStyle);
}

void EjBaseUjfDocument::indexRowIncr() {
    currentIndexRow++;
}


void EjBaseUjfDocument::resetIndexCol()
{
    currentIndexCol = -1;
}

void EjBaseUjfDocument::setWidthTopBorderCell(int w)
{
    curCellStyle.topBorder()->setWidth(w);
}

void EjBaseUjfDocument::setWidthLeftBorderCell(int w)
{
    curCellStyle.leftBorder()->setWidth(w);
}

void EjBaseUjfDocument::setWidthBottomBorderCell(int w)
{
    curCellStyle.bottomBorder()->setWidth(w);
}

void EjBaseUjfDocument::setWidthRightBorderCell(int w)
{
    curCellStyle.rightBorder()->setWidth(w);
}

void EjBaseUjfDocument::setFillCell(QColor color)
{
    curCellStyle.setBrushColor(color);
}

void EjBaseUjfDocument::merge(int row, int col)
{
    curCell->merge(row, col);
}

void EjBaseUjfDocument::showCurrentStyles()
{
    qDebug() << "Текущий цвет фона: " <<curTextStyle.m_brushColor;
    qDebug() << "Bold: " <<curTextStyle.m_font.bold();
    qDebug() << "Italic: " <<curTextStyle.m_font.italic();
    qDebug() << "Underline: " <<curTextStyle.m_font.underline();
    qDebug() << "Size text: " <<curTextStyle.m_font.pixelSize();
    qDebug() << "Color text: " <<curTextStyle.fontColor();
    qDebug() << "Align: " << curParagraph.align();
}

void EjBaseUjfDocument::temp()
{

        EjTextStyle textStyle;
        control.setParagraphStyle(&curParagraph);
        textStyle.m_font.setBold(true);
        textStyle.m_brushColor = QColor("red");
        textStyle.m_fontColor =  QColor("blue");

        control.setTextStyle(&textStyle);

        qDebug() << "ALIGN -- " << curParagraph.m_align;
        control.setText("RED BOLD TEXT");
        textStyle.setDefault();
        textStyle.m_font.setItalic(true);
        textStyle.m_brushColor = QColor("blue");
        textStyle.m_fontColor =  QColor("red");
        control.setTextStyle(&textStyle);
        control.setText("BLIE ITALIC TEXT");
        control.inputEnter();

         textStyle.setDefault();
         control.setTextStyle(&textStyle);
         control.setText("BLACK TEXT");
}

void EjBaseUjfDocument::clear()
{
    delete curTable;
    delete curCell;
}
