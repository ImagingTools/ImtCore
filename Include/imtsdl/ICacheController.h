#pragma once


// ImtCore includes
#include <imtsdl/imtsdl.h>
#include <imtsdl/CSdlCache.h>


namespace imtsdl
{


class ICacheController: virtual public istd::IPolymorphic
{

public:
	enum LoadError {
		LE_OK,				///< No errors
		LE_NULL_CACHE,		///< Maybe not an error. Occurs if file is not exsists or does not contain any data
		LE_DEVICE,			///< IODevice errors. e.g. file is not opened
		LE_INVALID_DATA		///< File contains invalid data
	};

	virtual CCache& GetCache() = 0;
	virtual const CCache& GetCache() const = 0;

	/**
		Sets an Operational Device, a device for loading and saving a cache
		\warning \c devicePtr MUST NOT be NULL (i.e. devicePtr.get() != nullptr)
		\warning \c devicePtr MUST NOT be Sequential (i.e. devicePtr->isSequential() == false) \sa QIODevice::isSequential
		\warning a \c The device MUST be opened in \c QIODevice::ReadWrite mode, or it MUST NOT be opened at all. If it is not opened, it will be automatically opened in the appropriate mode.
	*/
	virtual bool SetOperationalDevice(const std::shared_ptr<QIODevice>& devicePtr) = 0;

	/**
		Writes currenc cache to the operational device (ONLY if it is set)
	*/
	virtual bool Save() const = 0;

	/**
		\brief Loads a cache from a \c dataDevicePtr of from an Operational device, setted by \c SetOperationalDevice
		\param dataDevicePtr - [optional] if is not nullptr, the cache will be loaded from provided device, and from an Operational device otherwise
		\note if a \c dataDevicePtr is provided, the operational device will NOT be changed, even if it is not set. You MUST call \c SetOperationalDevice method, to set an operational device.
	*/
	virtual LoadError LoadFromData(QIODevice* dataDevicePtr = nullptr) = 0;

	/**
		Enables / disables auto saving cache to the Operational device on change
	 */
	virtual void EnableAutoSave(bool enableAutosave = true) = 0;
	inline void DisableAutoSave() { EnableAutoSave(false); }
};


} // namespace imtsdl


