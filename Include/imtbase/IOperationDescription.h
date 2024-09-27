#pragma once


// ACF includes
#include <iser/ISerializable.h>


namespace imtbase
{


class IOperationDescription: virtual public iser::ISerializable
{
public:
	enum OperationType
	{
		OT_UNKNOWN,
		OT_CREATE,
		OT_UPDATE,
		OT_REMOVE,
		OT_SET,
		OT_ADD,
		OT_CLEAR,
		OT_CHANGE,
		OT_USER = 1024
	};

	I_DECLARE_ENUM(OperationType, OT_UNKNOWN, OT_CREATE, OT_UPDATE, OT_REMOVE, OT_SET, OT_ADD, OT_CLEAR, OT_CHANGE, OT_USER);

	virtual int GetOperationType() const = 0;
	virtual QByteArray GetKey() const = 0;
	virtual QString GetKeyName() const = 0;
	virtual QByteArray GetOldValue() const = 0;
	virtual QByteArray GetNewValue() const = 0;
};


} // namespace imtbase


