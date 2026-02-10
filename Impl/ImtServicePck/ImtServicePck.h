// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ibase/TModelObserverCompWrap.h>
#include <icomp/TMakeComponentWrap.h>
#include <icomp/TModelCompWrap.h>

// ImtCore includes
#include <imtbase/TComponentFactoryComp.h>
#include <imtbase/IObjectCollection.h>
#include <imtservice/CConnectionCollectionComp.h>
#include <imtservice/CUrlConnectionParamComp.h>


/**
	ImtServicePck package
*/
namespace ImtServicePck
{


typedef imtbase::TComponentFactoryComp<imtbase::IObjectCollection> ObjectCollectionFactory;
typedef imtbase::TComponentFactoryComp<imtservice::IConnectionCollection> ConnectionCollectionFactory;
typedef imod::TModelWrap<imtservice::CConnectionCollectionComp> ConnectionCollection;
typedef imtservice::CUrlConnectionParamComp UrlConnectionParamInfo;


} // namespace ImtServicePck


