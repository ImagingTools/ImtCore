#include <imtgql/CGqlDemultiplexerComp.h>

namespace imtgql
{

QByteArrayList CGqlDemultiplexerComp::GetModelIds() const
{
	QByteArrayList modelIds;
	int dataControllersCount = m_gqlRepresentationDataControllerCompPtr.GetCount();
	for (int index = 0; index < dataControllersCount; index++){
		const imtgql::IGqlRepresentationDataController* representationControllerPtr = m_gqlRepresentationDataControllerCompPtr[index];
		if (representationControllerPtr != nullptr){
			modelIds.append(representationControllerPtr->GetModelIds());
		}
	}
	return modelIds;
}


QByteArrayList CGqlDemultiplexerComp::GetContextIds() const
{
	QByteArrayList contextIds;
	int dataControllersCount = m_gqlRepresentationDataControllerCompPtr.GetCount();
	for (int index = 0; index < dataControllersCount; index++){
		const imtgql::IGqlRepresentationDataController* representationControllerPtr = m_gqlRepresentationDataControllerCompPtr[index];
		if (representationControllerPtr != nullptr){
			contextIds.append(representationControllerPtr->GetContextIds());
		}
	}
	return contextIds;
}


imtbase::CTreeItemModel *CGqlDemultiplexerComp::CreateResponse(const CGqlRequest &gqlRequest, QString &errorMessage) const
{
	imtbase::CTreeItemModel* sourceItemModel = nullptr;
	QByteArray gqlCommand = gqlRequest.GetCommandId();
	QByteArrayList modelIds = GetModelIds();
	int dataControllersCount = m_gqlRepresentationDataControllerCompPtr.GetCount();
	if (modelIds.contains(gqlCommand)){
		for (int index = 0; index < dataControllersCount; index++){
			const imtgql::IGqlRepresentationDataController* representationControllerPtr = m_gqlRepresentationDataControllerCompPtr[index];
			if (representationControllerPtr != nullptr){
				QByteArrayList modelItemIds = representationControllerPtr->GetModelIds();
				if (modelItemIds.contains(gqlCommand)){
					sourceItemModel = representationControllerPtr->CreateResponse(gqlRequest, errorMessage);

					if (sourceItemModel != nullptr){
						return sourceItemModel;
					}
				}
			}
		}
	}

	return sourceItemModel;
}

}// namespace imtgql



