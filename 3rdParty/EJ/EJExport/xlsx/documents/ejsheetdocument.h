#ifndef WORKSHEETS_DOCUMENT_H
#define WORKSHEETS_DOCUMENT_H

#include <QXmlStreamWriter>
#include <QFile>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>


const int INDEX_STRING = 3;
const int INDEX_COLUMN = 2;

class EjSheetDocument {

	QByteArray streamData;
	QXmlStreamWriter* documentWriter;
	QJsonArray sheetDataModel;
	QJsonArray mergeCellsModel;
	// QJsonArray colsModel;
	QJsonArray rowsModel;
	QMap<int, float> colsModel;
	int index_string = INDEX_STRING;
	int index_column = INDEX_COLUMN;
	int count_cells = 0;
	QString alphabet = "0ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	bool mergeCellsExist = false;
	bool colsExist = false;
	int countColsCurrentTable = - 1;
	int countStrCurrentTable = - 1;
	bool activeTable = false;
	int maxIndexColForTable = 1;

	/*Ситуация аналогичная тексту, будем хранить id этих стилей,
      пока не встретим текстовый блоки, как встретим добавим текст сразу со стилями*/
	int fillId, borderId;
	float heightCell;

public:
	EjSheetDocument();
	~EjSheetDocument();
	bool addTextNumber(int number, int numberStyle);
	bool addTextNumberIntoTable(int number, int numberStyle);
	bool newString();
	bool newColumn();
	void setDefaultIndexColumn();
	void validate();
	QString getAlphabet();
	QByteArray getDocumentData();
	void addMergeCells(int rows, int cols);
	void addWidthCell(float width);
	void addHeightCell(float height);
	void setCountColsCurrentTable(int value);
	void setCountStrCurrentTable(int value);
	void setActiveTable(bool value);
	bool getActiveTable() const;
	int getIndexString() const;
	int getIndexColumn() const;
	void setMaxIndexColForTable(int value);

	int getFillId() const;
	void setFillId(int value);
	int getBorderId() const;
	void setBorderId(int value);
};

#endif // WORKSHEETS_DOCUMENT_H
