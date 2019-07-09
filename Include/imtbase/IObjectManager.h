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
			Active if rename of objects is supported.
		*/
		OF_SUPPORT_RENAME = 4,

		/**
			Active if insert of objects is possible.
		*/
		OF_SUPPORT_INSERT = 8,

		/**
			Active if delete of objects is possible.
		*/
		OF_SUPPORT_DELETE = 16,

		/**
			Active if swap of objects with the other one is possible.
			Make sense only for ordered object lists.
		*/
		OF_SUPPORT_SWAP = 32,

		/**
			Active if it is possible to get the disabled option.
		*/
		OF_DISABLE_ALLOWED = 64,

		/**
			Active if it is possible to edit parameter values.
		*/
		OF_SUPPORT_EDIT = 128,

		/**
			Option is inactive (i.e. cannot be activated or selected) 
		*/
		OF_INACTIVE = 256
	};

	I_DECLARE_FLAGS(OperationalFlags, OF_DEFAULT, OF_FIXED, OF_SUPPORT_RENAME, OF_SUPPORT_INSERT, OF_SUPPORT_DELETE, OF_SUPPORT_SWAP, OF_DISABLE_ALLOWED, OF_SUPPORT_EDIT, OF_INACTIVE);

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
				const QString& description,
				const istd::IChangeable* defaultValuePtr = nullptr) = 0;

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


