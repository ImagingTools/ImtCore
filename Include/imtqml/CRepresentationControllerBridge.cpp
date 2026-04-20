// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtqml/CRepresentationControllerBridge.h>


namespace imtqml
{


CRepresentationControllerBridge::CRepresentationControllerBridge() = default;


CRepresentationControllerBridge::~CRepresentationControllerBridge() = default;


// reimplemented (IRepresentationControllerBridge)

void CRepresentationControllerBridge::RegisterRepresentationController(const QString& documentId, QObject* controller)
{
	if (documentId.isEmpty()){
		return;
	}
	m_representationControllers.insert(documentId, QPointer<QObject>(controller));
}


void CRepresentationControllerBridge::UnregisterRepresentationController(const QString& documentId)
{
	m_representationControllers.remove(documentId);
}


QObject* CRepresentationControllerBridge::GetRepresentationController(const QString& documentId) const
{
	const auto it = m_representationControllers.constFind(documentId);
	if (it == m_representationControllers.constEnd()){
		return nullptr;
	}
	return it.value().data();
}


QStringList CRepresentationControllerBridge::RegisteredRepresentationControllerIds() const
{
	QStringList ids;
	ids.reserve(m_representationControllers.size());
	for (auto it = m_representationControllers.constBegin(); it != m_representationControllers.constEnd(); ++it){
		if (it.value()){
			ids.append(it.key());
		}
	}
	return ids;
}


} // namespace imtqml
