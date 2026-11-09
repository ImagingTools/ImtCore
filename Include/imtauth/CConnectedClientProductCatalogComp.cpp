// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CConnectedClientProductCatalogComp.h>


// Qt includes
#include <QtCore/QMutexLocker>
#include <QtCore/QUuid>

// ImtCore includes
#include <imtauth/CConnectedProduct.h>
#include <imtauth/CConnectedProductDocumentValidatorComp.h>


namespace imtauth
{


// public methods

// reimplemented (IClientProductCatalog)

bool CConnectedClientProductCatalogComp::IsClientProduct(const QByteArray& productId, bool* isReadPtr) const
{
	if (!m_productCollectionCompPtr.IsValid()){
		if (isReadPtr != nullptr){
			*isReadPtr = false;
		}

		return false;
	}

	return CConnectedProductDocumentValidatorComp::HasProduct(*m_productCollectionCompPtr, productId, isReadPtr);
}


bool CConnectedClientProductCatalogComp::AddClientProduct(const QByteArray& productId, const QString& name, QString& errorMessage, bool* isAddedPtr)
{
	if (isAddedPtr != nullptr){
		*isAddedPtr = false;
	}

	if (productId.isEmpty()){
		errorMessage = QStringLiteral("A product needs an ID");

		return false;
	}

	QMutexLocker locker(&m_addMutex);

	bool isRead = true;
	if (IsClientProduct(productId, &isRead)){
		return true;
	}

	if (!isRead){
		errorMessage = QStringLiteral("The client products of this server could not be read");

		return false;
	}

	const QByteArray objectId = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();
	const QString productName = name.isEmpty() ? QString::fromUtf8(productId) : name;

	IConnectedProductUniquePtr productPtr = m_productFactCompPtr.CreateInstance();
	CIdentifiableConnectedProduct* entryPtr = dynamic_cast<CIdentifiableConnectedProduct*>(productPtr.GetPtr());
	if (entryPtr == nullptr){
		errorMessage = QStringLiteral("The catalog cannot create a product");

		return false;
	}

	entryPtr->SetObjectUuid(objectId);
	entryPtr->SetProductId(productId);
	entryPtr->SetProductName(productName);

	if (m_productCollectionCompPtr->InsertNewObject("ConnectedProduct", productName, QString(), entryPtr, objectId).isEmpty()){
		errorMessage = QStringLiteral("The product '%1' could not be added").arg(QString::fromUtf8(productId));

		return false;
	}

	if (isAddedPtr != nullptr){
		*isAddedPtr = true;
	}

	return true;
}


} // namespace imtauth


