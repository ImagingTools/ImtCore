#include <imtbase/CMultiDocumentManager.h>


// Qt includes
#include <QtCore/QFileInfo>

// ACF includes
#include <istd/CChangeNotifier.h>


namespace imtbase
{


// public methods

bool CMultiDocumentManager::SetDocumentName(int documentIndex, const QString& documentName)
{
	SingleDocumentData* infoPtr = m_documentInfos.GetAt(documentIndex);
	if (infoPtr != nullptr){
		istd::IChangeable::ChangeSet changeSet(idoc::IDocumentManager::CF_DOCUMENT_RENAMED);
		istd::CChangeNotifier notifier(this, &changeSet);

		QFileInfo fileInfo(infoPtr->filePath);
		infoPtr->filePath = fileInfo.path() + "/" + documentName + "." + fileInfo.suffix();

		return true;
	}

	return false;
}


} // namespace imtbase


