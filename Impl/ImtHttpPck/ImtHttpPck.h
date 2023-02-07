#pragma once


// ACF includes
#include <icomp/TMakeComponentWrap.h>
#include <icomp/TModelCompWrap.h>

// ImtCore includes
#include <imthttp/CHttpFileTransfer.h>


namespace ImtHttpPck
{


typedef icomp::TMakeComponentWrap<imthttp::CHttpFileTransfer, imtcom::IFileTransfer> HttpFileTransfer;


} // namespace ImtHttpPck


