// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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
#include <iqtgui/TMakeIconProviderCompWrap.h>
#include <iqtgui/TDesignSchemaHandlerWrap.h>
#include <iqtgui/CHierarchicalCommand.h>
#include <iqtgui/CCommandTools.h>
#include <iwidgets/iwidgets.h>
#include <idoc/IDocumentManager.h>

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

	enum CommandOptions
	{
		CO_SHOW_NEW = 1,
		CO_SHOW_OPEN = 2,
		CO_SHOW_SAVE = 4,
		CO_SHOW_SAVE_AS = 8,
		CO_SHOW_ALL = CO_SHOW_NEW | CO_SHOW_OPEN | CO_SHOW_SAVE | CO_SHOW_SAVE_AS
	};

	typedef WorkspaceImpl Workspace;

	struct DecoratorConfiguration
	{
		DecoratorConfiguration()
			:iconSize(16),
			fileButtonsStyle(Qt::ToolButtonFollowStyle),
			documentButtonsStyle(Qt::ToolButtonFollowStyle),
			undoButtonsStyle(Qt::ToolButtonFollowStyle),
			showDocumentTitle(true),
			showDocumentControlFrame(false)
		{
		}

		int iconSize;
		Qt::ToolButtonStyle fileButtonsStyle;
		Qt::ToolButtonStyle documentButtonsStyle;
		Qt::ToolButtonStyle undoButtonsStyle;
		int commandOptions = CO_SHOW_SAVE;
		bool showDocumentTitle;
		bool showDocumentControlFrame;
		QVector<int> includedCommandGroups;
		QVector<int> excludedCommandGroups;
	};

	TStandardDocumentViewDecorator(
				WorkspaceImpl* parentPtr,
				istd::IPolymorphic* viewPtr,
				QWidget* parentWidgetPtr,
				const ifile::IFilePersistence* persistencePtr,
				const DecoratorConfiguration& configuration);

	virtual void UpdateButtonsStatus();
	virtual void UpdateAppearance();

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
	virtual void OnViewContraintsChanged();

	// reimplemented (imod::CMultiModelDispatcherBase)
	virtual void OnModelChanged(int modelId, const istd::IChangeable::ChangeSet& changeSet);

protected:
	istd::IPolymorphic* m_viewObjectPtr;
	const ifile::IFilePersistence* m_filePersistencePtr;
	idoc::IDocumentManager* m_parentPtr;
	bool m_isInitialized;
	QString m_documentName;
	QString m_comment;

	bool m_isUndoEnabled;
	bool m_isRedoEnabled;

	DecoratorConfiguration m_configuration;

	iqtgui::CHierarchicalCommand m_commands;

	iqtgui::CHierarchicalCommand m_newCommand;
	iqtgui::CHierarchicalCommand m_openCommand;
	iqtgui::CHierarchicalCommand m_saveCommand;
	iqtgui::CHierarchicalCommand m_saveAsCommand;
	iqtgui::CHierarchicalCommand m_undoCommand;
	iqtgui::CHierarchicalCommand m_redoCommand;
	iqtgui::CHierarchicalCommand m_closeCommand;

	class UiResourcesManager: public iqtgui::TMakeIconProviderCompWrap<QObject>
	{
	public:
		typedef iqtgui::TMakeIconProviderCompWrap<QObject> BaseClass;

		UiResourcesManager(TStandardDocumentViewDecorator& parent)
			:m_parent(parent)
		{
		}

	protected:
		virtual void OnDesignSchemaChanged(const QByteArray& themeId) override
		{
			BaseClass::OnDesignSchemaChanged(themeId);

			m_parent.UpdateAppearance();
		}

		TStandardDocumentViewDecorator& m_parent;
	};


	UiResourcesManager m_uiResourcesManager;
};


