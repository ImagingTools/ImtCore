// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/TIdentifiableWrap.h>
#include <imtauth/IContractData.h>


namespace imtauth
{


class CContract: virtual public IContractData, virtual public iser::ISerializable
{
public:
	CContract();

	virtual ContractInfo GetContractInfo() const override;
	virtual void SetContractInfo(const ContractInfo& contractInfo) override;
	virtual QByteArray GetContractId() const override;

	virtual bool Serialize(iser::IArchive& archive) override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	ContractInfo m_info;
};


typedef imtbase::TIdentifiableWrap<CContract> CIdentifiableContract;


} // namespace imtauth
