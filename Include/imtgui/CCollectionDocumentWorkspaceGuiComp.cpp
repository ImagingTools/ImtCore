#include <imtgui/CCollectionDocumentWorkspaceGuiComp.h>


// Qt includes
#include <QtCore/QMap>
#include <QtCore/QFileInfo>
#include <QtCore/QEvent>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QTabBar>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QTreeWidgetItem>

// ACF includes
#include <istd/CChangeNotifier.h>
#include <idoc/IDocumentTemplate.h>
#include <iqt/CSettingsWriteArchive.h>
#include <iqt/CSettingsReadArchive.h>


namespace imtgui
{


// public methods

CCollectionDocumentWorkspaceGuiComp::CCollectionDocumentWorkspaceGuiComp()
:	m_viewsCount(0),
	m_forceQuietClose(false)
{
	m_documentList.SetParent(*this);

	m_commands.InsertChild(&m_windowCommand, false);
	m_windowCommand.SetPriority(130);
	m_closeAllDocumentsCommand.SetGroupId(GI_DOCUMENT);
	m_windowCommand.InsertChild(&m_closeAllDocumentsCommand, false);

	connect(&m_closeAllDocumentsCommand, SIGNAL(triggered()), this, SLOT(OnCloseAllViews()));
}


// reimplemented (ibase::ICommandsProvider)

const ibase::IHierarchicalCommand* CCollectionDocumentWorkspaceGuiComp::GetCommands() const
{
	return &m_commands;
}


// reimplemented (iqtgui::IGuiObject)

void CCollectionDocumentWorkspaceGuiComp::OnTryClose(bool* ignoredPtr)
{
	//Save open document settings before exit
	if (m_rememberOpenDocumentsParamPtr.IsValid() && m_rememberOpenDocumentsParamPtr->IsEnabled()){
		if (!m_organizationName.isEmpty() && !m_applicationName.isEmpty()){
			iqt::CSettingsWriteArchive archive(
						m_organizationName,
						m_applicationName,
						"OpenDocumentList",
						QSettings::UserScope);

			SerializeOpenDocumentList(archive);
		}
	}

	if (SaveDirtyDocuments(false, ignoredPtr)){
		CloseAllDocuments();
	}

	if (ignoredPtr != NULL){
		*ignoredPtr = (GetDocumentsCount() > 0);
	}
}


// protected members

void CCollectionDocumentWorkspaceGuiComp::UpdateAllTitles()
{
	typedef QMap<QString, int> NameFrequencies;
	NameFrequencies nameFrequencies;

	typedef QMap<QString, QString> TitleToFilePathMap;
	TitleToFilePathMap titleToFilePathMap;

	int documentsCount = GetDocumentsCount();
	for (int i = 0; i < documentsCount; ++i){
		const SingleDocumentData& info = GetSingleDocumentData(i);

		QString titleName = info.filePath.isEmpty()?
					tr("<no name>"):
					QFileInfo(info.filePath).fileName();

		NameFrequencies::Iterator freqIter = nameFrequencies.find(titleName);
		if (freqIter != nameFrequencies.end()){
			int& frequency = freqIter.value();

			frequency++;

			titleName = tr("%1 <%2>").arg(titleName).arg(frequency + 1);
		}
		else{
			nameFrequencies[titleName] = 0;
		}

		if (info.isDirty){
			titleName += " *";
		}

		for (		Views::ConstIterator viewIter = info.views.begin();
					viewIter != info.views.end();
					++viewIter){
			const ViewInfo& viewInfo = *viewIter;
			Q_ASSERT(viewInfo.viewPtr.IsValid());

			const iqtgui::IGuiObject* guiObjectPtr = CompCastPtr<iqtgui::IGuiObject>(viewInfo.viewPtr.GetPtr());
			if (guiObjectPtr != NULL){
				QWidget* widgetPtr = guiObjectPtr->GetWidget();
				Q_ASSERT(widgetPtr != NULL);

				widgetPtr->setWindowTitle(titleName);

				titleToFilePathMap[titleName] = info.filePath;
			}
		}
	}
}


iqtgui::IGuiObject* CCollectionDocumentWorkspaceGuiComp::GetViewFromWidget(const QWidget& widget) const
{
	int documentInfosCount = GetDocumentsCount();
	for (int documentIndex = 0; documentIndex < documentInfosCount; ++documentIndex){
		SingleDocumentData& info = GetSingleDocumentData(documentIndex);

		for (		Views::ConstIterator viewIter = info.views.begin();
					viewIter != info.views.end();
					++viewIter){
			const ViewInfo& viewInfo = *viewIter;

			iqtgui::IGuiObject* guiObjectPtr = CompCastPtr<iqtgui::IGuiObject>(viewInfo.viewPtr.GetPtr());
			if (guiObjectPtr != NULL){
				if (guiObjectPtr->GetWidget() == &widget){
					return guiObjectPtr;
				}
			}
		}
	}

	return NULL;
}


int CCollectionDocumentWorkspaceGuiComp::GetDocumentIndexFromWidget(const QWidget& widget) const
{
	int documentInfosCount = GetDocumentsCount();
	for (int documentIndex = 0; documentIndex < documentInfosCount; ++documentIndex){
		SingleDocumentData& info = GetSingleDocumentData(documentIndex);

		for (		Views::ConstIterator viewIter = info.views.begin();
					viewIter != info.views.end();
					++viewIter){
			const ViewInfo& viewInfo = *viewIter;

			iqtgui::IGuiObject* guiObjectPtr = CompCastPtr<iqtgui::IGuiObject>(viewInfo.viewPtr.GetPtr());
			if (guiObjectPtr != NULL){
				if (guiObjectPtr->GetWidget() == &widget){
					return documentIndex;
				}
			}
		}
	}

	return -1;
}


void CCollectionDocumentWorkspaceGuiComp::OnViewsCountChanged()
{
	m_closeAllDocumentsCommand.SetEnabled(m_viewsCount > 0);
}


// reimplemented (idoc::CMultiDocumentManagerBase)

istd::IChangeable* CCollectionDocumentWorkspaceGuiComp::OpenSingleDocument(
			const QString& filePath,
			bool createView,
			const QByteArray& viewTypeId,
			QByteArray& documentTypeId,
			bool beQuiet,
			bool* ignoredPtr,
			ibase::IProgressManager* progressManagerPtr)
{
	bool allowViewRepeating = true;
	if (m_allowViewRepeatingAttrPtr.IsValid()){
		allowViewRepeating = *m_allowViewRepeatingAttrPtr;
	}

	SingleDocumentData* documentInfoPtr = GetDocumentInfoFromPath(filePath);
	if (documentInfoPtr != NULL && !allowViewRepeating){
		createView = false;
	}

	return BaseClass::OpenSingleDocument(filePath, createView, viewTypeId, documentTypeId, beQuiet, ignoredPtr, progressManagerPtr);
}


void CCollectionDocumentWorkspaceGuiComp::SetActiveView(istd::IPolymorphic* viewPtr)
{
	if (viewPtr != GetActiveView()){
		// TODO: Find and set index in the stack to show required view:

		idoc::CMultiDocumentManagerBase::SingleDocumentData* activeDocumentInfoPtr = GetDocumentInfoFromView(*viewPtr);
		if (activeDocumentInfoPtr == NULL){
			m_documentList.SetSelectedOptionIndex(iprm::ISelectionParam::NO_SELECTION);
		}
		else{
			int documentIndex = GetDocumentIndex(*activeDocumentInfoPtr);

			m_documentList.SetSelectedOptionIndex(documentIndex);
		}
	}

	BaseClass::SetActiveView(viewPtr);
}


// reimplemented (QObject)

bool CCollectionDocumentWorkspaceGuiComp::eventFilter(QObject* sourcePtr, QEvent* eventPtr)
{
	if (!m_forceQuietClose && (eventPtr->type() == QEvent::Close)){
		const QWidget* widgetPtr = dynamic_cast<const QWidget*>(sourcePtr);
		if (widgetPtr != NULL){
			int documentIndex = GetDocumentIndexFromWidget(*widgetPtr);
			if (documentIndex >= 0){
				bool isCloseIgnored = false;
				CloseDocument(documentIndex, m_forceQuietClose, &isCloseIgnored);

				if (isCloseIgnored){
					eventPtr->ignore();

					return true;
				}
			}
		}
	}

	return BaseClass::eventFilter(sourcePtr, eventPtr);
}


// reimplemented (idoc::CMultiDocumentManagerBase)

void CCollectionDocumentWorkspaceGuiComp::CloseAllDocuments()
{
	m_forceQuietClose = true;

	m_documentList.SetSelectedOptionIndex(iprm::ISelectionParam::NO_SELECTION);

	// TODO: Close all opened widgets:
// 	workspacePtr->closeAllSubWindows();

	BaseClass::CloseAllDocuments();

	m_forceQuietClose = false;
}


QStringList CCollectionDocumentWorkspaceGuiComp::GetOpenFilePaths(const QByteArray* documentTypeIdPtr) const
{
	QStringList files = GetOpenFilePathesFromDialog(documentTypeIdPtr);

	if (!files.isEmpty()){
		UpdateLastDirectory(files.at(0));
	}

	return files;
}


void CCollectionDocumentWorkspaceGuiComp::OnViewRegistered(istd::IPolymorphic* viewPtr, const SingleDocumentData& /*documentData*/)
{
	iqtgui::IGuiObject* guiObjectPtr = CompCastPtr<iqtgui::IGuiObject>(viewPtr);
	if (guiObjectPtr != NULL){
		QWidget* frameForGuiComponentPtr = NULL; // TODO Create stack page
		frameForGuiComponentPtr = new QWidget();
		DocumentStack->setCurrentIndex(DocumentStack->addWidget(frameForGuiComponentPtr));
		QVBoxLayout* frameForGuiLayout = new QVBoxLayout(frameForGuiComponentPtr);
		Q_UNUSED(frameForGuiLayout);

		if (guiObjectPtr->CreateGui(frameForGuiComponentPtr)){
			QWidget* widgetPtr = guiObjectPtr->GetWidget();
			Q_ASSERT(widgetPtr != NULL);

			widgetPtr->installEventFilter(this);
						
			// TODO: Add item to the list!

			SetActiveView(viewPtr);
		}
		else{
			DocumentStack->removeWidget(frameForGuiComponentPtr);
			delete frameForGuiComponentPtr;
		}

	}

	++m_viewsCount;

	OnViewsCountChanged();
}


void CCollectionDocumentWorkspaceGuiComp::OnViewRemoved(istd::IPolymorphic* viewPtr)
{
	--m_viewsCount;

	OnViewsCountChanged();

	iqtgui::IGuiObject* guiObjectPtr = CompCastPtr<iqtgui::IGuiObject>(viewPtr);
	if (guiObjectPtr != NULL){
		guiObjectPtr->DestroyGui();
	}
}


bool CCollectionDocumentWorkspaceGuiComp::QueryDocumentSave(const SingleDocumentData& info, bool* ignoredPtr)
{
	QFileInfo fileInfo(info.filePath);
	QMessageBox::StandardButtons buttons = QMessageBox::Yes | QMessageBox::No;

	if (ignoredPtr != NULL){
		*ignoredPtr = false;
		buttons |= QMessageBox::Cancel;
	}

	int response = QMessageBox::information(
				GetQtWidget(),
				tr("Close document"),
				tr("Do you want to save your changes made in document\n%1").arg(fileInfo.fileName()),
				buttons,
				QMessageBox::Yes);

	if (response == QMessageBox::Yes){
		return true;
	}
	else if ((ignoredPtr != NULL) && (response == QMessageBox::Cancel)){
		*ignoredPtr = true;
	}

	return false;
}


// reimplemented (iqt:CGuiObjectBase)

void CCollectionDocumentWorkspaceGuiComp::OnGuiCreated()
{
	BaseClass::OnGuiCreated();

	m_pToolBar = new QToolBar();

	m_pBtnBack = new QPushButton("Back", m_pToolBar);
	m_pBtnSave = new QPushButton("Save", m_pToolBar);
	m_pBtnClose = new QPushButton("Close", m_pToolBar);
	m_pToolBar->addWidget(m_pBtnBack);
	m_pToolBar->addWidget(m_pBtnSave);
	m_pToolBar->addWidget(m_pBtnClose);

	connect(m_pBtnBack, &QPushButton::clicked, this, &CCollectionDocumentWorkspaceGuiComp::OnBtnBack);
	connect(m_pBtnSave, &QPushButton::clicked, this, &CCollectionDocumentWorkspaceGuiComp::OnBtnSave);
	connect(m_pBtnClose, &QPushButton::clicked, this, &CCollectionDocumentWorkspaceGuiComp::OnBtnClose);

	verticalLayout->insertWidget(0, m_pToolBar);

	int documentsCount = GetDocumentsCount();
	for (int docIndex = 0; docIndex < documentsCount; ++docIndex){
		int viewsCount = GetViewsCount(docIndex);

		SingleDocumentData& info = GetSingleDocumentData(docIndex);

		for (int viewIndex = 0; viewIndex < viewsCount; ++viewIndex){
			istd::IPolymorphic* viewPtr = GetViewFromIndex(docIndex, viewIndex);
			Q_ASSERT(viewPtr != NULL);

			OnViewRegistered(viewPtr, info);
		}
	}

	OnViewsCountChanged();
}


void CCollectionDocumentWorkspaceGuiComp::OnGuiDestroyed()
{
	CloseAllDocuments();

	BaseClass::OnGuiDestroyed();
}


void CCollectionDocumentWorkspaceGuiComp::OnRetranslate()
{
	BaseClass::OnRetranslate();

	static const istd::IChangeable::ChangeSet commandsChangeSet(ibase::ICommandsProvider::CF_COMMANDS);
	istd::CChangeNotifier commandsNotifier(this, &commandsChangeSet);
	Q_UNUSED(commandsNotifier);

	m_windowCommand.SetName(tr("&Window"));
	m_closeAllDocumentsCommand.SetVisuals(tr("&Close All Documents"), tr("Close All"), tr("Closes all opened documents"));
}


void CCollectionDocumentWorkspaceGuiComp::OnGuiRetranslate()
{
	BaseClass::OnGuiRetranslate();

	UpdateAllTitles();
}


// reimplemented (icomp::CComponentBase)

void CCollectionDocumentWorkspaceGuiComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	SetDocumentTemplate(m_documentTemplateCompPtr.GetPtr());
}