// public methods
template <class WorkspaceImpl, class UI>
TStandardDocumentViewDecorator<WorkspaceImpl, UI>::TStandardDocumentViewDecorator(
			WorkspaceImpl* parentPtr,
			istd::IPolymorphic* viewPtr,
			QWidget* parentWidgetPtr,
			const ifile::IFilePersistence* persistencePtr,
			const DecoratorConfiguration& configuration)
	:QWidget(parentWidgetPtr),
	m_viewObjectPtr(viewPtr),
	m_filePersistencePtr(persistencePtr),
	m_parentPtr(parentPtr),
	m_isInitialized(false),
	m_configuration(configuration),
	m_newCommand("New", 110, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR, 20000),
	m_openCommand("Open", 109, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR, 20000),
	m_saveCommand("Save", 108, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR, 20000),
	m_saveAsCommand("SaveAs", 100, ibase::ICommand::CF_GLOBAL_MENU, 20000),
	m_undoCommand("Undo", 99, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR, 20000),
	m_redoCommand("Redo", 99, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR, 20000),
	m_closeCommand("Close", 99, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR, 20001),
	m_uiResourcesManager(*this)
{
	m_uiResourcesManager.EnableDesignHandler();

	Q_ASSERT(parentPtr != nullptr);
	Q_ASSERT(viewPtr != nullptr);

	UI::setupUi(this);

	setObjectName("DocumentView");

	UI::DocumentTitleFrame->setVisible(configuration.showDocumentTitle);

	istd::IChangeable* documentPtr = m_parentPtr->GetDocumentFromView(*viewPtr);
	Q_ASSERT(documentPtr != nullptr);

	if (configuration.commandOptions & CO_SHOW_NEW){
		m_commands.InsertChild(&m_newCommand);
	}

	if (configuration.commandOptions & CO_SHOW_OPEN){
		m_commands.InsertChild(&m_openCommand);
	}

	m_commands.InsertChild(&m_undoCommand);
	m_commands.InsertChild(&m_redoCommand);

	if (configuration.commandOptions & CO_SHOW_SAVE){
		m_commands.InsertChild(&m_saveCommand);
	}

	if (configuration.commandOptions & CO_SHOW_SAVE_AS){
		m_commands.InsertChild(&m_saveAsCommand);
	}

	m_commands.InsertChild(&m_closeCommand);

	m_isUndoEnabled = false;
	m_isRedoEnabled = false;

	idoc::IUndoManager* undoManagerPtr = m_parentPtr->GetUndoManagerForDocument(documentPtr);

	UI::UndoButton->setVisible(undoManagerPtr != nullptr);
	UI::RedoButton->setVisible(undoManagerPtr != nullptr);
	UI::UndoButton->setIconSize(QSize(configuration.iconSize, configuration.iconSize));
	UI::RedoButton->setIconSize(QSize(configuration.iconSize, configuration.iconSize));
	m_undoCommand.setVisible(undoManagerPtr != nullptr);
	m_redoCommand.setVisible(undoManagerPtr != nullptr);

	m_newCommand.setShortcut(Qt::CTRL | Qt::Key_N);
	m_openCommand.setShortcut(Qt::CTRL | Qt::Key_O);
	m_saveCommand.setShortcut(Qt::CTRL | Qt::Key_S);
	m_undoCommand.setShortcut(Qt::CTRL | Qt::Key_Z);
	m_redoCommand.setShortcut(Qt::CTRL | Qt::SHIFT | Qt::Key_Z);

	idoc::IDocumentMetaInfo* metaInfoPtr = CompCastPtr<idoc::IDocumentMetaInfo>(documentPtr);
	if (metaInfoPtr != nullptr){
		m_documentName = metaInfoPtr->GetMetaInfo(idoc::IDocumentMetaInfo::MIT_TITLE).toString();

		m_comment = metaInfoPtr->GetMetaInfo(idoc::IDocumentMetaInfo::MIT_DESCRIPTION).toString();
	}

	UI::NewButton->setToolButtonStyle(configuration.fileButtonsStyle);
	UI::NewButton->setDefaultAction(&m_newCommand);
	UI::NewButton->setVisible(configuration.commandOptions & CO_SHOW_NEW);
	UI::NewButton->setIconSize(QSize(configuration.iconSize, configuration.iconSize));

	UI::OpenButton->setToolButtonStyle(configuration.fileButtonsStyle);
	UI::OpenButton->setDefaultAction(&m_openCommand);
	UI::OpenButton->setVisible(configuration.commandOptions & CO_SHOW_OPEN);
	UI::OpenButton->setIconSize(QSize(configuration.iconSize, configuration.iconSize));

	UI::UndoButton->setToolButtonStyle(configuration.undoButtonsStyle);
	UI::UndoButton->setDefaultAction(&m_undoCommand);

	UI::RedoButton->setToolButtonStyle(configuration.undoButtonsStyle);
	UI::RedoButton->setDefaultAction(&m_redoCommand);

	UI::CloseButton->setDefaultAction(&m_closeCommand);

	UI::SaveButton->setToolButtonStyle(configuration.fileButtonsStyle);
	UI::SaveButton->setDefaultAction(&m_saveCommand);
	UI::SaveButton->setVisible(configuration.commandOptions & CO_SHOW_SAVE);
	UI::SaveButton->setIconSize(QSize(configuration.iconSize, configuration.iconSize));

	UI::SaveAsButton->setToolButtonStyle(configuration.fileButtonsStyle);
	UI::SaveAsButton->setDefaultAction(&m_saveAsCommand);
	UI::SaveAsButton->setVisible(configuration.commandOptions & CO_SHOW_SAVE_AS);

	connect(&m_newCommand, &QAction::triggered, parentPtr, &WorkspaceImpl::OnNew);
	connect(&m_openCommand, &QAction::triggered, parentPtr, &WorkspaceImpl::OnOpen);
	connect(&m_undoCommand, &QAction::triggered, parentPtr, &WorkspaceImpl::OnUndo);
	connect(&m_redoCommand, &QAction::triggered, parentPtr, &WorkspaceImpl::OnRedo);
	connect(&m_closeCommand, &QAction::triggered, parentPtr, &WorkspaceImpl::OnCloseDocument);
	connect(&m_saveCommand, &QAction::triggered, parentPtr, &WorkspaceImpl::OnSaveDocument);
	connect(&m_saveAsCommand, &QAction::triggered, parentPtr, &WorkspaceImpl::OnSaveDocumentAs);

	UpdateButtonsStatus();

	UpdateAppearance();

	m_isInitialized = true;

	const imtgui::IDocumentViewConstraints* viewConstraintsPtr = CompCastPtr<imtgui::IDocumentViewConstraints>(viewPtr);
	if (viewConstraintsPtr != nullptr){
		imod::IModel* viewModelPtr = const_cast<imod::IModel*>(dynamic_cast<const imod::IModel*>(viewConstraintsPtr));
		if (viewModelPtr != nullptr){
			RegisterModel(viewModelPtr, MI_VIEW_CONSTRAINTS);
		}
	}

	imod::IModel* modelPtr = dynamic_cast<imod::IModel*>(undoManagerPtr);
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
		else{
			OnModelChanged(MI_VIEW_COMMANDS, istd::IChangeable::GetAllChanges());
		}
	}

	UI::HeaderFrame->setVisible(configuration.showDocumentControlFrame);
}


