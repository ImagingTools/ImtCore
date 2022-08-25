#pragma once


// ImtCore includes
#include <imtguigql/CObjectCollectionControllerCompBase.h>
#include <imtauth/IUserInfo.h>


namespace imtauthgql
{


class CUserControllerComp: public imtguigql::CObjectCollectionControllerCompBase
{
public:
	typedef imtguigql::CObjectCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CUserControllerComp);
		I_ASSIGN(m_userInfoFactCompPtr, "UserFactory", "Factory used for creation of the new user", true, "UserFactory");
	I_END_COMPONENT;

protected:
	virtual imtbase::CTreeItemModel* GetObject(const QList<imtgql::CGqlObject>& inputParams, const imtgql::CGqlObject& gqlObject, QString& errorMessage) const override;
	virtual istd::IChangeable* CreateObject(const QList<imtgql::CGqlObject>& inputParams, QByteArray &objectId, QString &name, QString &description, QString& errorMessage) const override;

private:
	I_FACT(imtauth::IUserInfo, m_userInfoFactCompPtr);
};


} // namespace imtauthgql