// reimplemented (istd:IChangeable)

void CCollectionDocumentWorkspaceGuiComp::OnEndChanges(const ChangeSet& changeSet)
{
	BaseClass::OnEndChanges(changeSet);

	if (IsGuiCreated()){
		UpdateAllTitles();
	}

	idoc::CMultiDocumentManagerBase::SingleDocumentData* activeDocumentInfoPtr = GetActiveDocumentInfo();
	if (activeDocumentInfoPtr == NULL){
		m_documentList.SetSelectedOptionIndex(iprm::ISelectionParam::NO_SELECTION);
	}
	else{
		int documentIndex = GetDocumentIndex(*activeDocumentInfoPtr);

		m_documentList.SetSelectedOptionIndex(documentIndex);
	}
}


// protected slots

void CCollectionDocumentWorkspaceGuiComp::OnCloseAllViews()
{
	bool isCanceled = false;
	if (SaveDirtyDocuments(false, &isCanceled) && !isCanceled){
		CloseAllDocuments();
	}
}

// private slots

void CCollectionDocumentWorkspaceGuiComp::OnBtnBack()
{
	DocumentStack->setCurrentIndex(0);
}


void CCollectionDocumentWorkspaceGuiComp::OnBtnSave()
{

}


void CCollectionDocumentWorkspaceGuiComp::OnBtnClose()
{

}

