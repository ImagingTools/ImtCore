// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <imod/CMultiModelDispatcherBase.h>
#include <imod/IModel.h>

// ImtCore includes
#include <imtauth/ITenantDirectory.h>
#include <imtauth/ITenantManager.h>
#include <imtbase/IObjectCollection.h>


namespace imtauth
{


/**
	The profiles of the organizations in a collection, one per organization under its id.
	The collection selects the found profiles by the parameters of ITenantDirectory.
	A profile keeps the name of its organization and whether it is active, for the search: every change of a stored
	organization, whatever path wrote it, writes them again.
*/
class CTenantDirectoryComp:
			public ilog::CLoggerComponentBase,
			virtual public ITenantDirectory,
			protected imod::CMultiModelDispatcherBase
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;
	typedef imod::CMultiModelDispatcherBase BaseClass2;

	I_BEGIN_COMPONENT(CTenantDirectoryComp);
		I_REGISTER_INTERFACE(ITenantDirectory);
		I_ASSIGN(m_profileCollectionCompPtr, "ProfileCollection", "Profiles of the organizations", true, "TenantDirectoryProfileCollection");
		I_ASSIGN(m_profileFactoryCompPtr, "ProfileFactory", "Factory of a profile", true, "TenantDirectoryProfile");
		I_ASSIGN(m_tenantManagerCompPtr, "TenantManager", "Organizations: their names and whether they are active", true, "TenantManager");
		I_ASSIGN(m_tenantCollectionCompPtr, "TenantCollection", "Stored organizations, observed for the changes of their names and states", true, "TenantCollection");
		I_ASSIGN_TO(m_tenantCollectionModelCompPtr, m_tenantCollectionCompPtr, true);
	I_END_COMPONENT;

	// reimplemented (imtauth::ITenantDirectory)
	virtual ITenantDirectoryProfileUniquePtr GetProfile(const QByteArray& tenantId) const override;
	virtual bool SetProfile(const ITenantDirectoryProfile& profile, QString& errorMessage) override;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

	// reimplemented (imod::CMultiModelDispatcherBase)
	virtual void OnModelChanged(int modelId, const istd::IChangeable::ChangeSet& changeSet) override;

private:
	/**
		Write the current name and state of the organization into its stored profile.
	*/
	void UpdateTenantInfo(const QByteArray& tenantId);

	/**
		\param isStored Receives whether the organization has a stored profile.
		\return Null when the profile could not be read.
	*/
	ITenantDirectoryProfileUniquePtr ReadProfile(const QByteArray& tenantId, bool& isStored) const;

private:
	I_REF(imtbase::IObjectCollection, m_profileCollectionCompPtr);
	I_FACT(ITenantDirectoryProfile, m_profileFactoryCompPtr);
	I_REF(ITenantManager, m_tenantManagerCompPtr);
	I_REF(imtbase::IObjectCollection, m_tenantCollectionCompPtr);
	I_REF(imod::IModel, m_tenantCollectionModelCompPtr);
};


} // namespace imtauth


