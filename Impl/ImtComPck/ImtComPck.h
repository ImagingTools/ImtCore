#pragma once


// ACF includes
#include <ibase/TModelObserverCompWrap.h>
#include <icomp/TMakeComponentWrap.h>
#include <icomp/TModelCompWrap.h>

// ImtCore includes
#include <imtcom/CInternetConnectionCheckerComp.h>
#include <imtcom/CSimpleConnectionCheckerComp.h>
#include <imtcom/CAsyncConnectionCheckerComp.h>


/**
	ImtGuiPck package
*/
namespace ImtComPck
{


typedef icomp::TModelCompWrap<imtcom::CInternetConnectionCheckerComp> InternetConnectionChecker;
typedef imtcom::CSimpleConnectionCheckerComp SimpleConnectionChecker;
typedef icomp::TModelCompWrap<imtcom::CAsyncConnectionCheckerComp> AsyncConnectionChecker;


} // namespace ImtComPck


