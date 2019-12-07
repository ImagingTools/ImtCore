#include <imtgui/CDocumentWorkspaceGuiCompBase.h>


// Qt includes
#include <QtCore/QMap>
#include <QtCore/QFileInfo>
#include <QtCore/QUrl>
#include <QtCore/QMimeData>
#include <QtGui/QDragEnterEvent>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QTabBar>
#include <QtWidgets/QMenu>
#include <QtWidgets/QLabel>
#include <QtWidgets/QInputDialog>

// ACF includes
#include <istd/CChangeNotifier.h>
#include <istd/CChangeGroup.h>
#include <iser/CPrimitiveTypesSerializer.h>
#include <ibase/ICommandsProvider.h>
#include <idoc/IDocumentTemplate.h>
#include <idoc/IDocumentMetaInfo.h>
#include <iwidgets/CWidgetUpdateBlocker.h>


namespace imtgui
{


// public methods

CDocumentWorkspaceGuiCompBase::CDocumentWorkspaceGuiCompBase()
:	m_forceQuietClose(false),
	m_isUpdateBlocked(false),
	m_previousTabIndex(-1)
{
	m_documentList.SetParent(*this);
}


// reimplemented (ibase::ICommandsProvider)

const ibase::IHierarchicalCommand* CDocumentWorkspaceGuiCompBase::GetCommands() const
{
	if (IsGuiCreated()){
		int tabIndex = Tabs->currentIndex();
		if (tabIndex >= m_fixedTabs.count()){
			istd::IPolymorphic* viewPtr = GetActiveView();
			ibase::ICommandsProvider* viewCommandsProviderPtr = CompCastPtr<ibase::ICommandsProvider>(viewPtr);
			if (viewCommandsProviderPtr != nullptr){
				return viewCommandsProviderPtr->GetCommands();
			}
		}
		else{
			ibase::ICommandsProvider* viewCommandsProviderPtr = CompCastPtr<ibase::ICommandsProvider>(m_fixedTabs[tabIndex]);
			if (viewCommandsProviderPtr != nullptr){
				return viewCommandsProviderPtr->GetCommands();
			}
		}
	}

	return nullptr;
}


// reimplemented (iqtgui::IGuiObject)

void CDocumentWorkspaceGuiCompBase::OnTryClose(bool* ignoredPtr)
{
	if (SaveDirtyDocuments(false, ignoredPtr)){
		int documentInfosCount = GetDocumentsCount();
		for (int documentIndex = 0; documentIndex < documentInfosCount; ++documentIndex){
			SingleDocumentData& info = GetSingleDocumentData(documentIndex);

			info.isDirty = false;
		}

		CloseAllDocuments();
	}

	if (ignoredPtr != nullptr){
		*ignoredPtr = (GetDocumentsCount() > 0);
	}
}


// protected members

int CDocumentWorkspaceGuiCompBase::GetFixedWindowsCount() const
{
	return m_fixedTabs.size();
}


void CDocumentWorkspaceGuiCompBase::UpdateAllTitles()
{
	typedef QMap<QString, int> NameFrequencies;
	NameFrequencies nameFrequencies;

	for (int pageIndex = GetFixedWindowsCount(); pageIndex < Tabs->count(); ++pageIndex){
		IDocumentViewDecorator* documentViewPtr = dynamic_cast<IDocumentViewDecorator*>(Tabs->widget(pageIndex));
		Q_ASSERT(documentViewPtr != nullptr);

		istd::IPolymorphic* viewPtr = documentViewPtr->GetView();
		Q_ASSERT(viewPtr != nullptr);

		CMultiDocumentManagerBase::SingleDocumentData* infoPtr = GetDocumentInfoFromView(*viewPtr);

		if (infoPtr != nullptr){
			QString fileText = infoPtr->filePath.isEmpty()?
						"":
						QFileInfo(infoPtr->filePath).completeBaseName();
			
			QString titleName = infoPtr->filePath.isEmpty()?
						tr("<no name>"):
						QFileInfo(infoPtr->filePath).completeBaseName();

			NameFrequencies::Iterator freqIter = nameFrequencies.find(titleName);
			int& frequency = freqIter.value();
			if (freqIter != nameFrequencies.end()){
				frequency++;

				titleName = tr("%1 <%2>").arg(titleName).arg(frequency + 1);
			}
			else{
				nameFrequencies[titleName] = 0;
			}

			if (infoPtr->isDirty){
				titleName += " *";
			}

			documentViewPtr->SetTitle(titleName);
			documentViewPtr->SetDocumentTypeName(infoPtr->parentPtr->GetDocumentTypeName(infoPtr->documentTypeId));
			documentViewPtr->SetViewEnabled(true);

			Tabs->setTabText(pageIndex, titleName);
			QIcon documentIcon;
			iqtgui::IVisualStatus* visualStatusPtr = CompCastPtr<iqtgui::IVisualStatus>(viewPtr);
			if (visualStatusPtr != nullptr){
				documentIcon = visualStatusPtr->GetStatusIcon();
			}

			Tabs->setTabIcon(pageIndex, documentIcon);

			if (pageIndex == Tabs->currentIndex()){
				m_currentDocumentName.SetName(fileText);
			}
		}
		else{
			documentViewPtr->SetViewEnabled(false);
		}
	}
}


int CDocumentWorkspaceGuiCompBase::GetDocumentIndexFromWidget(const QWidget& widget) const
{
	int documentInfosCount = GetDocumentsCount();
	for (int documentIndex = 0; documentIndex < documentInfosCount; ++documentIndex){
		SingleDocumentData& info = GetSingleDocumentData(documentIndex);

		for (		Views::ConstIterator viewIter = info.views.begin();
					viewIter != info.views.end();
					++viewIter){
			const ViewInfo& viewInfo = *viewIter;

			iqtgui::IGuiObject* guiObjectPtr = CompCastPtr<iqtgui::IGuiObject>(viewInfo.viewPtr.GetPtr());
			if (guiObjectPtr != nullptr){
				if (guiObjectPtr->GetWidget() == &widget){
					return documentIndex;
				}
			}
		}
	}

	return -1;
}


void CDocumentWorkspaceGuiCompBase::InitializeDocumentView(IDocumentViewDecorator* /*documentViewPtr*/, const SingleDocumentData& /*documentData*/)
{
}


void CDocumentWorkspaceGuiCompBase::UpdateCommands()
{
	static ChangeSet changes(CF_COMMANDS);
	istd::CChangeNotifier changeNotifier(this, &changes);


}


// protected methods

bool CDocumentWorkspaceGuiCompBase::AddTab(const QString& name, iqtgui::IGuiObject* guiPtr, const QIcon& icon)
{
	Q_ASSERT(guiPtr != nullptr);

	QWidget* tabFrame = new QWidget;
	QVBoxLayout* tabFrameLayout = new QVBoxLayout(tabFrame);
	tabFrameLayout->setMargin(0);

	m_fixedTabs.push_back(guiPtr);

	if (guiPtr->CreateGui(tabFrame)){
		Tabs->addTab(tabFrame, icon, name);

		// Remove close button form the permanent tabs:
		int tabCount = Tabs->count();
		QWidget* widgetPtr = Tabs->tabBar()->tabButton(tabCount - 1, QTabBar::RightSide);
		if (widgetPtr != nullptr){
			widgetPtr->setVisible(false);
		}

		return true;
	}
	else{
		m_fixedTabs.pop_back();

		tabFrame->deleteLater();
		tabFrame = nullptr;

		return false;
	}
}


void CDocumentWorkspaceGuiCompBase::OnDragEnterEvent(QDragEnterEvent* dragEnterEventPtr)
{
	if (dragEnterEventPtr->mimeData()->hasFormat("text/uri-list")){
		dragEnterEventPtr->acceptProposedAction();
	}
}


void CDocumentWorkspaceGuiCompBase::OnDropEvent(QDropEvent* dropEventPtr)
{
	const QMimeData* mimeData = dropEventPtr->mimeData();
	if (mimeData->hasUrls()){
		QList<QUrl> files = mimeData->urls();

		for (int fileIndex = 0; fileIndex < files.count(); fileIndex++){
			QString filePath = files.at(fileIndex).toLocalFile();

			if (OpenDocument(nullptr, &filePath)){
				dropEventPtr->setAccepted(true);
				return;
			}
		}
	}

	dropEventPtr->setAccepted(false);
	return;
}


// reimplemented (imod::CMultiModelDispatcherBase)

void CDocumentWorkspaceGuiCompBase::OnModelChanged(int modelId, const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	if ((modelId >= MI_VISUAL_STATUS_BASE_INDEX) && (modelId < MI_DOCUMENT_COMMANDS_BASE_INDEX)){
		int tabIndex = modelId - MI_VISUAL_STATUS_BASE_INDEX;
		if (tabIndex < m_fixedVisualInfosCompPtr.GetCount()){
			const iqtgui::IVisualStatus* visualStatusPtr = m_fixedVisualInfosCompPtr[tabIndex];
			if (visualStatusPtr != nullptr){
				Tabs->setTabIcon(tabIndex, visualStatusPtr->GetStatusIcon());
			}
		}
	}

	if (modelId >= MI_DOCUMENT_COMMANDS_BASE_INDEX){
		UpdateCommands();
	}
}


// reimplemented (idoc::CMultiDocumentManagerBase)

istd::IChangeable* CDocumentWorkspaceGuiCompBase::OpenSingleDocument(
			const QString& filePath,
			bool createView,
			const QByteArray& viewTypeId,
			QByteArray& documentTypeId,
			bool beQuiet,
			bool* ignoredPtr,
			ibase::IProgressManager* progressManagerPtr)
{
	SingleDocumentData* documentInfoPtr = GetDocumentInfoFromPath(filePath);
	bool isNewViewType = true;
	if (documentInfoPtr != nullptr){
		for (int i = 0; i < documentInfoPtr->views.count(); ++i){
			if (documentInfoPtr->views[i].viewTypeId == viewTypeId){
				isNewViewType = false;
				break;
			}
		}
	}

	if (documentInfoPtr != nullptr && !isNewViewType){
		createView = false;
	}

	return BaseClass::OpenSingleDocument(filePath, createView, viewTypeId, documentTypeId, beQuiet, ignoredPtr, progressManagerPtr);
}


void CDocumentWorkspaceGuiCompBase::SetActiveView(istd::IPolymorphic* viewPtr)
{
	if (viewPtr != GetActiveView()){
		istd::CChangeGroup selectorChangeGroup(&m_documentList);

		m_isUpdateBlocked = true;

		BaseClass::SetActiveView(viewPtr);

		static istd::IChangeable::ChangeSet commandsChangeSet(ibase::ICommandsProvider::CF_COMMANDS);

		idoc::CMultiDocumentManagerBase::SingleDocumentData* activeDocumentInfoPtr = GetDocumentInfoFromView(*viewPtr);
		if (activeDocumentInfoPtr == nullptr){
			m_documentList.SetSelectedOptionIndex(iprm::ISelectionParam::NO_SELECTION);
		}
		else{
			m_documentList.SetSelectedOptionIndex(GetDocumentIndex(*activeDocumentInfoPtr));
		}

		int pageIndex = -1;
		int viewIndex = 0;
		for (; viewIndex < m_fixedTabs.size(); viewIndex++){
			if (viewPtr == m_fixedTabs[viewIndex]){
				pageIndex = viewIndex;

				m_currentDocumentName.SetName("");

				break;
			}
		}

		if (pageIndex < 0){
			int tabsCount = Tabs->count();
			for (; viewIndex < tabsCount; viewIndex++){
				IDocumentViewDecorator* documentViewPtr = dynamic_cast<IDocumentViewDecorator*>(Tabs->widget(viewIndex));
				Q_ASSERT(documentViewPtr != nullptr);

				if (viewPtr == documentViewPtr->GetView()){
					pageIndex = viewIndex;

					m_currentDocumentName.SetName(documentViewPtr->GetTitle());

					break;
				}
			}
		}

		if (pageIndex >= 0){
			m_previousTabIndex = Tabs->currentIndex();
			Tabs->setCurrentIndex(pageIndex);
		}

		m_isUpdateBlocked = false;
	}
}


// reimplemented (idoc::IDocumentManager)

bool CDocumentWorkspaceGuiCompBase::InsertNewDocument(
			const QByteArray& documentTypeId,
			bool createView,
			const QByteArray& viewTypeId,
			istd::IChangeable** newDocumentPtr,
			bool beQuiet,
			bool* ignoredPtr)
{
	bool retVal = BaseClass::InsertNewDocument(documentTypeId, createView, viewTypeId, newDocumentPtr, beQuiet, ignoredPtr);

	if (retVal && (newDocumentPtr != nullptr) && (*newDocumentPtr != nullptr) && createView){
		int documentsCount = GetDocumentsCount();
		for (int i = 0; i < documentsCount; ++i){
			if (&GetDocumentFromIndex(i) == *newDocumentPtr){
				istd::IPolymorphic* viewPtr = GetViewFromIndex(i, 0);
				if (viewPtr != nullptr){
					SetActiveView(viewPtr);
				}

				break;
			}
		}
	}

	return retVal;
}


// reimplemented (idoc::CMultiDocumentManagerBase)

void CDocumentWorkspaceGuiCompBase::CloseAllDocuments()
{
	if (!IsGuiCreated()){
		return;
	}

	iwidgets::CWidgetUpdateBlocker headerBarLocker(Tabs);
	Q_UNUSED(headerBarLocker);

	m_forceQuietClose = true;
	m_isUpdateBlocked = true;

	m_documentList.SetSelectedOptionIndex(iprm::ISelectionParam::NO_SELECTION);
	m_currentDocumentName.SetName("");

	int fixedTabsCount = GetFixedWindowsCount();
	int documentCount = Tabs->count();
	for (int index = documentCount - 1; index >= fixedTabsCount; --index)
	{
		Tabs->tabCloseRequested(index);
	}

	BaseClass::CloseAllDocuments();

	m_forceQuietClose = false;
	m_isUpdateBlocked = false;
}


QStringList CDocumentWorkspaceGuiCompBase::GetOpenFilePaths(const QByteArray* documentTypeIdPtr) const
{
	QStringList files = GetOpenFilePathesFromDialog(documentTypeIdPtr);

	if (!files.isEmpty()){
		UpdateLastDirectory(files.at(0));
	}

	return files;
}


void CDocumentWorkspaceGuiCompBase::OnViewRegistered(istd::IPolymorphic* viewPtr, const SingleDocumentData& documentData)
{
	ifile::IFilePersistence* persistencePtr = nullptr;
	const idoc::IDocumentTemplate* templatePtr = GetDocumentTemplate();
	if (templatePtr != nullptr){
		persistencePtr = templatePtr->GetFileLoader(documentData.documentTypeId);
	}

	iqtgui::IGuiObject* guiObjectPtr = CompCastPtr<iqtgui::IGuiObject>(viewPtr);
	if ((guiObjectPtr != nullptr) && IsGuiCreated()){
		istd::TDelPtr<IDocumentViewDecorator> documentViewPtr(CreateDocumentViewDecorator(viewPtr, Tabs, documentData, persistencePtr));
		if (guiObjectPtr->CreateGui(documentViewPtr->GetViewFrame())){
			Q_ASSERT(guiObjectPtr->GetWidget() != nullptr);

			int newViewIndex = Tabs->count();
			Tabs->insertTab(newViewIndex, documentViewPtr->GetDecoratorWidget(), "");

			SetActiveView(viewPtr);

			InitializeDocumentView(documentViewPtr.GetPtr(), documentData);

			documentViewPtr.PopPtr();
		}
	}
}


void CDocumentWorkspaceGuiCompBase::OnViewRemoved(istd::IPolymorphic* viewPtr)
{
	for (int pageIndex = GetFixedWindowsCount(); pageIndex < Tabs->count(); ++pageIndex){
		IDocumentViewDecorator* documentViewPtr = dynamic_cast<IDocumentViewDecorator*>(Tabs->widget(pageIndex));
		Q_ASSERT(documentViewPtr != nullptr);

		if (documentViewPtr->GetView() == viewPtr){
			int lastPageIndex = qMin(m_previousTabIndex, Tabs->count() - 2);

			iqtgui::IGuiObject* guiObjectPtr = CompCastPtr<iqtgui::IGuiObject>(viewPtr);
			Q_ASSERT(guiObjectPtr != nullptr);

			guiObjectPtr->DestroyGui();

			OnWindowActivated(lastPageIndex);

			Tabs->removeTab(pageIndex);

			break;
		}
	}
}


bool CDocumentWorkspaceGuiCompBase::QueryDocumentSave(const SingleDocumentData& info, bool* ignoredPtr)
{
	QFileInfo fileInfo(info.filePath);
	QMessageBox::StandardButtons buttons = QMessageBox::Yes | QMessageBox::No;

	if (ignoredPtr != nullptr){
		*ignoredPtr = false;
		buttons |= QMessageBox::Cancel;
	}

	int response = QMessageBox::information(
				GetQtWidget(),
				tr("Close document"),
				tr("Do you want to save your changes made in document\n%1").arg(QDir::toNativeSeparators(fileInfo.completeBaseName())),
				buttons,
				QMessageBox::Yes);

	if (response == QMessageBox::Yes){
		return true;
	}
	else if ((ignoredPtr != nullptr) && (response == QMessageBox::Cancel)){
		*ignoredPtr = true;
	}

	return false;
}


// reimplemented (iqt:CGuiObjectBase)

void CDocumentWorkspaceGuiCompBase::OnGuiCreated()
{
	BaseClass::OnGuiCreated();

	QWidget* mainWindowPtr = GetQtWidget();
	if (mainWindowPtr != nullptr){
		mainWindowPtr->setAcceptDrops(true);
	}

	Tabs->setDocumentMode(true);
	Tabs->tabBar()->setDrawBase(false);
	Tabs->setTabsClosable(true);
	Tabs->setElideMode(Qt::ElideMiddle);
	
	// Add additional fixed UI components to the tab bar:
	for (int i = 0; i < m_fixedTabsCompPtr.GetCount(); ++i){
		iqtgui::IGuiObject* guiPtr = m_fixedTabsCompPtr[i];
		if (guiPtr != nullptr){
			QString tabName = QString(tr("Tab %1").arg(i + 1));
			if (i < m_fixedTabsNamesAttrPtr.GetCount()){
				tabName = m_fixedTabsNamesAttrPtr[i];
			}

			QIcon tabIcon;
			if (i < m_fixedVisualInfosCompPtr.GetCount()){
				iqtgui::IVisualStatus* visualStatusPtr = m_fixedVisualInfosCompPtr[i];
				if (visualStatusPtr != nullptr){
					tabIcon = visualStatusPtr->GetStatusIcon();

					imod::IModel* visualStatusModelPtr = dynamic_cast<imod::IModel*>(visualStatusPtr);
					if (visualStatusModelPtr != nullptr){
						RegisterModel(visualStatusModelPtr, MI_VISUAL_STATUS_BASE_INDEX + i);
					}
				}
			}

			ibase::ICommandsProvider* commandsProviderPtr = CompCastPtr<ibase::ICommandsProvider>(guiPtr);
			if (commandsProviderPtr != nullptr) {
				imod::IModel* commandsProviderModelPtr = dynamic_cast<imod::IModel*>(commandsProviderPtr);
				if (commandsProviderModelPtr != nullptr) {
					RegisterModel(commandsProviderModelPtr, MI_DOCUMENT_COMMANDS_BASE_INDEX + i);
				}
			}

			AddTab(tabName, guiPtr, tabIcon);
		}
	}

	connect(Tabs, SIGNAL(currentChanged(int)), this, SLOT(OnWindowActivated(int)));
	connect(Tabs, SIGNAL(tabCloseRequested(int)), this, SLOT(OnTabCloseRequested(int)));

	int documentsCount = GetDocumentsCount();
	for (int docIndex = 0; docIndex < documentsCount; ++docIndex){
		int viewsCount = GetViewsCount(docIndex);

		SingleDocumentData& info = GetSingleDocumentData(docIndex);

		for (int viewIndex = 0; viewIndex < viewsCount; ++viewIndex){
			istd::IPolymorphic* viewPtr = GetViewFromIndex(docIndex, viewIndex);
			Q_ASSERT(viewPtr != nullptr);

			OnViewRegistered(viewPtr, info);
		}
	}
}


void CDocumentWorkspaceGuiCompBase::OnGuiDestroyed()
{
	BaseClass2::UnregisterAllModels();

	CloseAllDocuments();

	// Add additional fixed UI components to the tab bar:
	for (TabList::ConstIterator iter = m_fixedTabs.constBegin(); iter != m_fixedTabs.constEnd(); ++iter){
		iqtgui::IGuiObject* guiPtr = *iter;
		Q_ASSERT(guiPtr != nullptr);
		if (guiPtr->IsGuiCreated()){
			guiPtr->DestroyGui();
		}
	}

	m_fixedTabs.clear();

	BaseClass::OnGuiDestroyed();
}


void CDocumentWorkspaceGuiCompBase::OnRetranslate()
{
	BaseClass::OnRetranslate();
}


void CDocumentWorkspaceGuiCompBase::OnGuiRetranslate()
{
	BaseClass::OnGuiRetranslate();

	UpdateAllTitles();
	
	// refresh tab titles
	for (int i = 0; i < m_fixedTabsNamesAttrPtr.GetCount(); ++i){
		Tabs->setTabText(i, m_fixedTabsNamesAttrPtr[i]);
	}
}


// reimplemented (icomp::CComponentBase)

void CDocumentWorkspaceGuiCompBase::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	SetDocumentTemplate(m_documentTemplateCompPtr.GetPtr());
}


