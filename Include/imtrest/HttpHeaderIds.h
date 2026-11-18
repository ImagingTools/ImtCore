// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>


namespace imtrest
{


/**
	Names of the standard HTTP header fields (RFC 9110, RFC 6266, the Fetch standard with CORS, Referrer Policy), as they are written in
	the specifications. The names are case-insensitive: CHttpRequest::GetHeaderValue() finds a field by any case of its name.
*/
namespace HttpHeaderIds
{


inline const QByteArray s_authorization = QByteArrayLiteral("Authorization");
inline const QByteArray s_wwwAuthenticate = QByteArrayLiteral("WWW-Authenticate");
inline const QByteArray s_host = QByteArrayLiteral("Host");
inline const QByteArray s_origin = QByteArrayLiteral("Origin");
inline const QByteArray s_vary = QByteArrayLiteral("Vary");
inline const QByteArray s_cacheControl = QByteArrayLiteral("Cache-Control");
inline const QByteArray s_retryAfter = QByteArrayLiteral("Retry-After");
inline const QByteArray s_contentType = QByteArrayLiteral("Content-Type");
inline const QByteArray s_contentLength = QByteArrayLiteral("Content-Length");
inline const QByteArray s_contentRange = QByteArrayLiteral("Content-Range");
inline const QByteArray s_contentDisposition = QByteArrayLiteral("Content-Disposition");
inline const QByteArray s_transferEncoding = QByteArrayLiteral("Transfer-Encoding");
inline const QByteArray s_range = QByteArrayLiteral("Range");
inline const QByteArray s_ifRange = QByteArrayLiteral("If-Range");
inline const QByteArray s_referrerPolicy = QByteArrayLiteral("Referrer-Policy");
inline const QByteArray s_xContentTypeOptions = QByteArrayLiteral("X-Content-Type-Options");

inline const QByteArray s_accessControlAllowOrigin = QByteArrayLiteral("Access-Control-Allow-Origin");
inline const QByteArray s_accessControlAllowMethods = QByteArrayLiteral("Access-Control-Allow-Methods");
inline const QByteArray s_accessControlAllowHeaders = QByteArrayLiteral("Access-Control-Allow-Headers");
inline const QByteArray s_accessControlExposeHeaders = QByteArrayLiteral("Access-Control-Expose-Headers");
inline const QByteArray s_accessControlMaxAge = QByteArrayLiteral("Access-Control-Max-Age");


} // namespace HttpHeaderIds


} // namespace imtrest


