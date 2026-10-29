// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtauth/IDocumentShareManager.h>
#include <imtauth/ITenantFilterParam.h>
#include <imtbase/IObjectCollection.h>
#include <imtgql/CGqlRequest.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/ImtCollection_fwd.h>


namespace imtservergql
{


/**
	HTTP header selecting the documents of a list with respect to sharing: Own, All (default), Incoming or Outgoing.
*/
static const QByteArray s_sharingScopeHeaderId = QByteArrayLiteral("sharingScope");

/**
	List item field of the collections with document sharing: access level of the requesting organization (Owner, Edit or View).
	Who the document is shared with is part of the element meta info.
*/
static const QByteArray s_sharingAccessFieldId = QByteArrayLiteral("sharingAccess");


/**
	String values of the document sharing enums used in the GraphQL API.
*/
QString DocumentAccessLevelToString(imtauth::IDocumentShareInfo::AccessLevel accessLevel);
imtauth::IDocumentShareInfo::AccessLevel DocumentAccessLevelFromString(const QString& accessLevel);
QString DocumentShareStatusToString(imtauth::IDocumentShareInfo::ShareStatus status);
imtauth::SharingScope SharingScopeFromString(const QString& sharingScope);

sdl::V1_0::imtbase::CDocumentShare CreateDocumentShareRepresentation(const imtauth::IDocumentShareInfo& shareInfo);

/**
	Check whether the request bypasses the organization access checks: only a superuser outside of any organization.
	Inside an organization the superuser is bound by the same document shares as its members.
*/
bool IsAccessCheckBypassed(const imtgql::CGqlRequest& gqlRequest);

/**
	Check that all objects are selected by the tenant filter with the given sharing scope and access level.
	\param tenantFilter Tenant filter of the request; its scope and access level are replaced.
*/
bool AreObjectsInSharingScope(
			const imtbase::IObjectCollection& collection,
			const imtauth::ITenantFilterParam& tenantFilter,
			const QByteArrayList& objectIds,
			imtauth::SharingScope sharingScope,
			imtauth::IDocumentShareInfo::AccessLevel accessLevel);

/**
	Check that the organization of the request may access all objects of the collection at the given level:
	its own objects, the objects of no organization and the objects an organization it is related to shares with it.
	\c DAL_OWNER admits the own objects only. The superuser outside of any organization is not restricted.
	\param shareManagerPtr Document shares; without it no shared object is admitted.
*/
bool HasObjectAccess(
			const imtbase::IObjectCollection& collection,
			const imtauth::IDocumentShareManager* shareManagerPtr,
			const imtgql::CGqlRequest& gqlRequest,
			const QByteArrayList& objectIds,
			imtauth::IDocumentShareInfo::AccessLevel accessLevel);


} // namespace imtservergql


