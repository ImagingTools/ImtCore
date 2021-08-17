#pragma once


// Qt includes
#include <QtCore/QtCore>

// ACF includes


namespace imtstyle
{

/**
	The DesignTokenImageTargetState struct containing information?  where file should be placed with the color
 */
struct DesignTokenImageTargetState
{
	QByteArray sourceFile;
	QByteArray destinationPath;
	QByteArray destinationColor;
};


class IDesignTokenFileParser
{
public:
	/**
		Contains info about each file that should be preceed. The key of this map is a source file path. The value(s) of this map is info(s)? where should be placed converted files with same color
	*/
	typedef QList<DesignTokenImageTargetState> DesignTokenImageFileInfo;
	virtual bool SetFile(const QByteArray& filePath) = 0;
	virtual bool ParseFile(DesignTokenImageFileInfo* parsedImages = nullptr) = 0;
	virtual DesignTokenImageFileInfo GetFiles() const = 0;

	virtual QStringList GetStyleNames() const = 0;
	virtual QByteArray GetTemplateIconColor(const QString& styleName = QByteArray()) const = 0;
	virtual QByteArray GetOffNormalColor(const QString& styleName = QByteArray()) const = 0;
	virtual QByteArray GetOffDisabledColor(const QString& styleName = QByteArray()) const = 0;
	virtual QByteArray GetOffActiveColor(const QString& styleName = QByteArray()) const  = 0;
	virtual QByteArray GetOffSelectedColor(const QString& styleName = QByteArray()) const = 0;
	virtual QByteArray GetOnNormalColor(const QString& styleName = QByteArray()) const = 0;
	virtual QByteArray GetOnDisabledColor(const QString& styleName = QByteArray()) const = 0;
	virtual QByteArray GetOnActiveColor(const QString& styleName = QByteArray()) const = 0;
	virtual QByteArray GetOnSelectedColor(const QString& styleName = QByteArray()) const = 0;


};


} // namespace imtstyle
