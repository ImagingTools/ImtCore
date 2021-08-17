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
	QByteArray destinationPath;
	QByteArray destinationColor;
};


class IDesignTokenFileParser
{
public:
	/**
		Contains info about each file that should be preceed. The key of this map is a source file path. The value(s) of this map is info(s)? where should be placed converted files with same color
	*/
	typedef QMultiMap<QByteArray, DesignTokenImageTargetState> DesignTokenImageFileInfo;
	virtual bool SetFile(const QByteArray& filePath) = 0;
	virtual bool ParseFile() = 0;
	virtual DesignTokenImageFileInfo GetFiles() const = 0;

};


} // namespace imtstyle
