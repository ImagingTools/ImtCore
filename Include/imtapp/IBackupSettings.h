#pragma once


// ACF includes
#include <iser/IObject.h>
#include <ifile/IFileNameParam.h>

// ImtCore includes
#include <imtapp/IScheduler.h>


namespace imtapp
{


class IBackupSettings: virtual public iser::IObject,
						virtual public imtapp::IScheduler,
						virtual public ifile::IFileNameParam
{

};


} // namespace imtapp


