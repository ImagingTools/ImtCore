// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QMutex>

// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtauth/IContractTypeCatalog.h>
#include <imtauth/IPartyRoleCatalog.h>
#include <imtbase/IObjectCollection.h>


namespace imtauth
{


/**
	The system types of the .acc file and the types the organizations keep in a collection.
*/
class CContractTypeCatalogComp:
			public ilog::CLoggerComponentBase,
			virtual public IContractTypeCatalog
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CContractTypeCatalogComp);
		I_REGISTER_INTERFACE(IContractTypeCatalog);
		I_ASSIGN_MULTI_0(m_systemTypesCompPtr, "SystemTypes", "Types available to every organization", false);
		I_ASSIGN(m_typeCollectionCompPtr, "TypeCollection", "Types of the organizations", true, "ContractTypeCollection");
		I_ASSIGN(m_typeFactoryCompPtr, "TypeFactory", "Factory of a type", true, "ContractTypeInfo");
		I_ASSIGN(m_roleCatalogCompPtr, "PartyRoleCatalog", "Roles the slots of a type use", true, "PartyRoleCatalog");
	I_END_COMPONENT;

	// reimplemented (imtauth::IContractTypeCatalog)
	virtual ContractTypeInfoList GetTypes(const QByteArray& tenantId, bool includeArchived, bool* isReadPtr = nullptr) const override;
	virtual IContractTypeInfoUniquePtr GetType(const QByteArray& typeId) const override;
	virtual QByteArray CreateType(const IContractTypeInfo& type, QString& errorMessage) override;
	virtual bool UpdateType(const IContractTypeInfo& type, QString& errorMessage) override;
	virtual bool SetTypeArchived(const QByteArray& tenantId, const QByteArray& typeId, bool isArchived, QString& errorMessage) override;

private:
	IContractTypeInfoUniquePtr CopyType(const IContractTypeInfo& type) const;
	IContractTypeInfoUniquePtr ReadStoredType(const QByteArray& typeId) const;
	bool StoreType(const IContractTypeInfo& type, bool isNew);
	bool CheckType(const IContractTypeInfo& type, QString& errorMessage) const;
	bool IsRoleAvailable(const QByteArray& roleId, const QByteArray& tenantId) const;

private:
	I_MULTIREF(IContractTypeInfo, m_systemTypesCompPtr);
	I_REF(imtbase::IObjectCollection, m_typeCollectionCompPtr);
	I_FACT(IContractTypeInfo, m_typeFactoryCompPtr);
	I_REF(IPartyRoleCatalog, m_roleCatalogCompPtr);

	// The check of a name and the change are one step
	QMutex m_changeMutex;
};


} // namespace imtauth


