// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QDebug>
#include <QtCore/QElapsedTimer>


namespace imtlog
{


class CExecutionTimeChecker
{
public:
	CExecutionTimeChecker(const QString& text);
	~CExecutionTimeChecker();

private:
	QString m_text;
	QElapsedTimer m_time;
};


} // namespace imtlog


