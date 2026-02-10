// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtservergql/CObjectCollectionControllerCompBase.h>
#include <imtauth/ICompanyInfo.h>


namespace imtlicgql
{


class CAccountControllerComp: public imtservergql::CObjectCollectionControllerCompBase
{
public:
	typedef imtservergql::CObjectCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CAccountControllerComp);
		I_ASSIGN(m_accountInfoFactCompPtr, "AccountFactory", "Factory used for creation of the new account instance", true, "AccountFactory");
	I_END_COMPONENT;

protected:
	// reimplemented (imtservergql::CObjectCollectionControllerCompBase)
	virtual imtbase::CTreeItemModel* GetObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual istd::IChangeableUniquePtr CreateObjectFromInputParams(const QList<imtgql::CGqlParamObject>& inputParams, QByteArray &objectId, QString& errorMessage) const override;

private:
	I_FACT(imtauth::ICompanyInfo, m_accountInfoFactCompPtr);
};


} // namespace imtlicgql


