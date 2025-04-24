#pragma once


// ACF includes
#include <istd/IPolymorphic.h>

// ImtCore includes
#include <imtsdl/imtsdl.h>
#include <imtsdl/CSdlEntryBase.h>
#include <imtsdl/ModuleInfo.h>



namespace imtsdl
{

/**
	\brief The IModuleManager class used for modules management
*/
class IModuleManager: virtual public istd::IPolymorphic
{
public:
	struct ItemInfo
	{
		QString TargerHeaderFilePath;
		QString QmlModuleName;
	};

public:
	/**
		\brief Sets \c paths for modules lookup
		\note the lookup is \b recursive
		\warning all previous data will be lost!
	*/
	virtual bool Initialize(const QStringList& paths) = 0;

	// operation methods

	/**
		\brief founds \b enum \b type \b union by \c itemId
		\param itemId - ID of entry full or simple
		\details full ID looks like JAVA import
			\example imtbase.ImtBaseTypes.TimeRange i.e. (<namespace>.<name>.<entryID>[.<VersionID>])
			\example imtbase.ImtBaseTypes.TimeRange.1.0
		\param itemInfoPtr[opt_out] - if is set, the param fields will be filled with information about the found item.
		\returns a found item by id if exists if not \retval will be NULL
	 */
	virtual std::shared_ptr<CSdlEntryBase> GetItem(const QString& itemId, ItemInfo* itemInfoPtr = nullptr) const = 0;

	virtual ItemInfo GetItemInfo(const CSdlEntryBase& item) const = 0;
	virtual bool CreateModuleFile() const = 0;
};


} // namespace imtsdl