// public methods of the embedded class DocumentList

CCollectionDocumentWorkspaceGuiComp::DocumentList::DocumentList()
	:m_selectedDocumentIndex(iprm::ISelectionParam::NO_SELECTION),
	m_parent(NULL)
{
}


void CCollectionDocumentWorkspaceGuiComp::DocumentList::SetParent(CCollectionDocumentWorkspaceGuiComp& parent)
{
	m_parent = &parent;
}


// reimplemented (iprm::ISelectionParam)

const iprm::IOptionsList* CCollectionDocumentWorkspaceGuiComp::DocumentList::GetSelectionConstraints() const
{
	return this;
}


int CCollectionDocumentWorkspaceGuiComp::DocumentList::GetSelectedOptionIndex() const
{
	return m_selectedDocumentIndex;
}


bool CCollectionDocumentWorkspaceGuiComp::DocumentList::SetSelectedOptionIndex(int index)
{
	if (index >= GetOptionsCount()){
		return false;
	}

	if (m_selectedDocumentIndex != index){
		static const ChangeSet changeSet(CF_SELECTION_CHANGED);
		istd::CChangeNotifier notifier(this, &changeSet);
		Q_UNUSED(notifier);

		m_selectedDocumentIndex = index;

		if (m_selectedDocumentIndex >= 0){
			istd::IPolymorphic* viewPtr = m_parent->GetViewFromIndex(m_selectedDocumentIndex, 0);

			m_parent->SetActiveView(viewPtr);
		}
	}

	return true;
}


