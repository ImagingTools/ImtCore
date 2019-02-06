#pragma once


// ACF includes
#include <ibase/ICommandsProvider.h>
#include <imtreportgui/CReportDocumentPdfPersistenceComp.h>
#include <imtreportgui/CReportDocumentTiffPersistenceComp.h>
#include <iqtgui/TDesignerGuiObserverCompBase.h>
#include <iqtgui/CHierarchicalCommand.h>

// ImtCore includes
#include <imtreport/IReportDocument.h>
#include <GeneratedFiles/imtreportgui/ui_CReportDocumentViewComp.h>


class QGraphicsRectItem;
class QGraphicsItemGroup;


namespace imtreportgui
{


class CReportDocumentViewComp:
			public iqtgui::TDesignerGuiObserverCompBase<Ui::CReportDocumentViewComp, imtreport::IReportDocument>,
			virtual public ibase::ICommandsProvider,
			virtual public ilog::IMessageConsumer
{
	Q_OBJECT

public:
	typedef iqtgui::TDesignerGuiObserverCompBase<Ui::CReportDocumentViewComp, imtreport::IReportDocument> BaseClass;

	I_BEGIN_COMPONENT(CReportDocumentViewComp);
		I_REGISTER_INTERFACE(ibase::ICommandsProvider);
		I_REGISTER_INTERFACE(ilog::IMessageConsumer);
		I_ASSIGN(m_pdfExportCompPtr, "ReportDocumentPdfPersistence", "Export report document to PDF", false, "ReportDocumentPdfPersistence");
		I_ASSIGN(m_tiffExportCompPtr, "ReportDocumentTiffPersistence", "Export report document to TIFF", false, "ReportDocumentTiffPersistence");
	I_END_COMPONENT;

	CReportDocumentViewComp();

	// reimplemented (ibase::ICommandsProvider)
	const ibase::IHierarchicalCommand* GetCommands() const override;

	// reimplemented (ilog::IMessageConsumer)
	bool IsMessageSupported(
		int messageCategory = -1,
		int messageId = -1,
		const istd::IInformationProvider* messagePtr = NULL) const override;
	void AddMessage(const MessagePtr& messagePtr) override;

protected:
	// reimplemented (iqtgui::TGuiObserverWrap)
	void UpdateGui(const istd::IChangeable::ChangeSet& changeSet) override;

	// reimplemented (iqtgui::CGuiComponentBase)
	void OnGuiCreated() override;
	void OnGuiDestroyed() override;
	void OnGuiRetranslate() override;

private Q_SLOTS:
	void OnShowGrid();
	void OnExportToPdf();
	void OnExportToTiff();

private:
	void CreateSceneDecoration(QGraphicsScene& scene);
	void ShowSceneDecoration(const bool showBorder, const bool showGrid);

private:
	// commands
	iqtgui::CHierarchicalCommand m_rootCommands;
	iqtgui::CHierarchicalCommand m_fileCommands;
	iqtgui::CHierarchicalCommand m_viewCommands;
	iqtgui::CHierarchicalCommand m_exportToPdfCommand;
	iqtgui::CHierarchicalCommand m_exportToTiffCommand;
	iqtgui::CHierarchicalCommand m_showGridCommand;

	typedef QPair<QGraphicsRectItem*, QGraphicsItemGroup*> SceneDecoration; // scene border and scene grid
	typedef QVector<SceneDecoration> SceneDecorations;
	SceneDecorations m_sceneDecorations;
	bool m_isGridShown;

	I_REF(ifile::IFilePersistence, m_pdfExportCompPtr);
	I_REF(ifile::IFilePersistence, m_tiffExportCompPtr);
};


} // namespace imtreportgui



