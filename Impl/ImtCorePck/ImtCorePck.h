#pragma once


// ACF includes
#include <icomp/TModelCompWrap.h>
#include <icomp/TMakeComponentWrap.h>


// ImtCore includes
#include <imtcore/CWorkflowManagerComp.h>


/**
	ImtCorePck package
*/
namespace ImtCorePck
{

typedef icomp::TModelCompWrap<imtcore::CWorkflowManagerComp> WorkflowManager;


} // namespace ImtCorePck