// reimplemented (istd:IChangeable)

void CDocumentWorkspaceGuiCompBase::OnEndChanges(const ChangeSet& changeSet)
{
	BaseClass::OnEndChanges(changeSet);

	if (IsGuiCreated()){
		UpdateAllTitles();
	}

	if (changeSet.Contains(CF_VIEW_ACTIVATION_CHANGED)){
		return;
	}

	idoc::CMultiDocumentManagerBase::SingleDocumentData* activeDocumentInfoPtr = GetActiveDocumentInfo();
	if (activeDocumentInfoPtr == nullptr){
		m_documentList.SetSelectedOptionIndex(iprm::ISelectionParam::NO_SELECTION);
		m_currentDocumentName.SetName("");
	}
	else{
		int documentIndex = GetDocumentIndex(*activeDocumentInfoPtr);

		m_documentList.SetSelectedOptionIndex(documentIndex);
	}
}


// reimplemented (QObject)

bool CDocumentWorkspaceGuiCompBase::eventFilter(QObject* sourcePtr, QEvent* eventPtr)
{
	if (eventPtr->type() == QEvent::DragEnter){
		QDragEnterEvent* dragEnterEventPtr = dynamic_cast<QDragEnterEvent*>(eventPtr);
		Q_ASSERT(dragEnterEventPtr != nullptr);

		OnDragEnterEvent(dragEnterEventPtr);

		return true;
	}
	else if (eventPtr->type() == QEvent::Drop){
		QDropEvent* dropEventPtr = dynamic_cast<QDropEvent*>(eventPtr);
		Q_ASSERT(dropEventPtr != nullptr);

		OnDropEvent(dropEventPtr);

		return true;
	}

	return BaseClass::eventFilter(sourcePtr, eventPtr);
}


