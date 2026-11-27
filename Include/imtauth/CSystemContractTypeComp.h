// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtauth/CSystemContractTypeCompBase.h>
#include <imtauth/CContractTypeInfo.h>


namespace imtauth
{


/**
	A system type of contracts declared in an .acc file; its texts are translated in the context "Attribute".
	A slot of roles is written "RoleId:Min:Max", a maximum of 0 is no limit.
*/
class CSystemContractTypeComp:
			public CSystemContractTypeCompBase,
			public CIdentifiableContractTypeInfo
{
public:
	typedef CSystemContractTypeCompBase BaseClass;

	I_BEGIN_COMPONENT(CSystemContractTypeComp);
		I_REGISTER_INTERFACE(IContractTypeInfo);
		I_ASSIGN(m_typeIdAttrPtr, "TypeId", "Stable id of the type", true, "");
		I_ASSIGN(m_nameAttrPtr, "Name", "Name of the type", true, "");
		I_ASSIGN(m_descriptionAttrPtr, "Description", "Description of the type", false, "");
		I_ASSIGN(m_numberPrefixAttrPtr, "NumberPrefix", "Prefix of the numbers of the contracts", true, "C");
		I_ASSIGN_MULTI_0(m_roleSlotsAttrPtr, "RoleSlots", "Roles of the parties as RoleId:Min:Max", false);
		I_ASSIGN(m_linkSchemeAttrPtr, "LinkScheme", "Default links: 0 manual, 1 every party with every other, 2 every party with the parties of StarRoleId", true, 0);
		I_ASSIGN(m_starRoleIdAttrPtr, "StarRoleId", "Role in the center of the links", false, "");
	I_END_COMPONENT;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	I_ATTR(QByteArray, m_typeIdAttrPtr);
	I_TEXTATTR(m_nameAttrPtr);
	I_TEXTATTR(m_descriptionAttrPtr);
	I_ATTR(QString, m_numberPrefixAttrPtr);
	I_MULTIATTR(QByteArray, m_roleSlotsAttrPtr);
	I_ATTR(int, m_linkSchemeAttrPtr);
	I_ATTR(QByteArray, m_starRoleIdAttrPtr);
};


} // namespace imtauth


