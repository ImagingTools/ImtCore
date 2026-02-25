// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CClientRequestManagerCompBase.h>


// ImtCore includes
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/ImtCollection.h>


namespace imtauthgql
{


QByteArrayList CClientRequestManagerCompBase::GetElementIds(const QByteArray& collectionId) const
{
	namespace imtcollection = sdl::imtbase::ImtCollection;

	imtcollection::GetElementIdsRequestArguments arguments;
	arguments.input.Version_1_0.Emplace();
	arguments.input.Version_1_0->collectionId = collectionId;

	imtcollection::CGetElementIdsPayload payload;
	bool ok = SendModelRequestInternal<imtcollection::GetElementIdsRequestArguments, imtcollection::CGetElementIdsPayload, imtcollection::CGetElementIdsGqlRequest>(arguments, payload);
	if (!ok){
		return QByteArrayList();
	}

	if (!payload.Version_1_0->elementIds.HasValue()){
		return QByteArrayList();
	}

	return payload.Version_1_0->elementIds->ToList();
}


bool CClientRequestManagerCompBase::RemoveElements(const QByteArray& collectionId, const QByteArrayList& elementIds) const
{
	namespace imtcollection = sdl::imtbase::ImtCollection;

	imtcollection::RemoveElementsRequestArguments arguments;
	arguments.input.Version_1_0.Emplace();
	arguments.input.Version_1_0->elementIds.Emplace();
	arguments.input.Version_1_0->elementIds->FromList(elementIds);
	arguments.input.Version_1_0->collectionId = collectionId;

	imtcollection::CRemoveElementsPayload payload;
	bool ok = SendModelRequestInternal<imtcollection::RemoveElementsRequestArguments, imtcollection::CRemoveElementsPayload, imtcollection::CRemoveElementsGqlRequest>(arguments, payload);
	if (!ok){
		return false;
	}

	if (!payload.Version_1_0->success.HasValue()){
		return false;
	}

	return *payload.Version_1_0->success;
}


} // namespace imtauthgql


