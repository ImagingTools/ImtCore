// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QString>

// ACF includes
#include <istd/IPolymorphic.h>


namespace imtcache
{


/**
	Drives cache rebuilds.

	Deliberately not modelled on iotanalytics::IMdbxUpdateController: that interface derived from
	imtbase::ITransactionManager, but its StartTransaction()/EndTransaction() never opened a database
	transaction - they refcounted an istd::CChangeGroup over the *source* collections so that a bulk
	import would not trigger one cache rebuild per written row. That is change batching, not
	transactions, and it is spelled SuspendUpdates()/ResumeUpdates() here.
*/
class ICacheUpdateController: virtual public istd::IPolymorphic
{
public:
	enum UpdateMode
	{
		/**
			Applies only rows changed since the watermark stored in the CacheRevision table.
			Falls back to UM_FULL when no usable watermark exists (first run, schema change).
		*/
		UM_INCREMENTAL,
		/**
			Rebuilds each table from its source into a shadow table and swaps it in. Reserved for the
			first build, schema changes and an explicit operator-triggered rebuild; it is also the
			only way to reconcile hard deletes, which leave no trace for a watermark to find.
		*/
		UM_FULL
	};

	struct UpdateResult
	{
		bool isOk = false;
		bool wasFullRebuild = false;
		int rowsWritten = 0;
		int rowsDeleted = 0;
		QString errorMessage;
	};

	/**
		Runs an update synchronously on the calling thread. Prefer RequestUpdate() from anything
		serving requests - a full rebuild can take minutes.
	*/
	virtual UpdateResult Update(UpdateMode mode = UM_INCREMENTAL) = 0;

	/**
		Schedules an update on a worker thread and returns immediately.
		\return false if an update is already running, or if updates are suspended - in which case
				the request is remembered and run when ResumeUpdates() drops the last suspension.
	*/
	virtual bool RequestUpdate(UpdateMode mode = UM_INCREMENTAL) = 0;

	virtual bool IsUpdateRunning() const = 0;

	/**
		Holds off updates while the caller makes a burst of changes to the source collections, so
		they collapse into a single rebuild instead of one per change. Refcounted; every
		SuspendUpdates() must be paired with a ResumeUpdates().
	*/
	virtual void SuspendUpdates() = 0;
	virtual void ResumeUpdates() = 0;
};


} // namespace imtcache
