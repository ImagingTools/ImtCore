// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IChangeable.h>

// ImtCore includes
#include <imtauth/IContractInfo.h>
#include <imtauth/IContractPartyPrivateInfo.h>
#include <imtauth/IContractEventInfo.h>


namespace imtauth
{


/**
	Read access to the contracts of the organizations; every result is a copy of its own.
	\ingroup Tenant
*/
class IContractProvider: virtual public istd::IChangeable
{
public:
	/**
		\return Null when there is no such contract or it could not be read.
	*/
	virtual IContractInfoUniquePtr GetContract(const QByteArray& contractId) const = 0;

	/**
		The contracts the organization is a party of; a draft only for its initiator.
		\param isReadPtr [optional] False when the contracts could not be read completely.
	*/
	virtual ContractInfoList GetContractsOfParty(const QByteArray& tenantId, const QByteArray& productId, bool* isReadPtr = nullptr) const = 0;

	/**
		The sent contracts with a link of the two organizations.
		\param isReadPtr [optional] False when the contracts could not be read completely.
	*/
	virtual ContractInfoList GetContractsOfPair(const QByteArray& tenantId, const QByteArray& otherTenantId, const QByteArray& productId, bool* isReadPtr = nullptr) const = 0;

	/**
		The contracts that did not end: negotiated, signed or in force.
		\param isReadPtr [optional] False when the contracts could not be read completely.
	*/
	virtual ContractInfoList GetOpenContracts(bool* isReadPtr = nullptr) const = 0;

	/**
		What the party keeps for itself; an empty record for a party that kept nothing.
		\return Null when the record could not be read.
	*/
	virtual IContractPartyPrivateInfoUniquePtr GetPrivateInfo(const QByteArray& contractId, const QByteArray& tenantId) const = 0;

	/**
		The journal of the contract as the party sees it: the common entries and its own private ones, oldest first.
	*/
	virtual ContractEventInfoList GetEvents(const QByteArray& contractId, const QByteArray& tenantId, bool* isReadPtr = nullptr) const = 0;
};


} // namespace imtauth