template <class WorkspaceImpl, class UI>
void TStandardDocumentViewDecorator<WorkspaceImpl, UI>::UpdateButtonsStatus()
{
	bool isSaveActive = true;

	idoc::IUndoManager* undoManagerPtr = this->template GetObjectAt<idoc::IUndoManager>(MI_UNDO_MANAGER);
	if (undoManagerPtr != nullptr){
		isSaveActive = isSaveActive && (undoManagerPtr->GetDocumentChangeFlag() != idoc::IDocumentStateComparator::DCF_EQUAL);
	}

	idoc::IDocumentManager::DocumentInfo documentInfo = {};
	m_parentPtr->GetDocumentFromView(*m_viewObjectPtr, &documentInfo);

	isSaveActive = isSaveActive || documentInfo.isDirty;

	if (m_filePersistencePtr == nullptr){
		isSaveActive = false;
	}

	m_saveCommand.setEnabled(isSaveActive);
	UI::SaveButton->setEnabled(isSaveActive);

	m_saveAsCommand.setEnabled(m_filePersistencePtr != nullptr);
	UI::SaveAsButton->setEnabled(m_filePersistencePtr != nullptr);

	const imtgui::IDocumentViewConstraints* viewConstraintsPtr = CompCastPtr<imtgui::IDocumentViewConstraints>(m_viewObjectPtr);
	if (viewConstraintsPtr != nullptr){
		bool isSaveEnabled = viewConstraintsPtr->GetViewConstraints() & imtgui::IDocumentViewConstraints::CF_SAVE_DOCUMENT;

		m_saveCommand.setVisible(isSaveEnabled);
		UI::SaveButton->setVisible(isSaveEnabled);

		m_saveAsCommand.setVisible(isSaveEnabled);
		UI::SaveAsButton->setVisible(isSaveEnabled);
	}

	UI::UndoButton->setEnabled(m_isUndoEnabled);
	UI::RedoButton->setEnabled(m_isRedoEnabled);
	m_undoCommand.setEnabled(m_isUndoEnabled);
	m_redoCommand.setEnabled(m_isRedoEnabled);

	UI::CloseButton->setEnabled(true);
}


