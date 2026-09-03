// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtlog/CExecutionTimeChecker.h>


namespace imtlog
{


// public methods

CExecutionTimeChecker::CExecutionTimeChecker(const QString& text)
{
	qDebug() << QStringLiteral("Start execution time check [%1]").arg(text);
	m_time.start();
	m_text = text;
}

CExecutionTimeChecker::~CExecutionTimeChecker()
{
	qDebug() << QStringLiteral("End execution time check [%1]. Elapsed %2 msec ").arg(m_text, QString::number(m_time.elapsed()));
}


} // namespace imtlog


