#include <imtgui/CDocumentWorkspaceGuiCompBase.h>


// Qt includes
#include <QtCore/QMap>
#include <QtCore/QFileInfo>
#include <QtCore/QUrl>
#include <QtCore/QMimeData>
#include <QtGui/QDragEnterEvent>
#include <QtWidgets/QTabBar>
#include <QtWidgets/QMessageBox>

// ACF includes
#include <istd/CChangeNotifier.h>
#include <istd/CChangeGroup.h>
#include <iser/CPrimitiveTypesSerializer.h>
#include <ibase/ICommandsProvider.h>
#include <idoc/IDocumentMetaInfo.h>
#include <idoc/IDocumentTemplate.h>
#include <iwidgets/CWidgetUpdateBlocker.h>


namespace imtgui
{


// public methods

CDocumentWorkspaceGuiCompBase::CDocumentWorkspaceGuiCompBase()
	:m_forceQuietClose(false),
	m_isUpdateBlocked(false),
	m_previousTabIndex(-1)
{
	m_documentList.SetParent(*this);
	m_commands.SetParent(this);
}


// reimplemented (iqtgui::IGuiObject)

void CDocumentWorkspaceGuiCompBase::OnTryClose(bool* ignoredPtr)
{
	if (SaveDirtyDocuments(false, ignoredPtr)){
		int documentInfosCount = GetDocumentsCount();
		for (int documentIndex = 0; documentIndex < documentInfosCount; ++documentIndex){
			SingleDocumentData& documentData = GetSingleDocumentData(documentIndex);

			documentData.isDirty = false;
		}

		CloseAllDocuments();
	}

	if (ignoredPtr != nullptr){
		*ignoredPtr = (GetDocumentsCount() > 0);
	}
}


// reimplemented (idoc::IDocumentManager)

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
		const SingleDocumentData& documentData = GetSingleDocumentData(documentIndex);

		for (		Views::ConstIterator viewIter = documentData.views.begin();
					viewIter != documentData.views.end();
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
		Q_EMIT PostUpdateCommands();
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

			iqtgui::IVisualStatus* visualStatusPtr = CompCastPtr<iqtgui::IVisualStatus>(viewPtr);
			if (visualStatusPtr != nullptr){
				Tabs->setTabIcon(newViewIndex, visualStatusPtr->GetStatusIcon());
			}

			imod::IModel* visualStatusModelPtr = dynamic_cast<imod::IModel*>(visualStatusPtr);
			if (visualStatusModelPtr != nullptr){
				RegisterModel(visualStatusModelPtr, MI_VISUAL_STATUS_BASE_INDEX + newViewIndex);
			}

			ibase::ICommandsProvider* commandsProviderPtr = CompCastPtr<ibase::ICommandsProvider>(viewPtr);
			imod::IModel* commandsProviderModelPtr = dynamic_cast<imod::IModel*>(commandsProviderPtr);
			if (commandsProviderModelPtr != nullptr){
				RegisterModel(commandsProviderModelPtr, MI_DOCUMENT_COMMANDS_BASE_INDEX + newViewIndex);
			}

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

			OnViewActivated(lastPageIndex);

			Tabs->removeTab(pageIndex);

			break;
		}
	}
}


bool CDocumentWorkspaceGuiCompBase::QueryDocumentSave(const SingleDocumentData& info, bool* ignoredPtr)
{
	QMessageBox::StandardButtons buttons = QMessageBox::Yes | QMessageBox::No;

	if (ignoredPtr != nullptr){
		*ignoredPtr = false;

		buttons |= QMessageBox::Cancel;
	}

	QFileInfo fileInfo(info.filePath);

	int dialogResult = QMessageBox::information(
				GetQtWidget(),
				tr("Close document"),
				tr("Do you want to save your changes made in document\n%1").arg(QDir::toNativeSeparators(fileInfo.completeBaseName())),
				buttons,
				QMessageBox::Yes);

	if (dialogResult == QMessageBox::Yes){
		return true;
	}
	else if ((ignoredPtr != nullptr) && (dialogResult == QMessageBox::Cancel)){
		*ignoredPtr = true;
	}

	return false;
}


// reimplemented (iqt:CGuiObjectBase)

