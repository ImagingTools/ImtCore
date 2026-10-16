// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrest/CHttpRangeTools.h>


// STL includes
#include <limits>


namespace imtrest
{


namespace
{


/**
	Parse a non-negative number written with decimal digits only.
*/
bool ParseNumber(const QByteArray& text, qint64& value)
{
	if (text.isEmpty()){
		return false;
	}

	qint64 retVal = 0;

	for (char character : text){
		if ((character < '0') || (character > '9')){
			return false;
		}

		const int digit = character - '0';
		if (retVal > (std::numeric_limits<qint64>::max() - digit) / 10){
			return false;
		}

		retVal = retVal * 10 + digit;
	}

	value = retVal;

	return true;
}


} // namespace


// public methods of the embedded class ContentRange

bool CHttpRangeTools::ContentRange::IsByteRange() const
{
	return first >= 0;
}


qint64 CHttpRangeTools::ContentRange::GetLength() const
{
	return IsByteRange() ? last - first + 1 : 0;
}


// public static methods

bool CHttpRangeTools::ParseContentRange(const QByteArray& value, ContentRange& result)
{
	static const QByteArray unit("bytes ");

	const QByteArray text = value.trimmed();
	if ((text.size() <= unit.size()) || (text.left(unit.size()).toLower() != unit)){
		return false;
	}

	const QByteArray spec = text.mid(unit.size());

	const int slashIndex = int(spec.indexOf('/'));
	if (slashIndex < 0){
		return false;
	}

	const QByteArray rangePart = spec.left(slashIndex);
	const QByteArray lengthPart = spec.mid(slashIndex + 1);

	ContentRange parsedRange;

	if (lengthPart != "*"){
		if (!ParseNumber(lengthPart, parsedRange.completeLength)){
			return false;
		}
	}

	if (rangePart == "*"){
		// An unsatisfied range is described by the length only:
		if (parsedRange.completeLength < 0){
			return false;
		}

		result = parsedRange;

		return true;
	}

	const int dashIndex = int(rangePart.indexOf('-'));
	if (dashIndex < 0){
		return false;
	}

	if (!ParseNumber(rangePart.left(dashIndex), parsedRange.first) || !ParseNumber(rangePart.mid(dashIndex + 1), parsedRange.last)){
		return false;
	}

	if (parsedRange.first > parsedRange.last){
		return false;
	}

	if ((parsedRange.completeLength >= 0) && (parsedRange.last >= parsedRange.completeLength)){
		return false;
	}

	result = parsedRange;

	return true;
}


QByteArray CHttpRangeTools::CreateContentRange(qint64 first, qint64 length, qint64 completeLength)
{
	if ((first < 0) || (length <= 0) || (length - 1 > std::numeric_limits<qint64>::max() - first)){
		return {};
	}

	const qint64 last = first + length - 1;
	if ((completeLength >= 0) && (last >= completeLength)){
		return {};
	}

	QByteArray retVal("bytes ");
	retVal += QByteArray::number(first);
	retVal += '-';
	retVal += QByteArray::number(last);
	retVal += '/';
	retVal += (completeLength >= 0) ? QByteArray::number(completeLength) : QByteArray("*");

	return retVal;
}


} // namespace imtrest


