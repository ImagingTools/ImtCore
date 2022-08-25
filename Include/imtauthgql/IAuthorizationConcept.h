#pragma once


// ACF includes
#include <iser/IObject.h>


namespace imtauthgql
{



/**
	Interface authorization concept.
	\ingroup Authorization
*/
class IAuthorizationConcept: virtual public iser::IObject
{
public:
	enum
	{
		NO_USER_MANAGEMENT = 0,
		OPTIONAL_USER_MANAGEMENT = 1,
		STRONG_USER_MANAGEMENT = 2


	};

	virtual int GetUserMode() const = 0;
};


} // namespace imtauthgql


