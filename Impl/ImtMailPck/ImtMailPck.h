#pragma once


// ACF includes
#include <icomp/TModelCompWrap.h>
#include <icomp/TMakeComponentWrap.h>

// ImtCore includes
#include <imtmail/CSmtpClientComp.h>
#include <imtmail/CSmtpMessage.h>
#include <imtmail/CSmtpMessageCreatorComp.h>


/**
	ImtMailPck package
*/
namespace ImtMailPck
{


typedef imtmail::CSmtpClientComp SmtpClient;
typedef icomp::TModelCompWrap<
			icomp::TMakeComponentWrap<
				imtmail::CSmtpMessage,
				imtmail::ISmtpMessage,
				iser::ISerializable>> SmtpMessage;
typedef imtmail::CSmtpMessageCreatorComp SmtpMessageCreator;


} // namespace ImtMailPck


