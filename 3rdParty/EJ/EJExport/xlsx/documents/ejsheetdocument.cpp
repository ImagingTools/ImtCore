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

	colsModel.clear();


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
	QJsonObject row, rowAttributes, cItem, cAttributesModel;
	QJsonArray cModel;
	if (rowsModel.last().toObject()["attributes"].toObject()["r"].toString() == QString::number(index_string)){
		row = rowsModel.last().toObject();
		rowAttributes = rowsModel.last().toObject()["attributes"].toObject();
		cModel = rowsModel.last().toObject()["columns"].toArray();
	}
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
		cItem["v"] = QString::number(number);
	}

	row["attributes"] = rowAttributes;
	cItem["attributes"] = cAttributesModel;
	cModel.append(cItem);

	row["columns"] = cModel;
	if (rowsModel.last().toObject()["attributes"].toObject()["r"].toString() == QString::number(index_string)){
		rowsModel.replace(rowsModel.count()-1, row);
	}
	else{
		rowsModel.append(row);
	}


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
	QJsonObject row, rowAttributes, cItem, cAttributesModel;
	QJsonArray cModel;
	if (rowsModel.last().toObject()["attributes"].toObject()["r"].toString() == QString::number(index_string)){
		row = rowsModel.last().toObject();
		rowAttributes = rowsModel.last().toObject()["attributes"].toObject();
		cModel = rowsModel.last().toObject()["columns"].toArray();
	}
	rowAttributes["customHeight"] = "1";
	rowAttributes["ht"] = QString::number(heightCell);
	rowAttributes["r"] = QString::number(index_string);
	cAttributesModel["r"] = getAlphabet().at(index_column - 1) + QString::number(index_string);
	cAttributesModel["s"] = QString::number(numberStyle);

	if(number != -1){
		cAttributesModel["t"] = "s";
		cItem["v"] = QString::number(number);
	}

	row["attributes"] = rowAttributes;
	cItem["attributes"] = cAttributesModel;
	cModel.append(cItem);
	row["columns"] = cModel;
	if (rowsModel.last().toObject()["attributes"].toObject()["r"].toString() == QString::number(index_string)){
		rowsModel.replace(rowsModel.count()-1, row);
	}
	else{
		rowsModel.append(row);
	}


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
		colsModel.insert(index_column, width);
		colsExist = true;
	}
	else{
		if (colsModel.contains(index_column)){
			if (width > colsModel[index_column]){
				colsModel[index_column] = width;
			}
		}
		else{
			colsModel.insert(index_column, width);
		}
	}
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
	documentWriter->writeStartDocument("1.0", true);
	documentWriter->writeStartElement("worksheet");
	documentWriter->writeAttribute("xmlns", "http://schemas.openxmlformats.org/spreadsheetml/2006/main");
	documentWriter->writeAttribute("xmlns:r", "http://schemas.openxmlformats.org/officeDocument/2006/relationships");

	if (colsExist){
		documentWriter->writeStartElement("cols");
		float width = 0;
		int min = 0, max = 0;
		for (int key : colsModel.keys()){
			if (width == 0 && min == 0 && max == 0){
				documentWriter->writeStartElement("col");
				documentWriter->writeAttribute("customWidth", "1");
				documentWriter->writeAttribute("width", QString::number(colsModel[key]));
				width = colsModel[key];
				min = key;
				max = key;
			}
			else if (colsModel[key] == width && (key - max) == 1){
				max = key;
			}
			else{
				documentWriter->writeAttribute("max", QString::number(max));
				documentWriter->writeAttribute("min", QString::number(min));
				documentWriter->writeEndElement();
				documentWriter->writeStartElement("col");
				documentWriter->writeAttribute("customWidth", "1");
				documentWriter->writeAttribute("width", QString::number(colsModel[key]));
				width = colsModel[key];
				min = key;
				max = key;
			}
		}
		if (colsModel.count() > 0){
			documentWriter->writeAttribute("max", QString::number(max));
			documentWriter->writeAttribute("min", QString::number(min));
			documentWriter->writeEndElement();
		}

		// for (int i = 0; i < colsModel.count(); i++){
		// 	documentWriter->writeStartElement("col");
		// 	documentWriter->writeAttribute("customWidth", colsModel[i].toObject()["customWidth"].toString());
		// 	documentWriter->writeAttribute("max", colsModel[i].toObject()["max"].toString());
		// 	documentWriter->writeAttribute("min", colsModel[i].toObject()["min"].toString());
		// 	documentWriter->writeAttribute("width", colsModel[i].toObject()["width"].toString());
		// 	documentWriter->writeEndElement();
		// }

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

		if (rowsModel[i].toObject().contains("columns")){
			QJsonArray columns = rowsModel[i].toObject()["columns"].toArray();

			for (int j = 0; j < columns.count(); j++){
				documentWriter->writeStartElement("c");
				QJsonObject cModel = columns[j].toObject();

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
	documentWriter->writeEndDocument();

	return streamData;
}


QString EjSheetDocument::getAlphabet()
{
	return alphabet;
}

