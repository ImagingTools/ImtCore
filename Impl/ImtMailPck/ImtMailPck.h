#pragma once


// ACF includes
#include <icomp/TModelCompWrap.h>
#include <icomp/TMakeComponentWrap.h>

// ImtCore includes
#include <imtmail/CSmtpMailSendRequest.h>
#include <imtmail/CMailEngineComp.h>
#include <imtmail/CMailSettingsComp.h>


/**
	ImtMailPck package
*/
namespace ImtMailPck
{


typedef imtmail::CMailEngineComp EmailEngine;
typedef icomp::TModelCompWrap<icomp::TMakeComponentWrap<imtmail::CSmtpMailSendRequest>> MailSmtpRequestFactory;
typedef icomp::TModelCompWrap<imtmail::CMailSettingsComp> MailSettings;


} // namespace ImtMailPck


