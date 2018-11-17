#pragma once


// ACF includes
#include <icomp/TModelCompWrap.h>
#include <icomp/TMakeComponentWrap.h>

// ImtAlgo includes
#include <imt3proc/CMeshSupplierComp.h>


/**
	ImtProc3dPck package
*/
namespace ImtProc3dPck
{


typedef icomp::TModelCompWrap<imt3proc::CMeshSupplierComp> MeshSupplier;


} // namespace ImtProc3dPck


