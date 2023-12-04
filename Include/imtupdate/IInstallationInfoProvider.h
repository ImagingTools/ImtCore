#pragma once


// Acf includes
#include <istd/IPolymorphic.h>

// imtupdate includes
#include<imtupdate/IInstallationInfo.h>


namespace imtupdate
{


class IInstallationInfoProvider: virtual public istd::IPolymorphic
{
public:
	virtual const IInstallationInfo* GetInstallationInfo() const = 0;
};


} // namespace imtupdate

