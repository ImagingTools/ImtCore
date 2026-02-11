// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtbase/CQtResourceLoaderComp.h>


// Qt includes
#include <QtCore/QFile>

// ACF includes
#include <iser/CCompactXmlMemReadArchive.h>


namespace imtbase
{


// reimplemented (icomp::CComponentBase)

void CQtResourceLoaderComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_qrcPathAttrPtr.IsValid() && m_targetObjectCompPtr.IsValid()){
		QByteArray path = m_qrcPathAttrPtr->GetValue();
		QFile resourceFile(path);
		if (resourceFile.open(QIODevice::ReadOnly)){
			QByteArray data = resourceFile.readAll();
			resourceFile.close();

			iser::CCompactXmlMemReadArchive archive(data);

			if (!m_targetObjectCompPtr->Serialize(archive)){
				SendErrorMessage(0, QString("Unable to load resource '%1'. Error: Serialization failed").arg(qPrintable(path)), QString("CQtResourceLoaderComp"));
			}
		}
	}
}


} // namespace imtbase


