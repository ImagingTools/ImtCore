#include <imtstyle/CDesignTokenFileParserComp.h>


namespace imtstyle
{



// public methods
bool CDesignTokenFileParserComp::SetDesignTokenFilePath(const QByteArray& designTokenFilePath)
{
	bool retval = false;

	QFileInfo designTokenFileInfo(designTokenFilePath);

	retval = designTokenFileInfo.isReadable();

	if(retval){
		m_designTokenFileInfo = designTokenFileInfo;
	}

	return retval;
}


bool CDesignTokenFileParserComp::ParseFile()
{
	QFile designTokenFile;

	if (m_designTokenFileInfo.isReadable()){
		designTokenFile.setFileName(m_designTokenFileInfo.absoluteFilePath());
	}
	else {
		designTokenFile.setFileName(m_designTokenFilePathAttrPtr->GetPath());
	}

	if (!designTokenFile.isReadable()){
		qCritical() << "Cannot read file";
		return false;
	}

	Q_ASSERT(designTokenFile.open(QFile::ReadOnly));

	QJsonObject designTokenObject = QJsonDocument::fromJson(designTokenFile.readAll()).object();
	if(designTokenObject.isEmpty()) {
		qCritical() << "Cannot parse JSON";
		return false;
	}

	QJsonArray designTokenStylesArray = designTokenObject["Styles"].toArray();
	if(designTokenStylesArray.isEmpty()) {
		qCritical() << "Cannot parse Styles";
		return false;
	}


	return true;
}

// reimplemented (ilog::CLoggerComponentBase)
void CDesignTokenFileParserComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

}







} // namespace imtstyle


