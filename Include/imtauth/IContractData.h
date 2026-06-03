// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iser/IObject.h>
#include <istd/TSmartPtr.h>

// ImtCore includes
#include <imtauth/IContract.h>


namespace imtauth
{


/**
	Serializable data holder for a single cooperation contract.
	Used as the stored representation of a ContractInfo inside an
	object collection (e.g. an SQL-backed repository).
*/
class IContractData: virtual public iser::IObject
{
public:
	/**
		Get the full contract description.
	*/
	virtual ContractInfo GetContractInfo() const = 0;

	/**
		Set the full contract description.
	*/
	virtual void SetContractInfo(const ContractInfo& contractInfo) = 0;

	/**
		Convenience accessor for the contract ID, which is used as the
		collection element key.
	*/
	virtual QByteArray GetContractId() const = 0;
};


typedef istd::TUniqueInterfacePtr<IContractData> IContractDataUniquePtr;
typedef istd::TSharedInterfacePtr<IContractData> IContractDataSharedPtr;


} // namespace imtauth
