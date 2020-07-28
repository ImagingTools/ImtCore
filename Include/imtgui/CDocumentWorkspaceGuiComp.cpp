#include <imtgui/CDocumentWorkspaceGuiComp.h>


// Qt includes
#include <QtGui/QRegExpValidator>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QInputDialog>
#include <QtWidgets/QTabBar>
#include <QtWidgets/QToolBar>

// ACF includes
#include <imod/IModelEditor.h>
#include <ibase/ICommandsProvider.h>
#include <idoc/IDocumentMetaInfo.h>
#include <iqtgui/CCommandTools.h>
#include <iwidgets/iwidgets.h>

// ImtCore includes
#include <imtgui/IDocumentViewConstraints.h>


namespace imtgui
{


// protected methods

// reimplemented (imtgui::CDocumentWorkspaceGuiCompBase)

IDocumentViewDecorator* CDocumentWorkspaceGuiComp::CreateDocumentViewDecorator(
			istd::IPolymorphic* viewPtr,
			QWidget* parentWidgetPtr,
			const SingleDocumentData& documentData,
			const ifile::IFilePersistence* persistencePtr)
{
	return new CollectionDocumentViewDecorator(this, viewPtr, parentWidgetPtr, const_cast<SingleDocumentData&>(documentData), persistencePtr);
}


// reimplemented (idoc::CMultiDocumentManagerBase)

QString CDocumentWorkspaceGuiComp::GetSaveFilePath(const QByteArray& documentTypeId, const istd::IChangeable* dataObjectPtr, const QString& /*currentFilePath*/) const
{
	QInputDialog nameDialog(GetWidget());

	bool isAccepted;
	QString resourceName = nameDialog.getText(GetWidget(), tr("Document Name"), tr("Please enter the name of the document:"), QLineEdit::Normal, QString(), &isAccepted);
	if (!isAccepted){
		return QString();
	}

	if (!resourceName.isEmpty()){
		QRegExpValidator inputValidator(QRegExp("^[\\w,\\s-]+"));
		int pos;
		if (inputValidator.validate(resourceName, pos) != QValidator::Acceptable){
			QMessageBox::critical(NULL, tr("Error"), tr("The document name contains some not allowed characters"));

			return QString();
		}
	}
	else{
		QMessageBox::critical(NULL, tr("Error"), tr("The document name cannot be empty!"));
		return QString();
	}

	const ifile::IFileTypeInfo* fileTypeInfoPtr = GetDocumentFileTypeInfo(documentTypeId);
	if (fileTypeInfoPtr != NULL){
		QStringList extensions;
		if (fileTypeInfoPtr->GetFileExtensions(extensions, dataObjectPtr, ifile::IFileTypeInfo::QF_SAVE)){
			if (!extensions.isEmpty()){
				return resourceName + "." + extensions[0];
			}
		}
	}

	return QString();
}


// reimplemented (idoc::IDocumentManager)

QStringList CDocumentWorkspaceGuiComp::GetOpenFilePaths(const QByteArray* /*documentTypeIdPtr*/) const
{
	QStringList files;

	return files;
}


// reimplemented (iqt:CGuiComponentBase)

void CDocumentWorkspaceGuiComp::OnGuiCreated()
{
	BaseClass::OnGuiCreated();

	if (!(*m_tabStyleSheetPropertyAttrPtr).isEmpty()){
		Tabs->tabBar()->setProperty(*m_tabStyleSheetPropertyAttrPtr, true);
	}
}


// protected slots

void CDocumentWorkspaceGuiComp::OnSaveDocument()
{
	idoc::IDocumentManager::FileToTypeMap fileMap;

	bool ignoredFlag = false;
	if (SaveDocument(-1, false, &fileMap, false, &ignoredFlag)){
		UpdateAllTitles();
	}
	else if (!ignoredFlag){
		QMessageBox::critical(NULL, "", tr("File could not be saved!"));
	}
}


// public methods of the embedded class CollectionDocumentViewDecorator

CDocumentWorkspaceGuiComp::CollectionDocumentViewDecorator::CollectionDocumentViewDecorator(
			CDocumentWorkspaceGuiComp* parentPtr,
			istd::IPolymorphic* viewPtr,
			QWidget* parentWidgetPtr,
			SingleDocumentData& documentData,
			const ifile::IFilePersistence* persistencePtr)
:	QWidget(parentWidgetPtr),
	m_documentData(documentData),
	m_viewObjectPtr(viewPtr),
	m_filePersistencePtr(persistencePtr),
	m_undoManagerPtr(documentData.undoManagerPtr.GetPtr()),
	m_parentPtr(parentPtr),
	m_isInitialized(false),
	m_saveCommand("Save", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR, 20000),
	m_undoCommand("Undo", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR, 20000),
	m_redoCommand("Redo", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR, 20000),
	m_closeCommand("Close", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR, 20001)
{
	Q_ASSERT(parentPtr != NULL);

	setupUi(this);

	setObjectName("DocumentView");

	UndoButton->setEnabled(false);
	RedoButton->setEnabled(false);
	m_undoCommand.setEnabled(false);
	m_redoCommand.setEnabled(false);

	UndoButton->setVisible(m_undoManagerPtr != nullptr);
	RedoButton->setVisible(m_undoManagerPtr != nullptr);
	m_undoCommand.setVisible(m_undoManagerPtr != nullptr);
	m_redoCommand.setVisible(m_undoManagerPtr != nullptr);

	m_saveCommand.setShortcut(Qt::CTRL + Qt::Key_S);
	m_undoCommand.setShortcut(Qt::CTRL + Qt::Key_Z);
	m_redoCommand.setShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_Z);

