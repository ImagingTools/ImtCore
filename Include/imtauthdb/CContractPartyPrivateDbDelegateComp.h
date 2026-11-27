// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtauth/IContractPartyPrivateInfo.h>
#include <imtauthdb/CSerializedObjectDbDelegateCompBase.h>


namespace imtauthdb
{


/**
	SQL delegate of the records the parties of the contracts keep for themselves.
*/
class CContractPartyPrivateDbDelegateComp: public CSerializedObjectDbDelegateCompBase
{
public:
	typedef CSerializedObjectDbDelegateCompBase BaseClass;

	I_BEGIN_COMPONENT(CContractPartyPrivateDbDelegateComp)
		I_ASSIGN(m_dataFactoryCompPtr, "PrivateInfoFactory", "Factory of the stored objects", true, "ContractPartyPrivateInfo");
	I_END_COMPONENT

protected:
	// reimplemented (imtauthdb::CSerializedObjectDbDelegateCompBase)
	virtual istd::IChangeableUniquePtr CreateDataObject() const override;
	virtual bool GetColumns(const istd::IChangeable& object, Columns& columns) const override;

private:
	I_FACT(imtauth::IContractPartyPrivateInfo, m_dataFactoryCompPtr);
};


} // namespace imtauthdb


