// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtauth/IContractTypeInfo.h>
#include <imtauthdb/CSerializedObjectDbDelegateCompBase.h>


namespace imtauthdb
{


/**
	SQL delegate of the types of contracts the organizations created.
*/
class CContractTypeDbDelegateComp: public CSerializedObjectDbDelegateCompBase
{
public:
	typedef CSerializedObjectDbDelegateCompBase BaseClass;

	I_BEGIN_COMPONENT(CContractTypeDbDelegateComp)
		I_ASSIGN(m_dataFactoryCompPtr, "TypeFactory", "Factory of the stored objects", true, "ContractTypeInfo");
	I_END_COMPONENT

protected:
	// reimplemented (imtauthdb::CSerializedObjectDbDelegateCompBase)
	virtual istd::IChangeableUniquePtr CreateDataObject() const override;
	virtual bool GetColumns(const istd::IChangeable& object, Columns& columns) const override;

private:
	I_FACT(imtauth::IContractTypeInfo, m_dataFactoryCompPtr);
};


} // namespace imtauthdb


