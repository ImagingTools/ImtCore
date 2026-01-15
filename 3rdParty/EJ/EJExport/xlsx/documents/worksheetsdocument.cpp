#include "worksheetsdocument.h"

#include <QDebug>
#include <string>

WorksheetsDocument::WorksheetsDocument() {
    clear();
}

bool WorksheetsDocument::addTextNumber(int number, bool existText, bool acceptStyle) {
    QDomElement row = document.createElement("row");
    sheet_data.appendChild(row);
    row.setAttribute("r", QString::number(index_string));
    QDomElement c = document.createElement("c");
    row.appendChild(c);

    if(acceptStyle){
        c.setAttribute("s", number_style);
    }
    c.setAttribute("t", "s");
    if(table_is_started){
        c.setAttribute("r", getAlphabet().at(index_column) + QString::number(index_string));
    }else{
        c.setAttribute("r", getAlphabet().at(INDEX_COLUMN) + QString::number(index_string));
    }
    if(existText){
        QDomElement v = document.createElement("v");
        c.appendChild(v);
        v.appendChild(document.createTextNode(QString::number(number)));
    }
    return true;
}

void WorksheetsDocument::addMergeCells(int rows, int cols){
    if(!mergeCellsExist){
        mergeCells = document.createElement("mergeCells");
        worksheet.appendChild(mergeCells);
        mergeCellsExist = true;
    }
    QDomElement mergeCell = document.createElement("mergeCell");
    mergeCells.appendChild(mergeCell);
    mergeCell.setAttribute("ref", getAlphabet().at(index_column) + QString::number(index_string) + ":" +
                           getAlphabet().at(index_column + cols - 1) + QString::number(index_string + rows - 1));
    count_mergeCells++;
    mergeCells.setAttribute("count", count_mergeCells);
}

void WorksheetsDocument::addWidthCell(float width){
    if(!colsExist){
        cols = document.createElement("cols");
        worksheet.appendChild(cols);
        worksheet.insertBefore(cols, sheet_data);
        colsExist = true;
    }
    QDomElement col = document.createElement("col");
    cols.appendChild(col);
    col.setAttribute("customWidth", 1);
    col.setAttribute("width", width);
    col.setAttribute("max", index_column);
    col.setAttribute("min", index_column);
}

bool WorksheetsDocument::newString() {
    return true;
}

bool WorksheetsDocument::finish() {
    return true;
}

void WorksheetsDocument::clear() {
    document.clear();
    worksheet = document.createElement("worksheet");
    document.appendChild(worksheet);
    worksheet.setAttribute("xmlns", "http://schemas.openxmlformats.org/spreadsheetml/2006/main");
    sheet_data = document.createElement("sheetData");
    worksheet.appendChild(sheet_data);
}

const QDomDocument &WorksheetsDocument::getDocument() const {
    return document;
}

QString WorksheetsDocument::getAlphabet(){
    return alphabet;
}

int WorksheetsDocument::getIndexColumn(){
    return index_column;
}

void WorksheetsDocument::setIndexColumn(int idx){
    this->index_column = idx;
}

int WorksheetsDocument::getIndexString(){
    return index_string;
}

void WorksheetsDocument::setIndexString(int idx){
    this->index_string = idx;
}

int WorksheetsDocument::getCountCells(){
    return count_cells;
}

void WorksheetsDocument::setCountCells(int idx){
    this->count_cells = idx;
}


