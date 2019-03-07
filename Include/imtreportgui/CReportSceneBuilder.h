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
	static ReportScenes Build(const imtreport::IReportDocument& reportDocument, int resolutionDpi);

private:
	static void SetupScene(QGraphicsScene& scene, int resolutionDpi);

	static void CreateSceneShapes(const imtreport::IReportPage& page, QGraphicsScene& scene, int resolutionDpi);
	static void CreateSceneShape(const i2d::IObject2d& pageElement, QGraphicsScene& scene, int resolutionDpi);
	static void CreateRect(const imtreport::CRectangleElement& pageElement, QGraphicsScene& scene, int resolutionDpi);
	static void CreateEllipse(const imtreport::CCircleElement& pageElement, QGraphicsScene& scene, int resolutionDpi);
	static void CreateLabel(const imtreport::CTextLabelElement& pageElement, QGraphicsScene& scene, int resolutionDpi);
	static void CreateLine(const imtreport::CLineElement& pageElement, QGraphicsScene& scene, int resolutionDpi);
	static void CreatePolygone(const imtreport::CPolygonElement& pageElement, QGraphicsScene& scene, int resolutionDpi);
	static void CreateImage(const imtreport::CImageRectangleElement& pageElement, QGraphicsScene& scene, int resolutionDpi);
	static void CreateTextTable(const imtreport::CTextTable& pageElement, QGraphicsScene& scene, int resolutionDpi);
	static void CreateTextTableItem(const imtreport::CTextTableItem& tableItem, const QRectF& cellRect, QGraphicsScene& scene, int resolutionDpi);

	static double GetTextTableRowHeight(const CTextTableProxy& table, int row);
	static void SetShapePenAndBrush(const imtreport::IGraphicsElement& pageElement, QAbstractGraphicsShapeItem& sceneElement);

	static QPointF MapPointToScene(const QPointF& point, int resolutionDpi);
	static QRectF MapRectToScene(const QRectF& rect, int resolutionDpi);
	static QFont MapFontToScene(const imtreport::CFont& font, int resolutionDpi);
	static QColor ConvertToQColor(const icmm::CRgb& color);

	static const qreal s_A4WidthMm;
	static const qreal s_A4HeightMm;
};


} // namespace imtreportgui

