// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imthype/CTaskCollectionContextDemultiplexerComp.h>


namespace imthype
{


// public methods

#if QT_VERSION >= 0x060000
CTaskCollectionContextDemultiplexerComp::CTaskCollectionContextDemultiplexerComp()
{
}
#else
CTaskCollectionContextDemultiplexerComp::CTaskCollectionContextDemultiplexerComp()
	:m_dataMutex(QMutex::Recursive)
{
}
#endif


// reimplemented (imthype::ITaskCollectionContext)

QByteArray CTaskCollectionContextDemultiplexerComp::GetTaskId() const
{
	QMutexLocker lock(&m_dataMutex);

	if (m_outputsCompPtr.IsValid() && (m_outputsCompPtr.GetCount() != 0)){
		ITaskCollectionContext* contextPtr = m_outputsCompPtr[0];
		if (contextPtr != nullptr){
			return contextPtr->GetTaskId();
		}
	}

	return QByteArray();
}


void CTaskCollectionContextDemultiplexerComp::SetTaskId(const QByteArray& taskId)
{
	QMutexLocker lock(&m_dataMutex);

	for (int i = 0; i < m_outputsCompPtr.GetCount(); ++i){
		ITaskCollectionContext* outputPtr = m_outputsCompPtr[i];
		if (outputPtr != nullptr){
			outputPtr->SetTaskId(taskId);
		}
	}
}


QByteArray CTaskCollectionContextDemultiplexerComp::GetTaskInputId() const
{
	QMutexLocker lock(&m_dataMutex);

	if (m_outputsCompPtr.IsValid() && (m_outputsCompPtr.GetCount() != 0)){
		ITaskCollectionContext* contextPtr = m_outputsCompPtr[0];
		if (contextPtr != nullptr){
			return contextPtr->GetTaskInputId();
		}
	}

	return QByteArray();
}


void CTaskCollectionContextDemultiplexerComp::SetTaskInputId(const QByteArray& taskInputId)
{
	QMutexLocker lock(&m_dataMutex);

	for (int i = 0; i < m_outputsCompPtr.GetCount(); ++i){
		ITaskCollectionContext* outputPtr = m_outputsCompPtr[i];
		if (outputPtr != nullptr){
			outputPtr->SetTaskInputId(taskInputId);
		}
	}
}


// reimplemented (iser::ISerializable)

bool CTaskCollectionContextDemultiplexerComp::Serialize(iser::IArchive& /*archive*/)
{
	return false;
}


} // namespace imthype


