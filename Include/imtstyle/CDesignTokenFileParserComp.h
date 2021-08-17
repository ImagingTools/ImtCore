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



protected:
	// reimplemented (ilog::CLoggerComponentBase)
	virtual void OnComponentCreated() override;

private:



private:
	I_REF(ifile::IFileNameParam, m_designTokenFilePathAttrPtr);

	CDesignTokenFileParserComp::DesignTokenImageFileInfo m_parsedImagesInfo;
	QFileInfo m_designTokenFileInfo;

};


} // namespace imtstyle
