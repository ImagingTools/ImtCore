#pragma once


// ImtCore includes
#include <imtbase/IComplexCollectionFilter.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/ComplexCollectionFilter.h>


namespace imtbasesdl
{


bool ComplexCollectionFilterRepresentationToModel(
	sdl::imtbase::ComplexCollectionFilter::V1_0::CComplexCollectionFilter& filterRepresentaion,
	imtbase::IComplexCollectionFilter& filter);


} // namespace imtgql


