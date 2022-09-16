#ifndef TABLEBLOCKS_H
#define TABLEBLOCKS_H
#include <QDebug>
#include "ejtextcontrol.h"


class COMMONSHARED_EXPORT EjTableBlock : public EjGroupBlock
{
public:
    enum NumProp {
        TBL_KEY = 0,
        TBL_COLUMS,
        TBL_ROWS,
        TBL_VID,
        TBL_STYLE,
        TBL_NAME,
        TBL_COLUM_NAME,
        TBL_COLUM_MAX_WIDTH,
        TBL_COLUM_MIN_WIDTH,
        TBL_ADDITIONAL,
        TBL_PROP_COLUM,
        TBL_PROP_ROW,
        TBL_ACCESS = 100,
        TBL_COLUM_ACCESS,
        TBL_ROW_ACCESS,
        TBL_PROP_ACCESS,
        TBL_ADD_ROW_ACCESS,
        TBL_ADD_COLUM_ACCESS,
    };

	EjTableBlock();
	EjTableBlock(quint32 key_);
	EjTableBlock(int rows, int colums, EjDocument *doc, int index);
	virtual ~EjTableBlock() override;
    quint8 style; // 0,1 - small, price, qty, full 2 - checking 3 - numbering     // bits
    quint8 vid;  // 0 - shoplist, 1 - cleantable ...
    quint8 accuracy;
    int key;
    int start_column;
    int start_row;
    qint32 deltaProps;

    QColor borderColor;
    quint8 spacing;

    int m_rowStartSelect, m_columStartSelect, m_rowEndSelect, m_columEndSelect;
    int m_startSelectBlock, m_endSelectBlock;
    EjDocument *m_doc;


    qint32 startCell() { return m_index + deltaProps; }
    qint32 endBlock() { return m_index + m_counts; }

    enum VidProp {
        SHOP_LIST = 0,
        CLEAN_TABLE,
        LINKED_TABLE
    };

    struct ColumProp
    {
		EjSizeProp sizeProp;
    };
    QList<ColumProp*> lColums;
	QList<EjSizeProp*> lRows;
    int nRows() const { return lRows.count(); }
    int nColums() const { return lColums.count(); }
    int calc();
	EjBlock* makeCopy() override;
    void calcBlock(int &index, EjCalcParams *calcParams) override;
    bool isSelected(int &index, int &startSelect, int &endSelect) override;


	void cellParams(EjBlock *block, int &row, int &colum, QList<EjBlock *> *l_blocks = nullptr);
	void addString(EjTextControl *control, EjBlock *curBlock, bool force = true);
	void addColum(EjTextControl *control, EjBlock *curBlock);
	void delString(QList<EjBlock *> *l_blocks, int &active_block);
	void delColum(QList<EjBlock*> *l_blocks, int &active_block);
	void moveString(EjTextControl *control, EjBlock *curBlock, bool isUp);
	void moveColum(EjTextControl *control, EjBlock *curBlock, bool isLeft);

