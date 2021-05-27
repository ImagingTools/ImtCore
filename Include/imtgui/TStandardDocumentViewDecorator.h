#pragma once


// Qt includes
#include <QtWidgets/QToolButton>

// ACF includes
#include <imod/IModelEditor.h>
#include <imod/IModel.h>
#include <imod/CMultiModelDispatcherBase.h>
#include <ilog/IMessageConsumer.h>
#include <icomp/CComponentBase.h>
#include <ibase/ICommandsProvider.h>
#include <ifile/IFilePersistence.h>
#include <idoc/IDocumentMetaInfo.h>
#include <iqtgui/CHierarchicalCommand.h>
#include <iqtgui/CCommandTools.h>
#include <iwidgets/iwidgets.h>

// ImtCore includes
#include <imtgui/IDocumentViewConstraints.h>
#include <imtgui/IDocumentViewDecorator.h>


namespace imtgui
{


template <class WorkspaceImpl, class UI>
class TStandardDocumentViewDecorator:
			public QWidget,
			public UI,
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

	typedef WorkspaceImpl Workspace;

	TStandardDocumentViewDecorator(
				WorkspaceImpl* parentPtr,
				istd::IPolymorphic* viewPtr,
				QWidget* parentWidgetPtr,
				const ifile::IFilePersistence* persistencePtr);

	void UpdateSaveButtonsStatus();

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

protected:
	istd::IPolymorphic* m_viewObjectPtr;
	idoc::IUndoManager* m_undoManagerPtr;
	const ifile::IFilePersistence* m_filePersistencePtr;
	idoc::IDocumentManager* m_parentPtr;
	bool m_isInitialized;
	QString m_documentName;
	QString m_comment;

	iqtgui::CHierarchicalCommand m_commands;

	iqtgui::CHierarchicalCommand m_newCommand;
	iqtgui::CHierarchicalCommand m_openCommand;
	iqtgui::CHierarchicalCommand m_saveCommand;
	iqtgui::CHierarchicalCommand m_saveAsCommand;
	iqtgui::CHierarchicalCommand m_undoCommand;
	iqtgui::CHierarchicalCommand m_redoCommand;
	iqtgui::CHierarchicalCommand m_closeCommand;
};


// public methods
template <class WorkspaceImpl, class UI>
TStandardDocumentViewDecorator<WorkspaceImpl, UI>::TStandardDocumentViewDecorator(
			WorkspaceImpl* parentPtr,
			istd::IPolymorphic* viewPtr,
			QWidget* parentWidgetPtr,
			const ifile::IFilePersistence* persistencePtr)
