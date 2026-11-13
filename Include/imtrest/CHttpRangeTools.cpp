// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrest/CHttpRangeTools.h>


// STL includes
#include <algorithm>
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


/**
	Parse a non-negative number written with decimal digits only, a number out of the range is the largest number.
*/
bool ParseSaturatedNumber(const QByteArray& text, qint64& value)
{
	if (text.isEmpty()){
		return false;
	}

	qint64 retVal = 0;
	bool isOverflow = false;

	for (char character : text){
		if ((character < '0') || (character > '9')){
			return false;
		}

		const int digit = character - '0';
		if (!isOverflow && (retVal > (std::numeric_limits<qint64>::max() - digit) / 10)){
			isOverflow = true;
		}

		if (!isOverflow){
			retVal = retVal * 10 + digit;
		}
	}

	value = isOverflow ? std::numeric_limits<qint64>::max() : retVal;

	return true;
}


enum SpecStatus
{
	SS_INVALID,
	SS_NOT_SATISFIABLE,
	SS_SATISFIABLE
};


/**
	The suffix form "-count": the last bytes of the representation.
*/
SpecStatus ResolveSuffixSpec(const QByteArray& countText, qint64 completeLength, CHttpRangeTools::ByteRange& range)
{
	qint64 suffixLength = 0;
	if (!ParseSaturatedNumber(countText, suffixLength)){
		return SS_INVALID;
	}

	if ((suffixLength == 0) || (completeLength == 0)){
		return SS_NOT_SATISFIABLE;
	}

	range.first = (suffixLength >= completeLength) ? 0 : completeLength - suffixLength;
	range.last = completeLength - 1;

	return SS_SATISFIABLE;
}


/**
	The forms "first-last" and "first-".
*/
SpecStatus ResolvePositionSpec(const QByteArray& firstText, const QByteArray& lastText, qint64 completeLength, CHttpRangeTools::ByteRange& range)
{
	qint64 first = 0;
	if (!ParseSaturatedNumber(firstText, first)){
		return SS_INVALID;
	}

	qint64 last = std::numeric_limits<qint64>::max();
	if (!lastText.isEmpty() && (!ParseSaturatedNumber(lastText, last) || (last < first))){
		return SS_INVALID;
	}

	if (first >= completeLength){
		return SS_NOT_SATISFIABLE;
	}

	range.first = first;
	range.last = std::min(last, completeLength - 1);

	return SS_SATISFIABLE;
}


SpecStatus ResolveSpec(const QByteArray& spec, qint64 completeLength, CHttpRangeTools::ByteRange& range)
{
	const int dashIndex = int(spec.indexOf('-'));
	if (dashIndex < 0){
		return SS_INVALID;
	}

	const QByteArray firstText = spec.left(dashIndex);
	const QByteArray lastText = spec.mid(dashIndex + 1);

	if (firstText.isEmpty()){
		return ResolveSuffixSpec(lastText, completeLength, range);
	}

	return ResolvePositionSpec(firstText, lastText, completeLength, range);
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


// public methods of the embedded class ByteRange

qint64 CHttpRangeTools::ByteRange::GetLength() const
{
	return last - first + 1;
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


CHttpRangeTools::RangeStatus CHttpRangeTools::ParseRange(const QByteArray& value, qint64 completeLength, QList<ByteRange>& ranges, int maxRangeCount)
{
	const QByteArray text = value.trimmed();

	const int equalIndex = int(text.indexOf('='));
	if ((completeLength < 0) || (equalIndex < 0) || (text.left(equalIndex).toLower() != "bytes")){
		return RS_IGNORE;
	}

	const QList<QByteArray> specs = text.mid(equalIndex + 1).split(',');

	QList<ByteRange> satisfiableRanges;
	int specCount = 0;

	for (const QByteArray& rawSpec : specs){
		const QByteArray spec = rawSpec.trimmed();
		if (spec.isEmpty()){
			continue;
		}

		++specCount;
		if (specCount > maxRangeCount){
			return RS_IGNORE;
		}

		ByteRange range;

		const SpecStatus status = ResolveSpec(spec, completeLength, range);
		if (status == SS_INVALID){
			return RS_IGNORE;
		}

		if (status == SS_SATISFIABLE){
			satisfiableRanges.append(range);
		}
	}

	if (specCount == 0){
		return RS_IGNORE;
	}

	if (satisfiableRanges.isEmpty()){
		return RS_NOT_SATISFIABLE;
	}

	ranges = satisfiableRanges;

	return RS_SATISFIABLE;
}


QList<CHttpRangeTools::ByteRange> CHttpRangeTools::CoalesceRanges(const QList<ByteRange>& ranges)
{
	QList<ByteRange> sortedRanges = ranges;

	std::sort(sortedRanges.begin(), sortedRanges.end(), [](const ByteRange& left, const ByteRange& right){
		return left.first < right.first;
	});

	QList<ByteRange> retVal;

	for (const ByteRange& range : sortedRanges){
		// A range that starts right after the previous one is merged too, first - 1 does not overflow
		if (!retVal.isEmpty() && (range.first - 1 <= retVal.last().last)){
			retVal.last().last = std::max(retVal.last().last, range.last);
		}
		else{
			retVal.append(range);
		}
	}

	return retVal;
}


} // namespace imtrest


