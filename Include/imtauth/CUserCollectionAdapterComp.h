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
		I_ASSIGN(m_userCollectionCompPtr, "UserCollection", "Adapting the collection to the provider", true, "UserCollection");
		I_ASSIGN(m_separatorObjectIdAttrPtr, "SeparatorObjectId", "Separator of the object ID", false, "");
	I_END_COMPONENT;

	// reimplemented (IUserInfoProvider)
	virtual const imtbase::ICollectionInfo& GetUserList() const override;
	virtual const IUserInfo* GetUser(const QByteArray& objectId) const override;

protected:
	I_REF(imtbase::IObjectCollection, m_userCollectionCompPtr);
	I_ATTR(QByteArray, m_separatorObjectIdAttrPtr);
};


} // namespace imtauth


