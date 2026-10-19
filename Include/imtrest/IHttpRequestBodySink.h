// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IPolymorphic.h>

// Qt includes
#include <QtCore/QtGlobal>


namespace imtrest
{


/**
	Receiver of the body of an HTTP request, it takes the body by parts while the request is received, so the body is not
	kept in the memory of the request (for example the part of a file is written to the disk). The receiver is created
	by the policy of the body for the request, see IHttpRequestBodyPolicy::BM_STREAM. All methods are called from the
	thread of the connection, the request is not handled until the body is received and Finish() is called.
	The result of the receiving (for example the size or the hash of the body) is read by the owner of the receiver
	after the request is handled, it is not a part of the interface.
*/
class IHttpRequestBodySink: virtual public istd::IPolymorphic
{
public:
	/**
		Take the next part of the body. The pointer is valid only during the call.
		\return \c false if the part cannot be taken, for example there is no place on the disk. The client gets \c 500,
				Abort() is called and no more data are passed.
	*/
	virtual bool Write(const char* data, qint64 size) = 0;

	/**
		The body is received completely, it is called once, also for an empty body.
		\return \c false if the body cannot be completed, for example the file cannot be closed. The client gets \c 500
				and the request is not handled. Abort() is not called after Finish().
	*/
	virtual bool Finish() = 0;

	/**
		The body is not received to the end: the connection is broken, the data are not a valid request or the request is
		refused. The receiver should release what it has taken, for example delete the incomplete file. It is called at most
		once and never after Finish(). If the request is destroyed before the body is received, Abort() is called as well,
		so the receiver that is not used (the request is refused before the body) is released too.
	*/
	virtual void Abort() = 0;
};


} // namespace imtrest


