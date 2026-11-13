// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtauth/IPersonalAccessToken.h>
#include <imtauth/IPersonalAccessTokenPersistence.h>
#include <imtdb/CSqlDatabaseObjectDelegateCompBase.h>


namespace imtauthdb
{


class CPersonalAccessTokenDatabaseDelegateComp:
			public imtdb::CSqlDatabaseObjectDelegateCompBase,
			virtual public imtauth::IPersonalAccessTokenPersistence
{
public:
	typedef imtdb::CSqlDatabaseObjectDelegateCompBase BaseClass;

	I_BEGIN_COMPONENT(CPersonalAccessTokenDatabaseDelegateComp)
		I_REGISTER_INTERFACE(imtauth::IPersonalAccessTokenPersistence);
		I_ASSIGN(m_tokenFactCompPtr, "PersonalAccessToken", "Factory used for creation of the new token", true, "PersonalAccessToken");
	I_END_COMPONENT

	virtual bool RevokeToken(const QByteArray& tokenId) override;
	virtual bool SetTokenScopes(const QByteArray& tokenId, const QByteArrayList& scopes) override;
	virtual bool UpdateLastUsedAt(const QByteArray& tokenId, const QDateTime& usedAt) override;
	virtual QByteArray GetSelectionQuery(const QByteArray& objectId = QByteArray(), int offset = 0, int count = -1,
				const iprm::IParamsSet* paramsPtr = nullptr) const override;

	// reimplemented (imtdb::ISqlDatabaseObjectDelegate)
	virtual istd::IChangeableUniquePtr CreateObjectFromRecord(const QSqlRecord& record, const iprm::IParamsSet* dataConfigurationPtr = nullptr) const override;
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

protected:
	virtual bool CreateTableIfNeeded() override;
	// reimplemented (imtdb::CSqlDatabaseObjectDelegateCompBase)
	virtual idoc::MetaInfoPtr CreateObjectMetaInfo(const QByteArray& typeId) const override;
	virtual bool SetObjectMetaInfoFromRecord(const QSqlRecord& record, idoc::IDocumentMetaInfo& metaInfo) const override;

private:
	bool CheckSchema() const;
	I_FACT(imtauth::IPersonalAccessToken, m_tokenFactCompPtr);
	bool m_isSchemaReady = false;
};


} // namespace imtauthdb
