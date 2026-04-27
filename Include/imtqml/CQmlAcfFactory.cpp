// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtqml/CQmlAcfFactory.h>


namespace imtqml
{


CQmlAcfFactory::CQmlAcfFactory(QObject* parent)
	: QObject(parent)
{
}


const QString& CQmlAcfFactory::GetFactoryId() const
{
	return m_factoryId;
}


void CQmlAcfFactory::SetFactoryId(const QString& factoryId)
{
	if (m_factoryId != factoryId){
		m_factoryId = factoryId;
		Q_EMIT factoryIdChanged();
	}
}


const QString& CQmlAcfFactory::GetInterfaceName() const
{
	return m_interfaceName;
}


void CQmlAcfFactory::SetInterfaceName(const QString& interfaceName)
{
	if (m_interfaceName != interfaceName){
		m_interfaceName = interfaceName;
		Q_EMIT interfaceNameChanged();
	}
}


const QUrl& CQmlAcfFactory::GetSourceUrl() const
{
	return m_sourceUrl;
}


void CQmlAcfFactory::SetSourceUrl(const QUrl& sourceUrl)
{
	if (m_sourceUrl != sourceUrl){
		m_sourceUrl = sourceUrl;
		Q_EMIT sourceUrlChanged();
	}
}


const QString& CQmlAcfFactory::GetDescription() const
{
	return m_description;
}


void CQmlAcfFactory::SetDescription(const QString& description)
{
	if (m_description != description){
		m_description = description;
		Q_EMIT descriptionChanged();
	}
}


} // namespace imtqml
