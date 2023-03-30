#pragma once


// ACF includes
#include <iser/IObject.h>
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


