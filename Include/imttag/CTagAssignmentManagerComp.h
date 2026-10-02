// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtbase/IEntityTypeProvider.h>
#include <imtbase/IObjectCollection.h>
#include <imtbase/IObjectCollectionProvider.h>
#include <imtbase/ITransactionManager.h>
#include <imttag/ITag.h>
#include <imttag/ITagAssignment.h>
#include <imttag/ITagAssignmentManager.h>


namespace iprm
{
	class CParamsSet;
}


namespace imttag
{


class CTagAssignmentManagerComp:
			public ilog::CLoggerComponentBase,
			virtual public ITagAssignmentManager
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CTagAssignmentManagerComp);
		I_REGISTER_INTERFACE(ITagAssignmentManager);
		I_ASSIGN(m_tagCollectionCompPtr, "TagCollection", "Tag catalog", true, "TagCollection");
		I_ASSIGN(m_assignmentCollectionCompPtr, "AssignmentCollection", "SQL-backed TagAssignments collection", true, "TagAssignmentCollection");
		I_ASSIGN(m_assignmentFactoryCompPtr, "AssignmentFactory", "Factory of tag assignments", true, "TagAssignment");
		I_ASSIGN(m_transactionManagerCompPtr, "TransactionManager", "Transaction manager spanning the assignment changes of one call", false, "TransactionManager");
		I_ASSIGN_MULTI_0(m_entityTypesCompPtr, "EntityTypes", "Taggable entity types", false);
		I_ASSIGN_TO(m_entityCollectionsCompPtr, m_entityTypesCompPtr, false);
	I_END_COMPONENT;

	// reimplemented (imttag::ITagAssignmentManager)
	virtual bool AddTags(
				const QByteArray& entityType,
				const QByteArrayList& entityIds,
				const QByteArrayList& tagIds,
				const imtbase::IOperationContext* operationContextPtr,
				EntityTagChanges* changesPtr = nullptr,
				QString* errorMessagePtr = nullptr) override;
	virtual bool RemoveTags(
				const QByteArray& entityType,
				const QByteArrayList& entityIds,
				const QByteArrayList& tagIds,
				const imtbase::IOperationContext* operationContextPtr,
				EntityTagChanges* changesPtr = nullptr,
				QString* errorMessagePtr = nullptr) override;
	virtual bool SetTags(
				const QByteArray& entityType,
				const QByteArray& entityId,
				const QByteArrayList& tagIds,
				const imtbase::IOperationContext* operationContextPtr,
				EntityTagChanges* changesPtr = nullptr,
				QString* errorMessagePtr = nullptr) override;
	virtual bool ClearTags(
				const QByteArray& entityType,
				const QByteArrayList& entityIds,
				const imtbase::IOperationContext* operationContextPtr,
				EntityTagChanges* changesPtr = nullptr,
				QString* errorMessagePtr = nullptr) override;
	virtual EntityTagIds GetTagIds(
				const QByteArray& entityType,
				const QByteArrayList& entityIds,
				const imtbase::IOperationContext* operationContextPtr) const override;
	virtual QMap<QByteArray, int> GetUsageCounts(const QByteArrayList& tagIds) const override;
	virtual bool IsTaggableEntityType(const QByteArray& entityType) const override;
	virtual const imtbase::IObjectCollection* GetEntityCollection(const QByteArray& entityType) const override;

private:
	enum ChangeMode
	{
		CM_ADD,
		CM_REMOVE,
		CM_REPLACE
	};

	struct TagSnapshot
	{
		QString name;
		QString color;
	};

	typedef QMap<QByteArray, TagSnapshot> TagSnapshots;

	bool ChangeTags(
				ChangeMode mode,
				const QByteArray& entityType,
				const QByteArrayList& entityIds,
				const QByteArrayList& tagIds,
				const imtbase::IOperationContext* operationContextPtr,
				EntityTagChanges* changesPtr,
				QString* errorMessagePtr);
	bool ApplyChanges(
				const QByteArray& entityType,
				const EntityTagChanges& changes,
				const QMap<QByteArray, QMap<QByteArray, QByteArray>>& assignmentIds,
				const imtbase::IOperationContext* operationContextPtr);

	/**
		Load assignments of the entities as entity ID -> (tag ID -> assignment ID).
	*/
	QMap<QByteArray, QMap<QByteArray, QByteArray>> LoadAssignments(const QByteArray& entityType, const QByteArrayList& entityIds) const;

	/**
		Load active tags visible in the tenant of the operation context.
	*/
	TagSnapshots LoadVisibleTags(const QByteArrayList& tagIds, const imtbase::IOperationContext* operationContextPtr) const;
	QByteArrayList GetVisibleIds(
				const imtbase::IObjectCollection& collection,
				const QByteArrayList& ids,
				const imtbase::IOperationContext* operationContextPtr) const;
	void InitScopeParams(
				iprm::CParamsSet& paramsSet,
				const QByteArrayList& ids,
				const imtbase::IOperationContext* operationContextPtr) const;
	bool SetError(QString* errorMessagePtr, const QString& message) const;

	I_REF(imtbase::IObjectCollection, m_tagCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_assignmentCollectionCompPtr);
	I_FACT(ITagAssignment, m_assignmentFactoryCompPtr);
	I_REF(imtbase::ITransactionManager, m_transactionManagerCompPtr);
	I_MULTIREF(imtbase::IEntityTypeProvider, m_entityTypesCompPtr);
	I_MULTIREF(imtbase::IObjectCollectionProvider, m_entityCollectionsCompPtr);
};


} // namespace imttag


