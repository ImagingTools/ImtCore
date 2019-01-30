#pragma once


// ACF includes
#include <iqtgui/TDesignerGuiObserverCompBase.h>
#include <ibase/ICommandsProvider.h>
#include <iqtgui/CHierarchicalCommand.h>

// ImtCore includes
#include <imtreport/IReportDocument.h>
#include <imtreport/IGraphicsElement.h>
#include <imtreport/TGraphicsElement.h>
#include <imtreport/CTextTable.h>
#include <GeneratedFiles/imtreportgui/ui_CReportDocumentViewComp.h>


class QTableWidgetItem;


namespace imtreportgui
{


class CReportDocumentViewComp:
			public iqtgui::TDesignerGuiObserverCompBase<Ui::CReportDocumentViewComp, imtreport::IReportDocument>,
			public virtual ibase::ICommandsProvider
{
	Q_OBJECT

public:
	typedef iqtgui::TDesignerGuiObserverCompBase<Ui::CReportDocumentViewComp, imtreport::IReportDocument> BaseClass;

	I_BEGIN_COMPONENT(CReportDocumentViewComp);
		I_REGISTER_INTERFACE(ibase::ICommandsProvider);
	I_END_COMPONENT;

	CReportDocumentViewComp();

	// reimplemented (ibase::ICommandsProvider)
	virtual const ibase::IHierarchicalCommand* GetCommands() const override;

protected:
	// reimplemented (iqtgui::TGuiObserverWrap)
	virtual void UpdateGui(const istd::IChangeable::ChangeSet& changeSet) override;

	// reimplemented (iqtgui::CGuiComponentBase)
	virtual void OnGuiCreated() override;
	virtual void OnGuiDestroyed() override;
	virtual void OnGuiRetranslate() override;

private Q_SLOTS:
	void OnShowGrid();
	void OnExportToPdf();

private:
	void UpdateSceneDecoration(QGraphicsScene& scene);
	void UpdateSceneShapes(const imtreport::IReportPage& page, QGraphicsScene& scene);
	void ShowSceneDecoration(const bool showBorder, const bool showGrid);

	QPointF MapPointToScene(const QPointF& point) const;
	QRectF MapRectToScene(const QRectF& rect) const;
	QFont MapFontToScene(const QString& fontName, double fontSize) const;

	void ConvertShapeCoodinates(const i2d::IObject2d* pageElement, QGraphicsItem* sceneElement) const;
	void ConvertRectCoodinates(const imtreport::CRectangleElement* pageElement, QGraphicsItem* sceneElement) const;
	void ConvertEllipseCoodinates(const imtreport::CCircleElement* pageElement, QGraphicsItem* sceneElement) const;
	void ConvertLabelCoodinates(const imtreport::CTextLabelElement* pageElement, QGraphicsItem* sceneElement) const;
	void ConvertLineCoodinates(const imtreport::CLineElement* pageElement, QGraphicsItem* sceneElement) const;
	void ConvertPolygoneCoodinates(const imtreport::CPolygonElement* pageElement, QGraphicsItem* sceneElement) const;
	void ConvertImageCoodinates(const imtreport::CImageRectangleElement* pageElement, QGraphicsItem* sceneElement) const;
	void ConvertTableCoodinates(const imtreport::CTextTable* pageElement, QGraphicsItem* sceneElement) const;

	QTableWidgetItem* ConvertTableItem(const imtreport::CTextTableItem& tableItem) const;

private:
	// commands
	iqtgui::CHierarchicalCommand m_rootCommands;
	iqtgui::CHierarchicalCommand m_fileCommands;
	iqtgui::CHierarchicalCommand m_viewCommands;
	iqtgui::CHierarchicalCommand m_exportToPdfCommand;
	iqtgui::CHierarchicalCommand m_showGridCommand;

	typedef QPair<QGraphicsRectItem*, QGraphicsItemGroup*> SceneDecoration; // scene border and scene grid
	typedef QVector<SceneDecoration> SceneDecorations;
	SceneDecorations m_sceneDecorations;
	bool m_isGridShown = true;

	static const qreal s_A4WidthMm;
	static const qreal s_A4HeightMm;
	static const qreal s_MmPerInch;
};


} // namespace imtreportgui



