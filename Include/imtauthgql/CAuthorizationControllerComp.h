#pragma once


// ImtCore includes
#include <imtgql/CGqlRepresentationControllerCompBase.h>
#include <imtbase/IObjectCollection.h>
#include <imtcrypt/IHashCalculator.h>


namespace imtauthgql
{


class CAuthorizationControllerComp: public imtgql::CGqlRepresentationControllerCompBase
{
public:
	typedef imtgql::CGqlRepresentationControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CAuthorizationControllerComp);
		I_ASSIGN(m_sessionCollectionCompPtr, "SessionCollection", "Session collection", true, "SessionCollection");
		I_ASSIGN(m_userCollectionCompPtr, "UserCollection", "User collection", true, "UserCollection");
		I_ASSIGN(m_hashCalculatorCompPtr, "HashCalculator", "Hash calculator", true, "HashCalculator");
	I_END_COMPONENT;

protected:
	// reimplemented (imtgql::CGqlRepresentationControllerCompBase)
	virtual imtbase::CTreeItemModel* CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

private:
	I_REF(imtbase::IObjectCollection, m_sessionCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_userCollectionCompPtr);
	I_REF(imtcrypt::IHashCalculator, m_hashCalculatorCompPtr);
};


} // namespace imtauthgql


