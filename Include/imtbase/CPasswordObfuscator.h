// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QString>
#include <QtCore/QByteArray>


namespace imtbase
{


/**
 * @brief Utility for obfuscating and deobfuscating passwords for XML serialization
 *
 * CPasswordObfuscator provides simple reversible password obfuscation to prevent
 * storage of plain-text passwords in XML configuration files. Obfuscated passwords
 * are prefixed with a marker so they can be distinguished from plain-text values
 * during deserialization (backward compatibility).
 *
 * @note This is not a substitute for full encryption. It prevents casual reading
 * of passwords from configuration files. For strong encryption, use the imtcrypt module.
 */
class CPasswordObfuscator
{
public:
	/**
	 * @brief Obfuscate a plain-text password for safe storage
	 * @param password The plain-text password
	 * @return Obfuscated string with prefix marker, suitable for XML storage
	 */
	static QString Obfuscate(const QString& password);

	/**
	 * @brief Deobfuscate a previously obfuscated password
	 * @param value The obfuscated string (with prefix marker)
	 * @return The original plain-text password
	 * @note If the value is not obfuscated (no prefix marker), it is returned as-is
	 *       for backward compatibility with older configuration files.
	 */
	static QString Deobfuscate(const QString& value);

	/**
	 * @brief Check whether a string is an obfuscated password
	 * @param value The string to check
	 * @return true if the value starts with the obfuscation prefix marker
	 */
	static bool IsObfuscated(const QString& value);

private:
	static QByteArray Transform(const QByteArray& data);

	static const char* s_prefix;
	static const quint8 s_key[];
	static const int s_keyLength;
};


} // namespace imtbase

