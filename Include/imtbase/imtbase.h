// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// C++ includes
#include <limits>

// Qt includes
#include <QtCore/QList>



/**
	Basic functionality and interfaces
	This package is system independent.
*/
namespace imtbase
{

/**
	\brief This is a similar implementation from the Guidelines Support Library (GSL), which is used deliberately for explicit type narrowing. 
	 \details 
		Meaning "I know thatthere is a narrowing here, and I take responsibility for myself" 
		Makes a clear narrowing
		DOES NOT check for overflow (in release mode)
		Just a static_cast, but semantically expressive 
		\note Use it carefully. Only for old code or 3rd party functions which can't be changed 
		\code{.cpp} 
			QList<int> list; 
			int numberOfElements = list.size(); ///< generates warning 
			int iNumberOfElements = narrow_cast<int>(list.size()); ///< no warning => explicit narrowing 
		\endcode
*/
template <class T, class U>
constexpr T narrow_cast(U v) noexcept
{
	// Q_ASSERT(v <= std::numeric_limits<T>::max());

	return static_cast<T>(v);
}

/**
	\todo Move to imtrest
*/
static const inline QByteArray s_protocolVersionHeaderId = QByteArrayLiteral("X-Protocol-Version");
static const inline QByteArray s_authenticationTokenHeaderId = QByteArrayLiteral("x-authentication-token");
static const inline QByteArray s_productIdHeaderId = QByteArrayLiteral("productid");


} // namespace imtbase


