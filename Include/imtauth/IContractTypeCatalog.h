// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IChangeable.h>

// ImtCore includes
#include <imtauth/IContractTypeInfo.h>


namespace imtauth
{


/**
	Catalog of the types of contracts: the system types and the types of the organizations.
	\ingroup Tenant
*/
class IContractTypeCatalog: virtual public istd::IChangeable
{
public:
	/**
		\param isReadPtr [optional] False when the types of the organization could not be read.
	*/
	virtual ContractTypeInfoList GetTypes(const QByteArray& tenantId, bool includeArchived, bool* isReadPtr = nullptr) const = 0;

	/**
		\return Null when there is no such type or it could not be read.
	*/
	virtual IContractTypeInfoUniquePtr GetType(const QByteArray& typeId) const = 0;
	virtual QByteArray CreateType(const IContractTypeInfo& type, QString& errorMessage) = 0;
	virtual bool UpdateType(const IContractTypeInfo& type, QString& errorMessage) = 0;
	virtual bool SetTypeArchived(const QByteArray& tenantId, const QByteArray& typeId, bool isArchived, QString& errorMessage) = 0;
};


} // namespace imtauth


