#pragma once


// ACF includes
#include <icomp/TModelCompWrap.h>
#include <icomp/TMakeComponentWrap.h>


// ImtAlgo includes
#include <imt3dalgo/CMeshSupplierComp.h>
#include <imt3dalgo/CHeadExtractorSupplierComp.h>
#include <imt3dalgo/CForeheadExtractorSupplierComp.h>

/**
	ImtAlgoPck package
*/
namespace ImtAlgoPck
{
	typedef icomp::TModelCompWrap<imt3dalgo::CMeshSupplierComp> MeshSupplier;
	typedef icomp::TModelCompWrap<imt3dalgo::CHeadExtractorSupplierComp> HeadSupplier;
	typedef icomp::TModelCompWrap<imt3dalgo::CForeheadExtractorSupplierComp> ForeheadPoiSupplier;

} // namespace ImtAlgoPck



