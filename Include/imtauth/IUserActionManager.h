#pragma once


// ACF includes
#include <istd/IPolymorphic.h>

// ImtCore includes
#include <imtbase/IOperationContext.h>
#include <imtauth/IUserRecentAction.h>


namespace imtauth
{


class IUserActionManager : virtual public istd::IPolymorphic
{
public:
	virtual bool CreateUserAction(
				imtauth::IUserRecentAction::UserInfo userInfo,
				imtauth::IUserRecentAction::ActionTypeInfo actionTypeInfo,
				imtauth::IUserRecentAction::TargetInfo targetInfo,
				const iprm::IParamsSet* paramsPtr = nullptr) const = 0;
};


} // namespace imtauth


