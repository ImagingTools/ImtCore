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
	virtual QByteArray GetContextId() const = 0;
	virtual const IValidationContext* GetParentContext() const = 0;
	virtual IDataValidator* GetContextValidator(const istd::IChangeable* dataPtr) const = 0;
};


} // namespace imtbase


