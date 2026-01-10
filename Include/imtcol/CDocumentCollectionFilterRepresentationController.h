#pragma once


// ACF includes
#include <ilog/IMessageConsumer.h>

// ImtCore includes
#include <imtcol/IDocumentCollectionFilter.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/DocumentCollectionFilter.h>


namespace imtcol
{


class CDocumentCollectionFilterRepresentationController
{
public:
	static bool DocumentCollectionFilterRepresentationToModel(
				const sdl::imtbase::DocumentCollectionFilter::CDocumentCollectionFilter::V1_0& filterRepresentaion,
				IDocumentCollectionFilter& filterModel,
				ilog::IMessageConsumer* messageConsumerPtr);
};


} // namespace imtcol