// protected slots

void CDocumentWorkspaceGuiCompBase::OnCloseDocument()
{
	int pageIndex = Tabs->currentIndex();
	if (pageIndex > GetFixedWindowsCount() - 1){
		IDocumentViewDecorator* documentViewPtr = dynamic_cast<IDocumentViewDecorator*>(Tabs->widget(pageIndex));
		Q_ASSERT(documentViewPtr != nullptr);

		CloseView(documentViewPtr->GetView(), false);
	}
}


void CDocumentWorkspaceGuiCompBase::OnUndoDocument()
{
	SingleDocumentData* documentDataPtr = GetActiveDocumentInfo();
	if ((documentDataPtr != nullptr) && documentDataPtr->undoManagerPtr.IsValid() && documentDataPtr->undoManagerPtr->GetAvailableUndoSteps() > 0){
		documentDataPtr->undoManagerPtr->DoUndo();
	}
}


void CDocumentWorkspaceGuiCompBase::OnRedoDocument()
{
	SingleDocumentData* documentDataPtr = GetActiveDocumentInfo();
	if ((documentDataPtr != nullptr) && documentDataPtr->undoManagerPtr.IsValid() && documentDataPtr->undoManagerPtr->GetAvailableRedoSteps() > 0){
		documentDataPtr->undoManagerPtr->DoRedo();
	}
}


