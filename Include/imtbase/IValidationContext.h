#pragma once


// ACF includes
#include <istd/IChangeable.h>


namespace imtbase
{


class IDataValidator;


/**
	Common interface for a validation context.
*/
class IValidationContext: virtual public istd::IPolymorphic
{
public:
	/**
		Get ID of the context.
	*/
	virtual QByteArray GetContextId() const = 0;

	/**
		Get access to the parent context.
	*/
	virtual const IValidationContext* GetParentContext() const = 0;

	/**
		The context can provide a special validator for the context-related stuff.
		In this case you can get access to the validator instance using this method.
	*/
	virtual const IDataValidator* GetContextValidator(const istd::IChangeable* dataPtr) const = 0;
};


} // namespace imtbase


