#include "ejstylesdocument.h"

#include<QStandardPaths>
#include <QtCore/QDir>


void EjStylesDocument::setAlign(int value)
{
	align = value;
}


void EjStylesDocument::aligns_init()
{
	QJsonObject align1, align2, align3, align4, align5, align6, align7, align8, align9, align10, align11, align12, align13, align14, align15, align16;

	align1["horizontal"] = "center";
	aligns.insert(EjParagraphStyle::AlignHCenter, align1);

	align2["horizontal"] = "left";
	aligns.insert(EjParagraphStyle::AlignLeft, align2);

	align3["horizontal"] = "right";
	aligns.insert(EjParagraphStyle::AlignRight, align3);

	align4["horizontal"] = "center";
	align4["vertical"] = "center";
	aligns.insert(132, align4);

	align5["horizontal"] = "center";
	align5["vertical"] = "top";
	aligns.insert(36, align5);

	align6["horizontal"] = "center";
	align6["vertical"] = "bottom";
	aligns.insert(68, align6);

	align7["horizontal"] = "left";
	align7["vertical"] = "center";
	aligns.insert(129, align7);

	align8["horizontal"] = "left";
	align8["vertical"] = "top";
	aligns.insert(33, align8);

	align9["horizontal"] = "left";
	align9["vertical"] = "bottom";
	aligns.insert(65, align9);

	align10["horizontal"] = "right";
	align10["vertical"] = "bottom";
	aligns.insert(66, align10);

	align11["horizontal"] = "right";
	align11["vertical"] = "center";
	aligns.insert(130, align11);

	align12["horizontal"] = "right";
	align12["vertical"] = "top";
	aligns.insert(34, align12);

	align13["vertical"] = "bottom";
	aligns.insert(EjParagraphStyle::AlignBottom, align13);

	align14["vertical"] = "center";
	aligns.insert(EjParagraphStyle::AlignVCenter, align14);

	align15["vertical"] = "top";
	aligns.insert(EjParagraphStyle::AlignTop, align15);
}


int EjStylesDocument::addBorders(int left, int right, int top, int bottom)
{
	QString value;
	QJsonObject border;

	if((value = widthsBorders.value(left)) != nullptr){
		border["left"] = value;
	}

	if((value = widthsBorders.value(right)) != nullptr){
		border["right"] = value;
	}

	if((value = widthsBorders.value(top)) != nullptr){
		border["top"] = value;
	}

	if((value = widthsBorders.value(bottom)) != nullptr){
		border["bottom"] = value;
	}

	bordersModel.append(border);
	countBorders++;
	return countBorders;
}


int EjStylesDocument::addFill(QString color)
{
	QJsonObject fill;
	fill["patternType"] = "solid";
	fill["rgb"] = color;
	fill["indexed"] = "64";
	fillsModel.append(fill);
	countFills++;
	return countFills;
}


int EjStylesDocument::addXfToCellXfs(int fillId, int borderId)
{
	QJsonObject cellXfs;
	cellXfs["numFmtId"] = "0";
	cellXfs["fontId"] = "0";
	cellXfs["fillId"] = QString::number(fillId);

	if(fillId != 0){
		cellXfs["applyFill"] = QString::number(1);
	}

	cellXfs["borderId"] = QString::number(borderId);

	if(borderId != 0){
		cellXfs["applyBorder"] =  QString::number(1);
	}

	cellXfs["xfId"] = QString::number(0);

	if (aligns.contains(align)){
		QJsonObject alignJson = aligns.value(align);
		QStringList alignKeys = alignJson.keys();

		for (QString alignKey : alignKeys){
			cellXfs[alignKey] = alignJson[alignKey].toString();
		}

		cellXfs["wrapText"] = "1";
		cellXfs["applyAlignment"] = QString::number(1);
	}

	cellXfsModel.append(cellXfs);
	countCellXfs++;
	return countCellXfs;
}


