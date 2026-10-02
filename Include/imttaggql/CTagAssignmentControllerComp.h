// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imttag/ITagAssignmentManager.h>
#include <GeneratedFiles/imttagsdl/SDL/1.0/CPP/Tags_fwd.h>


namespace imttaggql
{


/**
	GraphQL controller of tag assignments: read, add and remove
	the tags of entities of the taggable types registered in the assignment manager.
*/
class CTagAssignmentControllerComp:
			public sdl::V1_0::imttag::CTagsGqlHandlerCompBase
{
public:
	typedef sdl::V1_0::imttag::CTagsGqlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CTagAssignmentControllerComp);
		I_ASSIGN(m_assignmentManagerCompPtr, "AssignmentManager", "Tag assignment manager", true, "TagAssignmentManager");
		I_ASSIGN(m_tagCollectionCompPtr, "TagCollection", "Tag catalog", true, "TagCollection");
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::V1_0::imttag::CTagsGqlHandlerCompBase)
	virtual sdl::V1_0::imttag::CEntityTagsPayload OnEntityTagsGet(
				const sdl::V1_0::imttag::CEntityTagsGetGqlRequest& entityTagsGetRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imttag::CEntityTagsChangedPayload OnEntityTagsAdd(
				const sdl::V1_0::imttag::CEntityTagsAddGqlRequest& entityTagsAddRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imttag::CEntityTagsChangedPayload OnEntityTagRemove(
				const sdl::V1_0::imttag::CEntityTagRemoveGqlRequest& entityTagRemoveRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

private:
	enum ChangeMode
	{
		CM_ADD,
		CM_REMOVE
	};

	sdl::V1_0::imttag::CEntityTagsChangedPayload ChangeTags(
				ChangeMode mode,
				const sdl::V1_0::imttag::CEntityTagsChangeInput& input,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const;
	static sdl::V1_0::imttag::CEntityTagsChangedPayload CreateChangedPayload(
				const QString& entityType,
				const imttag::ITagAssignmentManager::EntityTagChanges& changes);

	I_REF(imttag::ITagAssignmentManager, m_assignmentManagerCompPtr);
	I_REF(imtbase::IObjectCollection, m_tagCollectionCompPtr);
};


} // namespace imttaggql


