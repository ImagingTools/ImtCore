#pragma once


// Qt includes
#include <QtCore/QString>
#include <QtCore/QDir>

// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <ifile/IFilePersistence.h>
#include <ifileproc/IFileConversion.h>


namespace imtfile
{


/**
	This component allows conversion between two file formats of the same document.
*/
class CFileFormatConversionComp: virtual public ifileproc::IFileConversion, public ilog::CLoggerComponentBase
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CFileFormatConversionComp);
		I_REGISTER_INTERFACE(ifileproc::IFileConversion);
		I_ASSIGN(m_persistenceCompPtr, "Persistence", "Format support", true, "Persistence");
		I_ASSIGN(m_workingObjectCompPtr, "WorkingObject", "Working object", true, "WorkingObject");
	I_END_COMPONENT

	// reimplemented (ifileproc::IFileConversion)
	virtual int ConvertFiles(
				const QString& inputPath,
				QString& outputPath,
				const iprm::IParamsSet* paramsPtr = nullptr,
				ibase::IProgressManager* progressManagerPtr = nullptr) const override;

private:
	I_REF(ifile::IFilePersistence, m_persistenceCompPtr);
	I_REF(istd::IChangeable, m_workingObjectCompPtr);
};


} // namespace imtfile


