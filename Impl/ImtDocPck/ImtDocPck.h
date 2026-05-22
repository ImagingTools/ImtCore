// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/TMakeComponentWrap.h>
#include <icomp/TModelCompWrap.h>

// ImtCore includes
#include <imtdoc/CCollectionDocumentServiceComp.h>
#include <imtdoc/CDocumentServiceEventHandlerDemultiplexerComp.h>


/**
ImtDocPck package
*/
namespace ImtDocPck
{


typedef icomp::TModelCompWrap<imtdoc::CCollectionDocumentServiceComp> CollectionDocumentService;
typedef imtdoc::CDocumentServiceEventHandlerDemultiplexerComp DocumentServiceEventHandlerDemultiplexer;


} // namespace ImtDocPck


