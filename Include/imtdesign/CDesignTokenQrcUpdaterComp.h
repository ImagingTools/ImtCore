#pragma once


// Qt includes
#include <QtCore/QtCore>

// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <iprm/IParamsSet.h>

// ImtCore includes
#include <imtdesign/IDesignTokenProcessor.h>
#include <imtdesign/IDesignTokenArgumentsParser.h>
#include <imtdesign/IDesignTokenFileParser.h>
#include <imtdesign/IDesignTokenQrcUpdater.h>


namespace imtdesign
{


class CDesignTokenQrcUpdaterComp: public ilog::CLoggerComponentBase, public IDesignTokenQrcUpdater, public IDesignTokenProcessor
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CDesignTokenQrcUpdaterComp);
		I_REGISTER_INTERFACE(IDesignTokenQrcUpdater);
		I_REGISTER_INTERFACE(IDesignTokenProcessor);
		I_ASSIGN(m_argumentParserAttrPtr, "ArgumentsParser", "Arguments Parser", true, "IDesignTokenArgumentsParser")
		I_ASSIGN(m_designTokenFileParserAttrPtr, "DesignTokenFileParser", "Design token file parser", true, "IDesignTokenFileParser")

	I_END_COMPONENT;

	// reimplemented (IDesignTokenProcessor)
	int Exec() override;
	QByteArray GetHelpString()const override;

	// reimplemented (IDesignTokenQrcUpdater)
	bool CreateQrc(const QString& prefix, const QString& path, const QFileInfoList& files, const QByteArray& outputFileName) const override;
	bool CreateQrcForDirs(const QStringList& inputDirNames, const QByteArray& outputFileName) const override;
	bool CreateQrcRecursivly(const QString& inputDirName, const QByteArray& outputFileName) const override;
	bool ModifyQrc(const QString& prefix, const QString& path, const QFileInfoList& files, const QByteArray& outputFileName) const override;


private:
	bool CreateSingleEntry(const QString& prefix, const QString& inputDirPath, const QString& path, QXmlStreamWriter& xmlWriter) const;
	bool CreateSingleEntryInCurrentPrefix(const QString& inputDirPath, const QString& path, QXmlStreamWriter& xmlWriter) const;
	QString GetPathOverFile(const QString& dir, const QString& file) const;

private:
	I_REF(IDesignTokenArgumentsParser, m_argumentParserAttrPtr);
	I_REF(IDesignTokenFileParser, m_designTokenFileParserAttrPtr);

};


} // namespace imtdesign

