#pragma once


// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QString>
#include <QtCore/QVector>
#include <QtCore/QVariant>

// ACF includes
#include <ibase/ICommandsProvider.h>
#include <iqtgui/IVisualStatus.h>
#include <iqtgui/IGuiObject.h>

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
	enum ViewState
	{
		/**
			No flags.
		*/
		VS_NONE = 0,

		/**
			Collection item is enabled and can be processed.
		*/
		VS_ENABLED = 1,

		/**
			An editor for the object is available.
		*/
		VS_EDITOR_AVAILABLE = 2
	};

	enum CommandIdentifier
	{
		CI_USER = 1000
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
	};

	struct HeaderInfo
	{
		HeaderInfo(bool setIsFixed = false, quint16 setMinWidth = 0, quint16 setMaxWidth = 0xFFFF)
			: isFixed(setIsFixed),
			minWidth(setMinWidth),
			maxWidth(setMaxWidth)
		{
		}

		bool isFixed;
		quint16 minWidth;
		quint16 maxWidth;
	};
	
	struct SummaryInformation
	{
		QString text;
		QIcon icon;
		QVariant sortValue;
	};

	virtual bool InitializeDelegate(imtbase::IObjectCollection* collectionPtr, iqtgui::IGuiObject* parentGuiPtr) = 0;

	/**
		Get access to the visual status of the document type supported by this delegate.
	*/
	virtual const iqtgui::IVisualStatus& GetDocumentTypeStatus() const = 0;

	/**
		Get the type-IDs supported by the delegate implementation.
		The delegate can only work with objects of supported type.
	*/
	virtual QByteArray GetSupportedTypeId() const = 0;

	/**
		Update the commands state according to the state of the collection and the current state of the collection view.
	*/
	virtual void UpdateItemSelection(const imtbase::ICollectionInfo::Ids& selectedItems, const QByteArray& selectedTypeId) = 0;

	/**
		Create a new object in the collection.
	*/
	virtual QByteArray CreateNewObject(const QByteArray& typeId, const istd::IChangeable* defaultDataPtr = nullptr) const = 0;

	/**
		Import an object from an external source into the collection.
	*/
	virtual QByteArray ImportObject(const QByteArray& typeId, const QString& sourcePath = QString()) const = 0;

	/**
		Export an existing object from the collection to an external target.
	*/
	virtual bool ExportObject(const QByteArray& objectId, const QString& targetPath = QString()) const = 0;

	/**
		Remove objects with given IDs.
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

	/**
		Get the list of the summary informations for the document type managed by this delegate.
		Those informations will be shown on the collection view.
	*/
	virtual const imtbase::ICollectionInfo& GetSummaryInformationTypes() const = 0;

	/**
		Get summary information of a given type for a given object.
	*/
	virtual SummaryInformation GetSummaryInformation(const QByteArray& objectId, const QByteArray& informationId) const = 0;

	/**
		Get summary information header info of a given type for a given object.
	*/
	virtual HeaderInfo GetSummaryInformationHeaderInfo(const QByteArray& informationId) const = 0;

	/**
		Open a document editor for a given data object.
	*/
	virtual bool OpenDocumentEditor(const QByteArray& objectId, const QByteArray& viewTypeId = QByteArray()) const = 0;

	/**
		Get access to the information view.
		Information widget will show the information about currently selected item(s).
		\sa UpdateItemSelection
	*/
	virtual iqtgui::IGuiObject* GetInformationView() const = 0;

	/**
		By calling this method, the base class determines whether the command is supported by the child.
	*/
	virtual bool IsCommandSupported(int commandId) const = 0;
};


} // namespace imtgui


