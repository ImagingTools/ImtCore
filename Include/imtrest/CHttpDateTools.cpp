// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <array>
#include <imtrest/CHttpDateTools.h>


// Qt includes
#include <QtCore/QLocale>
#include <QtCore/QTimeZone>


namespace
{


const int s_dayCount = 7;
const int s_monthCount = 12;

const std::array<const char*, s_dayCount> s_dayNames = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};
const std::array<const char*, s_dayCount> s_longDayNames = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};
const std::array<const char*, s_monthCount> s_monthNames = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};


char CharAt(const QByteArray& value, qsizetype position)
{
	return ((position >= 0) && (position < value.size())) ? value.at(position) : '\0';
}


// Exactly count digits, no signs and no spaces
bool ReadNumber(const QByteArray& value, qsizetype position, int count, int& number)
{
	number = 0;

	for (int i = 0; i < count; ++i){
		const char c = CharAt(value, position + i);
		if ((c < '0') || (c > '9')){
			return false;
		}

		number = number * 10 + (c - '0');
	}

	return true;
}


// Position of the name in the list or -1
int FindName(const QByteArray& name, const char* const* names, int count)
{
	for (int i = 0; i < count; ++i){
		if (name == names[i]){
			return i;
		}
	}

	return -1;
}


// "HH:MM:SS"
bool ReadTime(const QByteArray& value, qsizetype position, int& hour, int& minute, int& second)
{
	return ReadNumber(value, position, 2, hour) && (CharAt(value, position + 2) == ':')
			&& ReadNumber(value, position + 3, 2, minute) && (CharAt(value, position + 5) == ':')
			&& ReadNumber(value, position + 6, 2, second);
}


QDateTime CreateTime(int year, int month, int day, int hour, int minute, int second)
{
	const QDate date(year, month, day);

	// The second 60 is a leap second
	if (!date.isValid() || (hour > 23) || (minute > 59) || (second > 60)){
		return {};
	}

	return QDateTime(date, QTime(hour, minute, 0), QTimeZone::UTC).addSecs(second);
}


// "Sun, 06 Nov 1994 08:49:37 GMT"
QDateTime ParseImfFixdate(const QByteArray& value)
{
	int day = 0;
	int year = 0;
	int hour = 0;
	int minute = 0;
	int second = 0;

	const int month = FindName(value.mid(8, 3), s_monthNames.data(), s_monthCount);

	const bool isValid = (value.size() == 29)
			&& (FindName(value.left(3), s_dayNames.data(), s_dayCount) >= 0)
			&& (value.mid(3, 2) == ", ")
			&& ReadNumber(value, 5, 2, day)
			&& (value.at(7) == ' ')
			&& (month >= 0)
			&& (value.at(11) == ' ')
			&& ReadNumber(value, 12, 4, year)
			&& (value.at(16) == ' ')
			&& ReadTime(value, 17, hour, minute, second)
			&& (value.mid(25) == " GMT");

	return isValid ? CreateTime(year, month + 1, day, hour, minute, second) : QDateTime();
}


// "Sunday, 06-Nov-94 08:49:37 GMT"
QDateTime ParseRfc850Date(const QByteArray& value, const QDateTime& now)
{
	const qsizetype comma = value.indexOf(',');
	if ((comma < 0) || (FindName(value.left(comma), s_longDayNames.data(), s_dayCount) < 0) || (CharAt(value, comma + 1) != ' ')){
		return {};
	}

	const QByteArray rest = value.mid(comma + 2);

	int day = 0;
	int shortYear = 0;
	int hour = 0;
	int minute = 0;
	int second = 0;

	const int month = FindName(rest.mid(3, 3), s_monthNames.data(), s_monthCount);

	const bool isValid = (rest.size() == 22)
			&& ReadNumber(rest, 0, 2, day)
			&& (rest.at(2) == '-')
			&& (month >= 0)
			&& (rest.at(6) == '-')
			&& ReadNumber(rest, 7, 2, shortYear)
			&& (rest.at(9) == ' ')
			&& ReadTime(rest, 10, hour, minute, second)
			&& (rest.mid(18) == " GMT");
	if (!isValid){
		return {};
	}

	// The latest year with the same last two digits that is not more than 50 years ahead (RFC 9110, section 5.6.7)
	const QDateTime limit = now.addYears(50);

	for (int year = (now.toUTC().date().year() / 100) * 100 + shortYear + 100; year > 0; year -= 100){
		const QDateTime candidate = CreateTime(year, month + 1, day, hour, minute, second);
		if (candidate.isValid() && (candidate <= limit)){
			return candidate;
		}
	}

	return {};
}


// "Sun Nov  6 08:49:37 1994"
QDateTime ParseAsctimeDate(const QByteArray& value)
{
	int day = 0;
	int year = 0;
	int hour = 0;
	int minute = 0;
	int second = 0;

	const int month = FindName(value.mid(4, 3), s_monthNames.data(), s_monthCount);

	// The day has two characters: two digits or a space and a digit
	const bool isDayValid = (CharAt(value, 8) == ' ') ? ReadNumber(value, 9, 1, day) : ReadNumber(value, 8, 2, day);

	const bool isValid = (value.size() == 24)
			&& (FindName(value.left(3), s_dayNames.data(), s_dayCount) >= 0)
			&& (value.at(3) == ' ')
			&& (month >= 0)
			&& (value.at(7) == ' ')
			&& isDayValid
			&& (value.at(10) == ' ')
			&& ReadTime(value, 11, hour, minute, second)
			&& (value.at(19) == ' ')
			&& ReadNumber(value, 20, 4, year);

	return isValid ? CreateTime(year, month + 1, day, hour, minute, second) : QDateTime();
}


} // namespace


namespace imtrest
{


// public static methods

QByteArray CHttpDateTools::ToImfFixdate(const QDateTime& dateTime)
{
	const QString date = QLocale::c().toString(dateTime.toUTC(), QStringLiteral("ddd, dd MMM yyyy HH:mm:ss")) + QStringLiteral(" GMT");

	return date.toLatin1();
}


QDateTime CHttpDateTools::FromHttpDate(const QByteArray& value, const QDateTime& now)
{
	const QByteArray trimmedValue = value.trimmed();

	// The formats differ by the first comma: the third character, later or none
	if (CharAt(trimmedValue, 3) == ','){
		return ParseImfFixdate(trimmedValue);
	}

	if (trimmedValue.contains(',')){
		return ParseRfc850Date(trimmedValue, now);
	}

	return ParseAsctimeDate(trimmedValue);
}


} // namespace imtrest


