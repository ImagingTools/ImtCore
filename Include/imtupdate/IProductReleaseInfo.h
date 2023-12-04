#pragma once


// Acf includes
#include <iser/ISerializable.h>


namespace imtupdate
{


class IProductReleaseInfo: virtual public iser::ISerializable
{


public:
	virtual const iser::IVersionInfo& GetProductVersion() const = 0;
	virtual QDateTime GetReleaseDate() const = 0;
	virtual QString GetChangeNotes() const = 0;
};


}//namespace imtupdate

