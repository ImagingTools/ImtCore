#ifndef WORKSHEETS_DOCUMENT_H
#define WORKSHEETS_DOCUMENT_H

#include <QDomDocument>
const int INDEX_STRING = 3;
const int INDEX_COLUMN = 2;

class WorksheetsDocument {

    QDomDocument document;
    QDomElement sheet_data;
    QDomElement mergeCells;
    QDomElement worksheet;
    QDomElement cols;
    int count_mergeCells = 0;
    bool mergeCellsExist = false;
    bool colsExist = false;
    int index_string = INDEX_STRING;
    int index_column = INDEX_COLUMN;
    int count_cells = 0;
    QString alphabet = "0ABCDEFGHIJKLMNOPQ";
    bool table_is_started = false;
//    bool styleTextIsWaiting = false;
   // int number_font;
    int number_style;
    int count_cols_in_table;


public:
    WorksheetsDocument();
//    bool emptyCell;
    bool tableExist = false;
    bool addTextNumber(int number, bool val, bool acceptStyle);
    bool newString();

    bool finish();

    void clear();

    QString getAlphabet();

    const QDomDocument& getDocument() const;
    bool addTextNumberTable(int number,int current_str, int current_col);
     void addMergeCells(int rows, int cols);
     void addWidthCell(float width);
    int getIndexColumn();
    void setIndexColumn(int idx);
    int getIndexString();
    void setIndexString(int idx);
    int getCountCells();
    void setCountCells(int idx);
    bool getTableIsStarted(){ return table_is_started; }
    void setTableIsStarted(bool val){ table_is_started = val; }
//    bool styleExistForText(){ return styleTextIsWaiting; }
//    void setStyleExistForText(bool val){ styleTextIsWaiting = val; }
    void setNumberStyle(int val){ number_style = val; }
    int getCountColsInTable(){ return count_cols_in_table; }
    void setCountColsInTable(int val){ count_cols_in_table = val; }

};

#endif // WORKSHEETS_DOCUMENT_H