:	QWidget(parentWidgetPtr),
	m_viewObjectPtr(viewPtr),
	m_filePersistencePtr(persistencePtr),
	m_parentPtr(parentPtr),
	m_isInitialized(false),
	m_newCommand("New", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR, 20000),
	m_openCommand("Open", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR, 20000),
	m_saveCommand("Save", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR, 20000),
	m_saveAsCommand("SaveAs", 100, ibase::ICommand::CF_GLOBAL_MENU, 20000),
	m_undoCommand("Undo", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR, 20000),
	m_redoCommand("Redo", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR, 20000),
	m_closeCommand("Close", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR, 20001)
{
	Q_ASSERT(parentPtr != nullptr);
	Q_ASSERT(viewPtr != nullptr);

	UI::setupUi(this);

	setObjectName("DocumentView");

	istd::IChangeable* documentPtr = m_parentPtr->GetDocumentFromView(*viewPtr);
	Q_ASSERT(documentPtr != nullptr);

	m_undoManagerPtr = m_parentPtr->GetUndoManagerForDocument(documentPtr);

	m_commands.InsertChild(&m_newCommand);
	m_commands.InsertChild(&m_openCommand);
	m_commands.InsertChild(&m_undoCommand);
	m_commands.InsertChild(&m_redoCommand);
	m_commands.InsertChild(&m_saveCommand);
	m_commands.InsertChild(&m_saveAsCommand);
	m_commands.InsertChild(&m_closeCommand);

	UI::UndoButton->setEnabled(false);
	UI::RedoButton->setEnabled(false);
	m_undoCommand.setEnabled(false);
	m_redoCommand.setEnabled(false);

	UI::UndoButton->setVisible(m_undoManagerPtr != nullptr);
	UI::RedoButton->setVisible(m_undoManagerPtr != nullptr);
	m_undoCommand.setVisible(m_undoManagerPtr != nullptr);
	m_redoCommand.setVisible(m_undoManagerPtr != nullptr);

	m_newCommand.setShortcut(Qt::CTRL + Qt::Key_N);
	m_openCommand.setShortcut(Qt::CTRL + Qt::Key_O);
	m_saveCommand.setShortcut(Qt::CTRL + Qt::Key_S);
	m_undoCommand.setShortcut(Qt::CTRL + Qt::Key_Z);
	m_redoCommand.setShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_Z);

	idoc::IDocumentMetaInfo* metaInfoPtr = CompCastPtr<idoc::IDocumentMetaInfo>(documentPtr);
	if (metaInfoPtr != nullptr){
		m_documentName = metaInfoPtr->GetMetaInfo(idoc::IDocumentMetaInfo::MIT_TITLE).toString();

		m_comment = metaInfoPtr->GetMetaInfo(idoc::IDocumentMetaInfo::MIT_DESCRIPTION).toString();
	}

	connect(UI::NewButton, &QToolButton::clicked, parentPtr, &WorkspaceImpl::OnNew);
	connect(UI::OpenButton, &QToolButton::clicked, parentPtr, &WorkspaceImpl::OnOpen);
	connect(UI::UndoButton, &QToolButton::clicked, parentPtr, &WorkspaceImpl::OnUndo);
	connect(UI::RedoButton, &QToolButton::clicked, parentPtr, &WorkspaceImpl::OnRedo);
	connect(UI::CloseButton, &QToolButton::clicked, parentPtr, &WorkspaceImpl::OnCloseDocument);
	connect(UI::SaveButton, &QToolButton::clicked, parentPtr, &WorkspaceImpl::OnSaveDocument);

	connect(&m_newCommand, &QAction::triggered, parentPtr, &WorkspaceImpl::OnNew);
	connect(&m_openCommand, &QAction::triggered, parentPtr, &WorkspaceImpl::OnOpen);
	connect(&m_undoCommand, &QAction::triggered, parentPtr, &WorkspaceImpl::OnUndo);
	connect(&m_redoCommand, &QAction::triggered, parentPtr, &WorkspaceImpl::OnRedo);
	connect(&m_closeCommand, &QAction::triggered, parentPtr, &WorkspaceImpl::OnCloseDocument);
	connect(&m_saveCommand, &QAction::triggered, parentPtr, &WorkspaceImpl::OnSaveDocument);
	connect(&m_saveAsCommand, &QAction::triggered, parentPtr, &WorkspaceImpl::OnSaveDocumentAs);

	m_newCommand.SetVisuals(QObject::tr("&New"), QObject::tr("New"), QObject::tr("Create new document"), QIcon(":/Icons/New"));
	m_openCommand.SetVisuals(QObject::tr("&Open..."), QObject::tr("Open..."), QObject::tr("Open an existing document"), QIcon(":/Icons/Open"));
	m_undoCommand.SetVisuals(QObject::tr("&Undo"), QObject::tr("Undo"), QObject::tr("Undo last document changes"), QIcon(":/Icons/Undo"));
	m_redoCommand.SetVisuals(QObject::tr("&Redo"), QObject::tr("Redo"), QObject::tr("Redo last document changes"), QIcon(":/Icons/Redo"));
	m_closeCommand.SetVisuals(QObject::tr("&Close"), QObject::tr("Close"), QObject::tr("Close the document"), QIcon(":/Icons/Close"));
	m_saveCommand.SetVisuals(QObject::tr("&Save"), QObject::tr("Save"), QObject::tr("Save the document changes"), QIcon(":/Icons/Save"));
	m_saveAsCommand.SetVisuals(QObject::tr("Save As"), QObject::tr("Save As"), QObject::tr("Save the document as..."), QIcon());

	UpdateSaveButtonsStatus();

	m_isInitialized = true;

	const imtgui::IDocumentViewConstraints* viewConstraintsPtr = CompCastPtr<imtgui::IDocumentViewConstraints>(viewPtr);
	if (viewConstraintsPtr != nullptr){
		imod::IModel* viewModelPtr = const_cast<imod::IModel*>(dynamic_cast<const imod::IModel*>(viewConstraintsPtr));
		if (viewModelPtr != nullptr){
			RegisterModel(viewModelPtr, MI_VIEW_CONSTRAINTS);
		}
	}
	
	imod::IModel* modelPtr = dynamic_cast<imod::IModel*>(m_undoManagerPtr);
	if (modelPtr != nullptr){
		RegisterModel(modelPtr, MI_UNDO_MANAGER);
	}

	imod::IModel* documentMetaInfoModelPtr = dynamic_cast<imod::IModel*>(metaInfoPtr);
	if (documentMetaInfoModelPtr != nullptr){
		RegisterModel(documentMetaInfoModelPtr, MI_DOCUMENT_META_INFO);
	}

	ibase::ICommandsProvider* commandsProviderPtr = CompCastPtr<ibase::ICommandsProvider>(viewPtr);
	if (commandsProviderPtr != nullptr){
		imod::IModel* commandsModelPtr = dynamic_cast<imod::IModel*>(commandsProviderPtr);
		if (commandsModelPtr != nullptr){
			RegisterModel(commandsModelPtr, MI_VIEW_COMMANDS);
		}
	}

	UI::HeaderFrame->setVisible(false);
}


