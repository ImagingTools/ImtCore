// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CTenantDirectoryCollectionControllerComp.h>


// ACF includes
#include <iprm/CTextParam.h>

// ImtCore includes
#include <imtauth/ITenantDirectory.h>
#include <imtauth/ITenantDirectoryProfile.h>
#include <imtauthgql/TenantAccess.h>
#include <imtbase/CComplexCollectionFilter.h>
#include <imtgql/imtgql.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/TenantDirectory.h>


namespace imtauthgql
{


namespace sdlauth = sdl::V1_0::imtauth;


// protected methods

// reimplemented (sdl::V1_0::imtauth::CDirectoryProfileEntryCollectionControllerCompBase)

bool CTenantDirectoryCollectionControllerComp::CreateRepresentationFromObject(
			const imtbase::IObjectCollectionIterator& objectCollectionIterator,
			const sdlauth::CDirectoryProfilesListGqlRequest& listRequest,
			sdlauth::CDirectoryProfileItemData& representationObject,
			QString& errorMessage) const
{
	imtbase::IObjectCollection::DataPtr dataPtr;
	const imtauth::ITenantDirectoryProfile* profilePtr = objectCollectionIterator.GetObjectData(dataPtr)
				? dynamic_cast<const imtauth::ITenantDirectoryProfile*>(dataPtr.GetPtr()) : nullptr;
	if (profilePtr == nullptr){
		errorMessage = QStringLiteral("Unable to read the directory profile '%1'").arg(QString::fromUtf8(objectCollectionIterator.GetObjectId()));

		return false;
	}

	const auto requestInfo = listRequest.GetRequestInfo();
	representationObject.id = objectCollectionIterator.GetObjectId();
	if (requestInfo.items.isTypeIdRequested){
		representationObject.typeId = objectCollectionIterator.GetObjectTypeId();
	}
	if (requestInfo.items.isTenantNameRequested){
		representationObject.tenantName = profilePtr->GetTenantName();
	}
	if (requestInfo.items.isIndustryRequested){
		representationObject.industry = profilePtr->GetIndustry();
	}
	if (requestInfo.items.isCountryRequested){
		representationObject.country = profilePtr->GetCountry();
	}
	if (requestInfo.items.isOfferedRolesRequested){
		representationObject.offeredRoles = GetRoleNames(profilePtr->GetOfferedRoleIds());
	}
	if (requestInfo.items.isDescriptionRequested){
		representationObject.description = profilePtr->GetDescription();
	}
	if (requestInfo.items.isWebsiteRequested){
		representationObject.website = profilePtr->GetWebsite();
	}
	if (requestInfo.items.isAcceptsInvitationsFromUnknownRequested){
		representationObject.acceptsInvitationsFromUnknown = profilePtr->IsAcceptingInvitationsFromUnknown();
	}

	return true;
}


// reimplemented (imtservergql::CObjectCollectionControllerCompBase)

void CTenantDirectoryCollectionControllerComp::PrepareFilters(
			const imtgql::CGqlRequest& gqlRequest,
			const imtgql::CGqlParamObject& viewParamsGql,
			iprm::CParamsSet& filterParams) const
{
	BaseClass::PrepareFilters(gqlRequest, viewParamsGql, filterParams);

	// The columns of the table are not the fields of the profile: the text goes to the search of the directory,
	// and the found organizations are given by their names
	QString text;
	imtbase::CComplexCollectionFilter* complexFilterPtr = dynamic_cast<imtbase::CComplexCollectionFilter*>(filterParams.GetEditableParameter(QByteArrayLiteral("ComplexFilter")));
	if (complexFilterPtr != nullptr){
		text = complexFilterPtr->GetTextFilter();

		imtbase::IComplexCollectionFilter::FieldMetaInfo sortInfo;
		sortInfo.flags = imtbase::IComplexCollectionFilter::SO_SORT;
		sortInfo.sortingOrder = imtbase::IComplexCollectionFilter::SO_ASC;

		complexFilterPtr->SetTextFilter(QString());
		complexFilterPtr->SetFilterExpression(imtbase::IComplexCollectionFilter::FilterExpression());
		complexFilterPtr->SetFields({imtbase::IComplexCollectionFilter::FieldInfo(QByteArrayLiteral("SearchText"), sortInfo)});
	}

	iprm::CTextParam* textParamPtr = new iprm::CTextParam;
	textParamPtr->SetText(text);
	filterParams.SetEditableParameter(imtauth::s_directorySearchTextParamId, textParamPtr, true);

	iprm::CTextParam* excludedParamPtr = new iprm::CTextParam;
	excludedParamPtr->SetText(QString::fromUtf8(GetSearchingTenantId(gqlRequest)));
	filterParams.SetEditableParameter(imtauth::s_directoryExcludedTenantIdParamId, excludedParamPtr, true);
}


// reimplemented (imtservergql::CPermissibleGqlRequestHandlerComp)

bool CTenantDirectoryCollectionControllerComp::CheckPermissions(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	// The directory is searched for a partner to connect with or to add to a contract
	const QByteArray tenantId = GetSearchingTenantId(gqlRequest);
	if (tenantId.isEmpty()){
		errorMessage = QStringLiteral("The directory is searched by an organization");

		return false;
	}

	return HasTenantManageAccess(*m_tenantManagerCompPtr, m_membershipManagerCompPtr.GetPtr(), gqlRequest, tenantId,
				{s_connectOrganizationPermission, s_editContractsPermission}, errorMessage);
}


// private methods

QByteArray CTenantDirectoryCollectionControllerComp::GetSearchingTenantId(const imtgql::CGqlRequest& gqlRequest)
{
	const imtgql::IGqlContext* contextPtr = gqlRequest.GetRequestContext();
	if (contextPtr == nullptr){
		return QByteArray();
	}

	imtbase::TenantType tenantType = imtbase::TT_USER;
	const QByteArray tenantId = imtgql::GetDataTenantId(*contextPtr, tenantType);

	return tenantType == imtbase::TT_ORGANIZATION ? tenantId : QByteArray();
}


QString CTenantDirectoryCollectionControllerComp::GetRoleNames(const QByteArrayList& roleIds) const
{
	QStringList names;
	for (const QByteArray& roleId : roleIds){
		imtauth::IPartyRoleInfoUniquePtr rolePtr = m_partyRoleCatalogCompPtr->GetRole(roleId);
		if (rolePtr.IsValid() && !rolePtr->IsArchived()){
			names << rolePtr->GetName();
		}
	}

	return names.join(QStringLiteral(", "));
}


} // namespace imtauthgql


