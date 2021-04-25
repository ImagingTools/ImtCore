#pragma once


// ACF includes
#include <iauth/ILogin.h>
#include <ilog/IMessageConsumer.h>

// ImtCore includes
#include <imtgui/CMultiDocumentWorkspaceGuiCompBase.h>
#include <GeneratedFiles/imtgui/ui_CStandardDocumentViewDecorator.h>


namespace imtgui
{


class CDocumentWorkspaceGuiComp: public imtgui::CMultiDocumentWorkspaceGuiCompBase
{
	Q_OBJECT

public:
	typedef imtgui::CMultiDocumentWorkspaceGuiCompBase BaseClass;

	I_BEGIN_COMPONENT(CDocumentWorkspaceGuiComp);
		I_ASSIGN(m_loginCompPtr, "Login", "Login component", false, "Login");
		I_ASSIGN(m_logCompPtr, "Log", "Message consumer component", false, "Log");
		I_ASSIGN(m_tabStyleSheetPropertyAttrPtr, "TabStyleSheetProperty", "Enable a stylesheet with a property for the tab", true, "");
	I_END_COMPONENT;

protected:
	// reimplemented (imtgui::CMultiDocumentWorkspaceGuiCompBase)
	virtual IDocumentViewDecorator* CreateDocumentViewDecorator(
				istd::IPolymorphic* viewPtr,
				QWidget* parentWidgetPtr,
				const SingleDocumentData& documentData,
				const ifile::IFilePersistence* persistencePtr) override;

	// reimplemented (idoc::CMultiDocumentManagerBase)
	virtual QString GetSaveFilePath(const QByteArray& documentTypeId, const istd::IChangeable* dataObjectPtr, const QString& currentFilePath) const override;
	virtual QStringList GetOpenFilePaths(const QByteArray* documentTypeIdPtr = NULL) const override;

	// reimplemented (iqt:CGuiComponentBase)
	virtual void OnGuiCreated() override;

protected Q_SLOTS:
	void OnSaveDocument();

private:
	enum DataRole
	{
		DR_PATH = Qt::UserRole,
		DR_TYPE_ID
	};

	friend class CollectionDocumentViewDecorator;

	I_ATTR(QByteArray, m_tabStyleSheetPropertyAttrPtr);
	I_REF(iauth::ILogin, m_loginCompPtr);
	I_REF(ilog::IMessageConsumer, m_logCompPtr);
};


class CollectionDocumentViewDecorator:
			public QWidget,
			public Ui::CStandardDocumentViewDecorator,
			virtual public IDocumentViewDecorator,
			virtual public ibase::ICommandsProvider,
			protected imod::CMultiModelDispatcherBase
{
public:
	enum ModelId
	{
		MI_VIEW_COMMANDS,
		MI_VIEW_CONSTRAINTS,
		MI_UNDO_MANAGER,
		MI_DOCUMENT_META_INFO
	};

	CollectionDocumentViewDecorator(
		CDocumentWorkspaceGuiComp* parentPtr,
		istd::IPolymorphic* viewPtr,
		QWidget* parentWidgetPtr,
		CDocumentWorkspaceGuiComp::SingleDocumentData& documentData,
		const ifile::IFilePersistence* persistencePtr);

	void UpdateSaveButtonsStatus();
	CDocumentWorkspaceGuiComp::SingleDocumentData& GetDocumentData() const;

	// reimplemeneted (IDocumentViewDecorator)
	virtual QWidget* GetDecoratorWidget() override;
	virtual QWidget* GetViewFrame() override;
	virtual istd::IPolymorphic* GetView() const override;
	virtual void SetViewEnabled(bool isEnabled) override;
	virtual QString GetTitle() override;
	virtual void SetTitle(const QString& title) override;
	virtual void SetDocumentTypeName(const QString& name) override;

	// reimplemented (ibase::ICommandsProvider)
	virtual const ibase::IHierarchicalCommand* GetCommands() const override;

protected:
	void OnViewContraintsChanged();

	// reimplemented (imod::CMultiModelDispatcherBase)
	virtual void OnModelChanged(int modelId, const istd::IChangeable::ChangeSet& changeSet);

private:
	CDocumentWorkspaceGuiComp::SingleDocumentData& m_documentData;
	istd::IPolymorphic* m_viewObjectPtr;
	idoc::IUndoManager* m_undoManagerPtr;
	const ifile::IFilePersistence* m_filePersistencePtr;
	CDocumentWorkspaceGuiComp* m_parentPtr;
	bool m_isInitialized;
	QString m_documentName;
	QString m_comment;

	iqtgui::CHierarchicalCommand m_commands;
	iqtgui::CHierarchicalCommand m_saveCommand;
	iqtgui::CHierarchicalCommand m_undoCommand;
	iqtgui::CHierarchicalCommand m_redoCommand;
	iqtgui::CHierarchicalCommand m_closeCommand;
};


} // namespace imtgui


