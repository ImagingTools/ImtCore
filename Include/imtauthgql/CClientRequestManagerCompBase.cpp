// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CClientRequestManagerCompBase.h>


// ImtCore includes
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/ImtCollection.h>


namespace imtauthgql
{


QByteArrayList CClientRequestManagerCompBase::GetElementIds(const QByteArray& collectionId) const
{
	namespace imtcollection = sdl::V1_0::imtbase;

	imtcollection::GetElementIdsRequestArguments arguments;
	arguments.input.emplace();
	arguments.input->collectionId = collectionId;

	imtcollection::CGetElementIdsPayload payload;
	bool ok = SendModelRequestInternal<imtcollection::GetElementIdsRequestArguments, imtcollection::CGetElementIdsPayload, imtcollection::CGetElementIdsGqlRequest>(arguments, payload);
	if (!ok){
		return QByteArrayList();
	}

	if (!payload.elementIds.HasValue()){
		return QByteArrayList();
	}

	return payload.elementIds->ToList();
}


bool CClientRequestManagerCompBase::RemoveElements(const QByteArray& collectionId, const QByteArrayList& elementIds) const
{
	namespace imtcollection = sdl::V1_0::imtbase;

	imtcollection::RemoveElementsRequestArguments arguments;
	arguments.input.emplace();
	arguments.input->elementIds.Emplace();
	arguments.input->elementIds->FromList(elementIds);
	arguments.input->collectionId = collectionId;

	imtcollection::CRemoveElementsPayload payload;
	bool ok = SendModelRequestInternal<imtcollection::RemoveElementsRequestArguments, imtcollection::CRemoveElementsPayload, imtcollection::CRemoveElementsGqlRequest>(arguments, payload);
	if (!ok){
		return false;
	}

	if (!payload.success.HasValue()){
		return false;
	}

	return *payload.success;
}


} // namespace imtauthgql


