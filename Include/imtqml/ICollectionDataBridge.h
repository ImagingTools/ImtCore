// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// std includes
#include <functional>

// Qt includes
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>


namespace imtqml
{


/**
	\brief Transport-agnostic abstract bridge for the collection-management
	operations exposed by \c CCollectionDataController.

	\details
	Two implementations are provided out of the box:

	- \c CGqlCollectionManagerBridge: dispatches every call as an
	  SDL-generated GraphQL request through \c imtclientgql::IGqlClient.
	- \c CCollectionManagerBridge: forwards every call directly to an
	  \c imtbase::IObjectCollection (the in-process / server scenario).

	All operations are asynchronous; implementations MUST invoke the
	callback exactly once on the GUI thread. An empty \c errorMessage
	signals success.
*/
class ICollectionDataBridge
{
public:
	typedef std::function<void(QVariant /*payload*/, QString /*errorMessage*/)> TVariantCallback;
	typedef std::function<void(QString /*objectId*/, QString /*errorMessage*/)> TIdCallback;
	typedef std::function<void(QStringList /*objectIds*/, QString /*errorMessage*/)> TIdListCallback;
	typedef std::function<void(QString /*errorMessage*/)> TVoidCallback;

	virtual ~ICollectionDataBridge() = default;

	virtual void GetCollectionHeaders(
			const QString& collectionId,
			TVariantCallback callback) = 0;

	virtual void GetObjectData(
			const QString& collectionId,
			const QString& objectId,
			TVariantCallback callback) = 0;

	virtual void InsertNewObject(
			const QString& collectionId,
			const QString& typeId,
			const QString& name,
			const QString& description,
			const QVariant& objectData,
			const QString& objectId,
			TIdCallback callback) = 0;

	virtual void SetObjectData(
			const QString& collectionId,
			const QString& objectId,
			const QVariant& newObjectData,
			TVoidCallback callback) = 0;

	virtual void RemoveObjects(
			const QString& collectionId,
			const QStringList& objectIds,
			TIdListCallback callback) = 0;

	virtual void RestoreObjects(
			const QString& collectionId,
			const QStringList& objectIds,
			TIdListCallback callback) = 0;

	virtual void SetObjectName(
			const QString& collectionId,
			const QString& objectId,
			const QString& name,
			TVoidCallback callback) = 0;

	virtual void SetObjectDescription(
			const QString& collectionId,
			const QString& objectId,
			const QString& description,
			TVoidCallback callback) = 0;
};


} // namespace imtqml
