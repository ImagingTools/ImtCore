// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtdb/CSqlDatabaseDocumentDelegateComp.h>


namespace imtauthdb
{


/**
	JSON product documents with one active revision per logical product: the product ID is a column with a unique index
	over the active revisions. Without that schema the delegate refuses to read or write.
*/
class CProductCatalogDatabaseDelegateComp: public imtdb::CSqlDatabaseDocumentDelegateComp
{
public:
	typedef imtdb::CSqlDatabaseDocumentDelegateComp BaseClass;

	I_BEGIN_COMPONENT(CProductCatalogDatabaseDelegateComp)
	I_END_COMPONENT

	virtual QByteArray GetCountQuery(const iprm::IParamsSet* paramsPtr = nullptr) const override;
	virtual QByteArray GetSelectionQuery(
				const QByteArray& objectId = QByteArray(),
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* paramsPtr = nullptr) const override;

protected:
	virtual bool CreateTableIfNeeded() override;
	virtual QVariantMap GetAdditionalInsertValues(const istd::IChangeable& object) const override;
	virtual QByteArray PrepareInsertNewObjectQuery(
				const QByteArray& typeId,
				const QByteArray& objectId,
				const QString& objectName,
				const QString& objectDescription,
				const istd::IChangeable& object,
				const imtbase::IOperationContext* operationContextPtr,
				const QVariant& revisionArgument) const override;
	virtual QString CreateJsonExtractSql(
				const QString& jsonName,
				const QString& key,
				QMetaType::Type metaType = QMetaType::QString,
				const QString& tableAlias = QString()) const override;

private:
	QString QualifiedTable() const;
	bool ExecuteSchemaQuery(const QString& query) const;
	QByteArray GetProductId(const istd::IChangeable& object) const;

	bool m_isSchemaReady = false;
};


} // namespace imtauthdb
