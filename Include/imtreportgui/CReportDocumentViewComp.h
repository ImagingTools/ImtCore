#pragma once


// ACF includes
#include <iqtgui/TDesignerGuiObserverCompBase.h>
#include <ibase/ICommandsProvider.h>
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
	void CreateSceneDecoration(QGraphicsScene& scene);
	void ShowSceneDecoration(const bool showBorder, const bool showGrid);

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
};


} // namespace imtreportgui



