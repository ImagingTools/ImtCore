#ifndef EJSTYLESDOCUMENT_H
#define EJSTYLESDOCUMENT_H

#include <QList>
#include "ejtableblocks.h"
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QXmlStreamWriter>
#include <QFile>

class EjStylesDocument
{
	QMap<int, QString> widthsBorders;
	QByteArray streamData;
	QXmlStreamWriter* documentWriter;
	QMap<int, QJsonObject> aligns;
	QJsonArray fillsModel;
	QJsonArray bordersModel;
	QJsonArray cellXfsModel;
	int countBorders = 0;
	int countCellXfs = 0/*Здесь по стандарту одна запись*/;
	int countFills = 1;/*1 потому что там по стандарту есть уже 2 записи*/
	int align = EjParagraphStyle::AlignLeft;
	void aligns_init();

public:
	int addBorders(int left, int right, int top, int bottom);
	int addFill(QString);
	int addXfToCellXfs(int fillId, int borderId);

	EjStylesDocument();
	~EjStylesDocument();
	QByteArray getDocumentData();
	void setAlign(int value);
};

#endif // EJSTYLESDOCUMENT_H
