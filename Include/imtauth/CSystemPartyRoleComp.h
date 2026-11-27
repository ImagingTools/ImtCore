// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtauth/CPartyRoleInfo.h>


namespace imtauth
{


/**
	A system role of the parties declared in an .acc file; its texts are translated in the context "Attribute".
*/
class CSystemPartyRoleComp:
			public icomp::CComponentBase,
			public CIdentifiablePartyRoleInfo
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CSystemPartyRoleComp);
		I_REGISTER_INTERFACE(IPartyRoleInfo);
		I_ASSIGN(m_roleIdAttrPtr, "RoleId", "Stable id of the role", true, "");
		I_ASSIGN(m_nameAttrPtr, "Name", "Name of the role", true, "");
		I_ASSIGN(m_descriptionAttrPtr, "Description", "Description of the role", false, "");
		I_ASSIGN(m_pairedRoleIdAttrPtr, "PairedRoleId", "Role of the other side of a link, the role itself for a symmetric one", false, "");
		I_ASSIGN(m_isPartyRoleAttrPtr, "IsPartyRole", "The role of a party in the whole contract", true, true);
		I_ASSIGN(m_isLinkRoleAttrPtr, "IsLinkRole", "The role of a party towards another one on a link", true, true);
	I_END_COMPONENT;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	I_ATTR(QByteArray, m_roleIdAttrPtr);
	I_TEXTATTR(m_nameAttrPtr);
	I_TEXTATTR(m_descriptionAttrPtr);
	I_ATTR(QByteArray, m_pairedRoleIdAttrPtr);
	I_ATTR(bool, m_isPartyRoleAttrPtr);
	I_ATTR(bool, m_isLinkRoleAttrPtr);
};


} // namespace imtauth


