#include <imtcol/CDocumentCollectionFilterRepresentationController.h>


namespace imtcol
{


bool CDocumentCollectionFilterRepresentationController::DocumentCollectionFilterRepresentationToModel(
			const sdl::imtbase::DocumentCollectionFilter::CDocumentCollectionFilter::V1_0& filterRepresentaion,
			imtcol::IDocumentCollectionFilter& filterModel,
			ilog::IMessageConsumer* /*messageConsumerPtr*/)
{
	namespace Filter = sdl::imtbase::DocumentCollectionFilter;

	if (filterRepresentaion.documentId.has_value()){
		QByteArray documentId = *filterRepresentaion.documentId;
		filterModel.SetDocumentId(documentId);
	}

	if (filterRepresentaion.documentStates.has_value()){
		QList<Filter::DocumentState> documentStates = *filterRepresentaion.documentStates;
		for (const Filter::DocumentState& state : documentStates){
			switch (state){
			case Filter::DocumentState::ACTIVE:
				filterModel.AddDocumentState(imtcol::IDocumentCollectionFilter::DS_ACTIVE);
				break;
			case Filter::DocumentState::INACTIVE:
				filterModel.AddDocumentState(imtcol::IDocumentCollectionFilter::DS_INACTIVE);
			case Filter::DocumentState::DISABLED:
				filterModel.AddDocumentState(imtcol::IDocumentCollectionFilter::DS_DISABLED);
				break;
			}
		}
	}

	return true;
}


} // namespace imtcol