	int cellIndex(int row, int colum, QList<EjBlock *> *l_blocks);
    int cellIndex(int row, int colum);
    int prevCell(int index);
    int currCellIndex(int index);
	EjCellBlock *currentCell(int index);
	EjCellBlock *prevCell(EjBlock*block); // slow function!
    int nextCell(int index);
	EjCellBlock *nextCell(EjBlock*block); // slow function!
	bool chekMinMax(int &startRow, int &startColum, int &endRow, int &endColum, EjCellBlock *cell); // return true if startRow or startColum is changed
    void setCellStyles(int startRow,int startColum,int endRow,int endColum,EjCellStyle *style);
	void setCellStyle(EjCellBlock *cell,EjCellStyle *style);
	void clearCellStyle(EjCellBlock *cell);
    void setParagraphStyle(int startRow,int startColum,int endRow,int endColum,EjParagraphStyle *style);
	void setParagraphStyle(EjCellBlock *cell,EjParagraphStyle *style);
    EjCellStyle *getCellStyle(int block);
	QQuickItem *onCellClicked(int statusMode, EjTextControl *control, EjCellBlock *cell, QQuickItem *parent);
    QString tableName();
    void setTableName(QString name);
    QString tableAdditional();
    void setTableAdditional(QString additional);
    QString columName(int colum);
    void setColumName(QString name, int colum);
    int lastIndexProp();
	void getAccessColum(EjPropAccessBlock *source, int colum);
	void setAccessColum(EjPropAccessBlock *source,int colum);
    void setColumMaxWidth(quint16 width, int colum);
    void setColumMinWidth(quint16 width, int colum);
    bool containsMerginCells(int &startSelect, int &endSelect);
    bool containsMerginCells(int startRow,int startColum,int endRow,int endColum);
    void selectParams(int &startRow, int &startColum, int &endRow, int &endColum, int &startSelect, int &endSelect);


protected:
    EjCellStyle *fromStyles(EjCellStyle *newStyle);
	virtual void childCalc(EjBlock *child, EjCalcParams *calcParams) override;
	EjPropByteArrayBlock *getNameColumProp(int colum);
    int getColumPropIndex(int colum);
	EjPropBase *getPropFromMulty(int startIndex, int type, int vid);
};



class COMMONSHARED_EXPORT EjCellBlock : public EjTextBlock
{
public:
	EjCellBlock();
	virtual ~EjCellBlock() override;
    quint8 vid; // 0 - auto, 1 - text, 2 - number, 3 - formula, 4 - check, 4 - refbook
    quint16 txtHeight;
    double value;
    EjCellStyle *cellStyle;

    bool visible;
    quint8 mergeRows;
    quint8 mergeColums;

    enum Vid {
        CELL_AUTO = 0,
        CELL_TEXT,
        CELL_NUMBER,
        CELL_FORMULA,
        CELL_CHECK,
        CELL_REFBOOK,
        CELL_DATETIME,
//        ENDTABLE
    };

    enum NumProp {
        CELL_ADDITIONAL = 0,
        CELL_MERGE_COLUMS,
        CELL_MERGE_ROWS,
        CELL_NAME,
        CELL_VID,
        CELL_GUID_REF,
        CELL_GUID_DATA,
        CELL_MULTI_SELECT,
        CELL_HIDDEN_TEXT,
        CELL_ACCESS = 100,
    };

	EjBlock* makeCopy() override {
		EjCellBlock *res = new EjCellBlock();
        copyData(res);
        res->value = value;
        res->vid = vid;
        return res;
    }
	void copyCell(EjCellBlock *res)
    {
        res->value = value; res->vid = vid;
    }
    void setValue(double value);
    void setFormula(QString formula);
    QString formula();
    void setAdditional(QString additional);
    QString additional();
    void setHiddenText(QString hiddenText);
    QString hiddentext();
    void setTimeFormat(QString timeFormat);
    QString timeFormat();
    void setName(QString name);
    QString getName();
    void setVid(quint8 vidSource);
    void setGUIDRefBook(qint64 guidRef);
    qint64 getGUIDRefBook();
    void setIsMultiSelect(bool isMultiSelect);
    bool getIsMultiSelect();
    void setGUIDRefData(QList<qint64> lGUIDData);
    QList<qint64> getGUIDRefData();
    void getCurrentCellStyle(EjCellStyle *style);
	void setAccessBlock(EjPropAccessBlock* source);
	void getAccessBlock(EjPropAccessBlock* source);

	EjTableBlock *getTable();

	void addProp(EjPropBase *prop);
	EjPropBase *findProp(int num);
    void removeProp(int num, bool isAll = false);
    void clearAll(EjDocument *doc);
    void clearData(EjDocument *doc, bool isAll = false);

    bool isSelected(int &index, int &startSelect, int &endSelect) override;
    void setText(const QString &source, EjTextControl *control = nullptr);
    QString getText();
    void setTextStyle(EjTextStyle *style, EjTextControl *control);
    void merge(int rows, int colums);
    void unMerge();
};


#endif // TABLEBLOCKS_H