iprm::ISelectionParam* CCollectionDocumentWorkspaceGuiComp::DocumentList::GetSubselection(int /*index*/) const
{
	return NULL;
}


// reimplemented (iprm::IOptionsList)

int CCollectionDocumentWorkspaceGuiComp::DocumentList::GetOptionsFlags() const
{
	return SCF_SUPPORT_UNIQUE_ID;
}


int CCollectionDocumentWorkspaceGuiComp::DocumentList::GetOptionsCount() const
{
	Q_ASSERT(m_parent != NULL);

	return m_parent->GetDocumentsCount();
}


QString CCollectionDocumentWorkspaceGuiComp::DocumentList::GetOptionName(int index) const
{
	SingleDocumentData& documentData = m_parent->GetSingleDocumentData(index);

	if (!documentData.filePath.isEmpty()){
		QFileInfo fileInfo(documentData.filePath);

		return fileInfo.fileName();
	}

	return QString(tr("<no name>"));
}


QString CCollectionDocumentWorkspaceGuiComp::DocumentList::GetOptionDescription(int /*index*/) const
{
	return QString();
}


QByteArray CCollectionDocumentWorkspaceGuiComp::DocumentList::GetOptionId(int index) const
{
	return GetOptionName(index).toLatin1();
}


bool CCollectionDocumentWorkspaceGuiComp::DocumentList::IsOptionEnabled(int /*index*/) const
{
	return true;
}


// reimplemented (iser::ISerializable)

bool CCollectionDocumentWorkspaceGuiComp::DocumentList::Serialize(iser::IArchive& /*archive*/)
{
	I_CRITICAL(); // NOT IMPLEMENTED

	return false;
}


} // namespace imtgui


