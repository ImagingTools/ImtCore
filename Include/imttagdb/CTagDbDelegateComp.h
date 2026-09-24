// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtdb/CSqlDatabaseDocumentDelegateComp.h>


namespace imttagdb
{


/**
	Document delegate of the tag catalog.
	A tenant sees its own tags plus the system tags, which have no tenant binding.
*/
class CTagDbDelegateComp: public imtdb::CSqlDatabaseDocumentDelegateComp
{
public:
	typedef imtdb::CSqlDatabaseDocumentDelegateComp BaseClass;

	I_BEGIN_COMPONENT(CTagDbDelegateComp)
	I_END_COMPONENT

protected:
	QString CreateIsSystemCondition() const;

	// reimplemented (imtdb::CSqlDatabaseDocumentDelegateCompBase)
	virtual QString CreateTenantBindingFilterQuery(const QByteArray& tenantId, imtauth::TenantFilterMode filterMode = imtauth::TFM_INCLUDE) const override;
	virtual QByteArray CreateTenantBindingDeleteQuery(const QByteArrayList& entityIds) const override;
};


} // namespace imttagdb


