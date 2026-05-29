// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <GeneratedFiles/imtdesksdl/SDL/1.0/CPP/ImtDesk>


namespace imtdeskgql
{


class CTicketCollectionControllerComp:
		public sdl::V1_0::imtdesk::CTicketCollectionControllerCompBase
{
public:
	typedef sdl::V1_0::imtdesk::CTicketCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CTicketCollectionControllerComp);
		I_ASSIGN(m_userCollectionCompPtr, "UserCollection", "Collection of users", true, "UserCollection");
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::V1_0::imtdesk::CTicketCollectionControllerCompBase)
	virtual bool CreateRepresentationFromObject(
				const imtbase::IObjectCollectionIterator& objectCollectionIterator,
				const sdl::V1_0::imtdesk::CTicketsListGqlRequest& listRequest,
				sdl::V1_0::imtdesk::CTicketItemData& representationObject,
				QString& errorMessage) const override;
	virtual void SetAdditionalFilters(
				const imtgql::CGqlRequest& gqlRequest,
				const imtgql::CGqlParamObject& viewParamsGql,
				iprm::CParamsSet* filterParamsPtr) const override;

protected:
	I_REF(imtbase::IObjectCollection, m_userCollectionCompPtr);
};


} // namespace imtdeskgql
