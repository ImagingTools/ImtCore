#include "xlsxtoujfdocument.h"

XlsxToUjfDocument::XlsxToUjfDocument()
{

}

const QString XlsxToUjfDocument::alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

void XlsxToUjfDocument::calcCoord(const QString name, int &i, int &j)
{
    int startIndexCol = alphabet.indexOf(startCoord.at(0));
    int startIndexRow = startCoord.mid(1).toInt();

    int curIndexCol = alphabet.indexOf(name.at(0));
    int curIndexRow = name.mid(1).toInt();

    i = curIndexRow - startIndexRow;
    j = curIndexCol - startIndexCol;
  //  curCell->setFormula();

}

void XlsxToUjfDocument::removeWidthCells(int row, int col)
{
    curCellStyle.topBorder()->setWidth(0);
    curCellStyle.rightBorder()->setWidth(0);
    curCellStyle.bottomBorder()->setWidth(0);
    curCellStyle.leftBorder()->setWidth(0);
    curTable->setCellStyles(0, 0, row - 1, col - 1, &curCellStyle);
}

void XlsxToUjfDocument::setCursorOnCell(int i, int j)
{
    control.activeIndex = curTable->cellIndex(i, j, doc->lBlocks);
    curCell = (EjCellBlock*)doc->lBlocks->at(control.activeIndex);
}

void XlsxToUjfDocument::setFormula(const QString value)
{
    curCell->setFormula(value);
}
