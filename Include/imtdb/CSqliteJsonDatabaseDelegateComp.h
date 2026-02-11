// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtdb/CSqlJsonDatabaseDelegateComp.h>


namespace imtdb
{


class CSqliteJsonDatabaseDelegateComp: public imtdb::CSqlJsonDatabaseDelegateComp
{
public:
	typedef imtdb::CSqlJsonDatabaseDelegateComp BaseClass;

	I_BEGIN_COMPONENT(CSqliteJsonDatabaseDelegateComp)
	I_END_COMPONENT

	// reimplemented (imtdb::CSqlJsonDatabaseDelegateComp)
	virtual QByteArray GetSelectionQuery(
				const QByteArray& objectId = QByteArray(),
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* paramsPtr = nullptr) const override;
	virtual bool CreateSortQuery(const imtbase::ICollectionFilter& collectionFilter, QString& sortQuery) const override;
	virtual bool CreateObjectFilterQuery(const iprm::IParamsSet& filterParams, QString& filterQuery) const override;
	virtual bool CreateTextFilterQuery(const imtbase::ICollectionFilter& collectionFilter, QString& textFilterQuery) const override;
	virtual bool CreatePaginationQuery(int offset, int count, QByteArray& paginationQuery) const override;
	virtual bool CreateTimeFilterQuery(const imtbase::ITimeFilterParam& timeFilter, QString& timeFilterQuery, const QString& fieldId = QString()) const override;
	virtual QByteArray GetObjectSelectionQuery(const QByteArray& objectId, const iprm::IParamsSet* paramsPtr = nullptr) const override;
};


} // namespace imtdb