EjStylesDocument::EjStylesDocument():
	streamData(QByteArray())
{
	documentWriter = new QXmlStreamWriter(&streamData);
	documentWriter->setAutoFormatting(true);
	documentWriter->setAutoFormattingIndent(1);
	QJsonObject fill1, fill2;
	fill1["patternType"] = "none";
	fill2["patternType"] = "gray125";
	fillsModel.append(fill1);
	fillsModel.append(fill2);
	QJsonObject border;
	bordersModel.append(border);
	QJsonObject cellXfs;
	cellXfs["numFmtId"] = "0";
	cellXfs["fontId"] = "0";
	cellXfs["fillId"] = "0";
	cellXfs["borderId"] = "0";
	cellXfs["xfId"] = "0";
	cellXfsModel.append(cellXfs);
	widthsBorders.insert(100, "thin");
	widthsBorders.insert(200, "medium");
	widthsBorders.insert(300, "thick");
	aligns_init();
}


EjStylesDocument::~EjStylesDocument()
{
	delete documentWriter;

	while (fillsModel.size() > 0) {
		fillsModel.removeLast();
	}

	while (bordersModel.size() > 0) {
		bordersModel.removeLast();
	}

	while (cellXfsModel.size() > 0) {
		cellXfsModel.removeLast();
	}

	widthsBorders.clear();
	aligns.clear();
	countBorders = 0;
	countCellXfs = 0;
	countFills = 1;
	align = EjParagraphStyle::AlignLeft;
}


