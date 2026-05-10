// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// ImtCore includes
#include <imtdb/CSqlDatabaseObjectDelegateCompBase.h>


namespace imtchatdb
{


/**
	Database delegate for the ConversationParticipants table.

	Manages the many-to-many relationship between conversations and users,
	tracking when each participant joined a conversation.
*/
class CParticipantDbDelegateComp: public imtdb::CSqlDatabaseObjectDelegateCompBase
{
public:
	typedef imtdb::CSqlDatabaseObjectDelegateCompBase BaseClass;

	I_BEGIN_COMPONENT(CParticipantDbDelegateComp)
	I_END_COMPONENT

	// reimplemented (imtdb::ISqlDatabaseObjectDelegate)
	virtual istd::IChangeableUniquePtr CreateObjectFromRecord(
				const QSqlRecord& record,
				const iprm::IParamsSet* dataConfigurationPtr = nullptr) const override;
	virtual NewObjectQuery CreateNewObjectQuery(
				const QByteArray& typeId,
				const QByteArray& proposedObjectId,
				const QString& objectName,
				const QString& objectDescription,
				const istd::IChangeable* valuePtr,
				const imtbase::IOperationContext* operationContextPtr) const override;
	virtual QByteArray CreateUpdateObjectQuery(
				const imtbase::IObjectCollection& collection,
				const QByteArray& objectId,
				const istd::IChangeable& object,
				const imtbase::IOperationContext* operationContextPtr,
				bool useExternDelegate = true) const override;
	virtual QByteArray CreateDeleteObjectsQuery(
				const imtbase::IObjectCollection& collection,
				const QByteArrayList& objectIds,
				const imtbase::IOperationContext* operationContextPtr) const override;
	virtual QByteArray CreateDeleteObjectSetQuery(
				const imtbase::IObjectCollection& collection,
				const iprm::IParamsSet* paramsPtr = nullptr,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const override;
	virtual QByteArray CreateRenameObjectQuery(
				const imtbase::IObjectCollection& collection,
				const QByteArray& objectId,
				const QString& newObjectName,
				const imtbase::IOperationContext* operationContextPtr) const override;
	virtual QByteArray CreateDescriptionObjectQuery(
				const imtbase::IObjectCollection& collection,
				const QByteArray& objectId,
				const QString& description,
				const imtbase::IOperationContext* operationContextPtr) const override;

	/**
		Build a query to add a user to a conversation.
		\param conversationId  Target conversation.
		\param userId          User to add.
		\return SQL insert query bytes.
	*/
	QByteArray CreateAddParticipantQuery(
				const QByteArray& conversationId,
				const QByteArray& userId) const;

	/**
		Build a query to remove a user from a conversation.
		\param conversationId  Target conversation.
		\param userId          User to remove.
		\return SQL delete query bytes.
	*/
	QByteArray CreateRemoveParticipantQuery(
				const QByteArray& conversationId,
				const QByteArray& userId) const;

protected:
	// reimplemented (imtdb::CSqlDatabaseObjectDelegateCompBase)
	virtual idoc::MetaInfoPtr CreateObjectMetaInfo(const QByteArray& typeId) const override;
	virtual bool SetObjectMetaInfoFromRecord(const QSqlRecord& record, idoc::IDocumentMetaInfo& metaInfo) const override;
};


} // namespace imtchatdb
