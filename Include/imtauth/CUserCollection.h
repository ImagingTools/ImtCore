#pragma once


// ImtCore includes
#include <imtbase/TAggergatedObjectCollectionWrap.h>
#include <imtbase/CCollectionInfo.h>
#include <imtauth/IUserInfoProvider.h>
#include <imtauth/CUserInfo.h>


namespace imtauth
{


class CUserCollection: public imtbase::TAggergatedObjectCollectionWrap<IUserInfoProvider, CUserInfo>
{
public:
	typedef imtbase::TAggergatedObjectCollectionWrap<IUserInfoProvider, CUserInfo> BaseClass;

	CUserCollection();

	// reimplemented (IAccountInfoProvider)
	virtual const imtbase::ICollectionInfo& GetUserList() const override;
    virtual const IUserInfo* GetUser(const QByteArray& userId) const override;
};


} // namespace imtauth
