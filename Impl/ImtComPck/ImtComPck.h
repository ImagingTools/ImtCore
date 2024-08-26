#pragma once


// ACF includes
#include <ibase/TModelObserverCompWrap.h>
#include <icomp/TMakeComponentWrap.h>
#include <icomp/TModelCompWrap.h>

// ImtCore includes
#include <imtcom/CInternetConnectionCheckerComp.h>
#include <imtcom/CSimpleConnectionCheckerComp.h>
#include <imtcom/CAsyncConnectionCheckerComp.h>
#include <imtcom/CSslConfigurationComp.h>
#include <imtcom/CSslKeyComp.h>
#include <imtcom/CSslCertificateComp.h>
#include <imtcom/CSslEncodingFormatOptionList.h>
#include <imtcom/CSslKeyAlgorithmOptionList.h>
#include <imtcom/CSslKeyTypeOptionList.h>
#include <imtcom/CSslProtocolOptionList.h>
#include <imtcom/CSslVerifyModeOptionList.h>


/**
	ImtGuiPck package
*/
namespace ImtComPck
{


typedef icomp::TModelCompWrap<imtcom::CInternetConnectionCheckerComp> InternetConnectionChecker;
typedef imtcom::CSimpleConnectionCheckerComp SimpleConnectionChecker;
typedef icomp::TModelCompWrap<imtcom::CAsyncConnectionCheckerComp> AsyncConnectionChecker;
typedef icomp::TModelCompWrap<imtcom::CSslConfigurationComp> SslConfiguration;
typedef icomp::TModelCompWrap<imtcom::CSslKeyComp> SslKey;
typedef icomp::TModelCompWrap<imtcom::CSslCertificateComp> SslCertificate;
typedef icomp::TModelCompWrap<
	icomp::TMakeComponentWrap<
		imtcom::CSslEncodingFormatOptionList,
		iprm::IOptionsManager, iprm::IOptionsList, iprm::ISelectionParam>> SslEncodingFormatOptionList;
typedef icomp::TModelCompWrap<
	icomp::TMakeComponentWrap<
		imtcom::CSslKeyAlgorithmOptionList,
		iprm::IOptionsManager, iprm::IOptionsList, iprm::ISelectionParam>> SslKeyAlgorithmOptionList;
typedef icomp::TModelCompWrap<
	icomp::TMakeComponentWrap<
		imtcom::CSslKeyTypeOptionList,
		iprm::IOptionsManager, iprm::IOptionsList, iprm::ISelectionParam>> SslKeyTypeOptionList;
typedef icomp::TModelCompWrap<
	icomp::TMakeComponentWrap<
		imtcom::CSslProtocolOptionList,
		iprm::IOptionsManager, iprm::IOptionsList, iprm::ISelectionParam>> SslProtocolOptionList;
typedef icomp::TModelCompWrap<
	icomp::TMakeComponentWrap<
		imtcom::CSslVerifyModeOptionList,
		iprm::IOptionsManager, iprm::IOptionsList, iprm::ISelectionParam>> SslVerifyModeOptionList;


} // namespace ImtComPck


