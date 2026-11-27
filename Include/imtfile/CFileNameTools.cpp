// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtfile/CFileNameTools.h>


// Qt includes
#include <QtCore/QStringView>


namespace imtfile
{
namespace
{


// The longest name ext4 and APFS take, in bytes of UTF-8
constexpr qsizetype s_maxNameBytes = 255;

// The characters NTFS does not take in a name; '/' separates the folders on all of the systems
constexpr QStringView s_forbiddenCharacters = u"<>:\"/\\|?*";


// A device name of Windows: no file can have it, also with an extension ("CON", "con.txt", "CON .txt")
bool IsReservedName(const QString& name)
{
	QString baseName = name.left(name.indexOf(QLatin1Char('.')));
	while (baseName.endsWith(QLatin1Char(' '))){
		baseName.chop(1);
	}
	baseName = baseName.toUpper();

	if (baseName == QLatin1String("CON") || baseName == QLatin1String("PRN") || baseName == QLatin1String("AUX") || baseName == QLatin1String("NUL")){
		return true;
	}

	// COM0-COM9 and LPT0-LPT9, with the superscripts 1, 2 and 3 as well
	if (baseName.size() == 4 && (baseName.startsWith(QLatin1String("COM")) || baseName.startsWith(QLatin1String("LPT")))){
		const QChar number = baseName.at(3);
		const bool isDigit = number >= QLatin1Char('0') && number <= QLatin1Char('9');
		const bool isSuperscript = number == QChar(u'\u00B9') || number == QChar(u'\u00B2') || number == QChar(u'\u00B3');

		return isDigit || isSuperscript;
	}

	return false;
}


// Why the name cannot be used; empty for a valid name
QString GetNameError(const QString& name)
{
	if (name.isEmpty()){
		return QStringLiteral("The name is empty");
	}

	if (name == QLatin1String(".") || name == QLatin1String("..")){
		return QStringLiteral("The name '%1' is not allowed").arg(name);
	}

	if (name.toUtf8().size() > s_maxNameBytes){
		return QStringLiteral("The name is longer than %1 bytes in UTF-8").arg(s_maxNameBytes);
	}

	for (qsizetype index = 0; index < name.size(); ++index){
		const QChar character = name.at(index);
		if (s_forbiddenCharacters.contains(character)){
			return QStringLiteral("The name contains the character '%1', which is not allowed").arg(character);
		}

		if (character.category() == QChar::Other_Control){
			return QStringLiteral("The name contains a control character");
		}

		if (character.isHighSurrogate() && index + 1 < name.size() && name.at(index + 1).isLowSurrogate()){
			++index;

			continue;
		}

		if (character.isSurrogate()){
			return QStringLiteral("The name contains an unpaired surrogate");
		}
	}

	if (name.endsWith(QLatin1Char('.')) || name.endsWith(QLatin1Char(' '))){
		return QStringLiteral("The name ends with a dot or a space");
	}

	if (IsReservedName(name)){
		return QStringLiteral("The name '%1' is reserved for a device on Windows").arg(name);
	}

	return QString();
}


} // namespace


// public static methods

bool CFileNameTools::CheckFileName(const QString& name, QString& error)
{
	error = GetNameError(name);

	return error.isEmpty();
}


} // namespace imtfile
