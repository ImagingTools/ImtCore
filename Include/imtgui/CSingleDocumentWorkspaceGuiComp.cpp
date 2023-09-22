#include <imtgui/CSingleDocumentWorkspaceGuiComp.h>


// Qt includes
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QToolBar>

// ImtCore includes
#include <imtgui/TStandardDocumentViewDecorator.h>
#include <GeneratedFiles/imtgui/ui_CStandardDocumentViewDecorator.h>


namespace imtgui
{


class SdiDocumentViewDecorator: public TStandardDocumentViewDecorator<CSingleDocumentWorkspaceGuiComp, Ui::CStandardDocumentViewDecorator>
{
public:
	typedef TStandardDocumentViewDecorator<CSingleDocumentWorkspaceGuiComp, Ui::CStandardDocumentViewDecorator> BaseClass;

	SdiDocumentViewDecorator(
		CSingleDocumentWorkspaceGuiComp* parentPtr,
		istd::IPolymorphic* viewPtr,
		QWidget* parentWidgetPtr,
		const ifile::IFilePersistence* persistencePtr,
		const DecoratorConfiguration& configuration);
};


// public methods

CSingleDocumentWorkspaceGuiComp::CSingleDocumentWorkspaceGuiComp()
{
	m_commands.SetParent(this);
}


// protected methods

// reimplemented (imtgui::CSingleDocumentWorkspaceGuiComp)

IDocumentViewDecorator* CSingleDocumentWorkspaceGuiComp::CreateDocumentViewDecorator(
			istd::IPolymorphic* viewPtr,
			QWidget* parentWidgetPtr,
			const ifile::IFilePersistence* persistencePtr)
{
	SdiDocumentViewDecorator::DecoratorConfiguration configuration;
	configuration.iconSize = *m_iconSizeAttrPtr;
	configuration.showDocumentTitle = *m_showDocumentTitleAttrPtr;
	configuration.fileButtonsStyle = Qt::ToolButtonStyle(*m_fileButtonsStyleAttrPtr);
	configuration.undoButtonsStyle = Qt::ToolButtonStyle(*m_undoButtonsStyleAttrPtr);
	configuration.documentButtonsStyle = Qt::ToolButtonStyle(*m_documentButtonsStyleAttrPtr);

	return new SdiDocumentViewDecorator(this, viewPtr, parentWidgetPtr, persistencePtr, configuration);
}


// reimplemented (iqtdoc::CSingleDocumentWorkspaceGuiComp)

void CSingleDocumentWorkspaceGuiComp::UpdateTitle()
{
	if (m_documentViewPtr.IsValid()){
		QString titleName = GenerateDocumentTitle();

		m_documentViewPtr->SetTitle(titleName);
	}
}


// reimplemented (idoc::CSingleDocumentManagerBase)

void CSingleDocumentWorkspaceGuiComp::OnViewRegistered(istd::IPolymorphic* viewPtr)
{
	ifile::IFilePersistence* persistencePtr = nullptr;
	const idoc::IDocumentTemplate* templatePtr = GetDocumentTemplate();
	if (templatePtr != nullptr){
		istd::IChangeable* documentPtr = GetDocumentFromView(*viewPtr);
		Q_ASSERT(documentPtr != nullptr);

		persistencePtr = templatePtr->GetFileLoader(GetDocumentTypeId(*documentPtr));
	}

	iqtgui::IGuiObject* guiObjectPtr = CompCastPtr<iqtgui::IGuiObject>(viewPtr);
	if ((guiObjectPtr != nullptr) && IsGuiCreated()){
		QWidget* rootWidgetPtr = GetWidget();
		QLayout* layoutPtr = rootWidgetPtr->layout();

		m_documentViewPtr.SetPtr(CreateDocumentViewDecorator(viewPtr, rootWidgetPtr, persistencePtr));
		if (guiObjectPtr->CreateGui(m_documentViewPtr->GetViewFrame())){
			Q_ASSERT(guiObjectPtr->GetWidget() != nullptr);

			layoutPtr->addWidget(m_documentViewPtr->GetDecoratorWidget());

			ibase::ICommandsProvider* commandsProviderPtr = CompCastPtr<ibase::ICommandsProvider>(viewPtr);
			imod::IModel* commandsProviderModelPtr = dynamic_cast<imod::IModel*>(commandsProviderPtr);
			if (commandsProviderModelPtr != nullptr){
				RegisterModel(commandsProviderModelPtr, MI_DOCUMENT_COMMANDS);
			}

			SetActiveView(viewPtr);

			this->m_lastViewPtr = guiObjectPtr;
		}
	}
}


void CSingleDocumentWorkspaceGuiComp::OnViewRemoved(istd::IPolymorphic* viewPtr)
{
	BaseClass::OnViewRemoved(viewPtr);

	m_documentViewPtr.Reset();

	QWidget* rootWidgetPtr = GetWidget();
	if (rootWidgetPtr != nullptr){
		QLayout* layoutPtr = rootWidgetPtr->layout();

		iwidgets::ClearLayout(layoutPtr);
	}
}


// reimplemented (imod::CMultiModelDispatcherBase)

void CSingleDocumentWorkspaceGuiComp::OnModelChanged(int modelId, const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	if (modelId == MI_DOCUMENT_COMMANDS){
		static ChangeSet changes(ibase::ICommandsProvider::CF_COMMANDS);

		istd::CChangeNotifier changeNotifier(&m_commands, &changes);
	}
}


void CSingleDocumentWorkspaceGuiComp::OnCloseDocument()
{
}


void CSingleDocumentWorkspaceGuiComp::OnUndo()
{
	istd::IPolymorphic* viewPtr = GetActiveView();
	if (viewPtr != nullptr){
		istd::IChangeable* documentPtr = GetDocumentFromView(*viewPtr);
		if (documentPtr != nullptr){
			idoc::IUndoManager* undoManagerPtr = GetUndoManagerForDocument(documentPtr);
			if (undoManagerPtr != nullptr){
				undoManagerPtr->DoUndo();
			}
		}
	}
}


void CSingleDocumentWorkspaceGuiComp::OnRedo()
{
	istd::IPolymorphic* viewPtr = GetActiveView();
	if (viewPtr != nullptr){
		istd::IChangeable* documentPtr = GetDocumentFromView(*viewPtr);
		if (documentPtr != nullptr){
			idoc::IUndoManager* undoManagerPtr = GetUndoManagerForDocument(documentPtr);
			if (undoManagerPtr != nullptr){
				undoManagerPtr->DoRedo();
			}
		}
	}
}


// protected slots

void CSingleDocumentWorkspaceGuiComp::OnNew()
{
	istd::IChangeable* newDocumentPtr = 0;
	if (InsertNewDocument("", false, "", &newDocumentPtr)){
		Q_ASSERT(newDocumentPtr != nullptr);

		AddViewToDocument(*newDocumentPtr);
	}
}


void CSingleDocumentWorkspaceGuiComp::OnOpen()
{
	istd::IChangeable* documentPtr = 0;
	if (OpenDocument(nullptr, nullptr, false, "", &documentPtr)){
		if (documentPtr != nullptr){
			AddViewToDocument(*documentPtr);
		}
	}
	else{
		OnNew();

		QMessageBox::critical(GetWidget(), tr("Open File"), tr("Selected file could not be loaded"));
	}
}


void CSingleDocumentWorkspaceGuiComp::OnSaveDocument()
{
	idoc::IDocumentManager::FileToTypeMap fileMap;

	bool ignoredFlag = false;
	if (SaveDocument(-1, false, &fileMap, false, &ignoredFlag)){
		UpdateTitle();
	}
	else if (!ignoredFlag){
		QMessageBox::critical(NULL, "", tr("File could not be saved!"));
	}
}


void CSingleDocumentWorkspaceGuiComp::OnSaveDocumentAs()
{
	idoc::IDocumentManager::FileToTypeMap fileMap;

	bool ignoredFlag = false;
	if (SaveDocument(-1, true, &fileMap, false, &ignoredFlag)){
		UpdateTitle();
	}
	else if (!ignoredFlag){
		QMessageBox::critical(NULL, "", tr("File could not be saved!"));
	}
}


// public methods of the embedded class Commands

CSingleDocumentWorkspaceGuiComp::Commands::Commands()
	:m_parentPtr(nullptr)
{
}


void CSingleDocumentWorkspaceGuiComp::Commands::SetParent(CSingleDocumentWorkspaceGuiComp* parentPtr)
{
	Q_ASSERT(parentPtr != nullptr);

	m_parentPtr = parentPtr;
}


// reimplemented (ibase::ICommandsProvider)

const ibase::IHierarchicalCommand* CSingleDocumentWorkspaceGuiComp::Commands::GetCommands() const
{
	Q_ASSERT(m_parentPtr != nullptr);

	if (m_parentPtr->IsGuiCreated()){
		istd::IPolymorphic* viewPtr = m_parentPtr->GetActiveView();
		ibase::ICommandsProvider* viewCommandsProviderPtr = CompCastPtr<ibase::ICommandsProvider>(viewPtr);
		if (viewCommandsProviderPtr != nullptr){
			return viewCommandsProviderPtr->GetCommands();
		}
	}

	return nullptr;
}


// public methods of the SdiDocumentViewDecorator

SdiDocumentViewDecorator::SdiDocumentViewDecorator(
			CSingleDocumentWorkspaceGuiComp* parentPtr,
			istd::IPolymorphic* viewPtr,
			QWidget* parentWidgetPtr,
			const ifile::IFilePersistence* persistencePtr,
			const DecoratorConfiguration& configuration)
	:BaseClass(parentPtr, viewPtr, parentWidgetPtr, persistencePtr, configuration)
{
	bool newButtonVisible = false;

	const idoc::IDocumentTemplate* templatePtr = parentPtr->GetDocumentTemplate();
	if (templatePtr != nullptr){
		istd::IChangeable* documentPtr = parentPtr->GetDocumentFromView(*viewPtr);
		if (documentPtr != nullptr){
			bool isNewSupported = templatePtr->IsFeatureSupported(idoc::IDocumentTemplate::SF_NEW_DOCUMENT, parentPtr->GetDocumentTypeId(*documentPtr));

			newButtonVisible = isNewSupported;
		}
	}

	NewButton->setVisible(newButtonVisible);

	CloseButton->setVisible(false);

	HeaderFrame->setVisible(true);
}

} // namespace imtgui


