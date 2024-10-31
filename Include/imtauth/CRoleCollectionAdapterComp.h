#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtauth/IRoleInfoProvider.h>


namespace imtauth
{


class CRoleCollectionAdapterComp:
			public icomp::CComponentBase,
			virtual public IRoleInfoProvider
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CRoleCollectionAdapterComp);
		I_REGISTER_INTERFACE(IRoleInfoProvider);
		I_ASSIGN(m_roleCollectionCompPtr, "RoleCollection", "Adapting the collection to the provider", true, "RoleCollection");
	I_END_COMPONENT;

	// reimplemented (IRoleInfoProvider)
	virtual const imtbase::ICollectionInfo& GetRoleList() const override;
	virtual const IRole* GetRole(const QByteArray& roleId, const QByteArray& productId) const override;
	virtual const IRole* GetRole(const QByteArray& objectId) const override;

protected:
	I_REF(imtbase::IObjectCollection, m_roleCollectionCompPtr);
};


} // namespace imtauth


