// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtqml/ICollectionDataBridge.h>


namespace imtqml
{


/**
	\brief In-process implementation of \c ICollectionDataBridge.

	\details
	An icomp component that owns an \c I_REF to an
	\c imtbase::IObjectCollection and forwards every
	\c ICollectionDataBridge call directly to it. Intended for the
	server-side / in-process scenario where a GraphQL client is not
	used.

	\note Bridge is intentionally NOT exposed to QML — clients
	(\c CCollectionDataController) resolve it through icomp.
*/
class CCollectionManagerBridge:
			public icomp::CComponentBase,
			virtual public ICollectionDataBridge
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CCollectionManagerBridge);
		I_REGISTER_INTERFACE(ICollectionDataBridge);
		I_ASSIGN(m_objectCollectionCompPtr, "ObjectCollection", "In-process object collection", true, "ObjectCollection");
	I_END_COMPONENT;

	CCollectionManagerBridge();
	~CCollectionManagerBridge() override;

	// reimplemented (ICollectionDataBridge)
	virtual void GetCollectionHeaders(
			const QString& collectionId,
			VariantCallback callback) override;

	virtual void GetObjectData(
			const QString& collectionId,
			const QString& objectId,
			VariantCallback callback) override;

	virtual void InsertNewObject(
			const QString& collectionId,
			const QString& typeId,
			const QString& name,
			const QString& description,
			const QVariant& objectData,
			const QString& objectId,
			IdCallback callback) override;

	virtual void SetObjectData(
			const QString& collectionId,
			const QString& objectId,
			const QVariant& newObjectData,
			VoidCallback callback) override;

	virtual void RemoveObjects(
			const QString& collectionId,
			const QStringList& objectIds,
			IdListCallback callback) override;

	virtual void RestoreObjects(
			const QString& collectionId,
			const QStringList& objectIds,
			IdListCallback callback) override;

	virtual void SetObjectName(
			const QString& collectionId,
			const QString& objectId,
			const QString& name,
			VoidCallback callback) override;

	virtual void SetObjectDescription(
			const QString& collectionId,
			const QString& objectId,
			const QString& description,
			VoidCallback callback) override;

private:
	I_REF(imtbase::IObjectCollection, m_objectCollectionCompPtr);
};


} // namespace imtqml