template <class WorkspaceImpl, class UI>
void TStandardDocumentViewDecorator<WorkspaceImpl, UI>::UpdateSaveButtonsStatus()
{
	bool isSaveActive = true;

	if (m_undoManagerPtr != nullptr){
		isSaveActive = isSaveActive && (m_undoManagerPtr->GetDocumentChangeFlag() != idoc::IDocumentStateComparator::DCF_EQUAL);
	}

	idoc::IDocumentManager::DocumentInfo documentInfo = {};
	m_parentPtr->GetDocumentFromView(*m_viewObjectPtr, &documentInfo);

	isSaveActive = isSaveActive || documentInfo.isDirty;
 
	if (m_filePersistencePtr == nullptr){
		isSaveActive = false;
	}

	m_saveCommand.setEnabled(isSaveActive);
	UI::SaveButton->setEnabled(isSaveActive);

	const imtgui::IDocumentViewConstraints* viewConstraintsPtr = CompCastPtr<imtgui::IDocumentViewConstraints>(m_viewObjectPtr);
	if (viewConstraintsPtr != nullptr){
		bool isSaveEnabled = viewConstraintsPtr->GetViewConstraints() & imtgui::IDocumentViewConstraints::CF_SAVE_DOCUMENT;

		UI::SaveButton->setVisible(isSaveEnabled);
		m_saveCommand.setVisible(isSaveEnabled);
	}
}


template <class WorkspaceImpl, class UI>
void TStandardDocumentViewDecorator<WorkspaceImpl, UI>::OnViewContraintsChanged()
{
	const imtgui::IDocumentViewConstraints* viewConstraintsPtr = CompCastPtr<imtgui::IDocumentViewConstraints>(m_viewObjectPtr);
	if (viewConstraintsPtr != nullptr){
		int viewFlags = viewConstraintsPtr->GetViewConstraints();

		imod::IModelEditor* modelEditorPtr = CompCastPtr<imod::IModelEditor>(m_viewObjectPtr);
		if (modelEditorPtr != nullptr){
			modelEditorPtr->SetReadOnly(!(viewFlags & imtgui::IDocumentViewConstraints::CF_EDIT_DOCUMENT));
		}

		UI::SaveButton->setVisible((viewFlags & imtgui::IDocumentViewConstraints::CF_SAVE_DOCUMENT));
		UI::UndoButton->setVisible((viewFlags & imtgui::IDocumentViewConstraints::CF_UNDO_SUPPORT));
		UI::RedoButton->setVisible((viewFlags & imtgui::IDocumentViewConstraints::CF_UNDO_SUPPORT));
		m_undoCommand.setVisible((viewFlags & imtgui::IDocumentViewConstraints::CF_UNDO_SUPPORT));
		m_redoCommand.setVisible((viewFlags & imtgui::IDocumentViewConstraints::CF_UNDO_SUPPORT));

		UI::CloseButton->setVisible((viewFlags & imtgui::IDocumentViewConstraints::CF_CLOSE_SUPPORT));
	}
}


// reimplemeneted (IDocumentViewDecorator)

