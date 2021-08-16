#pragma once


// Qt includes
#include <QtCore/QtCore>

// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <ifile/IFileNameParam.h>
#include <iprm/IParamsSet.h>

// ImtCore includes


namespace imtstyle
{

class CDesignTokenFileParserComp: public ilog::CLoggerComponentBase
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CDesignTokenFileParserComp);
		I_ASSIGN(m_designTokenFilePathAttrPtr, "designTokenFilePath", "Design token file path", false, "IFileNameParam")
	I_END_COMPONENT;

	// public methods
	bool SetDesignTokenFilePath(const QByteArray& designTokenFilePath);
	bool ParseFile();


protected:
	// reimplemented (ilog::CLoggerComponentBase)
	virtual void OnComponentCreated() override;

private:



private:
	I_REF(ifile::IFileNameParam, m_designTokenFilePathAttrPtr);

	QFileInfo m_designTokenFileInfo;

};


} // namespace imtstyle
