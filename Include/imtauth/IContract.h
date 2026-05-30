// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IChangeable.h>

// Qt includes
#include <QByteArray>
#include <QString>
#include <QList>


namespace imtauth
{


/**
	Lifecycle status of a cooperation contract.
	The contract lifecycle is independent of the underlying relationship
	lifecycle: a relationship may exist without any contract, and may carry
	several contracts over time (e.g. renewal, amendment).
*/
enum ContractStatus
{
	CTS_DRAFT = 0,
	CTS_ACTIVE,
	CTS_EXPIRED,
	CTS_TERMINATED,
	CTS_RENEWED
};


/**
	Structure describing a cooperation contract between two tenants.

	A contract defines the commercial/governance terms of a cooperation. It is
	always anchored to an existing tenant relationship (\a relationshipId is
	mandatory): the relationship models the long-lived identity/trust between
	the tenants, while the contract models the (potentially multiple, time-boxed)
	terms governing that cooperation.

	A contract is bilateral — it is stored once as a single shared record that
	both tenants reference (unlike relationships, which are mirrored per tenant).
	The \a sourceTenantId is the tenant that owns/initiated the contract and
	\a targetTenantId is the counterparty.
*/
struct ContractInfo
{
	QByteArray contractId;
	QByteArray relationshipId;
	QByteArray sourceTenantId;
	QByteArray targetTenantId;
	ContractStatus status = CTS_DRAFT;
	QString scope;
	QString validFrom;
	QString validUntil;
	QString description;
	QString terms;
	QString createdAt;
	QString updatedAt;

	bool operator==(const ContractInfo& other) const
	{
		return contractId == other.contractId
			&& relationshipId == other.relationshipId
			&& sourceTenantId == other.sourceTenantId
			&& targetTenantId == other.targetTenantId
			&& status == other.status
			&& scope == other.scope
			&& validFrom == other.validFrom
			&& validUntil == other.validUntil
			&& description == other.description
			&& terms == other.terms
			&& createdAt == other.createdAt
			&& updatedAt == other.updatedAt;
	}

	bool operator!=(const ContractInfo& other) const
	{
		return !(*this == other);
	}
};

typedef QList<ContractInfo> Contracts;


/**
	Interface for managing cooperation contracts between tenants.

	Contracts sit between relationships and cross-org grants in the cooperation
	hierarchy: Relationship (identity/trust) -> Contract (terms) -> Grant (access).
	Each contract references the relationship it belongs to. A relationship can
	have zero or more contracts; a grant may optionally reference the contract
	that governs it.

	\ingroup Tenant
*/
class IContract: virtual public istd::IChangeable
{
public:
	/**
		Create a new cooperation contract.
		\param relationshipId The relationship this contract is anchored to (mandatory).
		\param sourceTenantId Tenant owning/initiating the contract.
		\param targetTenantId Counterparty tenant.
		\param scope Optional scope describing what the contract covers.
		\param validFrom Optional validity start timestamp (empty for open start).
		\param validUntil Optional validity end timestamp (empty for no expiry).
		\param description Human-readable description of the contract.
		\param terms Optional free-form terms metadata (e.g. JSON).
		\return Contract ID if successful, empty if failed.
	*/
	virtual QByteArray CreateContract(
		const QByteArray& relationshipId,
		const QByteArray& sourceTenantId,
		const QByteArray& targetTenantId,
		const QString& scope = QString(),
		const QString& validFrom = QString(),
		const QString& validUntil = QString(),
		const QString& description = QString(),
		const QString& terms = QString()) = 0;

	/**
		Update the lifecycle status of a contract.
		\return true if updated successfully.
	*/
	virtual bool UpdateContractStatus(const QByteArray& contractId, ContractStatus status) = 0;

	/**
		Terminate a contract (convenience for setting status to Terminated).
		\return true if terminated successfully.
	*/
	virtual bool TerminateContract(const QByteArray& contractId) = 0;

	/**
		Get a specific contract by its ID.
	*/
	virtual ContractInfo GetContract(const QByteArray& contractId) const = 0;

	/**
		Get all contracts where the specified tenant is the source (owner).
	*/
	virtual Contracts GetContractsBySourceTenant(const QByteArray& sourceTenantId) const = 0;

	/**
		Get all contracts where the specified tenant is the target (counterparty).
	*/
	virtual Contracts GetContractsByTargetTenant(const QByteArray& targetTenantId) const = 0;

	/**
		Get all contracts anchored to a specific relationship.
	*/
	virtual Contracts GetContractsByRelationship(const QByteArray& relationshipId) const = 0;
};


} // namespace imtauth
