#pragma once


// ACF includes
#include <iser/IObject.h>
#include <iprm/IParamsSet.h>


namespace imtauth
{


class IUserSettings:
		virtual public iser::IObject
{
public:
	/**
		Get ID of the user.
	*/
	virtual QByteArray GetUserId() const = 0;

	/**
		Set ID of the user.
	*/
	virtual void SetUserId(const QByteArray& id) = 0;

	/**
		Get params set settings of the user .
	*/
	virtual iprm::IParamsSet* GetSettings() const = 0;

	/**
		Set params set settings of the user .
	*/
	virtual void SetSettings(const iprm::IParamsSet* paramsSet) = 0;
};


} // namespace imtauth


