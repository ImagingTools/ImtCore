// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtauth/IUserActionManager.h>


namespace imtauth
{


class CUserActionManagerComp:
			public icomp::CComponentBase,
			public imtauth::IUserActionManager
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CUserActionManagerComp);
		I_REGISTER_INTERFACE(imtauth::IUserActionManager);
		I_ASSIGN(m_userActionCollectionCompPtr, "UserActionCollection", "Role provider", false, "UserActionCollection");
	I_END_COMPONENT;

protected:
	// reimplemented (imtauth::IUserActionManager)
	virtual bool CreateUserAction(
				imtauth::IUserRecentAction::UserInfo userInfo,
				imtauth::IUserRecentAction::ActionTypeInfo actionTypeInfo,
				imtauth::IUserRecentAction::TargetInfo targetInfo,
				const iprm::IParamsSet* paramsPtr = nullptr) const override;

private:
	I_REF(imtbase::IObjectCollection, m_userActionCollectionCompPtr);
};


} // namespace imtauth
