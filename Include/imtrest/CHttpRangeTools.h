// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QList>


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

	/**
		Range of bytes of a representation, both positions are inclusive.
	*/
	struct ByteRange
	{
		qint64 first = 0;
		qint64 last = 0;

		/**
			Number of bytes in the range.
		*/
		qint64 GetLength() const;
	};

	/**
		What a server should do with the \c Range header field of a request.
	*/
	enum RangeStatus
	{
		/**
			The header field must be ignored: the unit is unknown, the syntax is wrong or there are too many ranges.
			The server answers 200 with the complete representation.
		*/
		RS_IGNORE,

		/**
			At least one range is inside of the representation. The server answers 206.
		*/
		RS_SATISFIABLE,

		/**
			No range is inside of the representation. The server answers 416 with the header
			\c Content-Range of the form "bytes *" + "/length".
		*/
		RS_NOT_SATISFIABLE
	};

	/**
		Parse the value of the \c Range header field (RFC 9110, section 14.1 and 14.2) for the representation of a known size.
		Supported are the forms \c "bytes=first-last", \c "bytes=first-" and the suffix form \c "bytes=-count" (the last
		bytes), several ranges separated by commas and empty list elements. The unit name is case-insensitive.
		If any element of the list is not valid, the whole header field is ignored. A range that starts at or after the end
		of the representation is not satisfiable and is skipped, the \c last position beyond the end is cut to the end,
		the suffix longer than the representation selects the whole representation. A suffix of zero bytes is not
		satisfiable. Every range of an empty representation (\c completeLength is 0) is not satisfiable.
		Numbers above the range of \c qint64 are accepted and mean the largest number.
		\param	value			Value of the header.
		\param	completeLength	Size of the representation, should not be negative.
		\param	ranges			Satisfiable ranges in the order of the request, they can overlap. It is not changed
								if the result is not \c RS_SATISFIABLE. See also \c CoalesceRanges().
		\param	maxRangeCount	Maximal number of ranges in the header field, a larger one is ignored (protection against
								the headers made of many small ranges, RFC 9110, section 14.2).
	*/
	static RangeStatus ParseRange(const QByteArray& value, qint64 completeLength, QList<ByteRange>& ranges, int maxRangeCount = 16);

	/**
		Sort the ranges by their position and merge the ranges that overlap or touch each other.
	*/
	static QList<ByteRange> CoalesceRanges(const QList<ByteRange>& ranges);
};


} // namespace imtrest


