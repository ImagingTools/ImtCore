#pragma once


// Qt includes
#include <QtCore/QVector>
#include <QtWidgets/QGraphicsScene>

// ACF includes
#include <i2d/IObject2d.h>

// ImtCore includes
#include <imtreport/IReportDocument.h>
#include <imtreport/CFont.h>
#include <imtreport/TGraphicsElement.h>
#include <imtreport/CTextTable.h>


class QTableWidgetItem;


namespace imtreportgui
{


class CReportSceneBuilder
{
public:
	CReportSceneBuilder();

	typedef QVector<QGraphicsScene*> ReportScenes;
	static ReportScenes Build(imtreport::IReportDocument& reportDocument);

private:
	static void BuildSceneShapes(const imtreport::IReportPage& page, QGraphicsScene& scene);

	static void ConvertShapeCoodinates(const i2d::IObject2d& pageElement, QGraphicsItem& sceneElement);
	static void ConvertRectCoodinates(const imtreport::CRectangleElement& pageElement, QGraphicsItem& sceneElement);
	static void ConvertEllipseCoodinates(const imtreport::CCircleElement& pageElement, QGraphicsItem& sceneElement);
	static void ConvertLabelCoodinates(const imtreport::CTextLabelElement& pageElement, QGraphicsItem& sceneElement);
	static void ConvertLineCoodinates(const imtreport::CLineElement& pageElement, QGraphicsItem& sceneElement);
	static void ConvertPolygoneCoodinates(const imtreport::CPolygonElement& pageElement, QGraphicsItem& sceneElement);
	static void ConvertImageCoodinates(const imtreport::CImageRectangleElement& pageElement, QGraphicsItem& sceneElement);
	static void ConvertTableCoodinates(const imtreport::CTextTable& pageElement, QGraphicsItem& sceneElement);

	static QTableWidgetItem* ConvertTableItem(const imtreport::CTextTableItem& tableItem);

	static QPointF MapPointToScene(const QPointF& point);
	static QRectF MapRectToScene(const QRectF& rect);
	static QFont MapFontToScene(const imtreport::CFont& font);
};


} // namespace imtreportgui