void CDocumentWorkspaceGuiCompBase::OnWindowActivated(int index)
{
	if (index < m_fixedTabs.size()){
		SetActiveView(m_fixedTabs[index]);

		Tabs->setCurrentIndex(index);
	}
	else{
		IDocumentViewDecorator* documentViewPtr = dynamic_cast<IDocumentViewDecorator*>(Tabs->widget(index));
		Q_ASSERT(documentViewPtr != nullptr);

		SetActiveView(documentViewPtr->GetView());
	}

	UpdateCommands();
}


void CDocumentWorkspaceGuiCompBase::OnCloseAllViews()
{
	bool isCanceled = false;
	if (SaveDirtyDocuments(false, &isCanceled) && !isCanceled){
		CloseAllDocuments();
	}
}


void CDocumentWorkspaceGuiCompBase::OnNewDocument(const QByteArray& documentTypeId)
{
	InsertNewDocument(documentTypeId);
}


void CDocumentWorkspaceGuiCompBase::OnOpenDocument(const QByteArray& documentTypeId)
{
	bool ignoredFlag = false;
	if (!OpenDocument(&documentTypeId, nullptr, true, "", nullptr, nullptr, false, &ignoredFlag)){
		if (!ignoredFlag){
			QMessageBox::warning(nullptr, "", tr("Document could not be opened"));
		}
	}
}


