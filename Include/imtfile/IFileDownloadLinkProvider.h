// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QDateTime>

// ACF includes
#include <istd/IPolymorphic.h>


namespace imtfile
{


/**
	Interface for providing download links to the files of a file system.
	Separate from IFileSystemStructureProvider: not every source can give out links.
 */
class IFileDownloadLinkProvider: virtual public istd::IPolymorphic
{
public:
	struct DownloadLink
	{
		// Self-contained URL: a plain GET without other credentials.
		QString url;
		// Other URLs of the same content by priority.
		QStringList alternativeUrls;
		// Until when the URLs are valid; invalid if unknown. A hint only:
		// the clocks of the client and of the source may differ.
		QDateTime expiresAt;
		// Size of the content in bytes; -1 if unknown.
		qint64 size = -1;
		// SHA-256 of the content, lowercase hex; empty if unknown.
		QString sha256;
		// Strong entity tag exactly as it goes in the ETag header, with the
		// quotes; empty if the source cannot resume a download.
		QString etag;
	};

	/**
		\brief Provides a download link to a file.
		\param path [INPUT] - path of the file as IFileSystemStructureProvider lists it
		\param link [OUTPUT] - the link
		\param error [OUTPUT] - error description if the operation fails
		\return \c true if the link could be provided
	*/
	virtual bool GetDownloadLink(
				const QString& path,
				DownloadLink& link,
				QString& error) const = 0;
};


} // namespace imtfile
