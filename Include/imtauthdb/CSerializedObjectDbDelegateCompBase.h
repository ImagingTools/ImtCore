// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QDateTime>
#include <QtCore/QList>
#include <QtCore/QPair>

// ImtCore includes
#include <imtdb/CSqlDatabaseObjectDelegateCompBase.h>


namespace imtauthdb
{


/**
	SQL delegate of a table holding a serialized object in the column "Data", beside the columns it is selected by.
	A subclass gives the object, its columns and the statements of the tables that depend on it, written in the same query.
	The texts are encoded for the split of the query at ';'.
*/
class CSerializedObjectDbDelegateCompBase: public imtdb::CSqlDatabaseObjectDelegateCompBase
{
public:
	typedef imtdb::CSqlDatabaseObjectDelegateCompBase BaseClass;

	I_BEGIN_BASE_COMPONENT(CSerializedObjectDbDelegateCompBase)
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

protected:
	/// A column and its SQL literal
	typedef QList<QPair<QString, QString>> Columns;

	virtual istd::IChangeableUniquePtr CreateDataObject() const = 0;

	/**
		The columns of the object besides "Id" and "Data".
		\return False for an object of another kind.
	*/
	virtual bool GetColumns(const istd::IChangeable& object, Columns& columns) const = 0;

	/**
		Statements of the dependent tables written after the object; \c objectPtr is null when the object is removed.
	*/
	virtual QString CreateDependentStatements(const QByteArray& objectId, const istd::IChangeable* objectPtr) const;

	QString GetQuotedTableName() const;

	QString TextLiteral(const QString& text) const;
	QString IdLiteral(const QByteArray& id) const;
	QString TimeLiteral(const QDateTime& time) const;
	static QString NumberLiteral(int value);

	/**
		A condition matching the column holding \c text anywhere, with the wildcards of the text escaped.
	*/
	QString ContainsCondition(const QString& columnName, const QString& text) const;

	// reimplemented (imtdb::CSqlDatabaseObjectDelegateCompBase)
	virtual bool SetCollectionItemMetaInfoFromRecord(const QSqlRecord& record, idoc::IDocumentMetaInfo& metaInfo) const override;
	virtual idoc::MetaInfoPtr CreateObjectMetaInfo(const QByteArray& typeId) const override;
	virtual bool SetObjectMetaInfoFromRecord(const QSqlRecord& record, idoc::IDocumentMetaInfo& metaInfo) const override;

private:
	QString CreateDataLiteral(const istd::IChangeable& object) const;
};


} // namespace imtauthdb