void CDocumentWorkspaceGuiCompBase::OnGuiCreated()
{
	BaseClass::OnGuiCreated();

	switch (*m_tabsPositionAttrPtr){
	case 1:
		Tabs->setTabPosition(QTabWidget::South);
		break;
	case 2:
		Tabs->setTabPosition(QTabWidget::West);
		break;
	case 3:
		Tabs->setTabPosition(QTabWidget::East);
		break;
	default:
		Tabs->setTabPosition(QTabWidget::North);
	}

	connect(
				this,
				&CDocumentWorkspaceGuiCompBase::PostUpdateCommands,
				this,
				&CDocumentWorkspaceGuiCompBase::UpdateCommands,
				Qt::QueuedConnection);

	QWidget* mainWindowPtr = GetQtWidget();
	if (mainWindowPtr != nullptr){
		mainWindowPtr->setAcceptDrops(true);
	}

	Tabs->setDocumentMode(true);
	Tabs->tabBar()->setDrawBase(false);
	Tabs->setTabsClosable(true);
	Tabs->setElideMode(Qt::ElideMiddle);
	Tabs->setIconSize(QSize(24, 24));

	m_closeCurrentTabShortcutPtr = new QShortcut(GetQtWidget());
	m_closeCurrentTabShortcutPtr->setKey(Qt::CTRL + Qt::Key_W);
	
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
			if (commandsProviderPtr != nullptr){
				imod::IModel* commandsProviderModelPtr = dynamic_cast<imod::IModel*>(commandsProviderPtr);
				if (commandsProviderModelPtr != nullptr){
					RegisterModel(commandsProviderModelPtr, MI_DOCUMENT_COMMANDS_BASE_INDEX + i);
				}
			}

			AddTab(tabName, guiPtr, tabIcon);
		}
	}

	connect(Tabs, &QTabWidget::currentChanged, this, &CDocumentWorkspaceGuiCompBase::OnViewActivated);
	connect(Tabs, &QTabWidget::tabCloseRequested, this, &CDocumentWorkspaceGuiCompBase::OnViewCloseTriggered);
	connect(m_closeCurrentTabShortcutPtr, &QShortcut::activated, this, &CDocumentWorkspaceGuiCompBase::OnCurrentViewCloseTriggered);

	int documentsCount = GetDocumentsCount();
	for (int documentIndex = 0; documentIndex < documentsCount; ++documentIndex){
		int viewsCount = GetViewsCount(documentIndex);

		const SingleDocumentData& documentInfo = GetSingleDocumentData(documentIndex);

		for (int viewIndex = 0; viewIndex < viewsCount; ++viewIndex){
			istd::IPolymorphic* viewPtr = GetViewFromIndex(documentIndex, viewIndex);
			Q_ASSERT(viewPtr != nullptr);

			OnViewRegistered(viewPtr, documentInfo);
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

	if (!changeSet.Contains(CF_VIEW_ACTIVATION_CHANGED)){
		idoc::CMultiDocumentManagerBase::SingleDocumentData* activeDocumentInfoPtr = GetActiveDocumentInfo();
		if (activeDocumentInfoPtr == nullptr){
			m_documentList.SetSelectedOptionIndex(iprm::ISelectionParam::NO_SELECTION);

			m_currentDocumentName.SetName(QString());
		}
		else{
			int documentIndex = GetDocumentIndex(*activeDocumentInfoPtr);

			m_documentList.SetSelectedOptionIndex(documentIndex);
		}
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

void CDocumentWorkspaceGuiCompBase::UpdateCommands()
{
	static ChangeSet changes(ibase::ICommandsProvider::CF_COMMANDS);

	istd::CChangeNotifier changeNotifier(&m_commands, &changes);
}


void CDocumentWorkspaceGuiCompBase::OnCloseDocument()
{
	int pageIndex = Tabs->currentIndex();
	if (pageIndex > GetFixedWindowsCount() - 1){
		IDocumentViewDecorator* documentViewPtr = dynamic_cast<IDocumentViewDecorator*>(Tabs->widget(pageIndex));
		Q_ASSERT(documentViewPtr != nullptr);

		bool ignored = false;
		CloseView(documentViewPtr->GetView(), false, &ignored);
	}
}


void CDocumentWorkspaceGuiCompBase::OnUndo()
{
	SingleDocumentData* documentDataPtr = GetActiveDocumentInfo();
	if ((documentDataPtr != nullptr) && documentDataPtr->undoManagerPtr.IsValid() && documentDataPtr->undoManagerPtr->GetAvailableUndoSteps() > 0){
		documentDataPtr->undoManagerPtr->DoUndo();
	}
}


void CDocumentWorkspaceGuiCompBase::OnRedo()
{
	SingleDocumentData* documentDataPtr = GetActiveDocumentInfo();
	if ((documentDataPtr != nullptr) && documentDataPtr->undoManagerPtr.IsValid() && documentDataPtr->undoManagerPtr->GetAvailableRedoSteps() > 0){
		documentDataPtr->undoManagerPtr->DoRedo();
	}
}


void CDocumentWorkspaceGuiCompBase::OnViewActivated(int index)
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


void CDocumentWorkspaceGuiCompBase::OnViewCloseTriggered(int index)
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


void CDocumentWorkspaceGuiCompBase::OnCurrentViewCloseTriggered()
{
	OnViewCloseTriggered(Tabs->currentIndex());
}


// public methods of the embedded class DocumentList

CDocumentWorkspaceGuiCompBase::DocumentList::DocumentList()
	:m_selectedIndex(iprm::ISelectionParam::NO_SELECTION),
	m_parentPtr(nullptr)
{
}


void CDocumentWorkspaceGuiCompBase::DocumentList::SetParent(CDocumentWorkspaceGuiCompBase& parent)
{
	m_parentPtr = &parent;
}


// reimplemented (iprm::ISelectionParam)

const iprm::IOptionsList* CDocumentWorkspaceGuiCompBase::DocumentList::GetSelectionConstraints() const
{
	return this;
}


int CDocumentWorkspaceGuiCompBase::DocumentList::GetSelectedOptionIndex() const
{
	return m_selectedIndex;
}


bool CDocumentWorkspaceGuiCompBase::DocumentList::SetSelectedOptionIndex(int index)
{
	if (index >= GetOptionsCount()){
		return false;
	}

	if (m_selectedIndex != index){
		static ChangeSet changeSet(CF_SELECTION_CHANGED);
		istd::CChangeNotifier changePtr(this, &changeSet);

		m_selectedIndex = index;

		istd::IPolymorphic* viewPtr = nullptr;
		if (m_selectedIndex >= 0){
			viewPtr = m_parentPtr->GetViewFromIndex(m_selectedIndex, 0);

			m_parentPtr->SetActiveView(viewPtr);
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
	Q_ASSERT(m_parentPtr != nullptr);

	return m_parentPtr->GetDocumentsCount();
}


QString CDocumentWorkspaceGuiCompBase::DocumentList::GetOptionName(int index) const
{
	const SingleDocumentData& documentData = m_parentPtr->GetSingleDocumentData(index);

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


// reimplemented (imtbase::IObjectCollectionInfo)

bool CDocumentWorkspaceGuiCompBase::DocumentList::GetCollectionItemMetaInfo(const QByteArray& /*objectId*/, idoc::IDocumentMetaInfo& /*metaInfo*/) const
{
	return false;
}


const iprm::IOptionsList* CDocumentWorkspaceGuiCompBase::DocumentList::GetObjectTypesInfo() const
{
	return nullptr;
}


imtbase::ICollectionInfo::Id CDocumentWorkspaceGuiCompBase::DocumentList::GetObjectTypeId(const QByteArray& /*objectId*/) const
{
	Id retVal;

	return retVal;
}


// reimplemented (imtbase::ICollectionInfo)

imtbase::ICollectionInfo::Ids CDocumentWorkspaceGuiCompBase::DocumentList::GetElementIds() const
{
	return Ids();
}


QVariant CDocumentWorkspaceGuiCompBase::DocumentList::GetElementInfo(const QByteArray& /*elementId*/, int /*infoType*/) const
{
	return QVariant();
}


// reimplemented (iser::ISerializable)

bool CDocumentWorkspaceGuiCompBase::DocumentList::Serialize(iser::IArchive& /*archive*/)
{
	I_CRITICAL(); // NOT IMPLEMENTED

	return false;
}




// public methods of the embedded class Commands

CDocumentWorkspaceGuiCompBase::Commands::Commands()
	:m_parentPtr(nullptr)
{
}


void CDocumentWorkspaceGuiCompBase::Commands::SetParent(CDocumentWorkspaceGuiCompBase* parentPtr)
{
	Q_ASSERT(parentPtr != nullptr);

	m_parentPtr = parentPtr;
}


// reimplemented (ibase::ICommandsProvider)

const ibase::IHierarchicalCommand* CDocumentWorkspaceGuiCompBase::Commands::GetCommands() const
{
	Q_ASSERT(m_parentPtr != nullptr);

	if (m_parentPtr->IsGuiCreated()){
		int tabIndex = m_parentPtr->Tabs->currentIndex();
		if (tabIndex >= 0){
			if (tabIndex >= m_parentPtr->m_fixedTabs.count()){
				QWidget* decoratorPagePtr = m_parentPtr->Tabs->currentWidget();
				ibase::ICommandsProvider* decoratorPtr = dynamic_cast<ibase::ICommandsProvider*>(decoratorPagePtr);
				if (decoratorPtr != nullptr){
					return decoratorPtr->GetCommands();
				}
				else{
					istd::IPolymorphic* viewPtr = m_parentPtr->GetActiveView();
					ibase::ICommandsProvider* viewCommandsProviderPtr = CompCastPtr<ibase::ICommandsProvider>(viewPtr);
					if (viewCommandsProviderPtr != nullptr){
						return viewCommandsProviderPtr->GetCommands();
					}
				}
			}
			else{
				ibase::ICommandsProvider* viewCommandsProviderPtr = CompCastPtr<ibase::ICommandsProvider>(m_parentPtr->m_fixedTabs[tabIndex]);
				if (viewCommandsProviderPtr != nullptr){
					return viewCommandsProviderPtr->GetCommands();
				}
			}
		}
	}

	return nullptr;
}


} // namespace imtgui


