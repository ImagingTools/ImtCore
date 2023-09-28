#pragma once


// ImtCore includes
#include <imtgql/CObjectCollectionControllerCompBase.h>
#include <imtauth/ICompanyInfo.h>


namespace imtlicgql
{


class CAccountControllerComp: public imtgql::CObjectCollectionControllerCompBase
{
public:
	typedef imtgql::CObjectCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CAccountControllerComp);
		I_ASSIGN(m_accountInfoFactCompPtr, "AccountFactory", "Factory used for creation of the new account instance", true, "AccountFactory");
	I_END_COMPONENT;

protected:
	// reimplemented (imtgql::CObjectCollectionControllerCompBase)
	virtual imtbase::CTreeItemModel* GetObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual istd::IChangeable* CreateObject(const QList<imtgql::CGqlObject>& inputParams, QByteArray &objectId, QString &name, QString &description, QString& errorMessage) const override;

private:
	I_FACT(imtauth::ICompanyInfo, m_accountInfoFactCompPtr);
};


} // namespace imtlicgql


