// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ibase/IProgressManager.h>


namespace imtbase
{


/**
	Common interface for managing progress sessions
*/
class IProgressSessionsManager : virtual public istd::IPolymorphic
{
public:
	/**
		Create a progress manager session and return a pointer to it
		The pointer remains valid until EndProgressSession or CancelProgressSession is called with given session ID
	*/
	virtual ibase::IProgressManager* BeginProgressSession(
				const QByteArray& sessionId,
				const QString& description) = 0;

	/**
		End progress session with given ID normally
	*/
	virtual bool EndProgressSession(const QByteArray& sessionId) = 0;

	/**
		Cancel progress session with given ID
	*/
	virtual bool CancelProgressSession(
				const QByteArray& sessionId,
				const QString& description = QString(),
				bool isFailed = false) = 0;
};


} // namespace imtbase