QByteArray EjStylesDocument::getDocumentData()
{
	documentWriter->writeStartElement("styleSheet"); // style sheet
	documentWriter->writeAttribute("xmlns", "http://schemas.openxmlformats.org/spreadsheetml/2006/main");
	documentWriter->writeStartElement("fonts"); // fonts
	documentWriter->writeStartElement("font"); // font
	documentWriter->writeStartElement("sz");
	documentWriter->writeAttribute("val", "11");
	documentWriter->writeEndElement();
	documentWriter->writeStartElement("color");
	documentWriter->writeAttribute("theme", "1");
	documentWriter->writeEndElement();
	documentWriter->writeStartElement("name");
	documentWriter->writeAttribute("val", "Calibri");
	documentWriter->writeEndElement();
	documentWriter->writeStartElement("family");
	documentWriter->writeAttribute("val", "2");
	documentWriter->writeEndElement();
	documentWriter->writeStartElement("charset");
	documentWriter->writeAttribute("val", "204");
	documentWriter->writeEndElement();
	documentWriter->writeStartElement("scheme");
	documentWriter->writeAttribute("val", "minor");
	documentWriter->writeEndElement();
	documentWriter->writeEndElement();
	documentWriter->writeEndElement();
	documentWriter->writeStartElement("fills");

	for (int i = 0; i < fillsModel.count(); i++){
		documentWriter->writeStartElement("fill");
		documentWriter->writeStartElement("patternFill");
		documentWriter->writeAttribute("patternType", fillsModel[i].toObject()["patternType"].toString());

		if (fillsModel[i].toObject().contains("rgb")){
			documentWriter->writeStartElement("fgColor");
			documentWriter->writeAttribute("rgb", fillsModel[i].toObject()["rgb"].toString());
			documentWriter->writeEndElement();
			documentWriter->writeStartElement("bgColor");
			documentWriter->writeAttribute("indexed", fillsModel[i].toObject()["indexed"].toString());
			documentWriter->writeEndElement();
		}

		documentWriter->writeEndElement();
		documentWriter->writeEndElement();
	}

	documentWriter->writeEndElement();
	documentWriter->writeStartElement("borders");

	for (int i = 0; i < bordersModel.count(); i++){
		documentWriter->writeStartElement("border");
		documentWriter->writeStartElement("left");

		if (bordersModel[i].toObject().contains("left")){
			documentWriter->writeAttribute("style", bordersModel[i].toObject()["left"].toString());
			documentWriter->writeStartElement("color");
			documentWriter->writeAttribute("auto", "1");
			documentWriter->writeEndElement();
		}

		documentWriter->writeEndElement();
		documentWriter->writeStartElement("right");

		if (bordersModel[i].toObject().contains("right")){
			documentWriter->writeAttribute("style", bordersModel[i].toObject()["right"].toString());
			documentWriter->writeStartElement("color");
			documentWriter->writeAttribute("auto", "1");
			documentWriter->writeEndElement();
		}

		documentWriter->writeEndElement();
		documentWriter->writeStartElement("top");

		if (bordersModel[i].toObject().contains("top")){
			documentWriter->writeAttribute("style", bordersModel[i].toObject()["top"].toString());
			documentWriter->writeStartElement("color");
			documentWriter->writeAttribute("auto", "1");
			documentWriter->writeEndElement();
		}

		documentWriter->writeEndElement();
		documentWriter->writeStartElement("bottom");

		if (bordersModel[i].toObject().contains("bottom")){
			documentWriter->writeAttribute("style", bordersModel[i].toObject()["bottom"].toString());
			documentWriter->writeStartElement("color");
			documentWriter->writeAttribute("auto", "1");
			documentWriter->writeEndElement();
		}

		documentWriter->writeEndElement();
		documentWriter->writeStartElement("diagonal");
		documentWriter->writeEndElement();
		documentWriter->writeEndElement();
	}

	documentWriter->writeEndElement();
	documentWriter->writeStartElement("cellStyleXfs");
	documentWriter->writeStartElement("xf");
	documentWriter->writeAttribute("borderId", "0");
	documentWriter->writeAttribute("fillId", "0");
	documentWriter->writeAttribute("fontId", "0");
	documentWriter->writeAttribute("numFmtId", "0");
	documentWriter->writeEndElement();
	documentWriter->writeEndElement();
	documentWriter->writeStartElement("cellXfs");

	for (int i = 0; i < cellXfsModel.count(); i++){
		documentWriter->writeStartElement("xf");

		for (QString cellXfsParam : cellXfsModel[i].toObject().keys()){

			if (cellXfsParam != "horizontal" && cellXfsParam != "vertical" && cellXfsParam != "wrapText"){
				documentWriter->writeAttribute(cellXfsParam, cellXfsModel[i].toObject()[cellXfsParam].toString());
			}
		}

		if (cellXfsModel[i].toObject().keys().contains("horizontal") || cellXfsModel[i].toObject().keys().contains("vertical")){
			documentWriter->writeStartElement("alignment");

			if (cellXfsModel[i].toObject().keys().contains("horizontal")){
				documentWriter->writeAttribute("horizontal", cellXfsModel[i].toObject()["horizontal"].toString());
			}

			if (cellXfsModel[i].toObject().keys().contains("vertical")){
				documentWriter->writeAttribute("vertical", cellXfsModel[i].toObject()["vertical"].toString());
			}

			if (cellXfsModel[i].toObject().keys().contains("wrapText")){
				documentWriter->writeAttribute("wrapText", cellXfsModel[i].toObject()["wrapText"].toString());
			}

			documentWriter->writeEndElement();
		}

		documentWriter->writeEndElement();
	}

	documentWriter->writeEndElement();
	documentWriter->writeStartElement("cellStyles");
	documentWriter->writeStartElement("cellStyle");
	documentWriter->writeAttribute("builtinId", "0");
	documentWriter->writeAttribute("name", QStringLiteral("Обычный"));
	documentWriter->writeAttribute("xfId", "0");
	documentWriter->writeEndElement();
	documentWriter->writeEndElement();
	documentWriter->writeStartElement("dxfs");
	documentWriter->writeEndElement();
	documentWriter->writeStartElement("tableStyles");
	documentWriter->writeAttribute("defaultPivotStyle", "PivotStyleLight16");
	documentWriter->writeAttribute("defaultTableStyle", "TableStyleMedium9");
	documentWriter->writeEndElement();
	documentWriter->writeEndElement();
	return streamData;
}


