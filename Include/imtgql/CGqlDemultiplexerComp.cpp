#include <imtgql/CGqlDemultiplexerComp.h>


namespace imtgql
{


// reimplemented (imtgql::IGqlRepresentationDataController)

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
	QByteArray gqlCommand = gqlRequest.GetCommandId();
	QByteArrayList modelIds = GetModelIds();
	int dataControllersCount = m_gqlRepresentationDataControllerCompPtr.GetCount();
	if (modelIds.contains(gqlCommand)){
		for (int index = 0; index < dataControllersCount; index++){
			const imtgql::IGqlRepresentationDataController* representationControllerPtr = m_gqlRepresentationDataControllerCompPtr[index];
			if (representationControllerPtr != nullptr){
				QByteArrayList modelItemIds = representationControllerPtr->GetModelIds();
				if (modelItemIds.contains(gqlCommand)){
					imtbase::CTreeItemModel* sourceItemModelPtr = representationControllerPtr->CreateResponse(gqlRequest, errorMessage);
					if (sourceItemModelPtr != nullptr){
						return sourceItemModelPtr;
					}
				}
			}
		}
	}

	return nullptr;
}


} // namespace imtgql


