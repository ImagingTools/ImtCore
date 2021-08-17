#pragma once


// Qt includes
#include <QtCore/QtCore>

// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <ifile/IFileNameParam.h>
#include <iprm/IParamsSet.h>

// ImtCore includes
#include <imtstyle/IDesignTokenFileParser.h>

namespace imtstyle
{

class CDesignTokenFileParserComp: public ilog::CLoggerComponentBase, public IDesignTokenFileParser
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CDesignTokenFileParserComp);
		I_REGISTER_INTERFACE(IDesignTokenFileParser);
		I_ASSIGN(m_designTokenFilePathAttrPtr, "DesignTokenFilePath", "Design token default file path", false, "IFileNameParam");
	I_END_COMPONENT;

	// reimplemented (IDesignTokenFileParser)
	virtual bool ParseFile(DesignTokenImageFileInfo* parsedImages = nullptr) override;
	virtual bool SetFile(const QByteArray& filePath) override;
	virtual DesignTokenImageFileInfo GetFiles() const override;
	virtual QStringList GetStyleNames() const override;
	virtual QByteArray GetTemplateIconColor(const QString& styleName) const override;
	virtual QByteArray GetOffNormalColor(const QString& styleName) const override;
	virtual QByteArray GetOffDisabledColor(const QString& styleName) const override;
	virtual QByteArray GetOffActiveColor(const QString& styleName) const override;
	virtual QByteArray GetOffSelectedColor(const QString& styleName) const override;
	virtual QByteArray GetOnNormalColor(const QString& styleName) const override;
	virtual QByteArray GetOnDisabledColor(const QString& styleName) const override;
	virtual QByteArray GetOnActiveColor(const QString& styleName) const override;
	virtual QByteArray GetOnSelectedColor(const QString& styleName) const override;


protected:
	// reimplemented (ilog::CLoggerComponentBase)
	virtual void OnComponentCreated() override;

private:
	constexpr static char* s_offNormalColorParamName = "offNormal";
	constexpr static char* s_offDisabledColorParamName = "offDisabled";
	constexpr static char* s_offActiveColorParamName = "offActive";
	constexpr static char* s_offSelectedColorParamName = "offSelected";
	constexpr static char* s_onNormalColorParamName = "onNormal";
	constexpr static char* s_onDisabledColorParamName = "onDisabled";
	constexpr static char* s_onActiveColorParamName = "onActive";
	constexpr static char* s_onSelectedColorParamName = "onSelected";



private:
	I_REF(ifile::IFileNameParam, m_designTokenFilePathAttrPtr);

	CDesignTokenFileParserComp::DesignTokenImageFileInfo m_parsedImagesInfo;
	QFileInfo m_designTokenFileInfo;

	/**
		m_stylesColors[styleName].toMap()[colorName].toByteArray();
	 */
	QVariantMap m_stylesColors;

};


} // namespace imtstyle
