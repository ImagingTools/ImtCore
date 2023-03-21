#pragma once


// ACF includes
#include <iser/IObject.h>
#include <ifile/IFileNameParam.h>


namespace imtapp
{


class IBackupSettings: virtual public iser::IObject
{
public:
	virtual ifile::IFileNameParam GetBackupFolderPath() const = 0;
	virtual QString GetBackupStartTime() const = 0;
};


} // namespace imtapp


