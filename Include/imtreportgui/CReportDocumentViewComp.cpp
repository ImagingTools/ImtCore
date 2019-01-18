#include <imtreportgui/CReportDocumentViewComp.h>


// Qt includes
#include <QtPrintSupport/QPrinter>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>

// ImtCore includes
#include <imtreport/IReportPage.h>
#include <imtreportgui/CGraphicsElementShapeFactory.h>


namespace imtreportgui
{


// static members
const qreal CReportDocumentViewComp::s_A4WidthMm = 210.0;
const qreal CReportDocumentViewComp::s_A4HeightMm = 297.0;


// public methods

CReportDocumentViewComp::CReportDocumentViewComp()
	:m_commands("&File", 100),
	m_exportToPdfCommand("Export to PDF...", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR, 1984)
{
	connect(&m_exportToPdfCommand, SIGNAL(triggered()), this, SLOT(OnExportToPdf()));

	m_commands.InsertChild(&m_exportToPdfCommand);
	m_rootCommands.InsertChild(&m_commands);
}


// reimplemented (ibase::ICommandsProvider)

const ibase::IHierarchicalCommand* CReportDocumentViewComp::GetCommands() const
{
	return &m_rootCommands;
}


// protected methods

// reimplemented (iqtgui::TGuiObserverWrap)

void CReportDocumentViewComp::UpdateGui(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	m_scene.clear();

	UpdateSceneRect();
	UpdateSceneShapes();
}


// reimplemented (iqtgui::CGuiComponentBase)

void CReportDocumentViewComp::OnGuiCreated()
{
	BaseClass::OnGuiCreated();

	PageView->setScene(&m_scene);
}


void CReportDocumentViewComp::OnGuiDestroyed()
{
	BaseClass::OnGuiDestroyed();
}


void CReportDocumentViewComp::OnGuiRetranslate()
{
	BaseClass::OnGuiRetranslate();
}


void CReportDocumentViewComp::OnExportToPdf()
{
	QString fileName = QFileDialog::getSaveFileName(GetWidget(), tr("Export to PDF..."), "", "*.pdf");

	if (!fileName.isEmpty()) {
		QPrinter printer(QPrinter::HighResolution);
		printer.setPageSize(QPrinter::A4);
		printer.setOrientation(QPrinter::Portrait);
		printer.setOutputFormat(QPrinter::PdfFormat);
		printer.setOutputFileName(fileName);
		printer.setPageMargins({ 0.0, 0.0, 0.0, 0.0 });

		QPainter painter(&printer);
		m_scene.render(&painter);

		QMessageBox::information(GetWidget(), "Export to PDF", "Report has been exported successfully");
	}
}


// private methods

void CReportDocumentViewComp::UpdateSceneRect()
{
	QRectF sceneRect(QPointF(0.0, 0.0), MapPointToScene(QPointF(s_A4WidthMm, s_A4HeightMm)));

	m_scene.setSceneRect(sceneRect);
	m_scene.addRect(sceneRect, QPen(Qt::red));
}


void CReportDocumentViewComp::UpdateSceneShapes()
{
	imtreport::IReportDocument* documentPtr = GetObjectPtr();
	Q_ASSERT(documentPtr != NULL);

	imtreportgui::CGraphicsElementShapeFactory shapeFactory;

	int pageCount = documentPtr->GetPagesCount();
	if (pageCount > 0) {
		const imtreport::IReportPage* pagePtr = documentPtr->GetReportPage(0);
		Q_ASSERT(pagePtr != NULL);

		imtreport::IReportPage::ElementIds ids = pagePtr->GetPageElements();

		for (const QByteArray& elementId : ids) {
			const imtreport::IGraphicsElement* elementPtr = pagePtr->GetPageElement(elementId);
			Q_ASSERT(elementPtr != nullptr);

			QGraphicsItem* itemPtr = shapeFactory.CreateShape(*elementPtr);
			ConvertShapeCoodinates(elementPtr, itemPtr);
			m_scene.addItem(itemPtr);
		}
	}
}


QPointF CReportDocumentViewComp::MapPointToScene(const QPointF& point) const
{
	// map shape coordinates given in mm to scene's coordinates
	qreal x = point.x() * static_cast<qreal>(PageView->physicalDpiX()) / 25.4;
	qreal y = point.y() * static_cast<qreal>(PageView->physicalDpiY()) / 25.4;
	return QPointF(x, y);
}


QRectF CReportDocumentViewComp::MapRectToScene(const QRectF& rect) const
{
	return QRectF(MapPointToScene(rect.topLeft()),
		MapPointToScene(rect.bottomRight()));
}


void CReportDocumentViewComp::ConvertShapeCoodinates(const imtreport::IGraphicsElement* pageElement, QGraphicsItem* sceneElement) const
{
	if (!pageElement || !sceneElement)
		return;

	const imtreport::CRectangleElement* rectPageElement = dynamic_cast<const imtreport::CRectangleElement*>(pageElement);
	if (rectPageElement) {
		return ConvertRectCoodinates(rectPageElement, sceneElement);
	}

	const imtreport::CCircleElement* circleSceneElement = dynamic_cast<const imtreport::CCircleElement*>(pageElement);
	if (circleSceneElement) {
		return ConvertEllipseCoodinates(circleSceneElement, sceneElement);
	}

	const imtreport::CTextLabelElement* labelSceneElement = dynamic_cast<const imtreport::CTextLabelElement*>(pageElement);
	if (labelSceneElement) {
		return ConvertLabelCoodinates(labelSceneElement, sceneElement);
	}

	const imtreport::CPolygonElement* polygonElement = dynamic_cast<const imtreport::CPolygonElement*>(pageElement);
	if (polygonElement) {
		return ConvertPolygoneCoodinates(polygonElement, sceneElement);
	}

	const imtreport::CImageRectangleElement* imageElement = dynamic_cast<const imtreport::CImageRectangleElement*>(pageElement);
	if (imageElement) {
		return ConvertImageCoodinates(imageElement, sceneElement);
	}
}


void CReportDocumentViewComp::ConvertRectCoodinates(const imtreport::CRectangleElement* pageElement, QGraphicsItem* sceneElement) const
{
	QGraphicsRectItem* rectSceneElement = dynamic_cast<QGraphicsRectItem*>(sceneElement);
	Q_ASSERT(rectSceneElement);
	Q_ASSERT(pageElement);

	QRectF rect = iqt::GetQRectF(*pageElement);
	rectSceneElement->setRect(rect);
}


void CReportDocumentViewComp::ConvertEllipseCoodinates(const imtreport::CCircleElement* pageElement, QGraphicsItem* sceneElement) const
{
	QGraphicsEllipseItem* ellipseSceneElement = dynamic_cast<QGraphicsEllipseItem*>(sceneElement);
	Q_ASSERT(ellipseSceneElement);
	Q_ASSERT(pageElement);

	QRectF rect = iqt::GetQRectF(pageElement->GetBoundingBox());
	ellipseSceneElement->setRect(MapRectToScene(rect));
}


void CReportDocumentViewComp::ConvertLabelCoodinates(const imtreport::CTextLabelElement* pageElement, QGraphicsItem* sceneElement) const
{
	QGraphicsSimpleTextItem* labelSceneElement = dynamic_cast<QGraphicsSimpleTextItem*>(sceneElement);
	Q_ASSERT(labelSceneElement);
	Q_ASSERT(pageElement);

	labelSceneElement->setPos(MapPointToScene(pageElement->GetPosition()));

	double fontSize = MapPointToScene(QPointF(pageElement->GetFontSize(), 0)).x();
	fontSize *= 0.9; // still unclear why the screen transformation of the scene displays the text to big - so scale it down hardcoded

	QFont font(pageElement->GetFontName(), fontSize);
	labelSceneElement->setFont(font);
}


void CReportDocumentViewComp::ConvertPolygoneCoodinates(const imtreport::CPolygonElement* pageElement, QGraphicsItem* sceneElement) const
{
	QGraphicsPolygonItem* polygoneSceneElement = dynamic_cast<QGraphicsPolygonItem*>(sceneElement);
	Q_ASSERT(polygoneSceneElement);
	Q_ASSERT(pageElement);

	QPolygonF polygon;
	polygon.reserve(pageElement->GetNodesCount());

	for (int i = 0; i < pageElement->GetNodesCount(); ++i) {
		const i2d::CVector2d& node = pageElement->GetNodePos(i);
		polygon.append(MapPointToScene(node));
	}

	polygoneSceneElement->setPolygon(polygon);
}


void CReportDocumentViewComp::ConvertImageCoodinates(const imtreport::CImageRectangleElement* pageElement, QGraphicsItem* sceneElement) const
{
	QGraphicsPixmapItem* imageSceneElement = dynamic_cast<QGraphicsPixmapItem*>(sceneElement);
	Q_ASSERT(imageSceneElement);
	Q_ASSERT(pageElement);

	QRectF rect = MapRectToScene(iqt::GetQRectF(*pageElement));
	imageSceneElement->setPos(rect.topLeft());

	QSize pixmapSize = imageSceneElement->pixmap().size();

	if (pixmapSize.width() > 0) {
		qreal scale = rect.width() / pixmapSize.width();
		imageSceneElement->setScale(scale);
	}
	else {
		imageSceneElement->setScale(1.0);
	}
}


} // namespace imtreportgui


