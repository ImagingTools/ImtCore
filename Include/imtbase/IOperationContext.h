#pragma once


// ImtCore includes
#include <istd/IChangeable.h>


namespace imtbase
{



/**
	Common interface for the context of an operation.
	This interface provide informations:
	1. Who/what has started the operation
	2. Human-readable description of the operation
*/
class IOperationContext: virtual public istd::IChangeable
{
public:
	struct IdentifableObjectInfo
	{
		QByteArray id;
		QString name;
	};

	/**
		Get the ID of the initiator of the operation. E.g user-ID
	*/
	virtual IdentifableObjectInfo GetOperationOwnerId() const = 0;

	/**
		Description related to the operation.
	*/
	virtual QString GetOperationDescription() const = 0;
};


} // namespace imtbase


