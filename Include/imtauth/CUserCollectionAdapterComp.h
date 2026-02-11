// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtauth/IUserInfoProvider.h>


namespace imtauth
{


class CUserCollectionAdapterComp: public icomp::CComponentBase, virtual public IUserInfoProvider
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CUserCollectionAdapterComp);
		I_REGISTER_INTERFACE(IUserInfoProvider);
		I_ASSIGN(m_userCollectionCompPtr, "UserCollection", "Undelaying user collection", true, "UserCollection");
	I_END_COMPONENT;

	// reimplemented (IUserInfoProvider)
	virtual const imtbase::ICollectionInfo& GetUserList() const override;
	virtual imtauth::IUserInfoUniquePtr GetUser(const QByteArray& objectId, const iprm::IParamsSet* paramsPtr = nullptr) const override;

protected:
	I_REF(imtbase::IObjectCollection, m_userCollectionCompPtr);
};


} // namespace imtauth


