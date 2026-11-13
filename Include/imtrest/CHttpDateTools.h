// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QDateTime>


namespace imtrest
{


/**
	Dates of HTTP header fields: \c Date, \c Last-Modified, \c If-Modified-Since, \c Expires, \c Retry-After (RFC 9110, section 5.6.7).
	HTTP-date is the format IMF-fixdate or one of two obsolete formats.
*/
class CHttpDateTools
{
public:
	/**
		Create the value of a date header field in the format IMF-fixdate, for example \c "Sun, 06 Nov 1994 08:49:37 GMT".
		The names of the days and months are never localized, the time is converted to GMT.
		A sender should generate only this format.
	*/
	static QByteArray ToImfFixdate(const QDateTime& dateTime);

	/**
		Parse the value of a date header field. A recipient should accept all formats of HTTP-date:
		- IMF-fixdate: \c "Sun, 06 Nov 1994 08:49:37 GMT";
		- obsolete RFC 850: \c "Sunday, 06-Nov-94 08:49:37 GMT". The two-digit year that appears to be more than 50 years
		  in the future represents the most recent year in the past with the same last two digits;
		- obsolete asctime: \c "Sun Nov  6 08:49:37 1994" (the time is GMT).
		The names of the days and months are case-sensitive and English. The name of the day is checked only to be
		a name of a day, it is not compared with the date. The second \c 60 (a leap second) is accepted and is the next
		minute. Spaces around the value are ignored.
		\param	value	Value of the header.
		\param	now		Current time, used for the two-digit year.
		\return The time in UTC or an invalid time if the value is not a date of any of the formats.
	*/
	static QDateTime FromHttpDate(const QByteArray& value, const QDateTime& now = QDateTime::currentDateTimeUtc());
};


} // namespace imtrest