void CDocumentWorkspaceGuiCompBase::OnTabCloseRequested(int index)
{
	if (index > 0){
		if (index > GetFixedWindowsCount() - 1){
			QWidget* widgetPtr = Tabs->widget(index);

			IDocumentViewDecorator* documentViewPtr = dynamic_cast<IDocumentViewDecorator*>(widgetPtr);
			Q_ASSERT(documentViewPtr != nullptr);

			CloseView(documentViewPtr->GetView(), m_forceQuietClose);
		}
	}
}


// public methods of the embedded class DocumentList

CDocumentWorkspaceGuiCompBase::DocumentList::DocumentList()
	:m_selectedDocumentIndex(iprm::ISelectionParam::NO_SELECTION)
{
}


void CDocumentWorkspaceGuiCompBase::DocumentList::SetParent(CDocumentWorkspaceGuiCompBase& parent)
{
	m_parent = &parent;
}


// reimplemented (iprm::ISelectionParam)

const iprm::IOptionsList* CDocumentWorkspaceGuiCompBase::DocumentList::GetSelectionConstraints() const
{
	return this;
}


int CDocumentWorkspaceGuiCompBase::DocumentList::GetSelectedOptionIndex() const
{
	return m_selectedDocumentIndex;
}


bool CDocumentWorkspaceGuiCompBase::DocumentList::SetSelectedOptionIndex(int index)
{
	if (index >= GetOptionsCount()){
		return false;
	}

	if (m_selectedDocumentIndex != index){
		static ChangeSet changeSet(CF_SELECTION_CHANGED);
		istd::CChangeNotifier changePtr(this, &changeSet);

		m_selectedDocumentIndex = index;

		istd::IPolymorphic* viewPtr = nullptr;
		if (m_selectedDocumentIndex >= 0){
			viewPtr = m_parent->GetViewFromIndex(m_selectedDocumentIndex, 0);

			m_parent->SetActiveView(viewPtr);
		}
	}

	return true;
}


