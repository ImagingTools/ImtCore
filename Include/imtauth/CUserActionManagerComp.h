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
		I_ASSIGN(m_userActionCollectionCompPtr, "UserActionCollection", "Role provider", true, "UserActionCollection");
		I_ASSIGN(m_userRecentActionFactCompPtr, "UserActionFactory", "User action factory", true, "UserActionFactory");
	I_END_COMPONENT;

protected:
	// reimplemented (imtauth::IUserActionManager)
	virtual bool CreateUserAction(
				imtauth::IUserRecentAction::UserInfo userInfo,
				imtauth::IUserRecentAction::ActionTypeInfo actionTypeInfo,
				imtauth::IUserRecentAction::TargetInfo targetInfo,
				const iser::ISerializableSharedPtr actionDataPtr = nullptr) const override;
	virtual IUserActionInfoUniquePtr GetUserAction(const QByteArray& actionId) const override;
	virtual QByteArrayList GetUserActionIds(int offset = 0, int count = -1, const iprm::IParamsSet* paramsSetPtr = nullptr) const override;

private:
	I_REF(imtbase::IObjectCollection, m_userActionCollectionCompPtr);
	I_FACT(imtauth::IUserRecentAction, m_userRecentActionFactCompPtr);
};


} // namespace imtauth


