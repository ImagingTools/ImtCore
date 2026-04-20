// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtclientgql/IGqlClient.h>
#include <imtqml/ICollectionDataBridge.h>


namespace imtqml
{


/**
	\brief GraphQL/SDL implementation of \c ICollectionDataBridge.

	\details
	An icomp component that owns an \c I_REF to an
	\c imtclientgql::IGqlClient and translates every
	\c ICollectionDataBridge call into the corresponding SDL-generated
	GraphQL request from
	\c Sdl/imtbase/1.0/CollectionController.sdl. Operations are
	dispatched on a worker thread; results are marshalled to the GUI
	thread before invoking the supplied callback.

	\note Bridge is intentionally NOT exposed to QML — clients
	(\c CCollectionDataController) resolve it through icomp.
*/
class CGqlCollectionManagerBridge:
			public icomp::CComponentBase,
			virtual public ICollectionDataBridge
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CGqlCollectionManagerBridge);
		I_REGISTER_INTERFACE(ICollectionDataBridge);
		I_ASSIGN(m_apiClientCompPtr, "ApiClient", "GraphQL API client", true, "ApiClient");
	I_END_COMPONENT;

	CGqlCollectionManagerBridge();
	~CGqlCollectionManagerBridge() override;

	// reimplemented (ICollectionDataBridge)
	virtual void GetCollectionHeaders(
			const QString& collectionId,
			TVariantCallback callback) override;

	virtual void GetObjectData(
			const QString& collectionId,
			const QString& objectId,
			TVariantCallback callback) override;

	virtual void InsertNewObject(
			const QString& collectionId,
			const QString& typeId,
			const QString& name,
			const QString& description,
			const QVariant& objectData,
			const QString& objectId,
			TIdCallback callback) override;

	virtual void SetObjectData(
			const QString& collectionId,
			const QString& objectId,
			const QVariant& newObjectData,
			TVoidCallback callback) override;

	virtual void RemoveObjects(
			const QString& collectionId,
			const QStringList& objectIds,
			TIdListCallback callback) override;

	virtual void RestoreObjects(
			const QString& collectionId,
			const QStringList& objectIds,
			TIdListCallback callback) override;

	virtual void SetObjectName(
			const QString& collectionId,
			const QString& objectId,
			const QString& name,
			TVoidCallback callback) override;

	virtual void SetObjectDescription(
			const QString& collectionId,
			const QString& objectId,
			const QString& description,
			TVoidCallback callback) override;

private:
	I_REF(imtclientgql::IGqlClient, m_apiClientCompPtr);
};


} // namespace imtqml
