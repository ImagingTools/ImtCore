// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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


