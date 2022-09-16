#ifndef EJDOCXTOUJFDOCUMENT_H
#define EJDOCXTOUJFDOCUMENT_H
//#include "textcontrol.h"
//#include "tablesplug.h"
#include "../ejbaseujfdocument.h"

class EjDocxToUjfDocument: public EjBaseUjfDocument
{

    QMap<int, EjTextStyle::e_fontStyle> headingStyle;
public:
	EjDocxToUjfDocument();
    void setHeadingStyle(int value);
    void setHighlight(QColor color);
    void newString();
    void setDefaultStyles();
    void setDefaultCellStyles();
    void setDefaultAlign();
    void applyTextStyles();
    void applyAlignToParagraph();
    void applyAlignToCell();
    void moveCursor();
    void indexColIncr();
    void resetIndexesTable();
    void setBrushColor(QString color);
    bool tableIsActive();
};

#endif // EJDOCXTOUJFDOCUMENT_H
