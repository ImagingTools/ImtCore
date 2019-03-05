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
#include <imtreportgui/CTextTableProxy.h>


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
	static void CreateSceneShape(const i2d::IObject2d& pageElement, QGraphicsScene& scene);
	static void CreateRect(const imtreport::CRectangleElement& pageElement, QGraphicsScene& scene);
	static void CreateEllipse(const imtreport::CCircleElement& pageElement, QGraphicsScene& scene);
	static void CreateLabel(const imtreport::CTextLabelElement& pageElement, QGraphicsScene& scene);
	static void CreateLine(const imtreport::CLineElement& pageElement, QGraphicsScene& scene);
	static void CreatePolygone(const imtreport::CPolygonElement& pageElement, QGraphicsScene& scene);
	static void CreateImage(const imtreport::CImageRectangleElement& pageElement, QGraphicsScene& scene);
	static void CreateTextTable(const imtreport::CTextTable& pageElement, QGraphicsScene& scene);
	static void CreateTextTableItem(const imtreport::CTextTableItem& tableItem, const QRectF& cellRect, QGraphicsScene& scene);

	static double GetTextTableRowHeight(const CTextTableProxy& table, int row);
	static void SetShapePenAndBrush(const imtreport::IGraphicsElement& pageElement, QAbstractGraphicsShapeItem& sceneElement);

	static QPointF MapPointToScene(const QPointF& point);
	static QRectF MapRectToScene(const QRectF& rect);
	static QFont MapFontToScene(const imtreport::CFont& font);
	static QColor ConvertToQColor(const icmm::CRgb& color);

	static const qreal s_A4WidthMm;
	static const qreal s_A4HeightMm;
};


} // namespace imtreportgui

