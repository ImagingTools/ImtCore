#pragma once


// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QString>
#include <QtCore/QVector>
#include <QtCore/QVariant>

// ACF includes
#include <ibase/ICommandsProvider.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>


namespace imtgui
{


/**
	Interface used for realization the business logic of a collection view and commands related to the items in the collection.
	\ingroup Collection
*/
class ICollectionViewDelegate: virtual public ibase::ICommandsProvider
{
public:
	/**
		Type of the command for operations on a collection item or on the entire collection.
	*/
	enum CommandType
	{
		/**
			Neues object will be created in the collection.
		*/
		CT_NEW,

		/**
			An existing object will be imported from an external source.
		*/
		CT_IMPORT,

		/**
			An existing object will be exported from the collection to an external source.
		*/
		CT_EXPORT,

		/**
			An existing object will be removed from the collection.
		*/
		CT_REMOVE,

		/**
			The contents of an existing object should be edited.
		*/
		CT_EDIT,

		/**
			The name of the object will be changed.
		*/
		CT_RENAME,

		/**
			Base index for used-defined operations.
		*/
		CT_USER = 1000
	};

	enum ViewState
	{
		/**
			No state.
		*/
		VS_NONE = 0,

		/**
			Something is selected.
		*/
		VS_SELECTED = 1,

		/**
			Single instance is selected.
		*/
		VS_SINGLE_SELECTION = 2,

		/**
			Object is enabled and can be processed.
		*/
		VS_ENABLED = 4,

		/**
			An editor for the object is available.
		*/
		VS_EDITOR_AVAILABLE = 8
	};

	/**
		Information about an object from the collection used for editing.
	*/
	struct ObjectInfo
	{
		ObjectInfo()
			:objectPtr(nullptr)
		{
		}

		istd::IChangeable* objectPtr;
		QByteArray uuid;
		QString name;
		QString description;
		QByteArray typeId;
		QString typeName;
		QString tempFilePath;
	};

	virtual bool InitializeDelegate(imtbase::IObjectCollection* collectionPtr, iqtgui::IGuiObject* parentGuiPtr) = 0;

	/**
		Get the type-IDs supported by the delegate implementation.
		The delegate can only work with objects of supported type.
	*/
	virtual QByteArray GetSupportedTypeId() const = 0;

	/**
		Update the commands state according to the state of the collection and the current state of the collection view.
	*/
	virtual void UpdateItemSelection(
				int viewStateFlags,
				const imtbase::ICollectionInfo::Ids& selectedItems,
				const QByteArray& selectedTypeId) = 0;

	/**
		Create a new object in the collection.
	*/
	virtual QByteArray CreateNewObject(const QByteArray& typeId) const = 0;

	/**
		Import an object from an external source into the collection.
	*/
	virtual QByteArray ImportObject(const QByteArray& typeId, const QString& sourcePath = QString()) const = 0;

	/**
		Export an existing object from the collection to an external target.
	*/
	virtual bool ExportObject(const QByteArray& objectId, const QString& targetPath = QString()) const = 0;

	/**
		Get the list of type-IDs supported by the delegate implementation.
		The delegate can only work with the supported objects.
	*/
	virtual void RemoveObjects(const imtbase::ICollectionInfo::Ids& objectIds)  const = 0;

	/**
		Update the contents of an existing object.
	*/
	virtual bool UpdateObject(const QByteArray& objectId, const istd::IChangeable& contents) const = 0;

	/**
		Rename an existing object.
	*/
	virtual bool RenameObject(const QByteArray& objectId, const QString& newName) const = 0;
};


} // namespace imtgui


