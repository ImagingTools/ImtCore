// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/TModelCompWrap.h>

// Local includes
#include "CRevisionObjectCollectionComp.h"
#include "CTextDocumentValidatorComp.h"
#include "CTextDocumentNameProviderComp.h"


/**
	Test components for DocumentServiceIntegrationTest.
*/
namespace DocumentServiceIntegrationTestPck
{


typedef CTextDocumentValidatorComp TextDocumentValidator;
typedef CTextDocumentNameProviderComp TextDocumentNameProvider;
typedef icomp::TModelCompWrap<CRevisionObjectCollectionComp> RevisionObjectCollection;


} // namespace DocumentServiceIntegrationTestPck
