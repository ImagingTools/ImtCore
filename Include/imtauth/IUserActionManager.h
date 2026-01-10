#pragma once


// ACF includes
#include <istd/IPolymorphic.h>

// ImtCore includes
#include <imtauth/IUserRecentAction.h>


namespace imtauth
{


class IUserActionManager : virtual public istd::IPolymorphic
{
public:
	virtual bool CreateUserAction(
				IUserRecentAction::UserInfo userInfo,
				IUserRecentAction::ActionTypeInfo actionTypeInfo,
				IUserRecentAction::TargetInfo targetInfo,
				const iprm::IParamsSet* paramsPtr = nullptr) const = 0;
};


} // namespace imtauth


