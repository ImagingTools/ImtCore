#include <imtstyle/CDesignTokenFileParserComp.h>

// Qt includes
#include <QtCore/QRegularExpression>
#include <QtCore/QRegularExpressionMatch>
#include <QtCore/QRegularExpressionMatchIterator>


namespace imtstyle
{


const QMap<QString, QPalette::ColorGroup> CDesignTokenFileParserComp::s_colorGroupNamesMap = {
			std::make_pair("Active", QPalette::ColorGroup::Active),
			std::make_pair("Disabled", QPalette::ColorGroup::Disabled),
			std::make_pair("Inactive", QPalette::ColorGroup::Inactive),
			std::make_pair("NColorGroups", QPalette::ColorGroup::NColorGroups),
			std::make_pair("Current", QPalette::ColorGroup::Current),
			std::make_pair("All", QPalette::ColorGroup::All),
			std::make_pair("Normal", QPalette::ColorGroup::Normal)
};


const QMap<QString, QPalette::ColorRole>CDesignTokenFileParserComp::s_colorRolesNamesMap = {
			std::make_pair("WindowText", QPalette::ColorRole::WindowText),
			std::make_pair("Button", QPalette::ColorRole::Button),
			std::make_pair("Light", QPalette::ColorRole::Light),
			std::make_pair("Midlight", QPalette::ColorRole::Midlight),
			std::make_pair("Dark", QPalette::ColorRole::Dark),
			std::make_pair("Mid", QPalette::ColorRole::Mid),
			std::make_pair("Text", QPalette::ColorRole::Text),
			std::make_pair("BrightText", QPalette::ColorRole::BrightText),
			std::make_pair("ButtonText", QPalette::ColorRole::ButtonText),
			std::make_pair("Base", QPalette::ColorRole::Base),
			std::make_pair("Window", QPalette::ColorRole::Window),
			std::make_pair("Shadow", QPalette::ColorRole::Shadow),
			std::make_pair("Highlight", QPalette::ColorRole::Highlight),
			std::make_pair("HighlightedText", QPalette::ColorRole::HighlightedText),
			std::make_pair("Link", QPalette::ColorRole::Link),
			std::make_pair("LinkVisited", QPalette::ColorRole::LinkVisited),
			std::make_pair("AlternateBase", QPalette::ColorRole::AlternateBase),
			std::make_pair("NoRole", QPalette::ColorRole::NoRole),
			std::make_pair("ToolTipBase", QPalette::ColorRole::ToolTipBase),
			std::make_pair("ToolTipText", QPalette::ColorRole::ToolTipText),
			std::make_pair("PlaceholderText", QPalette::ColorRole::PlaceholderText),
			std::make_pair("Foreground", QPalette::ColorRole::Foreground),
			std::make_pair("Background", QPalette::ColorRole::Background)
};


QString CDesignTokenFileParserComp::GetColorName(QPalette::ColorGroup group, QPalette::ColorRole role) const
{
	return CDesignTokenFileParserComp::s_colorGroupNamesMap.key(group) + CDesignTokenFileParserComp::s_colorRolesNamesMap.key(role);
}


bool CDesignTokenFileParserComp::GetColorRoleGroup(const QString& name, QPalette::ColorGroup& group, QPalette::ColorRole& role) const
{
	const QStringList& groupNames = CDesignTokenFileParserComp::s_colorGroupNamesMap.keys();

	for(const QString& groupName: groupNames){

		QRegularExpression groupRegEx(QString("^" + groupName), QRegularExpression::PatternOption::CaseInsensitiveOption);
		QRegularExpressionMatchIterator globalMatch = groupRegEx.globalMatch(name);

		if(globalMatch.hasNext()){

			QRegularExpressionMatch groupRegExMatch;
			groupRegExMatch = globalMatch.next();

			if(!s_colorGroupNamesMap.contains(groupName)) {
				return false;
			}

			group = s_colorGroupNamesMap[groupName];

			QString roleName = name;
			roleName.remove(groupRegExMatch.capturedStart(), groupRegExMatch.capturedLength());

			if(!s_colorRolesNamesMap.contains(roleName)) {
				return false;
			}

			role = s_colorRolesNamesMap[roleName];
			return true;
		}
	}
	return false;
}


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


bool CDesignTokenFileParserComp::ParseFile()
{
	QFile designTokenFile;

	if (m_designTokenFileInfo.isReadable()){
		designTokenFile.setFileName(m_designTokenFileInfo.absoluteFilePath());
	}
	else {
		designTokenFile.setFileName(m_designTokenFilePathAttrPtr->GetPath());
	}

	if (!designTokenFile.open(QFile::ReadOnly)){
		qCritical() << "Cannot read file" << ::qPrintable(designTokenFile.fileName());
		Q_ASSERT(0);
		return false;
	}


	QJsonObject designTokenObject = QJsonDocument::fromJson(designTokenFile.readAll()).object();
	if(designTokenObject.isEmpty()) {
		qCritical() << "Cannot parse JSON";
		return false;
	}

	m_templateIconColor = designTokenObject["TemplateIconColor"].toString().toUtf8();

	QJsonArray designTokenStylesArray = designTokenObject["Styles"].toArray();
	if(designTokenStylesArray.isEmpty()) {
		qCritical() << "Cannot parse Styles";
		return false;
	}

	for (const QJsonValue& style: ::qAsConst(designTokenStylesArray)){

		QJsonObject styleEntry = style.toObject();
		QString styleName = styleEntry["StyleName"].toString();

		if(!styleName.length()){
			qInfo() << "Skipping invalid object";
			continue;
		}

		QJsonObject colorsObject = styleEntry["IconColor"].toObject();
		if(colorsObject.isEmpty()){
			qInfo() << "Skipping empty object";
			continue;
		}

		QVariantMap colorsMap = colorsObject.toVariantMap();
		m_iconColors.insert(styleName, colorsMap);
		m_designSchemaList.InsertItem(styleName.toUtf8(), styleName,"");
		this->GetPaletteFromEntry(styleName, styleEntry["PaletteColor"]);
	}
	return true;
}

const imtbase::ICollectionInfo& CDesignTokenFileParserComp::GetDesignSchemaList() const
{
	return m_designSchemaList;
}


bool CDesignTokenFileParserComp::GetColorPalette(const QByteArray& designSchemaId, QPalette& palette) const
{
	palette = m_stylesPalettes[designSchemaId];
	return true;
}


QByteArray imtstyle::CDesignTokenFileParserComp::GetTemplateIconColor(const QByteArray&) const
{
	return m_templateIconColor;
}


QByteArray imtstyle::CDesignTokenFileParserComp::GetOffNormalColor(const QByteArray& styleName) const
{
	return m_iconColors[styleName].toMap()[s_offNormalColorParamName].toByteArray();
}


QByteArray imtstyle::CDesignTokenFileParserComp::GetOffDisabledColor(const QByteArray& styleName) const
{
	return m_iconColors[styleName].toMap()[s_offDisabledColorParamName].toByteArray();
}


QByteArray imtstyle::CDesignTokenFileParserComp::GetOffActiveColor(const QByteArray& styleName) const
{
	return m_iconColors[styleName].toMap()[s_offActiveColorParamName].toByteArray();
}


QByteArray imtstyle::CDesignTokenFileParserComp::GetOffSelectedColor(const QByteArray& styleName) const
{
	return m_iconColors[styleName].toMap()[s_offSelectedColorParamName].toByteArray();
}


QByteArray imtstyle::CDesignTokenFileParserComp::GetOnNormalColor(const QByteArray& styleName) const
{
	return m_iconColors[styleName].toMap()[s_onNormalColorParamName].toByteArray();
}


QByteArray imtstyle::CDesignTokenFileParserComp::GetOnDisabledColor(const QByteArray& styleName) const
{
	return m_iconColors[styleName].toMap()[s_onDisabledColorParamName].toByteArray();
}


QByteArray imtstyle::CDesignTokenFileParserComp::GetOnActiveColor(const QByteArray& styleName) const
{
	return m_iconColors[styleName].toMap()[s_onActiveColorParamName].toByteArray();
}


QByteArray imtstyle::CDesignTokenFileParserComp::GetOnSelectedColor(const QByteArray& styleName) const
{
	return m_iconColors[styleName].toMap()[s_onSelectedColorParamName].toByteArray();
}


// reimplemented (ilog::CLoggerComponentBase)
void CDesignTokenFileParserComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();
}


void CDesignTokenFileParserComp::GetPaletteFromEntry(const QString& styleName, const QJsonValue& paletteEntry)
{
	QPalette palette;

	const QJsonObject& paletteEntryObject = paletteEntry.toObject();

	for(QJsonObject::const_iterator value = paletteEntryObject.constBegin(); value != paletteEntryObject.constEnd(); ++value){
		QPalette::ColorGroup colorGroup;
		QPalette::ColorRole colorRole;
		this->GetColorRoleGroup(value.key(), colorGroup, colorRole);

		QColor color;
		color.setNamedColor(value->toString());
		if(color.isValid()){
			palette.setColor(colorGroup, colorRole, color);
		}
	}

	m_stylesPalettes.insert(styleName, palette);
}



} // namespace imtstyle


