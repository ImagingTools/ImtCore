#pragma once


// ACF includes
#include <istd/IChangeable.h>


namespace imtbase
{
	class ICollectionInfo;
}


namespace imtauth
{


class IPersonInfo;


/**
	Provider of persons.
	\ingroup Authentification
*/
class IPersonInfoProvider: virtual public istd::IChangeable
{
public:
	virtual const imtbase::ICollectionInfo& GetPersonList() const = 0;
	virtual const IPersonInfo* GetPerson(const QByteArray& personId) const = 0;
};


} // namespace imtauth