	idoc::IDocumentMetaInfo* metaInfoPtr = CompCastPtr<idoc::IDocumentMetaInfo>(m_documentData.documentPtr.GetPtr());
	if (metaInfoPtr != NULL){
		m_documentName = metaInfoPtr->GetMetaInfo(idoc::IDocumentMetaInfo::MIT_TITLE).toString();

		m_comment = metaInfoPtr->GetMetaInfo(idoc::IDocumentMetaInfo::MIT_DESCRIPTION).toString();
	}

	connect(UndoButton, SIGNAL(clicked()), parentPtr, SLOT(OnUndoDocument()));
	connect(RedoButton, SIGNAL(clicked()), parentPtr, SLOT(OnRedoDocument()));
	connect(CloseButton, SIGNAL(clicked()), parentPtr, SLOT(OnCloseDocument()));
	connect(SaveButton, SIGNAL(clicked()), parentPtr, SLOT(OnSaveDocument()));

	connect(&m_undoCommand, SIGNAL(triggered()), parentPtr, SLOT(OnUndoDocument()));
	connect(&m_redoCommand, SIGNAL(triggered()), parentPtr, SLOT(OnRedoDocument()));
	connect(&m_closeCommand, SIGNAL(triggered()), parentPtr, SLOT(OnCloseDocument()));
	connect(&m_saveCommand, SIGNAL(triggered()), parentPtr, SLOT(OnSaveDocument()));

	m_undoCommand.SetVisuals(tr("&Undo"), tr("Undo"), tr("Undo last document changes"), QIcon(":/Icons/Undo"));
	m_redoCommand.SetVisuals(tr("&Redo"), tr("Redo"), tr("Redo last document changes"), QIcon(":/Icons/Redo"));
	m_closeCommand.SetVisuals(tr("&Close"), tr("Close"), tr("Close the document"), QIcon(":/Icons/Remove"));
	m_saveCommand.SetVisuals(tr("&Save"), tr("Save"), tr("Save the document changes"), QIcon(":/Icons/Save"));

	UpdateSaveButtonsStatus();

	m_isInitialized = true;

	const imtgui::IDocumentViewConstraints* viewConstraintsPtr = CompCastPtr<imtgui::IDocumentViewConstraints>(viewPtr);
	if (viewConstraintsPtr != NULL){
		imod::IModel* viewModelPtr = const_cast<imod::IModel*>(dynamic_cast<const imod::IModel*>(viewConstraintsPtr));
		if (viewModelPtr != NULL){
			RegisterModel(viewModelPtr, MI_VIEW_CONSTRAINTS);
		}
	}

	imod::IModel* modelPtr = documentData.undoManagerPtr.Cast<imod::IModel*>();
	if (modelPtr != NULL){
		RegisterModel(modelPtr, MI_UNDO_MANAGER);
	}

	imod::IModel* documentMetaInfoModelPtr = dynamic_cast<imod::IModel*>(CompCastPtr<idoc::IDocumentMetaInfo>(documentData.documentPtr.GetPtr()));
	if (documentMetaInfoModelPtr != NULL){
		RegisterModel(documentMetaInfoModelPtr, MI_DOCUMENT_META_INFO);
	}

	ibase::ICommandsProvider* commandsProviderPtr = CompCastPtr<ibase::ICommandsProvider>(viewPtr);
	if (commandsProviderPtr != NULL){
		imod::IModel* commandsModelPtr = dynamic_cast<imod::IModel*>(commandsProviderPtr);
		if (commandsModelPtr != NULL){
			RegisterModel(commandsModelPtr, MI_VIEW_COMMANDS);
		}
	}

	HeaderFrame->setVisible(false);
}


void CDocumentWorkspaceGuiComp::CollectionDocumentViewDecorator::UpdateSaveButtonsStatus()
{
	bool isSaveActive = true;

	if (m_undoManagerPtr != NULL){
		isSaveActive = isSaveActive && (m_undoManagerPtr->GetDocumentChangeFlag() != idoc::IDocumentStateComparator::DCF_EQUAL);
	}

	isSaveActive = isSaveActive || m_documentData.isDirty;
 
	if (m_filePersistencePtr == NULL){
		isSaveActive = false;
	}

	m_saveCommand.setEnabled(isSaveActive);
	SaveButton->setEnabled(isSaveActive);

	const imtgui::IDocumentViewConstraints* viewConstraintsPtr = CompCastPtr<imtgui::IDocumentViewConstraints>(m_viewObjectPtr);
	if (viewConstraintsPtr != nullptr){
		bool isSaveEnabled = viewConstraintsPtr->GetViewConstraints() & imtgui::IDocumentViewConstraints::CF_SAVE_DOCUMENT;

		SaveButton->setVisible(isSaveEnabled);
		m_saveCommand.setVisible(isSaveEnabled);
	}
}


