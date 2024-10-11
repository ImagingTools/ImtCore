#include "ejsharedstringsdocument.h"

#include <QtCore/QFile>
#include <QtCore/QDir>
#include<QStandardPaths>

int EjSharedStringsDocument::getCountWords() const
{
	return count_words;
}


QByteArray EjSharedStringsDocument::getDocumentData()
{
	documentWriter->writeStartDocument("1.0", true);
	documentWriter->writeStartElement("sst");
	documentWriter->writeAttribute("xmlns", "http://schemas.openxmlformats.org/spreadsheetml/2006/main");
	for(int i = 0; i < siModel.count(); i++){
		if (siModel[i].toObject()["text"].toString().isEmpty()){
			continue;
		}
		documentWriter->writeStartElement("si");
		if (siModel[i].toObject().contains("sz")){
			documentWriter->writeStartElement("r");
			documentWriter->writeStartElement("rPr");
			if (siModel[i].toObject().contains("b")){
				documentWriter->writeStartElement("b");
				documentWriter->writeEndElement();
			}
			if (siModel[i].toObject().contains("i")){
				documentWriter->writeStartElement("i");
				documentWriter->writeEndElement();
			}
			if (siModel[i].toObject().contains("u")){
				documentWriter->writeStartElement("u");
				documentWriter->writeEndElement();
			}
			if (siModel[i].toObject().contains("strike")){
				documentWriter->writeStartElement("strike");
				documentWriter->writeEndElement();
			}
			documentWriter->writeStartElement("sz");
			documentWriter->writeAttribute("val", siModel[i].toObject()["sz"].toString());
			documentWriter->writeEndElement();
			documentWriter->writeStartElement("color");
			if (siModel[i].toObject().contains("theme")){
				documentWriter->writeAttribute("theme", siModel[i].toObject()["theme"].toString());
			}
			else{
				documentWriter->writeAttribute("rgb", siModel[i].toObject()["rgb"].toString());
			}
			documentWriter->writeEndElement();
			documentWriter->writeStartElement("rFont");
			documentWriter->writeAttribute("val", siModel[i].toObject()["rFont"].toString());
			documentWriter->writeEndElement();

			documentWriter->writeStartElement("family");
			documentWriter->writeAttribute("val", siModel[i].toObject()["family"].toString());
			documentWriter->writeEndElement();

			documentWriter->writeStartElement("charset");
			documentWriter->writeAttribute("val", siModel[i].toObject()["charset"].toString());
			documentWriter->writeEndElement();

			documentWriter->writeStartElement("scheme");
			documentWriter->writeAttribute("val",siModel[i].toObject()["scheme"].toString());
			documentWriter->writeEndElement();
			documentWriter->writeEndElement();
			documentWriter->writeStartElement("t");
			if (siModel[i].toObject().contains("xml:space")){
				documentWriter->writeAttribute("xml:space", siModel[i].toObject()["xml:space"].toString());
			}
			documentWriter->writeCharacters(siModel[i].toObject()["text"].toString());
			documentWriter->writeEndElement();
			documentWriter->writeEndElement();
		}
		else{
			documentWriter->writeStartElement("t");
			if (siModel[i].toObject().contains("xml:space")){
				documentWriter->writeAttribute("xml:space", siModel[i].toObject()["xml:space"].toString());
			}
			documentWriter->writeCharacters(siModel[i].toObject()["text"].toString());
			documentWriter->writeEndElement();
		}
		documentWriter->writeEndElement();
	}
	documentWriter->writeEndElement();
	documentWriter->writeEndDocument();

	return streamData;
}


void EjSharedStringsDocument::createSi() {
	/*Будет принята ранее созданная si,
    и создастся новая*/
	QJsonObject si;
	siModel.append(si);
	si_index = siModel.count()-1;
}


void EjSharedStringsDocument::insertIntoSi(QString str)
{
	/*Для переданной строки будет создана такая структура
            <r>
                <rPr>
                     <sz val="16"/>
                     <color rgb="FFFF0000"/>
                     <rFont val="Calibri"/>
                     <family val="2"/>
                     <charset val="204"/>
                     <scheme val="minor"/>
                </rPr>
                <t>HELLO</t>
            </r>
*/
	QJsonObject selecedSi = siModel[si_index].toObject();

	if (bold || italic || underline || strikeOut || sizeText != 12 || colorText != "FF000000" || nameFont != "Calibri" || str == " "){

		if(bold){
			selecedSi["b"] = true;
		}

		if(italic){
			selecedSi["i"] = true;
		}

		if(underline){
			selecedSi["u"] = true;
		}

		if(strikeOut){
			selecedSi["strike"] = true;
		}

		selecedSi["sz"] = QString::number(sizeText);


		if(str == " "){
			selecedSi["theme"] =  QString::number(1);
		}
		else{
			selecedSi["rgb"] =  colorText;
		}

		selecedSi["rFont"] =  nameFont;
		selecedSi["family"] =  QString::number(2);
		selecedSi["charset"] =  QString::number(204);
		selecedSi["scheme"] =  "minor";

		if(str == " "){
			selecedSi["xml:space"] =  "preserve";
		}
	}

	selecedSi["text"] =  str;

	siModel[si_index] = selecedSi;
}


QString EjSharedStringsDocument::getTempTextFromCell() const
{
	return tempTextFromCell;
}


void EjSharedStringsDocument::setTempTextFromCell(const QString &value)
{
	tempTextFromCell = value;
}


void EjSharedStringsDocument::setSizeText(int value)
{
	sizeText = value;
}


void EjSharedStringsDocument::setBold(bool value)
{
	bold = value;
}


void EjSharedStringsDocument::setUnderline(bool value)
{
	underline = value;
}


void EjSharedStringsDocument::setItalic(bool value)
{
	italic = value;
}


void EjSharedStringsDocument::setStrikeOut(bool value)
{
	strikeOut = value;
}


void EjSharedStringsDocument::setColorText(const QString &value)
{
	colorText = value;
}


void EjSharedStringsDocument::setNameFont(const QString &value)
{
	nameFont = value;
}


void EjSharedStringsDocument::setStylesToDefault()
{
	nameFont = "Calibri";
	colorText = "FF000000";
	sizeText = 12;
	bold = false;
	underline = false;
	italic = false;
	strikeOut = false;
}


EjSharedStringsDocument::EjSharedStringsDocument():
	streamData(QByteArray())
{
	documentWriter = new QXmlStreamWriter(&streamData);
	documentWriter->setAutoFormatting(true);
	documentWriter->setAutoFormattingIndent(1);
	createSi();
}


EjSharedStringsDocument::~EjSharedStringsDocument()
{
	delete documentWriter;

	while (siModel.size() > 0) {
		siModel.removeLast();
	}

	text.clear();
	index_string = 0;
	count_words = 0;
	nameFont = "Calibri";
	colorText = "FF000000";
	sizeText = 12;
	bold = false;
	underline = false;
	italic = false;
	strikeOut = false;
	first = true;
	startNewString = false;
}


bool EjSharedStringsDocument::addSpase() {
	return addText(" ", false);
}


bool EjSharedStringsDocument::addText(QString text, bool isNew) {
	/* Если isNew будет передан как true, то текущий тег si будет вставлен в документ
       Для текста вне таблицы ожидается, что isNew = true когда будет встречен блок
       переноса строки */
	if(isNew){
		createSi();
		count_words++;
		return true;
	}
	insertIntoSi(text);
	return true;
}


bool EjSharedStringsDocument::addTextIntoTable(QString new_text)
{
	insertIntoSi(new_text);
	count_words++;
	createSi();
	return true;
}
