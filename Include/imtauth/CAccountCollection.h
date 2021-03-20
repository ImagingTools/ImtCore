#pragma once


// ImtCore includes
#include <imtbase/TAggergatedObjectCollectionWrap.h>
#include <imtbase/CCollectionInfo.h>
#include <imtauth/IAccountInfoProvider.h>
#include <imtauth/CAccountInfo.h>


namespace imtauth
{


class CAccountCollection: public imtbase::TAggergatedObjectCollectionWrap<IAccountInfoProvider, CAccountInfo>
{
public:
	typedef imtbase::TAggergatedObjectCollectionWrap<IAccountInfoProvider, CAccountInfo> BaseClass;

	CAccountCollection();

	// reimplemented (IAccountInfoProvider)
	virtual const imtbase::ICollectionInfo& GetAccountList() const override;
	virtual const IAccountInfo* GetAccount(const QByteArray& accountId) const override = 0;
};


} // namespace imtauth


