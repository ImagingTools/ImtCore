// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtauth/IContractInfo.h>
#include <imtauthdb/CSerializedObjectDbDelegateCompBase.h>


namespace imtauthdb
{


/**
	SQL delegate of the contracts: the contract with its state for the lists, and the index of its parties.
	The parameter "PartyTenantId" selects the contracts of a party.
*/
class CContractDbDelegateComp: public CSerializedObjectDbDelegateCompBase
{
public:
	typedef CSerializedObjectDbDelegateCompBase BaseClass;

	I_BEGIN_COMPONENT(CContractDbDelegateComp)
		I_ASSIGN(m_dataFactoryCompPtr, "ContractFactory", "Factory of the stored objects", true, "ContractInfo");
		I_ASSIGN(m_partiesTableNameAttrPtr, "PartiesTableName", "Table of the parties of the contracts", true, "TenantContractParties");
	I_END_COMPONENT

protected:
	// reimplemented (imtauthdb::CSerializedObjectDbDelegateCompBase)
	virtual istd::IChangeableUniquePtr CreateDataObject() const override;
	virtual bool GetColumns(const istd::IChangeable& object, Columns& columns) const override;
	virtual QString CreateDependentStatements(const QByteArray& objectId, const istd::IChangeable* objectPtr) const override;
	virtual QString CreateAdditionalFiltersQuery(const iprm::IParamsSet& filterParams) const override;

private:
	I_FACT(imtauth::IContractInfo, m_dataFactoryCompPtr);
	I_ATTR(QByteArray, m_partiesTableNameAttrPtr);
};


} // namespace imtauthdb


