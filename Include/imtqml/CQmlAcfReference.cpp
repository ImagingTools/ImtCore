// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtqml/CQmlAcfReference.h>


namespace imtqml
{


CQmlAcfReference::CQmlAcfReference(QObject* parent)
	: QObject(parent)
{
}


const QString& CQmlAcfReference::GetRefId() const
{
	return m_refId;
}


void CQmlAcfReference::SetRefId(const QString& refId)
{
	if (m_refId != refId){
		m_refId = refId;
		Q_EMIT refIdChanged();
	}
}


const QString& CQmlAcfReference::GetInterfaceName() const
{
	return m_interfaceName;
}


void CQmlAcfReference::SetInterfaceName(const QString& interfaceName)
{
	if (m_interfaceName != interfaceName){
		m_interfaceName = interfaceName;
		Q_EMIT interfaceNameChanged();
	}
}


bool CQmlAcfReference::IsRequired() const
{
	return m_required;
}


void CQmlAcfReference::SetRequired(bool required)
{
	if (m_required != required){
		m_required = required;
		Q_EMIT requiredChanged();
	}
}


const QString& CQmlAcfReference::GetTargetId() const
{
	return m_targetId;
}


void CQmlAcfReference::SetTargetId(const QString& targetId)
{
	if (m_targetId != targetId){
		m_targetId = targetId;
		Q_EMIT targetIdChanged();
	}
}


QObject* CQmlAcfReference::GetTarget() const
{
	return m_target;
}


void CQmlAcfReference::SetTarget(QObject* target)
{
	if (m_target != target){
		m_target = target;
		Q_EMIT targetChanged();
	}
}


const QString& CQmlAcfReference::GetDescription() const
{
	return m_description;
}


void CQmlAcfReference::SetDescription(const QString& description)
{
	if (m_description != description){
		m_description = description;
		Q_EMIT descriptionChanged();
	}
}


bool CQmlAcfReference::isResolved() const
{
	return m_target != nullptr;
}


} // namespace imtqml
