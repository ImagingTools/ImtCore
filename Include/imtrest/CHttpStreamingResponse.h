// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// STL includes
#include <functional>

// ImtCore includes
#include <imtrest/CHttpResponse.h>
#include <imtrest/IStreamingResponse.h>


namespace imtrest
{


/**
	Streaming response whose body source is created by a function. The status, the headers and the data type are the same as
	of CHttpResponse, the body is not in memory.
	The function is called in the thread of the connection when the body is sent, and the response can be destroyed right after
	that, so the function must not refer to the request or to objects of the servlet that can be destroyed: it
	should capture copies of everything it needs.
*/
class CHttpStreamingResponse: public CHttpResponse, virtual public IStreamingResponse
{
public:
	typedef std::function<IResponseBodySourcePtr()> SourceFactory;

	/**
		\param	bodySize		Size of the body or -1 if it is not known, see IStreamingResponse::GetBodySize().
		\param	sourceFactory	Creates the source of the body, it returns \c nullptr if the source cannot be created.
	*/
	CHttpStreamingResponse(
				int statusCode,
				const QByteArray& dataTypeId,
				const IProtocolEngine& engine,
				qint64 bodySize,
				const SourceFactory& sourceFactory);

	// reimplemented (imtrest::IStreamingResponse)
	virtual qint64 GetBodySize() const override;
	virtual IResponseBodySourcePtr CreateBodySource() const override;

private:
	qint64 m_bodySize;
	SourceFactory m_sourceFactory;
};


} // namespace imtrest


