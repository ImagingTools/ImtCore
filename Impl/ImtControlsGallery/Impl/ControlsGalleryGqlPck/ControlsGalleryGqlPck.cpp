#include "ControlsGalleryGqlPck.h"


// ACF includes
#include <icomp/export.h>


namespace ControlsGalleryGqlPck
{


I_EXPORT_PACKAGE(
		"ControlsGalleryGqlPck",
		"ControlsGallery management Gql-component package",
		IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools"));

I_EXPORT_COMPONENT(
		ContactInfoCollectionController,
		"Contact info collection controller",
		"Contact Info Collection Controller");

I_EXPORT_COMPONENT(
		ContactInfoMetaInfoDelegate,
		"Contact info meta info delegate",
		"ContactInfo Meta Info Delegate");

I_EXPORT_COMPONENT(
		ContactInfoCollectionDocumentManager,
		"Contact info collection document manager",
		"Contact Info Collection Document Manager");

I_EXPORT_COMPONENT(
		ParamsSetTestController,
		"Params set test controller",
		"ParamsSet Test Controller");


} // namespace ControlsGalleryGqlPck


