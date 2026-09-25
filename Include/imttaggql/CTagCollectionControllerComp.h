// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/TDelPtr.h>

// ImtCore includes
#include <imtbase/IOperationContext.h>
#include <imttag/ITag.h>
#include <GeneratedFiles/imttagsdl/SDL/1.0/CPP/Tags_fwd.h>


namespace imttaggql
{


/**
	GraphQL controller of the tag catalog.

	A tenant sees its own tags and the system tags. Names are unique, case-insensitively,
	within the system tags plus the tags of one tenant. System tags can be created,
	changed, deleted and restored by the superuser only; the system flag cannot change.
*/
class CTagCollectionControllerComp:
			public sdl::V1_0::imttag::CTagCollectionControllerCompBase
{
public:
	typedef sdl::V1_0::imttag::CTagCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CTagCollectionControllerComp);
		I_ASSIGN(m_tagFactCompPtr, "TagFactory", "Factory of tags", true, "TagFactory");
	I_END_COMPONENT;

protected:
	bool FillTagFromRepresentation(const sdl::V1_0::imttag::CTagData& representation, imttag::ITag& tag, QString& errorMessage) const;
	bool IsTagVisible(const QByteArray& tagId, const imtgql::CGqlRequest& gqlRequest, bool includeDeleted = false) const;
	void InitOperationContextForTag(const imttag::ITag& tag, const QByteArray& tagId, const QString& operation, const imtgql::CGqlRequest& gqlRequest, istd::TDelPtr<imtbase::IOperationContext>& operationContextPtr) const;
	bool IsNameAvailable(
				const QString& name,
				bool isSystem,
				const QByteArray& excludedTagId,
				const imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const;
	bool CheckWriteAccess(const imttag::ITag& tag, const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
	imtbase::IObjectCollection::DataPtr GetTagData(const QByteArray& tagId) const;

	// reimplemented (imtservergql::CObjectCollectionControllerCompBase)
	virtual QJsonObject InsertObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual QJsonObject UpdateObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual bool OnBeforeRemoveElements(const QByteArrayList& elementIds, const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual bool OnBeforeSetObjectName(const QByteArray& objectId, QString& newName, const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual sdl::V1_0::imtbase::CRestoreObjectsPayload OnRestoreObjects(
				const sdl::V1_0::imtbase::CRestoreObjectsGqlRequest& restoreObjectsRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtbase::CRemoveElementSetPayload OnRemoveElementSet(
				const sdl::V1_0::imtbase::CRemoveElementSetGqlRequest& removeElementSetRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtbase::CRestoreObjectSetPayload OnRestoreObjectSet(
				const sdl::V1_0::imtbase::CRestoreObjectSetGqlRequest& restoreObjectSetRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

	// reimplemented (sdl::V1_0::imttag::CTagCollectionControllerCompBase)
	virtual istd::IChangeableUniquePtr CreateObjectFromRepresentation(
				const sdl::V1_0::imttag::CTagData& tagDataRepresentation,
				QByteArray& newObjectId,
				QString& errorMessage) const override;
	virtual bool UpdateObjectFromRepresentationRequest(
				const ::imtgql::CGqlRequest& rawGqlRequest,
				const sdl::V1_0::imttag::CTagUpdateGqlRequest& tagUpdateRequest,
				istd::IChangeable& object,
				QString& errorMessage) const override;

private:
	I_FACT(imttag::ITag, m_tagFactCompPtr);
};


} // namespace imttaggql


