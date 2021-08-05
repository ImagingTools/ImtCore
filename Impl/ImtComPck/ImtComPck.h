#pragma once


// ACF includes
#include <ibase/TModelObserverCompWrap.h>
#include <icomp/TMakeComponentWrap.h>
#include <icomp/TModelCompWrap.h>

// ImtCore includes
#include <imtcom/CInternetConnectionCheckerComp.h>


/**
	ImtGuiPck package
*/
namespace ImtComPck
{


typedef icomp::TModelCompWrap<imtcom::CInternetConnectionCheckerComp> InternetConnectionChecker;


} // namespace ImtComPck


