// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IPolymorphic.h>


namespace imtdev
{


/**
	@brief Interface for asynchronous device enumeration
	
	IDeviceEnumerator provides an asynchronous protocol for discovering and enumerating
	devices. The enumeration process runs in the background and delivers results through
	a callback interface.
	
	This interface is the base for IDeviceController and enables non-blocking device
	discovery, which is essential for responsive user interfaces and scenarios where
	device enumeration may take significant time (e.g., network devices, USB scanning).
	
	@par Enumeration Protocol:
	1. Client calls StartEnumeration() with a result handler
	2. Enumerator performs discovery asynchronously
	3. Upon completion, enumerator calls handler's OnEnumerationResult()
	4. Client can cancel enumeration at any time via CancelEnumeration()
	
	@par Usage Example:
	@code{.cpp}
	class MyEnumerationHandler : public IDeviceEnumerator::IResultHandler
	{
	public:
		void OnEnumerationResult(EnumerationResult result, 
		                         const IDeviceEnumerator* enumeratorPtr) override
		{
			if (result == ER_OK)
			{
				// Enumeration completed successfully
				// Update device list from controller
			}
			else if (result == ER_CANCELED)
			{
				// Enumeration was canceled
			}
			else
			{
				// Enumeration failed
			}
		}
	};
	
	MyEnumerationHandler handler;
	IDeviceEnumerator::StartResult result = pEnumerator->StartEnumeration(&handler);
	
	if (result == IDeviceEnumerator::SR_OK)
	{
		// Enumeration started successfully
		// Wait for callback or continue with other work
	}
	@endcode
	
	@par Thread Safety:
	Implementations should ensure that result handler callbacks are thread-safe.
	The callback may be invoked from a different thread than the one that called
	StartEnumeration().
	
	@see IDeviceController
	@see IResultHandler
	@ingroup imtdev
*/
class IDeviceEnumerator: virtual public istd::IPolymorphic
{
public:
	/**
		@brief Callback interface for receiving enumeration results
		
		Clients must implement this interface to receive asynchronous notifications
		about enumeration completion, cancellation, or failure.
	*/
	class IResultHandler: virtual public istd::IPolymorphic
	{
	public:
		/**
			@brief Enumeration result codes
			
			Indicates the outcome of the enumeration process.
		*/
		enum EnumerationResult
		{
			/// Enumeration completed successfully
			ER_OK,
			/// Enumeration was canceled via CancelEnumeration()
			ER_CANCELED,
			/// Enumeration failed due to an error
			ER_FAILED
		};

		/**
			@brief Process the result of device enumeration
			
			This callback is invoked when the enumeration process completes,
			is canceled, or fails. The callback may be invoked from any thread.
			
			@param result Outcome of the enumeration process
			@param enumeratorPtr Pointer to the enumerator that completed
			@note Implement thread-safe handling as this may be called from background threads
		*/
		virtual void OnEnumerationResult(
					EnumerationResult result,
					const IDeviceEnumerator* enumeratorPtr) = 0;
	};

	/**
		@brief Start enumeration result codes
		
		Indicates whether enumeration was successfully initiated.
	*/
	enum StartResult
	{
		/// Enumeration started successfully
		SR_OK,
		/// Cannot start - previous enumeration still running
		SR_PREVIOUS_ENUMERATION_NOT_FINISHED,
		/// Failed to start enumeration
		SR_FAILED
	};

	/**
		@brief Start device enumeration process
		
		Initiates asynchronous device discovery. If enumeration starts successfully
		(SR_OK is returned), the provided result handler's OnEnumerationResult()
		will be called when enumeration completes.
		
		@param resultHandlerPtr Handler to receive enumeration completion notification
		@return SR_OK if enumeration started, SR_PREVIOUS_ENUMERATION_NOT_FINISHED if 
		        enumeration is already running, or SR_FAILED on error
		@note Only one enumeration can run at a time per enumerator instance
		@see IResultHandler
		@see CancelEnumeration()
	*/
	virtual StartResult StartEnumeration(IResultHandler* resultHandlerPtr) = 0;

	/**
		@brief Cancel device enumeration process
		
		Requests cancellation of the current enumeration. The result handler will
		be notified with ER_CANCELED status.
		
		@note This is a request - enumeration may not stop immediately
		@see StartEnumeration()
	*/
	virtual void CancelEnumeration() = 0;
};


} // namespace imtdev


