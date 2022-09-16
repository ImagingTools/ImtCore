#include "ejsheetdocument.h"

#include <QDebug>

void EjSheetDocument::setCountColsCurrentTable(int value)
{
    countColsCurrentTable = value;
}

void EjSheetDocument::setCountStrCurrentTable(int value)
{
    countStrCurrentTable = value;
}

void EjSheetDocument::setActiveTable(bool value)
{
    activeTable = value;
}

bool EjSheetDocument::getActiveTable() const
{
    return activeTable;
}

int EjSheetDocument::getIndexString() const
{
    return index_string;
}

int EjSheetDocument::getIndexColumn() const
{
    return index_column;
}

void EjSheetDocument::setMaxIndexColForTable(int value)
{
    maxIndexColForTable = value;
}

int EjSheetDocument::getFillId() const
{
    return fillId;
}

void EjSheetDocument::setFillId(int value)
{
    fillId = value;
}

int EjSheetDocument::getBorderId() const
{
    return borderId;
}

void EjSheetDocument::setBorderId(int value)
{
    borderId = value;
}

EjSheetDocument::EjSheetDocument() {
    worksheet = document.createElement("worksheet");
    worksheet.setAttribute("xmlns", "http://schemas.openxmlformats.org/spreadsheetml/2006/main");
    worksheet.setAttribute("xmlns:r", "http://schemas.openxmlformats.org/officeDocument/2006/relationships");
    sheetData = document.createElement("sheetData");
    document.appendChild(worksheet);
    worksheet.appendChild(sheetData);
}


bool EjSheetDocument::addTextNumber(int number, int numberStyle) {
    /*Здесь будет передаваться number = -1,
       когда будет передана информация из клетки таблицы,
       в которой нет текста, но ему нужно добавить стили для бордеров*/
    QDomElement row = document.createElement("row");
    sheetData.appendChild(row);
    row.setAttribute("r", QString::number(index_string));
    QDomElement c = document.createElement("c");
    row.appendChild(c);
    if(!activeTable){
        row.setAttribute("customHeight", "1");
        row.setAttribute("ht", 25);
    }
    if(activeTable && number == -1){
        row.setAttribute("customHeight", "1");
        row.setAttribute("ht", heightCell);
    }

    if(numberStyle != false){
        c.setAttribute("s", numberStyle);
    }

    c.setAttribute("r", getAlphabet().at(index_column) + QString::number(index_string));

    if(number != -1){
        c.setAttribute("t", "s");
        QDomElement v = document.createElement("v");
        c.appendChild(v);
        v.appendChild(document.createTextNode(QString::number(number)));
    }

    /*Если добавляется текст который находится вне таблицы,
      то делаем объединение клеток до максимального индекса
      столбца среди всех таблиц в документе*/
    if(!activeTable){
        /*Если в документе нет таблицы то сделаем, какую то стандартную ширину для клетки*/
        if(maxIndexColForTable == -1){
            addWidthCell(150);
        }else{
            addMergeCells(1, maxIndexColForTable);
        }
    }

    return true;
}

bool EjSheetDocument::addTextNumberIntoTable(int number, int numberStyle)
{
    QDomElement row = document.createElement("row");
    sheetData.appendChild(row);
    row.setAttribute("r", QString::number(index_string));
    QDomElement c = document.createElement("c");
    row.appendChild(c);
    row.setAttribute("customHeight", "1");
    row.setAttribute("ht", heightCell);
    c.setAttribute("s", numberStyle);
    c.setAttribute("r", getAlphabet().at(index_column - 1) + QString::number(index_string));

    if(number != -1){
        c.setAttribute("t", "s");
        QDomElement v = document.createElement("v");
        c.appendChild(v);
        v.appendChild(document.createTextNode(QString::number(number)));
    }
    return true;
}

void EjSheetDocument::addMergeCells(int rows, int cols){
    if(!mergeCellsExist){
        mergeCells = document.createElement("mergeCells");
        worksheet.appendChild(mergeCells);
        mergeCellsExist = true;
    }
    QDomElement mergeCell = document.createElement("mergeCell");
    mergeCells.appendChild(mergeCell);
    mergeCell.setAttribute("ref", getAlphabet().at(index_column) +
                           QString::number(index_string) + ":" +
                           getAlphabet().at(index_column + cols - 1) +
                           QString::number(index_string + rows - 1));
}

void EjSheetDocument::addWidthCell(float width){
    if(!colsExist){
        cols = document.createElement("cols");
        worksheet.appendChild(cols);
        worksheet.insertBefore(cols, sheetData);
        colsExist = true;
    }
    QDomElement col = document.createElement("col");
    cols.appendChild(col);
    col.setAttribute("customWidth", 1);
    col.setAttribute("width", width);
    col.setAttribute("max", index_column);
    col.setAttribute("min", index_column);
}

void EjSheetDocument::addHeightCell(float height)
{
    /*qDebug() << "Высота  -  " << height;
    QDomNodeList list = document.elementsByTagName("row");
    qDebug() << "Размер списка   -  " << list.size();
     QDomNode row = list.at(list.size() - 1);
     QDomElement rowElem = row.toElement();
     rowElem.setAttribute("customHeight", "1");
     rowElem.setAttribute("ht", height);*/
    heightCell = height;
}

bool EjSheetDocument::newString() {
    index_string++;
    return true;
}

bool EjSheetDocument::newColumn()
{
    index_column++;
    return true;
}

void EjSheetDocument::setDefaultIndexColumn()
{
    index_column = INDEX_COLUMN;
}

void EjSheetDocument::validate()
{
    if(index_column >= countColsCurrentTable + INDEX_COLUMN){
        index_column = INDEX_COLUMN;
        newString();
    }
}

const QDomDocument &EjSheetDocument::getDocument() const {
    return document;
}

QString EjSheetDocument::getAlphabet(){
    return alphabet;
}


