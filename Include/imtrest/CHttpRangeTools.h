// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>


namespace imtrest
{


/**
	Parsing and creation of the HTTP range headers (RFC 9110, section 14).
*/
class CHttpRangeTools
{
public:
	/**
		Value of the \c Content-Range header with the range unit \c bytes (RFC 9110, section 14.4).
		Both positions are inclusive: the range 0-999 contains the first 1000 bytes.
	*/
	struct ContentRange
	{
		/**
			First position of the range or -1 for an unsatisfied range (the form "bytes *" + "/" + length).
		*/
		qint64 first = -1;

		/**
			Last position of the range (inclusive).
		*/
		qint64 last = -1;

		/**
			Size of the complete representation or -1 if it is unknown ("*").
		*/
		qint64 completeLength = -1;

		/**
			Returns \c true if the value describes the range of bytes and not an unsatisfied range.
		*/
		bool IsByteRange() const;

		/**
			Number of bytes in the range or 0 for an unsatisfied range.
		*/
		qint64 GetLength() const;
	};

	/**
		Parse the value of the \c Content-Range header.
		Supported forms are \c "bytes first-last/length", \c "bytes first-last/" followed by a star for an unknown length
		and \c "bytes " followed by a star and \c "/length" for an unsatisfied range.
		The value is rejected if the unit is not \c bytes (the unit name is case-insensitive), if the numbers are not
		plain digits, if \c first is greater than \c last or if \c last is not less than the known complete length.
		\param	value	Value of the header.
		\param	result	Parsed range. It is not changed if the value is rejected.
		\return \c true if the value is valid.
	*/
	static bool ParseContentRange(const QByteArray& value, ContentRange& result);

	/**
		Create the value of the \c Content-Range header for a range of bytes.
		\param	first			First position of the range.
		\param	length			Number of bytes in the range, should be positive.
		\param	completeLength	Size of the complete representation or -1 if it is unknown.
		\return The header value or an empty array if the range is empty, negative or outside of the complete length.
	*/
	static QByteArray CreateContentRange(qint64 first, qint64 length, qint64 completeLength = -1);
};


} // namespace imtrest


