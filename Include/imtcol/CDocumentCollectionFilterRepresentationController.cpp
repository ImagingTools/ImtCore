// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtcol/CDocumentCollectionFilterRepresentationController.h>


namespace imtcol
{


bool CDocumentCollectionFilterRepresentationController::DocumentCollectionFilterRepresentationToModel(
			const sdl::imtbase::DocumentCollectionFilter::CDocumentCollectionFilter::V1_0& filterRepresentaion,
			IDocumentCollectionFilter& filterModel,
			ilog::IMessageConsumer* /*messageConsumerPtr*/)
{
	namespace Filter = sdl::imtbase::DocumentCollectionFilter;

	if (filterRepresentaion.documentId.has_value()){
		QByteArray documentId = *filterRepresentaion.documentId;
		filterModel.SetDocumentId(documentId);
	}

	if (filterRepresentaion.documentStates.has_value()){
		imtsdl::TElementList<Filter::DocumentState> documentStates = *filterRepresentaion.documentStates;
		for (const istd::TSharedNullable<Filter::DocumentState>& state : documentStates){
			switch (*state){
			case Filter::DocumentState::ACTIVE:
				filterModel.AddDocumentState(IDocumentCollectionFilter::DS_ACTIVE);
				break;
			case Filter::DocumentState::INACTIVE:
				filterModel.AddDocumentState(IDocumentCollectionFilter::DS_INACTIVE);
			case Filter::DocumentState::DISABLED:
				filterModel.AddDocumentState(IDocumentCollectionFilter::DS_DISABLED);
				break;
			}
		}
	}

	return true;
}


} // namespace imtcol


