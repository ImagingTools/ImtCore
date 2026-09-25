// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtservergql/CFilterableSelectControllerComp.h>
#include <imttag/ITagAssignmentManager.h>
#include <imttag/ISystemTagSeeder.h>


namespace imttaggql
{


/**
	Filterable select of the tag catalog: every item carries the tag color and the
	text parameters \c IsSystem ("true"/"false") and \c UsageCount.
*/
class CTagSelectControllerComp: public imtservergql::CFilterableSelectControllerComp
{
public:
	typedef imtservergql::CFilterableSelectControllerComp BaseClass;

	I_BEGIN_COMPONENT(CTagSelectControllerComp)
		I_ASSIGN(m_assignmentManagerCompPtr, "AssignmentManager", "Tag assignment manager providing the usage counts", false, "TagAssignmentManager");
		I_ASSIGN(m_systemTagSeederCompPtr, "SystemTagSeeder", "Seeder of the default system tags", false, "SystemTagSeeder");
	I_END_COMPONENT

	static const QByteArray s_isSystemParamId;
	static const QByteArray s_usageCountParamId;

protected:
	// reimplemented (imtservergql::CFilterableSelectControllerComp)
	virtual sdl::V1_0::imtbase::CGetSelectableItemsPayload OnGetSelectableItems(
				const sdl::V1_0::imtbase::CGetSelectableItemsGqlRequest& getSelectableItemsRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual void OnSelectableItemCreated(
				const imtbase::IObjectCollectionIterator& iterator,
				sdl::V1_0::imtbase::CSelectableItemData& itemRepresentation,
				const ::imtgql::CGqlRequest& gqlRequest) const override;

private:
	static void AddTextParameter(sdl::V1_0::imtbase::CSelectableItemData& itemRepresentation, const QByteArray& parameterId, const QString& text);

protected:
	I_REF(imttag::ITagAssignmentManager, m_assignmentManagerCompPtr);
	I_REF(imttag::ISystemTagSeeder, m_systemTagSeederCompPtr);
};


} // namespace imttaggql


