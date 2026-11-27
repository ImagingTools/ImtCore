// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QString>

// ACF includes
#include <istd/IPolymorphic.h>


namespace imtfile
{


/**
	Receiver of the changes of the watched folders of a file system.
*/
class IFileSystemChangeListener: virtual public istd::IPolymorphic
{
public:
	/**
		\brief The content of a watched folder or the folder itself changed (also its renaming, moving or removal).
		It may be called from any thread.
		\param watchId [INPUT] - the watch, as IFileSystemChangeNotifier::AddWatch gave it
		\param path [INPUT] - the path of the watch
	*/
	virtual void OnFileSystemChanged(const QByteArray& watchId, const QString& path) = 0;
};


/**
	Source of the changes of the folders of a file system, for a provider that can report them.
*/
class IFileSystemChangeNotifier: virtual public istd::IPolymorphic
{
public:
	/**
		\brief Starts to watch a folder.
		The path is resolved and the access is checked in the context of the call (e.g. of the request of a subscription);
		the watch keeps them. AddWatch itself does not call the listener.
		\param path [INPUT] - path of the folder as IFileSystemStructureProvider lists it; empty for the root
		\param listener [INPUT] - gets the changes; it must stay alive until RemoveWatch returns
		\param watchId [OUTPUT] - ID of the new watch
		\param error [OUTPUT] - why the folder cannot be watched
		\return \c true if the watch is started
	*/
	virtual bool AddWatch(
				const QString& path,
				IFileSystemChangeListener& listener,
				QByteArray& watchId,
				QString& error) = 0;

	/**
		\brief Stops a watch. After it returns, the listener of the watch is not called any more.
		Do not call it while holding a lock the listener takes.
	*/
	virtual void RemoveWatch(const QByteArray& watchId) = 0;
};


} // namespace imtfile
