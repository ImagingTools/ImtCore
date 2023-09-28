#pragma once


// ImtCore includes
#include <imtgql/CObjectCollectionControllerCompBase.h>
#include <imtauth/IUserInfo.h>
#include <imtcrypt/IHashGenerator.h>


namespace imtauthgql
{


class CUserControllerComp: public imtgql::CObjectCollectionControllerCompBase
{
public:
	typedef imtgql::CObjectCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CUserControllerComp);
		I_ASSIGN(m_userInfoFactCompPtr, "UserFactory", "Factory used for creation of the new user", true, "UserFactory");
		I_ASSIGN(m_hashCalculatorCompPtr, "HashCalculator", "Hash calculator", true, "HashCalculator");
	I_END_COMPONENT;

protected:
	virtual imtbase::CTreeItemModel* GetObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual imtbase::CTreeItemModel* UpdateObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual istd::IChangeable* CreateObject(const QList<imtgql::CGqlObject>& inputParams, QByteArray &objectId, QString &name, QString &description, QString& errorMessage) const override;
	virtual imtbase::CTreeItemModel* DeleteObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

private:
	I_FACT(imtauth::IUserInfo, m_userInfoFactCompPtr);
	I_REF(imtcrypt::IHashGenerator, m_hashCalculatorCompPtr);
};


} // namespace imtauthgql


