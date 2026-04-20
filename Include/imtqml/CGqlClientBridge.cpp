// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtqml/CGqlClientBridge.h>


namespace imtqml
{


CGqlClientBridge* CGqlClientBridge::s_instancePtr = nullptr;


// public methods

CGqlClientBridge::CGqlClientBridge(QObject* parent)
	:QObject(parent),
	BaseClass()
{
}


CGqlClientBridge::~CGqlClientBridge() = default;


CGqlClientBridge* CGqlClientBridge::Instance()
{
	return s_instancePtr;
}


// public methods (representation controller registry)

void CGqlClientBridge::registerRepresentationController(const QString& documentId, QObject* controller)
{
	if (documentId.isEmpty()){
		return;
	}
	m_representationControllers.insert(documentId, QPointer<QObject>(controller));
	Q_EMIT representationControllerRegistered(documentId, controller);
}


void CGqlClientBridge::unregisterRepresentationController(const QString& documentId)
{
	if (m_representationControllers.remove(documentId) > 0){
		Q_EMIT representationControllerUnregistered(documentId);
	}
}


QObject* CGqlClientBridge::getRepresentationController(const QString& documentId) const
{
	const auto it = m_representationControllers.constFind(documentId);
	if (it == m_representationControllers.constEnd()){
		return nullptr;
	}
	return it.value().data();
}


QStringList CGqlClientBridge::registeredRepresentationControllerIds() const
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


// protected methods

// reimplemented (icomp::CComponentBase)

void CGqlClientBridge::OnComponentCreated()
{
	BaseClass::OnComponentCreated();
	s_instancePtr = this;
}


void CGqlClientBridge::OnComponentDestroyed()
{
	if (s_instancePtr == this){
		s_instancePtr = nullptr;
	}
	BaseClass::OnComponentDestroyed();
}


} // namespace imtqml
