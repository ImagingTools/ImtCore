// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CUserDocumentSaveDelegateComp.h>


// Qt includes
#include <QtCore/QMutexLocker>

// ImtCore includes
#include <imtauth/CUserBaseInfo.h>
#include <imtbase/IObjectCollection.h>


namespace imtauth
{


// reimplemented (imtdoc::IDocumentSaveDelegate)

bool CUserDocumentSaveDelegateComp::UpdateCollectionObject(
			imtbase::IObjectCollection& collection,
			const QByteArray& objectId,
			istd::IChangeable& document,
			const imtbase::IOperationContext* operationContextPtr) const
{
	CUserBaseInfo* userInfoPtr = dynamic_cast<CUserBaseInfo*>(&document);
	if (userInfoPtr == nullptr){
		return collection.SetObjectData(objectId, document, istd::IChangeable::CM_WITHOUT_REFS, operationContextPtr);
	}

	QMutexLocker locker(&CUserBaseInfo::GetUserUpdateMutex(objectId));

	imtbase::IObjectCollection::DataPtr storedDataPtr;
	const CUserBaseInfo* storedUserPtr = nullptr;
	if (collection.GetObjectData(objectId, storedDataPtr)){
		storedUserPtr = dynamic_cast<const CUserBaseInfo*>(storedDataPtr.GetPtr());
	}

	if (storedUserPtr == nullptr){
		return false;
	}

	userInfoPtr->CopyDirectoryRolesFrom(*storedUserPtr);

	return collection.SetObjectData(objectId, document, istd::IChangeable::CM_WITHOUT_REFS, operationContextPtr);
}


} // namespace imtauth


