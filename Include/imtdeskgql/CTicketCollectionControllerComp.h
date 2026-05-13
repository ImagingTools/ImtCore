// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <GeneratedFiles/imtdesksdl/SDL/1.0/CPP/ImtDesk.h>


namespace imtdeskgql
{


class CTicketCollectionControllerComp:
		public sdl::imtdesk::ImtDesk::CTicketCollectionControllerCompBase
{
public:
	typedef sdl::imtdesk::ImtDesk::CTicketCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CTicketCollectionControllerComp);
		I_ASSIGN(m_userCollectionCompPtr, "UserCollection", "Collection of users", true, "UserCollection");
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::imtdesk::ImtDesk::CTicketCollectionControllerCompBase)
	virtual bool CreateRepresentationFromObject(
				const imtbase::IObjectCollectionIterator& objectCollectionIterator,
				const sdl::imtdesk::ImtDesk::CTicketsListGqlRequest& listRequest,
				sdl::imtdesk::ImtDesk::CTicketItemData::V1_0& representationObject,
				QString& errorMessage) const override;
	virtual void SetAdditionalFilters(
				const imtgql::CGqlRequest& gqlRequest,
				const imtgql::CGqlParamObject& viewParamsGql,
				iprm::CParamsSet* filterParamsPtr) const override;

protected:
	I_REF(imtbase::IObjectCollection, m_userCollectionCompPtr);
};


} // namespace imtdeskgql
