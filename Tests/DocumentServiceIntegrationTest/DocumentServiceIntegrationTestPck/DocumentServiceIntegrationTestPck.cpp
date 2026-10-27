// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "DocumentServiceIntegrationTestPck.h"


// ACF includes
#include <icomp/export.h>


namespace DocumentServiceIntegrationTestPck
{


I_EXPORT_PACKAGE(
			"DocumentServiceIntegrationTestPck",
			"Test components for the collection document service integration test",
			IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools"));

I_EXPORT_COMPONENT(
			TextDocumentValidator,
			"Validator of text parameter documents",
			"Document Validator Text Test");

I_EXPORT_COMPONENT(
			TextDocumentNameProvider,
			"Name provider of text parameter documents",
			"Document Name Provider Text Test");


} // namespace DocumentServiceIntegrationTestPck
