// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QString>

// ACF includes
#include <istd/IPolymorphic.h>

// ImtCore includes
#include <imtrest/IProtocolEngine.h>
#include <imtrest/IResponse.h>


namespace imtrest
{


class CHttpRequest;


/**
	Finds the file that a file servlet sends for a request, and decides whether the client can get it (see CHttpFileServletComp).
	The servlet knows how to send a file (\c Range, conditions, streaming), the resolver knows which file it is and who can get it.

	The resolver is called in the thread of the request handler, as any servlet, from several threads at once, so it MUST be thread-safe.
	The path of the file comes from the resolver, so it is its duty to protect from the paths outside of the allowed directory
	(for example, \c "../", absolute paths and the links that lead out of the directory).
*/
class IHttpFileResolver: virtual public istd::IPolymorphic
{
public:
	/**
		The file that the resolver gives for a request.
	*/
	struct FileDescription
	{
		/**
			Path of the file in the file system.
		*/
		QString filePath;

		/**
			Value of \c Content-Type, for example \c "application/pdf". An empty value means \c "application/octet-stream".
		*/
		QByteArray dataTypeId;

		/**
			Value of \c ETag with the quotes (see CHttpConditionalTools::CreateEntityTag()). An empty value means that
			the tag is made of the size of the file and the time of its last change.
		*/
		QByteArray eTag;

		/**
			Additional headers of the response, for example \c Content-Disposition or \c Cache-Control. They should not
			contain \c ETag, \c Last-Modified, \c Content-Range and \c Accept-Ranges: these headers are made by the servlet.
			The headers are also sent if the request is refused (for example, \c WWW-Authenticate).
		*/
		IResponse::Headers headers;
	};

	/**
		Find the file of a request.
		\param	request	The request, used to determine the file path and other request-specific information.
		\param	file	The file that is sent. It is used only if the result is \c IProtocolEngine::SC_OK, but
						the headers are used in any case.
		\return	\c IProtocolEngine::SC_OK to send the file or the status of the refusal, for example
				\c SC_NOT_FOUND, \c SC_FORBIDDEN or \c SC_UNAUTHORIZED: the client gets it without a body.
	*/
	virtual IProtocolEngine::StatusCode ResolveFile(const CHttpRequest& request, FileDescription& file) const = 0;
};


} // namespace imtrest


