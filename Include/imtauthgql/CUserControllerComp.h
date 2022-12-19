#pragma once


// ImtCore includes
#include <imtguigql/CObjectCollectionControllerCompBase.h>
#include <imtauth/IUserInfo.h>
#include <imtcrypt/IHashCalculator.h>


namespace imtauthgql
{


class CUserControllerComp: public imtguigql::CObjectCollectionControllerCompBase
{
public:
	typedef imtguigql::CObjectCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CUserControllerComp);
		I_ASSIGN(m_userInfoFactCompPtr, "UserFactory", "Factory used for creation of the new user", true, "UserFactory");
		I_ASSIGN(m_rolesCollectionCompPtr, "RolesCollection", "Roles collection", true, "RolesCollection");
		I_ASSIGN(m_hashCalculatorCompPtr, "HashCalculator", "Hash calculator", true, "HashCalculator");
	I_END_COMPONENT;

protected:
	virtual imtbase::CHierarchicalItemModelPtr GetObject(const QList<imtgql::CGqlObject>& inputParams, const imtgql::CGqlObject& gqlObject, const imtgql::IGqlContext* gqlContext, QString& errorMessage) const override;
	virtual istd::IChangeable* CreateObject(const QList<imtgql::CGqlObject>& inputParams, QByteArray &objectId, QString &name, QString &description, QString& errorMessage) const override;

private:
	I_FACT(imtauth::IUserInfo, m_userInfoFactCompPtr);
	I_REF(imtbase::IObjectCollection, m_rolesCollectionCompPtr);
	I_REF(imtcrypt::IHashCalculator, m_hashCalculatorCompPtr);
};


} // namespace imtauthgql


