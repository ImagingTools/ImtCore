// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QDateTime>


namespace imtrest
{


/**
	Conditional requests (RFC 9110, section 13): entity tags and the header fields \c If-Match, \c If-None-Match,
	\c If-Modified-Since, \c If-Unmodified-Since and \c If-Range.
	All methods are stateless and know nothing about the request object, they get the values of the header fields.
*/
class CHttpConditionalTools
{
public:
	/**
		Entity tag (RFC 9110, section 8.8.3), for example \c "xyzzy" or \c W/"xyzzy".
	*/
	struct EntityTag
	{
		/**
			Characters between the quotes.
		*/
		QByteArray opaqueTag;

		/**
			\c true for a weak validator (the prefix \c W/).
		*/
		bool isWeak = false;
	};

	/**
		The current state of the selected representation.
	*/
	struct Validators
	{
		/**
			Value of the \c ETag header field of the representation (with the quotes) or an empty array if there is no entity tag.
		*/
		QByteArray eTag;

		/**
			Time of the last change of the representation (\c Last-Modified) or an invalid time if it is unknown.
			The fractions of a second are ignored, the resolution of HTTP-date is one second.
		*/
		QDateTime lastModified;
	};

	/**
		Values of the conditional header fields of a request. An empty array means that the field is absent.
	*/
	struct Preconditions
	{
		QByteArray ifMatch;
		QByteArray ifNoneMatch;
		QByteArray ifModifiedSince;
		QByteArray ifUnmodifiedSince;
	};

	/**
		What a server should do after the evaluation of the preconditions.
	*/
	enum PreconditionResult
	{
		/**
			Continue the processing of the request: the answer 200, 206 or 416.
		*/
		PR_PROCEED,

		/**
			Answer 304 Not Modified (only for GET and HEAD).
		*/
		PR_NOT_MODIFIED,

		/**
			Answer 412 Precondition Failed.
		*/
		PR_PRECONDITION_FAILED,

		/**
			Answer 400 Bad Request: \c If-Match or \c If-None-Match is not a valid list of entity tags.
		*/
		PR_BAD_REQUEST
	};

	/**
		Result of the comparison of an entity tag with the list of entity tags of a header field.
	*/
	enum EntityTagMatch
	{
		ETM_NO_MATCH,
		ETM_MATCH,

		/**
			The value is not \c * and not a valid list of entity tags.
		*/
		ETM_INVALID
	};

	/**
		Parse an entity tag: an optional \c W/ and a string in quotes. The string consists of the characters
		\c 0x21, \c 0x23-0x7E and \c 0x80-0xFF, so it can contain commas but not quotes and not spaces.
		Spaces around the value are ignored.
		\param	result	Parsed entity tag. It is not changed if the value is rejected.
		\return \c true if the value is valid.
	*/
	static bool ParseEntityTag(const QByteArray& value, EntityTag& result);

	/**
		Create the value of the \c ETag header field.
		\return The value with the quotes or an empty array if the opaque tag contains a character that is not allowed.
	*/
	static QByteArray CreateEntityTag(const QByteArray& opaqueTag, bool isWeak = false);

	/**
		Compare the entity tag of the representation with the value of \c If-Match or \c If-None-Match
		(RFC 9110, section 8.8.3.2).
		The value \c * matches any representation, even a representation without an entity tag. The list is checked
		to the end even after a match: a bad element makes the whole value invalid.
		\param	listValue			Value of the header field: \c * or a list of entity tags.
		\param	eTag				Value of the \c ETag header field of the representation or an empty array.
		\param	useStrongComparison	\c true for \c If-Match: both tags must be strong and equal. \c false for
									\c If-None-Match: the tags are equal, the weak prefix is ignored.
	*/
	static EntityTagMatch MatchEntityTagList(const QByteArray& listValue, const QByteArray& eTag, bool useStrongComparison);

	/**
		Evaluate the preconditions in the order of RFC 9110, section 13.2.2 (the steps 1 to 4):
		1. \c If-Match: if no tag matches (strong comparison), the answer is 412.
		2. Only if \c If-Match is absent, \c If-Unmodified-Since: if the representation is changed after the date, 412.
		3. \c If-None-Match: if a tag matches (weak comparison), the answer is 304 for GET and HEAD and 412 for the other methods.
		4. Only if \c If-None-Match is absent and the method is GET or HEAD, \c If-Modified-Since: if the representation
		   is not changed after the date, 304.
		A date that is not a valid HTTP-date is ignored, as well as a date when the last change of the representation
		is unknown. \c If-Range and \c Range are processed after this method, see \c EvaluateIfRange().
		\param	method			Request method in upper case, for example \c "GET".
	*/
	static PreconditionResult EvaluatePreconditions(const QByteArray& method, const Preconditions& preconditions, const Validators& validators);

	/**
		Check the \c If-Range header field (RFC 9110, section 13.1.5). It should be called only for a GET request
		that has the \c Range header field and the \c If-Range header field.
		\param	ifRange	Value of the header field: a strong entity tag or an HTTP-date.
		\return \c true if the representation is not changed and the \c Range header field should be processed.
				\c false if the \c Range header field must be ignored and the complete representation sent with 200.
				That includes the weak tags, the dates that are not equal to the time of the last change, and the values
				that are not valid.
	*/
	static bool EvaluateIfRange(const QByteArray& ifRange, const Validators& validators);
};


} // namespace imtrest


