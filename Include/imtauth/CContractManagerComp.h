// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// Qt includes
#include <functional>

// ImtCore includes
#include <imtauth/IContract.h>
#include <imtauth/IContractData.h>
#include <imtbase/IObjectCollection.h>


namespace imtauth
{


class CContractManagerComp:
			public ilog::CLoggerComponentBase,
			virtual public imtauth::IContract
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CContractManagerComp);
		I_REGISTER_INTERFACE(imtauth::IContract);
		I_ASSIGN(m_contractCollectionCompPtr, "ContractCollection", "Contract collection", false, "ContractCollection");
		I_ASSIGN(m_contractFactoryCompPtr, "ContractFactory", "Contract factory", false, "ContractInfo");
	I_END_COMPONENT;

	virtual QByteArray CreateContract(
				const QByteArray& relationshipId,
				const QByteArray& sourceTenantId,
				const QByteArray& targetTenantId,
				const QString& scope = QString(),
				const QString& validFrom = QString(),
				const QString& validUntil = QString(),
				const QString& description = QString(),
				const QString& terms = QString()) override;
	virtual bool UpdateContractStatus(const QByteArray& contractId, ContractStatus status) override;
	virtual bool TerminateContract(const QByteArray& contractId) override;
	virtual ContractInfo GetContract(const QByteArray& contractId) const override;
	virtual Contracts GetContractsBySourceTenant(const QByteArray& sourceTenantId) const override;
	virtual Contracts GetContractsByTargetTenant(const QByteArray& targetTenantId) const override;
	virtual Contracts GetContractsByRelationship(const QByteArray& relationshipId) const override;

private:
	Contracts CollectContracts(const std::function<bool(const ContractInfo&)>& predicate) const;
	bool SetContractStatus(const QByteArray& contractId, ContractStatus status);

private:
	I_REF(imtbase::IObjectCollection, m_contractCollectionCompPtr);
	I_FACT(imtauth::IContractData, m_contractFactoryCompPtr);
};


} // namespace imtauth
