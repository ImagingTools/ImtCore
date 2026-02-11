// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtdb/CSqlDatabaseDocumentDelegateComp.h>


namespace imtauthdb
{


class CUserActionDatabaseDelegateComp: public imtdb::CSqlDatabaseDocumentDelegateComp
{
public:
	typedef imtdb::CSqlDatabaseDocumentDelegateComp BaseClass;

	I_BEGIN_COMPONENT(CUserActionDatabaseDelegateComp)
		I_ASSIGN(m_userCollectionCompPtr, "UserCollection", "User collection", false, "UserCollection");
	I_END_COMPONENT

protected:
	// reimplemented (imtdb::CSqlDatabaseDocumentDelegateComp)
	virtual QString CreateAdditionalFiltersQuery(const iprm::IParamsSet& filterParams) const override;
	virtual QByteArray CreateJoinTablesQuery() const override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	I_REF(imtbase::IObjectCollection, m_userCollectionCompPtr);
};


} // namespace imtauthdb