idoc::CMultiDocumentManagerBase::SingleDocumentData& CDocumentWorkspaceGuiComp::CollectionDocumentViewDecorator::GetDocumentData() const
{
	return m_documentData;
}


void CDocumentWorkspaceGuiComp::CollectionDocumentViewDecorator::OnViewContraintsChanged()
{
	const imtgui::IDocumentViewConstraints* viewConstraintsPtr = CompCastPtr<imtgui::IDocumentViewConstraints>(m_viewObjectPtr);
	if (viewConstraintsPtr != NULL){
		int viewFlags = viewConstraintsPtr->GetViewConstraints();

		imod::IModelEditor* modelEditorPtr = CompCastPtr<imod::IModelEditor>(m_viewObjectPtr);
		if (modelEditorPtr != NULL){
			modelEditorPtr->SetReadOnly(!(viewFlags & imtgui::IDocumentViewConstraints::CF_EDIT_DOCUMENT));
		}

		SaveButton->setVisible((viewFlags & imtgui::IDocumentViewConstraints::CF_SAVE_DOCUMENT));
		UndoButton->setVisible((viewFlags & imtgui::IDocumentViewConstraints::CF_UNDO_SUPPORT));
		RedoButton->setVisible((viewFlags & imtgui::IDocumentViewConstraints::CF_UNDO_SUPPORT));
		m_undoCommand.setVisible((viewFlags & imtgui::IDocumentViewConstraints::CF_UNDO_SUPPORT));
		m_redoCommand.setVisible((viewFlags & imtgui::IDocumentViewConstraints::CF_UNDO_SUPPORT));

		CloseButton->setVisible((viewFlags & imtgui::IDocumentViewConstraints::CF_CLOSE_SUPPORT));
	}
}


// reimplemeneted (IDocumentViewDecorator)

QWidget* CDocumentWorkspaceGuiComp::CollectionDocumentViewDecorator::GetDecoratorWidget()
{
	return this;
}


QWidget* CDocumentWorkspaceGuiComp::CollectionDocumentViewDecorator::GetViewFrame()
{
	return DocumentFrame;
}


istd::IPolymorphic* CDocumentWorkspaceGuiComp::CollectionDocumentViewDecorator::GetView() const
{
	return m_viewObjectPtr;
}


void CDocumentWorkspaceGuiComp::CollectionDocumentViewDecorator::SetViewEnabled(bool isEnabled)
{
	setEnabled(isEnabled);
}


QString CDocumentWorkspaceGuiComp::CollectionDocumentViewDecorator::GetTitle()
{
	return QString();
}


void CDocumentWorkspaceGuiComp::CollectionDocumentViewDecorator::SetTitle(const QString& /*title*/)
{
}


void CDocumentWorkspaceGuiComp::CollectionDocumentViewDecorator::SetDocumentTypeName(const QString& /*name*/)
{
}


// reimplemented (ibase::ICommandsProvider)

const ibase::IHierarchicalCommand * CDocumentWorkspaceGuiComp::CollectionDocumentViewDecorator::GetCommands() const
{
	return &m_commands;
}


// protected methods of the embedded class CollectionDocumentViewDecorator

// reimplemented (imod::CMultiModelDispatcherBase)

void CDocumentWorkspaceGuiComp::CollectionDocumentViewDecorator::OnModelChanged(int modelId, const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	Q_ASSERT(m_parentPtr != NULL);

	if (!m_isInitialized){
		return;
	}

	switch (modelId){
		case MI_UNDO_MANAGER:
			if (m_undoManagerPtr != NULL){
				UndoButton->setEnabled(m_undoManagerPtr->GetAvailableUndoSteps() > 0);
				RedoButton->setEnabled(m_undoManagerPtr->GetAvailableRedoSteps() > 0);
				m_undoCommand.setEnabled(m_undoManagerPtr->GetAvailableUndoSteps() > 0);
				m_redoCommand.setEnabled(m_undoManagerPtr->GetAvailableRedoSteps() > 0);
			}
			break;

		case MI_VIEW_COMMANDS:{
			ibase::ICommandsProvider* commandsProviderPtr = CompCastPtr<ibase::ICommandsProvider>(m_viewObjectPtr);
			m_commands.ResetChilds();

			if (commandsProviderPtr != NULL){
				iwidgets::ClearLayout(CommandToolBarFrame->layout());
				const iqtgui::CHierarchicalCommand* guiCommandPtr = dynamic_cast<const iqtgui::CHierarchicalCommand*>(commandsProviderPtr->GetCommands());
				if (guiCommandPtr != NULL){
					QToolBar* toolBarPtr = new QToolBar(CommandToolBarFrame);
					toolBarPtr->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
					CommandToolBarFrame->layout()->addWidget(toolBarPtr);
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


