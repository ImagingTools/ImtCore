#pragma once


// ImtCore includes
#include <imtbase/TAggergatedObjectCollectionWrap.h>
#include <imtbase/CCollectionInfo.h>
#include <imtauth/IPersonInfoProvider.h>
#include <imtauth/CPersonInfo.h>


namespace imtauth
{


class CPersonCollection: public imtbase::TAggergatedObjectCollectionWrap<IPersonInfoProvider, CPersonInfo>
{
public:
	typedef imtbase::TAggergatedObjectCollectionWrap<IPersonInfoProvider, CPersonInfo> BaseClass;

	CPersonCollection();

	// reimplemented (IPersonInfoProvider)
	virtual const imtbase::ICollectionInfo& GetPersonList() const override;
	virtual const IPersonInfo* GetPerson(const QByteArray& personId) const override;
};


} // namespace imtauth