iprm::ISelectionParam* CDocumentWorkspaceGuiCompBase::DocumentList::GetSubselection(int /*index*/) const
{
	return nullptr;
}


// reimplemented (iprm::IOptionsList)

int CDocumentWorkspaceGuiCompBase::DocumentList::GetOptionsFlags() const
{
	return SCF_SUPPORT_UNIQUE_ID;
}


int CDocumentWorkspaceGuiCompBase::DocumentList::GetOptionsCount() const
{
	Q_ASSERT(m_parent != nullptr);

	return m_parent->GetDocumentsCount();
}


QString CDocumentWorkspaceGuiCompBase::DocumentList::GetOptionName(int index) const
{
	SingleDocumentData& documentData = m_parent->GetSingleDocumentData(index);

	if (!documentData.filePath.isEmpty()){
		QFileInfo fileInfo(QDir::toNativeSeparators(documentData.filePath));

		return fileInfo.fileName();
	}

	return QString(tr("<no name>"));
}


QString CDocumentWorkspaceGuiCompBase::DocumentList::GetOptionDescription(int /*index*/) const
{
	return QString();
}


QByteArray CDocumentWorkspaceGuiCompBase::DocumentList::GetOptionId(int index) const
{
	return GetOptionName(index).toLatin1();
}


bool CDocumentWorkspaceGuiCompBase::DocumentList::IsOptionEnabled(int /*index*/) const
{
	return true;
}


// reimplemented (iser::ISerializable)

bool CDocumentWorkspaceGuiCompBase::DocumentList::Serialize(iser::IArchive& /*archive*/)
{
	I_CRITICAL(); // NOT IMPLEMENTED

	return false;
}


} // namespace imtgui


