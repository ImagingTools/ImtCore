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

			m_targetObjectCompPtr->Serialize(archive);
		}
	}
}


} // namespace imtbase


