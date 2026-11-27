// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtauth/IPartyRoleInfo.h>
#include <imtauthdb/CSerializedObjectDbDelegateCompBase.h>


namespace imtauthdb
{


/**
	SQL delegate of the roles of the parties the organizations created.
*/
class CPartyRoleDbDelegateComp: public CSerializedObjectDbDelegateCompBase
{
public:
	typedef CSerializedObjectDbDelegateCompBase BaseClass;

	I_BEGIN_COMPONENT(CPartyRoleDbDelegateComp)
		I_ASSIGN(m_dataFactoryCompPtr, "RoleFactory", "Factory of the stored objects", true, "PartyRoleInfo");
	I_END_COMPONENT

protected:
	// reimplemented (imtauthdb::CSerializedObjectDbDelegateCompBase)
	virtual istd::IChangeableUniquePtr CreateDataObject() const override;
	virtual bool GetColumns(const istd::IChangeable& object, Columns& columns) const override;

private:
	I_FACT(imtauth::IPartyRoleInfo, m_dataFactoryCompPtr);
};


} // namespace imtauthdb


