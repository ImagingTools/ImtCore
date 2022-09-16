#ifndef EJBASEUJFDOCUMENT_H
#define EJBASEUJFDOCUMENT_H
#include "ejtextcontrol.h"
#include "ejtableblocks.h"
#include <QString>

class EjBaseUjfDocument
{
protected:
    EjDocument* doc;
    EjTableBlock *curTable;
    EjCellBlock* curCell;
    EjCellStyle curCellStyle;
    EjTextStyle curTextStyle;
    EjParagraphStyle curParagraph;
    EjTextControl control;
    int currentIndexRow = -1;
    int currentIndexCol = -1;
    bool activeTable = false;
    QMap<QString, int> hAligns;
    QMap<QString, int> vAligns;
public:
	EjBaseUjfDocument();
	~EjBaseUjfDocument();
    void controlSetDocument(EjDocument* doc);
    void setHAlign(QString align);
    void setVAlign(QString align);
    void setColorText(QColor color);

    int getIndexRow();
    int getIndexCol();
    void setIndexCol(int value);
    void setIndexRow(int value);
    void resetIndexCol();
    void indexRowIncr();

    void setBold();
    void setItalic();
    void setUnderline();
    void setStrikeOut();
    void setText(QString text);
    void setSizeText(int size);
    void createTable(int str, int cols);
    void createCell();
    void applyCellStyles();

    void setWidthTopBorderCell(int);
    void setWidthLeftBorderCell(int);
    void setWidthBottomBorderCell(int);
    void setWidthRightBorderCell(int);
    void setFillCell(QColor color);

    void merge(int row, int col);

    void showCurrentStyles();
    void temp();
    void clear();
};

#endif // EJBASEUJFDOCUMENT_H
