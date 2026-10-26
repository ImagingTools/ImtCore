// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// STL includes
#include <atomic>

// Qt includes
#include <QtCore/QMutex>
#include <QtCore/QSet>

// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtbase/CObjectCollectionBase.h>
#include <imtbase/IComplexCollectionFilter.h>
#include <imtbase/TFilterableCollectionWrap.h>
#include <imtauth/ITenantRelationshipProvider.h>


/**
	In-memory collection of the organization data that selects like the SQL collections do:
	a field filter of the "ComplexFilter" parameter (and the "ParentTenantId" parameter) is applied to the column of the same name,
	FO_CONTAINS ignores the case like ILIKE.
	It counts the loaded documents, so a test sees whether a lookup reads only its candidates.
*/
class CTestCollectionComp:
			public icomp::CComponentBase,
			public imtbase::TFilterableCollectionWrap<imtbase::CObjectCollectionBase>
{
public:
	typedef icomp::CComponentBase BaseClass;
	typedef imtbase::TFilterableCollectionWrap<imtbase::CObjectCollectionBase> BaseClass2;

	I_BEGIN_COMPONENT(CTestCollectionComp);
		I_REGISTER_INTERFACE(imtbase::IObjectCollection);
		I_REGISTER_INTERFACE(imtbase::IObjectCollectionInfo);
		I_REGISTER_INTERFACE(imtbase::ICollectionInfo);
	I_END_COMPONENT;

	/**
		Insert a copy of the object under the given id.
	*/
	QByteArray AddObject(const QByteArray& typeId, const istd::IChangeable& object, const QByteArray& objectId);

	/**
		A collection of another kind ignores the field filters and returns every id.
	*/
	void SetFilterSupported(bool isSupported);

	/**
		Columns of the UUID type: like PostgreSQL, a selection comparing one with another value fails and returns nothing.
	*/
	void SetUuidFieldIds(const QSet<QByteArray>& fieldIds);

	int GetLoadCount() const;
	void ResetLoadCount();

	/**
		Number of the selections made without any field filter, i.e. reading every id.
	*/
	int GetUnfilteredSelectionCount() const;

	/**
		Field ids the filters asked for that the collection does not know: an SQL table would refuse them.
	*/
	QSet<QByteArray> GetUnknownFieldIds() const;

	/**
		Text values the field filters asked for, in the order of the selections.
	*/
	QStringList GetRequestedFilterValues() const;

	/**
		Failures of the storage: a failing selection reports an error to the log of the caller and returns nothing.
	*/
	void SetSelectionFailing(bool isFailing);
	void SetLoadFailingIds(const QSet<QByteArray>& objectIds);
	void SetRemovalFailing(bool isFailing);
	void SetStoreFailing(bool isFailing);

	// reimplemented (imtbase::IObjectCollection)
	virtual bool GetObjectData(const Id& objectId, DataPtr& dataPtr, const iprm::IParamsSet* dataConfigurationPtr = nullptr) const override;
	virtual bool RemoveElements(const Ids& elementIds, const imtbase::IOperationContext* operationContextPtr = nullptr) override;
	virtual bool SetObjectData(
				const Id& objectId,
				const istd::IChangeable& object,
				CompatibilityMode mode = CM_WITHOUT_REFS,
				const imtbase::IOperationContext* operationContextPtr = nullptr) override;

	// reimplemented (imtbase::ICollectionInfo)
	virtual int GetElementsCount(const iprm::IParamsSet* selectionParamPtr = nullptr, ilog::IMessageConsumer* logPtr = nullptr) const override;
	virtual Ids GetElementIds(
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* selectionParamsPtr = nullptr,
				ilog::IMessageConsumer* logPtr = nullptr) const override;

protected:
	// reimplemented (imtbase::CObjectCollectionBase)
	virtual istd::IChangeableUniquePtr CreateObjectInstance(const QByteArray& typeId) const override;

private:
	bool IsSelected(const QByteArray& objectId, const iprm::IParamsSet& selectionParams) const;
	bool Matches(const istd::IChangeable& object, const imtbase::IComplexCollectionFilter::FilterExpression& expression) const;
	bool HasInvalidUuid(const imtbase::IComplexCollectionFilter::FilterExpression& expression) const;
	bool GetFieldValue(const istd::IChangeable& object, const QByteArray& fieldId, QString& value) const;

	bool m_isFilterSupported = true;
	QSet<QByteArray> m_uuidFieldIds;
	// The verification controller reads from its own thread
	mutable std::atomic<int> m_loadCount{0};
	mutable std::atomic<int> m_unfilteredSelectionCount{0};
	// The concurrency tests select from several threads
	mutable QMutex m_recordMutex;
	mutable QSet<QByteArray> m_unknownFieldIds;
	mutable QStringList m_requestedFilterValues;
	bool m_isSelectionFailing = false;
	QSet<QByteArray> m_loadFailingIds;
	bool m_isRemovalFailing = false;
	bool m_isStoreFailing = false;
};


/**
	Relationship provider answering from a fixed list; an unreadable answer simulates a failed request to the authorization server.
*/
class CTestRelationshipProviderComp:
			public icomp::CComponentBase,
			virtual public imtauth::ITenantRelationshipProvider
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CTestRelationshipProviderComp);
		I_REGISTER_INTERFACE(imtauth::ITenantRelationshipProvider);
	I_END_COMPONENT;

	void AddRelationship(const QByteArray& tenantId, const QByteArray& otherTenantId, const QDateTime& relatedSince = QDateTime(), bool isEffective = true);
	void SetReadable(bool isReadable);
	/// The relationships are answered, but the read is reported incomplete, e.g. part of their history is missing.
	void SetPartialRead(bool isPartial);
	int GetReadCount() const;

	// reimplemented (imtauth::ITenantRelationshipProvider)
	virtual imtauth::RelatedTenantInfos GetRelatedTenants(const QByteArray& tenantId, QString* tenantNamePtr = nullptr, bool* isReadPtr = nullptr) const override;

private:
	struct Relationship
	{
		QByteArray tenantId;
		QByteArray otherTenantId;
		QDateTime relatedSince;
		bool isEffective = true;
	};

	QList<Relationship> m_relationships;
	bool m_isReadable = true;
	bool m_isPartialRead = false;
	mutable int m_readCount = 0;
};