template <class WorkspaceImpl, class UI>
QWidget* TStandardDocumentViewDecorator<WorkspaceImpl, UI>::GetDecoratorWidget()
{
	return this;
}


template <class WorkspaceImpl, class UI>
QWidget* TStandardDocumentViewDecorator<WorkspaceImpl, UI>::GetViewFrame()
{
	return UI::DocumentFrame;
}


template <class WorkspaceImpl, class UI>
istd::IPolymorphic* TStandardDocumentViewDecorator<WorkspaceImpl, UI>::GetView() const
{
	return m_viewObjectPtr;
}


template <class WorkspaceImpl, class UI>
void TStandardDocumentViewDecorator<WorkspaceImpl, UI>::SetViewEnabled(bool isEnabled)
{
	setEnabled(isEnabled);
}


template <class WorkspaceImpl, class UI>
QString TStandardDocumentViewDecorator<WorkspaceImpl, UI>::GetTitle()
{
	return QString();
}


template <class WorkspaceImpl, class UI>
void TStandardDocumentViewDecorator<WorkspaceImpl, UI>::SetTitle(const QString& title)
{
	UI::DocumentTitle->setText(title);

	UI::DocumentTitle->setVisible(!title.isEmpty());
}


template <class WorkspaceImpl, class UI>
void TStandardDocumentViewDecorator<WorkspaceImpl, UI>::SetDocumentTypeName(const QString& /*name*/)
{
}


// reimplemented (ibase::ICommandsProvider)

template <class WorkspaceImpl, class UI>
const ibase::IHierarchicalCommand * TStandardDocumentViewDecorator<WorkspaceImpl, UI>::GetCommands() const
{
	return &m_commands;
}


// protected methods

// reimplemented (imod::CMultiModelDispatcherBase)

template <class WorkspaceImpl, class UI>
void TStandardDocumentViewDecorator<WorkspaceImpl, UI>::OnModelChanged(int modelId, const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	Q_ASSERT(m_parentPtr != nullptr);

	if (!m_isInitialized){
		return;
	}

	switch (modelId){
		case MI_UNDO_MANAGER:
			if (m_undoManagerPtr != nullptr){
				UI::UndoButton->setEnabled(m_undoManagerPtr->GetAvailableUndoSteps() > 0);
				UI::RedoButton->setEnabled(m_undoManagerPtr->GetAvailableRedoSteps() > 0);
				m_undoCommand.setEnabled(m_undoManagerPtr->GetAvailableUndoSteps() > 0);
				m_redoCommand.setEnabled(m_undoManagerPtr->GetAvailableRedoSteps() > 0);
			}
			break;

		case MI_VIEW_COMMANDS:{
			ibase::ICommandsProvider* commandsProviderPtr = CompCastPtr<ibase::ICommandsProvider>(m_viewObjectPtr);
			m_commands.ResetChilds();

			if (commandsProviderPtr != nullptr){
				iwidgets::ClearLayout(UI::CommandToolBarFrame->layout());
				const iqtgui::CHierarchicalCommand* guiCommandPtr = dynamic_cast<const iqtgui::CHierarchicalCommand*>(commandsProviderPtr->GetCommands());
				if (guiCommandPtr != nullptr){
					QToolBar* toolBarPtr = new QToolBar(UI::CommandToolBarFrame);
					toolBarPtr->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
					UI::CommandToolBarFrame->layout()->addWidget(toolBarPtr);
					iqtgui::CCommandTools::SetupToolbar(*guiCommandPtr, *toolBarPtr);
					toolBarPtr->setIconSize(QSize(16, 16));
				}

				const ibase::IHierarchicalCommand* viewCommandsPtr = commandsProviderPtr->GetCommands();
				if (viewCommandsPtr != nullptr){
					m_commands.JoinLinkFrom(viewCommandsPtr);
				}

				m_commands.InsertChild(&m_undoCommand);
				m_commands.InsertChild(&m_redoCommand);
				m_commands.InsertChild(&m_saveCommand);
				m_commands.InsertChild(&m_closeCommand);
			}
		}
		break;

		case MI_VIEW_CONSTRAINTS:
			OnViewContraintsChanged();
			break;

		default:
			break;
	}

	UpdateSaveButtonsStatus();
}


} // namespace imtgui


