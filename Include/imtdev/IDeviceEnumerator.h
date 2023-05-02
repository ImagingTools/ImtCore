#pragma once


// ACF includes
#include <istd/IPolymorphic.h>


namespace imtdev
{


class IDeviceEnumerator: virtual public istd::IPolymorphic
{
public:
	class IResultHandler: virtual public istd::IPolymorphic
	{
	public:
		enum EnumerationResult
		{
			ER_OK,
			ER_CANCELED,
			ER_FAILED
		};

		/**
			Processing the result of device enumeration.
		*/
		virtual void OnEnumerationResult(
					EnumerationResult result,
					const IDeviceEnumerator* enumeratorPtr) = 0;
	};

	enum StartResult
	{
		SR_OK,
		SR_PREVIOUS_ENUMERATION_NOT_FINISHED,
		SR_FAILED
	};

	/**
		Start device enumeration proccess.
		After a successful start of enumeration (if SR_OK is returned), the DeviceEnumerator must call IDeviceEnumerationResultHandler::SetEnumerationResult.
	*/
	virtual StartResult StartEnumeration(IResultHandler* resultHandlerPtr) = 0;

	/**
		Cancel device enumeration proccess.
	*/
	virtual void CancelEnumeration() = 0;
};


} // namespace imtdev


