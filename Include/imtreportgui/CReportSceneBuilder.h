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
	static ReportScenes Build(const imtreport::IReportDocument& reportDocument);

private:
	static void SetupScene(QGraphicsScene& scene);

	static void CreateSceneShapes(const imtreport::IReportPage& page, QGraphicsScene& scene);
	static QGraphicsItem* CreateSceneShape(const i2d::IObject2d& pageElement);

	static QGraphicsItem* CreateRect(const imtreport::CRectangleElement& pageElement);
	static QGraphicsItem* CreateEllipse(const imtreport::CCircleElement& pageElement);
	static QGraphicsItem* CreateLabel(const imtreport::CTextLabelElement& pageElement);
	static QGraphicsItem* CreateLine(const imtreport::CLineElement& pageElement);
	static QGraphicsItem* CreatePolygone(const imtreport::CPolygonElement& pageElement);
	static QGraphicsItem* CreateImage(const imtreport::CImageRectangleElement& pageElement);
	static QGraphicsItem* CreateTextTable(const imtreport::CTextTable& pageElement);
	static QGraphicsTextItem* CreateTextTableItem(const imtreport::CTextTable& table,
		const imtreport::CTextTableItem& tableItem,
		int row,
		int col,
		double cellWidth,
		double cellHeight);

	static void SetShapePenAndBrush(const imtreport::IGraphicsElement& pageElement, QAbstractGraphicsShapeItem& sceneElement);

	static QPointF MapPointToScene(const QPointF& point);
	static QRectF MapRectToScene(const QRectF& rect);
	static QFont MapFontToScene(const imtreport::CFont& font);
	static QColor ConvertToQColor(const icmm::CRgb& color);

	static const qreal s_A4WidthMm;
	static const qreal s_A4HeightMm;
};


} // namespace imtreportgui

