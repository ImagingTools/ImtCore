#pragma once


// ACF includes
#include <ibase/TModelObserverCompWrap.h>
#include <icomp/TMakeComponentWrap.h>
#include <icomp/TModelCompWrap.h>

// ImtCore includes
#include <imtbase/TComponentFactoryComp.h>
#include <imtservice/CConnectionCollectionComp.h>


/**
	ImtServicePck package
*/
namespace ImtServicePck
{


typedef imtbase::TComponentFactoryComp<imtservice::IConnectionCollection> ConnectionCollectionFactory;
typedef imtservice::CConnectionCollectionComp ConnectionCollection;


} // namespace ImtServicePck