template<class WorkspaceImpl, class UI>
inline void TStandardDocumentViewDecorator<WorkspaceImpl, UI>::UpdateAppearance()
{
	m_newCommand.SetVisuals(QObject::tr("&New"), QObject::tr("New"), QObject::tr("Create new document"), m_uiResourcesManager.GetIcon(":/Icons/New"));
	m_openCommand.SetVisuals(QObject::tr("&Open..."), QObject::tr("Open..."), QObject::tr("Open an existing document"), m_uiResourcesManager.GetIcon(":/Icons/Open"));
	m_undoCommand.SetVisuals(QObject::tr("&Undo"), QObject::tr("Undo"), QObject::tr("Undo last document changes"), m_uiResourcesManager.GetIcon(":/Icons/Undo"));
	m_redoCommand.SetVisuals(QObject::tr("&Redo"), QObject::tr("Redo"), QObject::tr("Redo last document changes"), m_uiResourcesManager.GetIcon(":/Icons/Redo"));
	m_closeCommand.SetVisuals(QObject::tr("&Close"), QObject::tr("Close"), QObject::tr("Close the document"), m_uiResourcesManager.GetIcon(":/Icons/Close"));
	m_saveCommand.SetVisuals(QObject::tr("&Save"), QObject::tr("Save"), QObject::tr("Save the document changes"), m_uiResourcesManager.GetIcon(":/Icons/Save"));
	m_saveAsCommand.SetVisuals(QObject::tr("Save As"), QObject::tr("Save As"), QObject::tr("Save the document as..."), QIcon());
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
		UI::SaveAsButton->setVisible((viewFlags & imtgui::IDocumentViewConstraints::CF_SAVE_DOCUMENT));
		UI::UndoButton->setVisible((viewFlags & imtgui::IDocumentViewConstraints::CF_UNDO_SUPPORT));
		UI::RedoButton->setVisible((viewFlags & imtgui::IDocumentViewConstraints::CF_UNDO_SUPPORT));
		UI::CloseButton->setVisible((viewFlags & imtgui::IDocumentViewConstraints::CF_CLOSE_SUPPORT));
		m_saveCommand.setVisible((viewFlags & imtgui::IDocumentViewConstraints::CF_SAVE_DOCUMENT));
		m_saveAsCommand.setVisible((viewFlags & imtgui::IDocumentViewConstraints::CF_SAVE_DOCUMENT));
		m_undoCommand.setVisible((viewFlags & imtgui::IDocumentViewConstraints::CF_UNDO_SUPPORT));
		m_redoCommand.setVisible((viewFlags & imtgui::IDocumentViewConstraints::CF_UNDO_SUPPORT));
		m_closeCommand.setVisible((viewFlags & imtgui::IDocumentViewConstraints::CF_CLOSE_SUPPORT));
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

	UI::DocumentTitle->setVisible(!title.isEmpty() && m_configuration.showDocumentTitle);
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
			{
				idoc::IUndoManager* undoManagerPtr = this->template GetObjectAt<idoc::IUndoManager>(MI_UNDO_MANAGER);
				Q_ASSERT(undoManagerPtr != nullptr);

				m_isUndoEnabled = undoManagerPtr->GetAvailableUndoSteps() > 0;
				m_isRedoEnabled = undoManagerPtr->GetAvailableRedoSteps() > 0;
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
					toolBarPtr->setToolButtonStyle(m_configuration.documentButtonsStyle);
					UI::CommandToolBarFrame->layout()->addWidget(toolBarPtr);
					iqtgui::CCommandTools::SetupToolbar(*guiCommandPtr, *toolBarPtr, -1, m_configuration.includedCommandGroups, m_configuration.excludedCommandGroups);
					toolBarPtr->setIconSize(QSize(m_configuration.iconSize, m_configuration.iconSize));
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

	UpdateButtonsStatus();
}


} // namespace imtgui


