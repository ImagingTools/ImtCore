#pragma once


// Qt includes
#include <QtCore/QFileInfo>
#include <QtCore/QUrl>
#include <QtCore/QMimeData>
#include <QtGui/QDragEnterEvent>
#include <QtGui/QDropEvent>
#include <QtWidgets/QMessageBox>

// ACF includes
#include <imod/TModelWrap.h>
#include <iprm/CNameParam.h>
#include <idoc/IDocumentTemplate.h>
#include <iqtgui/TDesignerGuiCompBase.h>
#include <iqtgui/TRestorableGuiWrap.h>
#include <iqtdoc/TQtDocumentManagerWrap.h>

// ImtCore includes
#include <imtbase/IObjectCollectionInfo.h>
#include <imtgui/IDocumentViewDecorator.h>


namespace imtgui
{


template <class DocumentManagerBase, class UI>
class TDocumentWorkspaceGuiCompBase:
			public iqtdoc::TQtDocumentManagerWrap<DocumentManagerBase, UI>
{
	Q_DECLARE_TR_FUNCTIONS(TDocumentWorkspaceGuiCompBase)
public:
	typedef iqtdoc::TQtDocumentManagerWrap<DocumentManagerBase, UI> BaseClass;
	typedef typename DocumentManagerBase::SingleDocumentData SingleDocumentData;
	typedef typename DocumentManagerBase::Views Views;
	typedef typename Views::ConstIterator ConstIterator;
	typedef typename DocumentManagerBase::ViewInfo ViewInfo;

	I_BEGIN_BASE_COMPONENT(TDocumentWorkspaceGuiCompBase);
		I_REGISTER_INTERFACE(idoc::IDocumentManager);
		I_REGISTER_INTERFACE(idoc::IDocumentTypesInfo);
		I_REGISTER_SUBELEMENT(CurrentDocumentName);
		I_REGISTER_SUBELEMENT_INTERFACE(CurrentDocumentName, iprm::INameParam, ExtractCurrentDocumentName);
		I_REGISTER_SUBELEMENT_INTERFACE(CurrentDocumentName, imod::IModel, ExtractCurrentDocumentName);
		I_ASSIGN(m_documentTemplateCompPtr, "DocumentTemplate", "Document template", true, "DocumentTemplate");
	I_END_COMPONENT;

	TDocumentWorkspaceGuiCompBase();

	// reimplemented (iqtgui::IGuiObject)
	virtual void OnTryClose(bool* ignoredPtr = nullptr) override;

protected:
	virtual void UpdateAllTitles() = 0;
	virtual int GetDocumentIndexFromWidget(const QWidget& widget) const;
	virtual void InitializeDocumentView(IDocumentViewDecorator* pageViewPtr, const SingleDocumentData& documentData) = 0;

protected:
	virtual void OnDragEnterEvent(QDragEnterEvent* dragEnterEventPtr);
	virtual void OnDropEvent(QDropEvent* dropEventPtr);

	// reimplemented (DocumentManagerBase)
	virtual istd::IChangeable* OpenSingleDocument(
				const QString& filePath,
				bool createView,
				const QByteArray& viewTypeId,
				QByteArray& documentTypeId,
				bool beQuiet,
				bool* ignoredPtr,
				ibase::IProgressManager* progressManagerPtr) override;

	// reimplemented (idoc::IDocumentManager)
	virtual bool InsertNewDocument(
				const QByteArray& documentTypeId,
				bool createView = true,
				const QByteArray& viewTypeId = "",
				istd::IChangeable** newDocumentPtr = nullptr,
				bool beQuiet = false,
				bool* ignoredPtr = nullptr) override;

	// reimplemented (DocumentManagerBase)
	virtual QStringList GetOpenFilePaths(const QByteArray* documentTypeIdPtr = nullptr) const override;
	virtual bool QueryDocumentSave(const SingleDocumentData& info, bool* ignoredPtr) override;

	// reimplemented (iqt:CGuiComponentBase)
	virtual void OnGuiCreated() override;
	virtual void OnGuiDestroyed() override;
	virtual void OnGuiRetranslate() override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

	// reimplemented (istd:IChangeable)
	virtual void OnEndChanges(const istd::IChangeable::ChangeSet& changeSet) override;

	// reimplemented (QObject)
	virtual bool eventFilter(QObject* sourcePtr, QEvent* eventPtr) override;

	// abstract methods
	virtual IDocumentViewDecorator* CreateDocumentViewDecorator(
				istd::IPolymorphic* viewPtr,
				QWidget* parentWidgetPtr,
				const ifile::IFilePersistence* persistencePtr) = 0;

protected:
	virtual void DoUndo();
	virtual void DoRedo();
	virtual void OpenDocumentForType(const QByteArray& documentTypeId);

private:
	// static template methods for subelement access
	template <class InterfaceType>
	static InterfaceType* ExtractCurrentDocumentName(TDocumentWorkspaceGuiCompBase& component)
	{
		return &component.m_currentDocumentName;
	}

protected:
	imod::TModelWrap<iprm::CNameParam> m_currentDocumentName;

	I_REF(idoc::IDocumentTemplate, m_documentTemplateCompPtr);
};


// public methods

template <class DocumentManagerBase, class UI>
TDocumentWorkspaceGuiCompBase<DocumentManagerBase, UI>::TDocumentWorkspaceGuiCompBase()
{
}


// reimplemented (iqtgui::IGuiObject)

template <class DocumentManagerBase, class UI>
void TDocumentWorkspaceGuiCompBase<DocumentManagerBase, UI>::OnTryClose(bool* ignoredPtr)
{
	if (BaseClass::SaveDirtyDocuments(false, ignoredPtr)){
		int documentInfosCount = BaseClass::GetDocumentsCount();
		for (int documentIndex = 0; documentIndex < documentInfosCount; ++documentIndex){
			SingleDocumentData& documentData = BaseClass::GetSingleDocumentData(documentIndex);

			documentData.isDirty = false;
		}

		CloseAllDocuments();
	}

	if (ignoredPtr != nullptr){
		*ignoredPtr = (BaseClass::GetDocumentsCount() > 0);
	}
}


// protected members

template <class DocumentManagerBase, class UI>
int TDocumentWorkspaceGuiCompBase<DocumentManagerBase, UI>::GetDocumentIndexFromWidget(const QWidget& widget) const
{
	int documentInfosCount = BaseClass::GetDocumentsCount();
	for (int documentIndex = 0; documentIndex < documentInfosCount; ++documentIndex){
		const SingleDocumentData& documentData = BaseClass::GetSingleDocumentData(documentIndex);

		for (		ConstIterator viewIter = documentData.views.begin();
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


// protected methods

template <class DocumentManagerBase, class UI>
void TDocumentWorkspaceGuiCompBase<DocumentManagerBase, UI>::OnDragEnterEvent(QDragEnterEvent* dragEnterEventPtr)
{
	if (dragEnterEventPtr->mimeData()->hasFormat("text/uri-list")){
		dragEnterEventPtr->acceptProposedAction();
	}
}


template <class DocumentManagerBase, class UI>
void TDocumentWorkspaceGuiCompBase<DocumentManagerBase, UI>::OnDropEvent(QDropEvent* dropEventPtr)
{
	const QMimeData* mimeData = dropEventPtr->mimeData();
	if (mimeData->hasUrls()){
		QList<QUrl> files = mimeData->urls();

		for (int fileIndex = 0; fileIndex < files.count(); fileIndex++){
			QString filePath = files.at(fileIndex).toLocalFile();

			if (BaseClass::OpenDocument(nullptr, &filePath)){
				dropEventPtr->setAccepted(true);

				return;
			}
		}
	}

	dropEventPtr->setAccepted(false);
}


// reimplemented (DocumentManagerBase)

template <class DocumentManagerBase, class UI>
istd::IChangeable* TDocumentWorkspaceGuiCompBase<DocumentManagerBase, UI>::OpenSingleDocument(
			const QString& filePath,
			bool createView,
			const QByteArray& viewTypeId,
			QByteArray& documentTypeId,
			bool beQuiet,
			bool* ignoredPtr,
			ibase::IProgressManager* progressManagerPtr)
{
	SingleDocumentData* documentInfoPtr = BaseClass::GetDocumentInfoFromPath(filePath);
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

template <class DocumentManagerBase, class UI>
bool TDocumentWorkspaceGuiCompBase<DocumentManagerBase, UI>::InsertNewDocument(
			const QByteArray& documentTypeId,
			bool createView,
			const QByteArray& viewTypeId,
			istd::IChangeable** newDocumentPtr,
			bool beQuiet,
			bool* ignoredPtr)
{
	bool retVal = BaseClass::InsertNewDocument(documentTypeId, createView, viewTypeId, newDocumentPtr, beQuiet, ignoredPtr);

	if (retVal && (newDocumentPtr != nullptr) && (*newDocumentPtr != nullptr) && createView){
		int documentsCount = BaseClass::GetDocumentsCount();
		for (int i = 0; i < documentsCount; ++i){
			if (&BaseClass::GetDocumentFromIndex(i) == *newDocumentPtr){
				istd::IPolymorphic* viewPtr = BaseClass::GetViewFromIndex(i, 0);
				if (viewPtr != nullptr){
					BaseClass::SetActiveView(viewPtr);
				}

				break;
			}
		}
	}

	return retVal;
}


// reimplemented (DocumentManagerBase)

template <class DocumentManagerBase, class UI>
QStringList TDocumentWorkspaceGuiCompBase<DocumentManagerBase, UI>::GetOpenFilePaths(const QByteArray* documentTypeIdPtr) const
{
	QStringList files = BaseClass::GetOpenFilePathesFromDialog(documentTypeIdPtr);

	if (!files.isEmpty()){
		BaseClass::UpdateLastDirectory(files.at(0));
	}

	return files;
}


template <class DocumentManagerBase, class UI>
bool TDocumentWorkspaceGuiCompBase<DocumentManagerBase, UI>::QueryDocumentSave(const SingleDocumentData& info, bool* ignoredPtr)
{
	QMessageBox::StandardButtons buttons = QMessageBox::Yes | QMessageBox::No;

	if (ignoredPtr != nullptr){
		*ignoredPtr = false;

		buttons |= QMessageBox::Cancel;
	}

	QFileInfo fileInfo(info.filePath);

	int dialogResult = QMessageBox::information(
				BaseClass::GetQtWidget(),
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

template <class DocumentManagerBase, class UI>
void TDocumentWorkspaceGuiCompBase<DocumentManagerBase, UI>::OnGuiCreated()
{
	BaseClass::OnGuiCreated();

	QWidget* mainWindowPtr = BaseClass::GetQtWidget();
	if (mainWindowPtr != nullptr){
		mainWindowPtr->setAcceptDrops(true);
	}
}


template <class DocumentManagerBase, class UI>
void TDocumentWorkspaceGuiCompBase<DocumentManagerBase, UI>::OnGuiDestroyed()
{
	CloseAllDocuments();

	BaseClass::OnGuiDestroyed();
}


template <class DocumentManagerBase, class UI>
void TDocumentWorkspaceGuiCompBase<DocumentManagerBase, UI>::OnGuiRetranslate()
{
	BaseClass::OnGuiRetranslate();

	UpdateAllTitles();
}


// reimplemented (icomp::CComponentBase)

template <class DocumentManagerBase, class UI>
void TDocumentWorkspaceGuiCompBase<DocumentManagerBase, UI>::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	BaseClass::SetDocumentTemplate(m_documentTemplateCompPtr.GetPtr());
}


// reimplemented (istd:IChangeable)

template <class DocumentManagerBase, class UI>
void TDocumentWorkspaceGuiCompBase<DocumentManagerBase, UI>::OnEndChanges(const istd::IChangeable::ChangeSet& changeSet)
{
	BaseClass::OnEndChanges(changeSet);

	if (BaseClass::IsGuiCreated()){
		UpdateAllTitles();
	}
}


// reimplemented (QObject)

template <class DocumentManagerBase, class UI>
bool TDocumentWorkspaceGuiCompBase<DocumentManagerBase, UI>::eventFilter(QObject* sourcePtr, QEvent* eventPtr)
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


// protected methods

template <class DocumentManagerBase, class UI>
void TDocumentWorkspaceGuiCompBase<DocumentManagerBase, UI>::DoUndo()
{
	SingleDocumentData* documentDataPtr = BaseClass::GetActiveDocumentInfo();
	if ((documentDataPtr != nullptr) && documentDataPtr->undoManagerPtr.IsValid() && documentDataPtr->undoManagerPtr->GetAvailableUndoSteps() > 0){
		documentDataPtr->undoManagerPtr->DoUndo();
	}
}


template <class DocumentManagerBase, class UI>
void TDocumentWorkspaceGuiCompBase<DocumentManagerBase, UI>::DoRedo()
{
	SingleDocumentData* documentDataPtr = BaseClass::GetActiveDocumentInfo();
	if ((documentDataPtr != nullptr) && documentDataPtr->undoManagerPtr.IsValid() && documentDataPtr->undoManagerPtr->GetAvailableRedoSteps() > 0){
		documentDataPtr->undoManagerPtr->DoRedo();
	}
}


template <class DocumentManagerBase, class UI>
void TDocumentWorkspaceGuiCompBase<DocumentManagerBase, UI>::OpenDocumentForType(const QByteArray& documentTypeId)
{
	bool ignoredFlag = false;
	if (!BaseClass::OpenDocument(&documentTypeId, nullptr, true, "", nullptr, nullptr, false, &ignoredFlag)){
		if (!ignoredFlag){
			QMessageBox::warning(nullptr, "", tr("Document could not be opened"));
		}
	}
}


} // namespace imtgui


