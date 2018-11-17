#pragma once


// ACF includes
#include <icomp/TModelCompWrap.h>
#include <icomp/TMakeComponentWrap.h>

// ImtAlgo includes
#include <imt3proc/CMeshSupplierComp.h>
#include <imt3proc/CHeadExtractorSupplierComp.h>
#include <imt3proc/CForeheadExtractorSupplierComp.h>


/**
	ImtAlgoPck package
*/
namespace ImtAlgoPck
{


typedef icomp::TModelCompWrap<imt3proc::CMeshSupplierComp> MeshSupplier;
typedef icomp::TModelCompWrap<imt3proc::CHeadExtractorSupplierComp> HeadSupplier;
typedef icomp::TModelCompWrap<imt3proc::CForeheadExtractorSupplierComp> ForeheadPoiSupplier;


} // namespace ImtAlgoPck

