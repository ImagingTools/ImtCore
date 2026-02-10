// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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


class CDesignTokenIconQmlGeneratorComp: public ilog::CLoggerComponentBase, public IDesignTokenQrcUpdater, public IDesignTokenProcessor
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CDesignTokenIconQmlGeneratorComp);
		I_REGISTER_INTERFACE(IDesignTokenQrcUpdater);
		I_REGISTER_INTERFACE(IDesignTokenProcessor);
		I_ASSIGN(m_argumentParserCompPtr, "ArgumentsParser", "Arguments Parser", true, "IDesignTokenArgumentsParser")
	I_END_COMPONENT;

	// reimplemented (IDesignTokenProcessor)
	virtual int Exec() override;
	virtual QByteArray GetHelpString()const override;

	// reimplemented (IDesignTokenQrcUpdater)
	virtual bool CreateQrc(const QString& prefix, const QString& path, const QFileInfoList& files, const QByteArray& outputFileName) const override;
	virtual bool CreateQrcForDirs(const QStringList& inputDirNames, const QByteArray& outputFileName) const override;
	virtual bool CreateQrcRecursivly(const QString& inputDirName, const QByteArray& outputFileName) const override;
	virtual bool ModifyQrc(const QString& prefix, const QString& path, const QFileInfoList& files, const QByteArray& outputFileName) const override;


private:
	bool CreateSingleEntry(const QString& prefix, const QString& inputDirPath, const QString& path, QXmlStreamWriter& xmlWriter) const;
	bool CreateSingleEntryInCurrentPrefix(const QString& inputDirPath, const QString& path, QXmlStreamWriter& xmlWriter) const;
	QString GetPathOverFile(const QString& dir, const QString& file) const;

private:
	I_REF(IDesignTokenArgumentsParser, m_argumentParserCompPtr);
};


} // namespace imtdesign

