#include "testDocxToUjf/docxtoujf/ejdocxtoujfdocument.h"


EjDocxToUjfDocument::EjDocxToUjfDocument()
{
    hAligns.insert("left", EjParagraphStyle::AlignLeft);
    hAligns.insert("center", EjParagraphStyle::AlignHCenter);
    hAligns.insert("right", EjParagraphStyle::AlignRight);
    vAligns.insert("top", EjParagraphStyle::AlignTop);
    vAligns.insert("center", EjParagraphStyle::AlignVCenter);
    vAligns.insert("bottom", EjParagraphStyle::AlignBottom);
    headingStyle.insert(1, EjTextStyle::HEADING1);
    headingStyle.insert(2, EjTextStyle::HEADING2);
    headingStyle.insert(3, EjTextStyle::HEADING3);
    headingStyle.insert(4, EjTextStyle::HEADING4);
    headingStyle.insert(5, EjTextStyle::HEADING5);
}

void EjDocxToUjfDocument::setHighlight(QColor color)
{
    curTextStyle.setBrushColor(color);
}

void EjDocxToUjfDocument::setHeadingStyle(int value)
{
    curTextStyle.m_fontStyle = headingStyle.value(value);
}

void EjDocxToUjfDocument::newString()
{
    control.inputEnter();
}

void EjDocxToUjfDocument::setBrushColor(/*const ?*/QString color)
{
    curTextStyle.setBrushColor(QColor(color));
}

void EjDocxToUjfDocument::setDefaultStyles()
{
    curTextStyle.setFontColor("#000000");
    curTextStyle.setBrushColor("#FFFFFF");
    curTextStyle.setFontBold(false);
    curTextStyle.setFontItalic(false);
    curTextStyle.setFontUnderline(false);
    curTextStyle.setFontSize(11);
}

void EjDocxToUjfDocument::setDefaultAlign()
{
    curParagraph.setAlign(EjParagraphStyle::AlignLeft);
}

void EjDocxToUjfDocument::applyTextStyles()
{
}


void EjDocxToUjfDocument::applyAlignToParagraph()
{
    control.setParagraphStyle(&curParagraph);
}

void EjDocxToUjfDocument::applyAlignToCell()
{
     curTable->setParagraphStyle(curCell, &curParagraph);
}


void EjDocxToUjfDocument::moveCursor()
{
    control.cursorLast();
}

void EjDocxToUjfDocument::indexColIncr()
{
    currentIndexCol++;
}

void EjDocxToUjfDocument::resetIndexesTable()
{
    currentIndexCol = -1;
    currentIndexRow = -1;
    activeTable = false;
}

void EjDocxToUjfDocument::setDefaultCellStyles()
{
   curCellStyle.m_brushColor = QColor("white");
   curCellStyle.topBorder()->setWidth(100);
   curCellStyle.leftBorder()->setWidth(100);
   curCellStyle.bottomBorder()->setWidth(100);
   curCellStyle.rightBorder()->setWidth(100);
}

bool EjDocxToUjfDocument::tableIsActive()
{
    return activeTable;
}
