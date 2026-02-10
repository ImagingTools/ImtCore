// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ifile/IFileNameParam.h>

// ImtCore includes
#include <imtapp/ISchedulerParams.h>


namespace imtapp
{


class IBackupSettings: virtual public imtapp::ISchedulerParams,
						virtual public ifile::IFileNameParam
{

};


} // namespace imtapp


