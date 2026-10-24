// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrest/CHttpConditionalTools.h>


// ImtCore includes
#include <imtrest/CHttpDateTools.h>


namespace imtrest
{


namespace
{


bool IsEntityTagCharacter(char character)
{
	const auto value = static_cast<unsigned char>(character);

	return (value == 0x21) || ((value >= 0x23) && (value <= 0x7E)) || (value >= 0x80);
}


bool IsWhitespace(char character)
{
	return (character == ' ') || (character == '\t');
}


/**
	Read one entity tag that starts at the position. The position is moved behind the tag.
*/
bool ScanEntityTag(const QByteArray& text, qsizetype& position, CHttpConditionalTools::EntityTag& tag)
{
	qsizetype index = position;
	bool isWeak = false;

	if (text.mid(index, 2) == "W/"){
		isWeak = true;
		index += 2;
	}

	if ((index >= text.size()) || (text.at(index) != '"')){
		return false;
	}

	++index;
	const qsizetype start = index;

	while ((index < text.size()) && (text.at(index) != '"')){
		if (!IsEntityTagCharacter(text.at(index))){
			return false;
		}

		++index;
	}

	if (index >= text.size()){
		return false;
	}

	tag.opaqueTag = text.mid(start, index - start);
	tag.isWeak = isWeak;
	position = index + 1;

	return true;
}


bool IsMatching(const CHttpConditionalTools::EntityTag& first, const CHttpConditionalTools::EntityTag& second, bool useStrongComparison)
{
	if (useStrongComparison && (first.isWeak || second.isWeak)){
		return false;
	}

	return first.opaqueTag == second.opaqueTag;
}


/**
	Time of HTTP-date has the resolution of one second.
*/
qint64 ToSeconds(const QDateTime& dateTime)
{
	return dateTime.toSecsSinceEpoch();
}


} // namespace


// public static methods

bool CHttpConditionalTools::ParseEntityTag(const QByteArray& value, EntityTag& result)
{
	const QByteArray text = value.trimmed();

	qsizetype position = 0;
	EntityTag tag;

	if (!ScanEntityTag(text, position, tag) || (position != text.size())){
		return false;
	}

	result = tag;

	return true;
}


QByteArray CHttpConditionalTools::CreateEntityTag(const QByteArray& opaqueTag, bool isWeak)
{
	for (char character : opaqueTag){
		if (!IsEntityTagCharacter(character)){
			return {};
		}
	}

	QByteArray retVal = isWeak ? QByteArray("W/\"") : QByteArray("\"");
	retVal += opaqueTag;
	retVal += '"';

	return retVal;
}


CHttpConditionalTools::EntityTagMatch CHttpConditionalTools::MatchEntityTagList(const QByteArray& listValue, const QByteArray& eTag, bool useStrongComparison)
{
	const QByteArray text = listValue.trimmed();

	if (text == "*"){
		return ETM_MATCH;
	}

	EntityTag current;
	const bool hasCurrent = ParseEntityTag(eTag, current);

	bool isMatching = false;
	int tagCount = 0;
	qsizetype position = 0;

	while (true){
		// Empty elements of the list are allowed
		while ((position < text.size()) && (IsWhitespace(text.at(position)) || (text.at(position) == ','))){
			++position;
		}

		if (position >= text.size()){
			break;
		}

		EntityTag tag;
		if (!ScanEntityTag(text, position, tag)){
			return ETM_INVALID;
		}

		++tagCount;

		if (hasCurrent && IsMatching(tag, current, useStrongComparison)){
			isMatching = true;
		}

		while ((position < text.size()) && IsWhitespace(text.at(position))){
			++position;
		}

		if ((position < text.size()) && (text.at(position) != ',')){
			return ETM_INVALID;
		}
	}

	if (tagCount == 0){
		return ETM_INVALID;
	}

	return isMatching ? ETM_MATCH : ETM_NO_MATCH;
}


CHttpConditionalTools::PreconditionResult CHttpConditionalTools::EvaluatePreconditions(
			const QByteArray& method,
			const Preconditions& preconditions,
			const Validators& validators)
{
	const bool isReadMethod = (method == "GET") || (method == "HEAD");

	// Step 1 and 2
	if (!preconditions.ifMatch.isEmpty()){
		switch (MatchEntityTagList(preconditions.ifMatch, validators.eTag, true)){
		case ETM_INVALID:
			return PR_BAD_REQUEST;
		case ETM_NO_MATCH:
			return PR_PRECONDITION_FAILED;
		case ETM_MATCH:
			break;
		}
	}
	else if (!preconditions.ifUnmodifiedSince.isEmpty() && validators.lastModified.isValid()){
		const QDateTime date = CHttpDateTools::FromHttpDate(preconditions.ifUnmodifiedSince);
		if (date.isValid() && (ToSeconds(validators.lastModified) > ToSeconds(date))){
			return PR_PRECONDITION_FAILED;
		}
	}

	// Step 3 and 4
	if (!preconditions.ifNoneMatch.isEmpty()){
		switch (MatchEntityTagList(preconditions.ifNoneMatch, validators.eTag, false)){
		case ETM_INVALID:
			return PR_BAD_REQUEST;
		case ETM_MATCH:
			return isReadMethod ? PR_NOT_MODIFIED : PR_PRECONDITION_FAILED;
		case ETM_NO_MATCH:
			break;
		}
	}
	else if (isReadMethod && !preconditions.ifModifiedSince.isEmpty() && validators.lastModified.isValid()){
		const QDateTime date = CHttpDateTools::FromHttpDate(preconditions.ifModifiedSince);
		if (date.isValid() && (ToSeconds(validators.lastModified) <= ToSeconds(date))){
			return PR_NOT_MODIFIED;
		}
	}

	return PR_PROCEED;
}


bool CHttpConditionalTools::EvaluateIfRange(const QByteArray& ifRange, const Validators& validators)
{
	const QByteArray text = ifRange.trimmed();

	if (text.startsWith('"') || text.startsWith("W/")){
		EntityTag tag;
		EntityTag current;

		return ParseEntityTag(text, tag) && ParseEntityTag(validators.eTag, current) && IsMatching(tag, current, true);
	}

	const QDateTime date = CHttpDateTools::FromHttpDate(text);

	return date.isValid() && validators.lastModified.isValid() && (ToSeconds(validators.lastModified) == ToSeconds(date));
}


} // namespace imtrest


