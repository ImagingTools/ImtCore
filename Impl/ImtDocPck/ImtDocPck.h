// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/TMakeComponentWrap.h>
#include <icomp/TModelCompWrap.h>

// ImtCore includes
#include <imtdoc/CCollectionDocumentManagerComp.h>
#include <imtdoc/CDocumentManagerEventHandlerDemultiplexerComp.h>


/**
	ImtDocPck package
*/
namespace ImtDocPck
{


typedef icomp::TModelCompWrap<imtdoc::CCollectionDocumentManagerComp> CollectionDocumentManager;
typedef imtdoc::CDocumentManagerEventHandlerDemultiplexerComp DocumentManagerEventHandlerDemultiplexer;


} // namespace ImtDocPck


