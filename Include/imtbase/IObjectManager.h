#pragma once


// ImtCore includes
#include <imtbase/IObjectProvider.h>


namespace imtbase
{


class IObjectManager: virtual public IObjectProvider
{
public:

	enum OperationalFlags
	{
		OF_DEFAULT = 1,

		/**
			Object is fixed and cannot be removed.
		*/
		OF_FIXED = 2,

		/**
			Manager flag for allowing the adding of the new element.
		*/
		OF_MANAGER_ADD = 4
	};

	I_DECLARE_FLAGS(OperationalFlags, OF_DEFAULT, OF_FIXED, OF_MANAGER_ADD);

	/**
		Get binary flags describing the possible operations on the single object or on the whole manager/container.
	*/
	virtual int GetOperationFlags(const QByteArray& objectId = QByteArray()) const = 0;

	/**
		Create a new object in the container.
		\param typeId	Type-ID of the object to be created.
	*/
	virtual QByteArray InsertNewObject(
				const QByteArray& typeId,
				const QString& name,
				const QString& description) = 0;

	/**
		Get object with the given ID for the editing.
	*/
	virtual ObjectPtr GetEditableObject(const QByteArray& objectId) = 0;

	/**
		Remove element with the given ID.
	*/
	virtual bool RemoveObject(const QByteArray& objectId) = 0;

	/**
		Set name of the element with the given ID.
	*/
	virtual void SetObjectName(const QByteArray& objectId, const QString& objectName) = 0;

	/**
		Set description of the element with the given ID.
	*/
	virtual void SetObjectDescription(const QByteArray& objectId, const QString& objectDescription) = 0;

	/**
		Enable/Disable element with the given ID.
	*/
	virtual void SetObjectEnabled(const QByteArray& objectId, bool isEnabled = true) = 0;
};


} // namespace imtbase


