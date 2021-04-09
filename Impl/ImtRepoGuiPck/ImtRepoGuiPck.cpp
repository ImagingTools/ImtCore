#include "ImtRepoGuiPck.h"


// ACF includes
#include <icomp/export.h>


I_REGISTER_QT_RESOURCE(imtguilight);


namespace ImtRepoGuiPck
{


I_EXPORT_PACKAGE(
			"ImtGuiPck",
			"ImagingTools core framework UI-component package",
			IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools"));

I_EXPORT_COMPONENT(
			FileObjectCollectionViewDelegate,
			"View delegate for a file-based collection",
			"Object Collection File Delegate View");

I_EXPORT_COMPONENT(
			DocumentBasedFileCollectionViewDelegate,
			"View delegate for a file collection using document manager for the data operations",
			"Object Collection File Delegate View Document MDI");


} // namespace ImtRepoGuiPck


