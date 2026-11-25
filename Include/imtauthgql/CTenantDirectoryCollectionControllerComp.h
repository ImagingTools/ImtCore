// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtauth/IPartyRoleCatalog.h>
#include <imtauth/ITenantManager.h>
#include <imtauth/ITenantMembershipManager.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/TenantDirectory_fwd.h>


namespace imtauthgql
{


/**
	The directory of the organizations as the collection "DirectoryProfiles" (command DirectoryProfilesList).
	The organization of the session searches it by a text: only the listed active organizations matching it are given,
	the organization itself left out; without a text nothing is given, the directory is never browsed whole.
*/
class CTenantDirectoryCollectionControllerComp: public sdl::V1_0::imtauth::CDirectoryProfileEntryCollectionControllerCompBase
{
public:
	typedef sdl::V1_0::imtauth::CDirectoryProfileEntryCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CTenantDirectoryCollectionControllerComp);
		I_ASSIGN(m_tenantManagerCompPtr, "TenantManager", "Organizations", true, "TenantManager");
		I_ASSIGN(m_membershipManagerCompPtr, "MembershipManager", "Members and their permissions in the organizations", true, "TenantMembershipManager");
		I_ASSIGN(m_partyRoleCatalogCompPtr, "PartyRoleCatalog", "The roles the organizations offer, for their names", true, "PartyRoleCatalog");
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::V1_0::imtauth::CDirectoryProfileEntryCollectionControllerCompBase)
	virtual bool CreateRepresentationFromObject(
				const imtbase::IObjectCollectionIterator& objectCollectionIterator,
				const sdl::V1_0::imtauth::CDirectoryProfilesListGqlRequest& listRequest,
				sdl::V1_0::imtauth::CDirectoryProfileItemData& representationObject,
				QString& errorMessage) const override;

	// reimplemented (imtservergql::CObjectCollectionControllerCompBase)
	virtual void PrepareFilters(const imtgql::CGqlRequest& gqlRequest, const imtgql::CGqlParamObject& viewParamsGql, iprm::CParamsSet& filterParams) const override;

	// reimplemented (imtservergql::CPermissibleGqlRequestHandlerComp)
	virtual bool CheckPermissions(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

private:
	/**
		The organization of the session searching the directory; empty outside of an organization.
	*/
	static QByteArray GetSearchingTenantId(const imtgql::CGqlRequest& gqlRequest);

	QString GetRoleNames(const QByteArrayList& roleIds) const;

private:
	I_REF(imtauth::ITenantManager, m_tenantManagerCompPtr);
	I_REF(imtauth::ITenantMembershipManager, m_membershipManagerCompPtr);
	I_REF(imtauth::IPartyRoleCatalog, m_partyRoleCatalogCompPtr);
};


} // namespace imtauthgql


