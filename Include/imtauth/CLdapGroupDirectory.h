// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QByteArrayList>


namespace imtauth
{


class CLdapGroupDirectory
{
public:
	// Keys are immutable AD security identifiers, values are display names.
	static bool DiscoverGroups(const QString& pattern, QMap<QByteArray, QString>& groups);
	static bool GetSecurityGroups(const QByteArray& userSid, QByteArrayList& groupSids);
};


} // namespace imtauth
