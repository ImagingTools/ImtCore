// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ibase/TModelObserverCompWrap.h>
#include <icomp/TMakeComponentWrap.h>
#include <icomp/TModelCompWrap.h>

// ImtCore includes
#include <imtcom/CInternetConnectionCheckerComp.h>
#include <imtcom/CSimpleConnectionCheckerComp.h>
#include <imtcom/CAsyncConnectionCheckerComp.h>
#include <imtcom/CSslConfigurationManagerComp.h>
#include <imtcom/CSslEncodingFormatOptionList.h>
#include <imtcom/CSslKeyAlgorithmOptionList.h>
#include <imtcom/CSslKeyTypeOptionList.h>
#include <imtcom/CSslProtocolOptionList.h>
#include <imtcom/CSslVerifyModeOptionList.h>
#include <imtcom/CServerConnectionInterfaceParamComp.h>
#include <imtcom/CServerDispatcherComp.h>
#include <imtcom/CSslConfigurationAdapterComp.h>


/**
	ImtComPck package
*/
namespace ImtComPck
{


typedef icomp::TModelCompWrap<imtcom::CInternetConnectionCheckerComp> InternetConnectionChecker;
typedef imtcom::CSimpleConnectionCheckerComp SimpleConnectionChecker;
typedef icomp::TModelCompWrap<imtcom::CAsyncConnectionCheckerComp> AsyncConnectionChecker;
typedef imtcom::CSslConfigurationManagerComp SslConfigurationManager;
typedef icomp::TModelCompWrap<
	icomp::TMakeComponentWrap<
		imtcom::CSslEncodingFormatOptionList,
		iser::ISerializable, iprm::IOptionsManager, iprm::IOptionsList, iprm::ISelectionParam>> SslEncodingFormatOptionList;
typedef icomp::TModelCompWrap<
	icomp::TMakeComponentWrap<
		imtcom::CSslKeyAlgorithmOptionList,
		iser::ISerializable, iprm::IOptionsManager, iprm::IOptionsList, iprm::ISelectionParam>> SslKeyAlgorithmOptionList;
typedef icomp::TModelCompWrap<
	icomp::TMakeComponentWrap<
		imtcom::CSslKeyTypeOptionList,
		iser::ISerializable, iprm::IOptionsManager, iprm::IOptionsList, iprm::ISelectionParam>> SslKeyTypeOptionList;
typedef icomp::TModelCompWrap<
	icomp::TMakeComponentWrap<
		imtcom::CSslProtocolOptionList,
		iser::ISerializable, iprm::IOptionsManager, iprm::IOptionsList, iprm::ISelectionParam>> SslProtocolOptionList;
typedef icomp::TModelCompWrap<
	icomp::TMakeComponentWrap<
		imtcom::CSslVerifyModeOptionList,
		iser::ISerializable, iprm::IOptionsManager, iprm::IOptionsList, iprm::ISelectionParam>> SslVerifyModeOptionList;
typedef icomp::TModelCompWrap<imtcom::CServerConnectionInterfaceParamComp> ServerConnectionInterfaceParam;
typedef imtcom::CServerDispatcherComp ServerController;
typedef imtcom::CSslConfigurationAdapterComp SslConfigurationAdapter;


} // namespace ImtComPck


