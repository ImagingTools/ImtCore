#include "ejsheetdocument.h"

#include <QDebug>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include<QStandardPaths>


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


EjSheetDocument::EjSheetDocument():
	streamData(QByteArray())
{
	documentWriter = new QXmlStreamWriter(&streamData);
	documentWriter->setAutoFormatting(true);
	documentWriter->setAutoFormattingIndent(1);
}


EjSheetDocument::~EjSheetDocument()
{
	delete documentWriter;

	while (sheetDataModel.size() > 0) {
		sheetDataModel.removeLast();
	}

	while (rowsModel.size() > 0) {
		rowsModel.removeLast();
	}

	while (colsModel.size() > 0) {
		colsModel.removeLast();
	}

	index_string = INDEX_STRING;
	index_column = INDEX_COLUMN;
	count_cells = 0;
	alphabet = "0ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	mergeCellsExist = false;
	colsExist = false;
	countColsCurrentTable = - 1;
	countStrCurrentTable = - 1;
	activeTable = false;
	maxIndexColForTable = 1;
}


bool EjSheetDocument::addTextNumber(int number, int numberStyle)
{
	/*Здесь будет передаваться number = -1,
       когда будет передана информация из клетки таблицы,
       в которой нет текста, но ему нужно добавить стили для бордеров*/
	QJsonObject row, rowAttributes, cModel, cAttributesModel;
	rowAttributes["r"] = QString::number(index_string);

	if(!activeTable){
		rowAttributes["customHeight"] = "1";
		rowAttributes["ht"] = "25";
	}

	if(activeTable && number == -1){
		rowAttributes["customHeight"] = "1";
		rowAttributes["ht"] = QString::number(heightCell);
	}

	cAttributesModel["r"] = getAlphabet().at(index_column) + QString::number(index_string);

	if(numberStyle != false){
		cAttributesModel["s"] = QString::number(numberStyle);
	}

	if(number != -1){
		cAttributesModel["t"] = "s";
		cModel["v"] = QString::number(number);
	}

	row["attributes"] = rowAttributes;
	cModel["attributes"] = cAttributesModel;
	row["c"] = cModel;
	rowsModel.append(row);

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
	QJsonObject row, rowAttributes, cModel, cAttributesModel;
	rowAttributes["customHeight"] = "1";
	rowAttributes["ht"] = QString::number(heightCell);
	rowAttributes["r"] = QString::number(index_string);
	cAttributesModel["r"] = getAlphabet().at(index_column - 1) + QString::number(index_string);
	cAttributesModel["s"] = QString::number(numberStyle);

	if(number != -1){
		cAttributesModel["t"] = "s";
		cModel["v"] = QString::number(number);
	}

	row["attributes"] = rowAttributes;
	cModel["attributes"] = cAttributesModel;
	row["c"] = cModel;
	rowsModel.append(row);

	return true;
}


void EjSheetDocument::addMergeCells(int rows, int cols)
{
	if(!mergeCellsExist){
		mergeCellsExist = true;
	}

	QJsonObject mergeCell;
	mergeCell["ref"] = getAlphabet().at(index_column) + QString::number(index_string) + ":" + getAlphabet().at(index_column + cols - 1) + QString::number(index_string + rows - 1);
	mergeCellsModel.append(mergeCell);
}


void EjSheetDocument::addWidthCell(float width)
{
	if(!colsExist){
		colsExist = true;
	}

	QJsonObject col;
	col["customWidth"] = "1";
	col["max"] = QString::number(index_column);
	col["min"] = QString::number(index_column);
	col["width"] = QString::number(width);

	colsModel.append(col);
}


void EjSheetDocument::addHeightCell(float height)
{
	heightCell = height;
}


bool EjSheetDocument::newString()
{
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


QByteArray EjSheetDocument::getDocumentData()
{
	documentWriter->writeStartElement("worksheet");
	documentWriter->writeAttribute("xmlns", "http://schemas.openxmlformats.org/spreadsheetml/2006/main");
	documentWriter->writeAttribute("xmlns:r", "http://schemas.openxmlformats.org/officeDocument/2006/relationships");

	if (colsExist){
		documentWriter->writeStartElement("cols");

		for (int i = 0; i < colsModel.count(); i++){
			documentWriter->writeStartElement("col");
			documentWriter->writeAttribute("customWidth", colsModel[i].toObject()["customWidth"].toString());
			documentWriter->writeAttribute("max", colsModel[i].toObject()["max"].toString());
			documentWriter->writeAttribute("min", colsModel[i].toObject()["min"].toString());
			documentWriter->writeAttribute("width", colsModel[i].toObject()["width"].toString());
			documentWriter->writeEndElement();
		}

		documentWriter->writeEndElement();
	}

	documentWriter->writeStartElement("sheetData");

	for (int i = 0; i < rowsModel.count(); i++){
		documentWriter->writeStartElement("row");

		if (rowsModel[i].toObject().contains("attributes")){
			QJsonObject rowAttributesModel = rowsModel[i].toObject()["attributes"].toObject();
			QList<QString> keys = rowAttributesModel.keys();
			keys.sort();

			for (QString key : keys){
				documentWriter->writeAttribute(key, rowAttributesModel[key].toString());
			}
		}

		if (rowsModel[i].toObject().contains("c")){
			documentWriter->writeStartElement("c");
			QJsonObject cModel = rowsModel[i].toObject()["c"].toObject();

			if (cModel.contains("attributes")){
				QJsonObject cAttributesModel = cModel["attributes"].toObject();
				QList<QString> keys = cAttributesModel.keys();
				keys.sort();

				for (QString key : keys){
					documentWriter->writeAttribute(key, cAttributesModel[key].toString());
				}
			}

			if (cModel.contains("v")){
				documentWriter->writeTextElement("v", cModel["v"].toString());
			}

			documentWriter->writeEndElement();
		}

		documentWriter->writeEndElement();
	}

	documentWriter->writeEndElement();

	if (mergeCellsExist){
		documentWriter->writeStartElement("mergeCells");

		for (int i = 0; i < mergeCellsModel.count(); i++){
			documentWriter->writeStartElement("mergeCell");
			documentWriter->writeAttribute("ref", mergeCellsModel[i].toObject()["ref"].toString());
			documentWriter->writeEndElement();
		}

		documentWriter->writeEndElement();
	}

	documentWriter->writeEndElement();
	return streamData;
}


QString EjSheetDocument::getAlphabet()
{
	return alphabet;
}

