// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>
#include <iser/ISerializable.h>

// ImtCore includes
#include <imtauth/CUserRecentAction.h>


namespace imtauth
{


class CUserRecentActionComp: 
			public icomp::CComponentBase,
			public CIdentifiableUserRecentAction
{
public:
	typedef icomp::CComponentBase BaseClass;
	typedef CUserRecentAction BaseClass2;

	I_BEGIN_COMPONENT(CUserRecentActionComp);
		I_REGISTER_INTERFACE(IUserRecentAction);
		I_REGISTER_INTERFACE(iser::ISerializable);
		I_REGISTER_INTERFACE(istd::IChangeable);
		I_ASSIGN_MULTI_0(m_actionDataFactPtr, "ActionDataFactories", "Factory used for creation of action data instance", false);
		I_ASSIGN_MULTI_0(m_actionTypeIdsAttrPtr, "ActionTypeIds", "List of type IDs for user action", false);
	I_END_COMPONENT;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	I_MULTIATTR(QByteArray, m_actionTypeIdsAttrPtr);
	I_MULTIFACT(iser::ISerializable, m_actionDataFactPtr);
};


} // namespace imtauth


