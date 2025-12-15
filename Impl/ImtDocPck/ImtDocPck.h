#pragma once


// ACF includes
#include <icomp/TMakeComponentWrap.h>
#include <icomp/TModelCompWrap.h>

// ImtCore includes
#include <imtdoc/CCollectionDocumentManagerComp.h>


/**
	ImtDocPck package
*/
namespace ImtDocPck
{


typedef icomp::TModelCompWrap<imtdoc::CCollectionDocumentManagerComp> CollectionDocumentManager;


} // namespace ImtDocPck


