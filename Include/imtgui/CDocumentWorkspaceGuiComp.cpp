#include <imtgui/CDocumentWorkspaceGuiComp.h>


// Qt includes
#include <QtGui/QRegExpValidator>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QInputDialog>
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
	QInputDialog resourceNameDialog(GetWidget());

	bool isAccepted;
	QString resourceName = resourceNameDialog.getText(GetWidget(), tr("Document Name"), tr("Please enter the name of the document:"), QLineEdit::Normal, QString(), &isAccepted);
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
	m_isInitialized(false)
{
	Q_ASSERT(parentPtr != NULL);

	setupUi(this);

	setObjectName("DocumentView");

	UndoButton->setEnabled(false);
	RedoButton->setEnabled(false);

	UndoButton->setVisible(m_undoManagerPtr != nullptr);
	RedoButton->setVisible(m_undoManagerPtr != nullptr);

	const imtgui::IDocumentViewConstraints* viewConstraintsPtr = CompCastPtr<imtgui::IDocumentViewConstraints>(viewPtr);
	if (viewConstraintsPtr != NULL){
		imod::IModel* viewModelPtr = const_cast<imod::IModel*>(dynamic_cast<const imod::IModel*>(viewConstraintsPtr));
		if (viewModelPtr != NULL){
			RegisterModel(viewModelPtr, MI_VIEW_CONSTRAINTS);
		}
	}

	idoc::IDocumentMetaInfo* metaInfoPtr = CompCastPtr<idoc::IDocumentMetaInfo>(m_documentData.documentPtr.GetPtr());
	if (metaInfoPtr != NULL){
		m_documentName = metaInfoPtr->GetMetaInfo(idoc::IDocumentMetaInfo::MIT_TITLE).toString();

		m_comment = metaInfoPtr->GetMetaInfo(idoc::IDocumentMetaInfo::MIT_DESCRIPTION).toString();
	}

	connect(UndoButton, SIGNAL(clicked()), parentPtr, SLOT(OnUndoDocument()));
	connect(RedoButton, SIGNAL(clicked()), parentPtr, SLOT(OnRedoDocument()));
	connect(CloseButton, SIGNAL(clicked()), parentPtr, SLOT(OnCloseDocument()));
	connect(SaveButton, SIGNAL(clicked()), parentPtr, SLOT(OnSaveDocument()));

	UpdateSaveButtonsStatus();

	m_isInitialized = true;

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

	SaveButton->setEnabled(isSaveActive);
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
//		CloseButton->setDefault(!!(viewFlags & imtgui::IDocumentViewConstraints::CF_SAVE_DOCUMENT));

		UndoButton->setVisible((viewFlags & imtgui::IDocumentViewConstraints::CF_UNDO_SUPPORT));
		RedoButton->setVisible((viewFlags & imtgui::IDocumentViewConstraints::CF_UNDO_SUPPORT));

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
			}
			break;

		case MI_VIEW_COMMANDS:{
			ibase::ICommandsProvider* commandsProviderPtr = CompCastPtr<ibase::ICommandsProvider>(m_viewObjectPtr);
			if (commandsProviderPtr != NULL){
				iwidgets::ClearLayout(CommandToolBarFrame->layout());
				const iqtgui::CHierarchicalCommand* guiCommandPtr = dynamic_cast<const iqtgui::CHierarchicalCommand*>(commandsProviderPtr->GetCommands());
				if (guiCommandPtr != NULL){
					QToolBar* toolBarPtr = new QToolBar(CommandToolBarFrame);
					toolBarPtr->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
					CommandToolBarFrame->layout()->addWidget(toolBarPtr);
					toolBarPtr->clear();
					toolBarPtr->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
					iqtgui::CCommandTools::SetupToolbar(*guiCommandPtr, *toolBarPtr);
					toolBarPtr->setIconSize(QSize(32, 32));
				}
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


