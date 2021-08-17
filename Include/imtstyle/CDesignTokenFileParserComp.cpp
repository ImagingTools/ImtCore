#include <imtstyle/CDesignTokenFileParserComp.h>


namespace imtstyle
{


// reimplemented (IDesignTokenFileParser)
bool CDesignTokenFileParserComp::CDesignTokenFileParserComp::SetFile(const QByteArray& filePath)
{
	bool retval = false;

	QFileInfo designTokenFileInfo(filePath);

	retval = designTokenFileInfo.isReadable();

	if(retval){
		m_designTokenFileInfo = designTokenFileInfo;
	}

	return retval;
}


bool CDesignTokenFileParserComp::ParseFile(DesignTokenImageFileInfo* parsedImages)
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

	if (parsedImages){
		*parsedImages = m_parsedImagesInfo;
	}
	return false;
}


CDesignTokenFileParserComp::DesignTokenImageFileInfo CDesignTokenFileParserComp::CDesignTokenFileParserComp::GetFiles() const
{
	return m_parsedImagesInfo;
}

QStringList imtstyle::CDesignTokenFileParserComp::GetStyleNames() const
{
	return m_stylesColors.keys();
}

QByteArray imtstyle::CDesignTokenFileParserComp::GetTemplateIconColor(const QString& styleName) const
{
}

QByteArray imtstyle::CDesignTokenFileParserComp::GetOffNormalColor(const QString& styleName) const
{
	return m_stylesColors[styleName].toMap()[s_offNormalColorParamName].toByteArray();
}

QByteArray imtstyle::CDesignTokenFileParserComp::GetOffDisabledColor(const QString& styleName) const
{
	return m_stylesColors[styleName].toMap()[s_offDisabledColorParamName].toByteArray();
}

QByteArray imtstyle::CDesignTokenFileParserComp::GetOffActiveColor(const QString& styleName) const
{
	return m_stylesColors[styleName].toMap()[s_offActiveColorParamName].toByteArray();
}

QByteArray imtstyle::CDesignTokenFileParserComp::GetOffSelectedColor(const QString& styleName) const
{
	return m_stylesColors[styleName].toMap()[s_offSelectedColorParamName].toByteArray();
}

QByteArray imtstyle::CDesignTokenFileParserComp::GetOnNormalColor(const QString& styleName) const
{
	return m_stylesColors[styleName].toMap()[s_onNormalColorParamName].toByteArray();
}

QByteArray imtstyle::CDesignTokenFileParserComp::GetOnDisabledColor(const QString& styleName) const
{
	return m_stylesColors[styleName].toMap()[s_onDisabledColorParamName].toByteArray();
}

QByteArray imtstyle::CDesignTokenFileParserComp::GetOnActiveColor(const QString& styleName) const
{
	return m_stylesColors[styleName].toMap()[s_onActiveColorParamName].toByteArray();
}

QByteArray imtstyle::CDesignTokenFileParserComp::GetOnSelectedColor(const QString& styleName) const
{
	return m_stylesColors[styleName].toMap()[s_onSelectedColorParamName].toByteArray();
}


// reimplemented (ilog::CLoggerComponentBase)
void CDesignTokenFileParserComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

}


} // namespace imtstyle




