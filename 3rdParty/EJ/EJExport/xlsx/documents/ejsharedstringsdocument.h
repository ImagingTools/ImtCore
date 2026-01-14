#ifndef SHEET_DOCUMENT_H
#define SHEET_DOCUMENT_H

#include <QXmlStreamWriter>
#include <QFile>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>


class EjSharedStringsDocument {
	QByteArray streamData;
	QXmlStreamWriter* documentWriter;
	QJsonArray siModel;
	QString text;
	int index_string = 0;
	int count_words = 0;
	int si_index = -1;

	/*Все текстовые стили я применяю именно в этом файле*/
	QString nameFont = "Calibri";
	QString colorText = "FF000000";
	int sizeText = 12;
	bool bold = false;
	bool underline = false;
	bool italic = false;
	bool strikeOut = false;
	void createSi();
	void insertIntoSi(QString str);

	/*Как встретили блок 7 сразу берем у него текст, и добавляем в эту переменную
      и добавим этот текст когда дойдем до текстовых блоков в ячейке*/
	QString tempTextFromCell;


public:
	/*Для удобства текста в ячейках, то есть мы берем текст из ячейки и добавляем его в файл
      когда встретится первый блок текста (2 или 3), втретили первый остальные трогать не будем*/
	bool first = true;

	/*для определения был ли ентер после текста*/
	bool startNewString = false;
	EjSharedStringsDocument();
	~EjSharedStringsDocument();
	bool addSpase();
	bool addText(QString new_text, bool isNew);
	bool addTextIntoTable(QString new_text);
	QString getText(){ return text ;}
	void setText(QString str){ text = str;}
	bool addTextTable(QString txt);
	void setSizeText(int value);
	void setBold(bool value);
	void setUnderline(bool value);
	void setItalic(bool value);
	void setStrikeOut(bool value);
	void setColorText(const QString &value);
	void setNameFont(const QString &value);
	void setStylesToDefault();
	int getCountWords() const;
	QByteArray getDocumentData();
	void setTempTextFromCell(const QString &value);
	QString getTempTextFromCell() const;
};

#endif // SHEET_DOCUMENT_H
