#include <imtlog/CExecutionTimeChecker.h>


namespace imtlog
{


// public methods

CExecutionTimeChecker::CExecutionTimeChecker(const QString& text)
{
	qDebug() << QString("Start execution time check [%1]").arg(text);
	m_time.start();
	m_text = text;
}

CExecutionTimeChecker::~CExecutionTimeChecker()
{
	qDebug() << QString("End execution time check [%1]. Elapsed %2 msec ").arg(m_text).arg(m_time.elapsed());
}


} // namespace imtlog


