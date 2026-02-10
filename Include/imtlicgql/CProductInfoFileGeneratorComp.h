// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ifile/IFilePersistence.h>
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtlic/CProductInfo.h>


namespace imtlicgql
{


class CProductInfoFileGeneratorComp: virtual public ifile::IFilePersistence, public ilog::CLoggerComponentBase
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CProductInfoFileGeneratorComp);
		I_REGISTER_INTERFACE(ifile::IFilePersistence)
	I_END_COMPONENT;

	// reimplemented (ifile::IFilePersistence)
	virtual bool IsOperationSupported(
				const istd::IChangeable* dataObjectPtr,
				const QString* filePathPtr = NULL,
				int flags = -1,
				bool beQuiet = true) const override;
	virtual OperationState LoadFromFile(
				istd::IChangeable& data,
				const QString& filePath = QString(),
				ibase::IProgressManager* progressManagerPtr = NULL) const override;
	virtual OperationState SaveToFile(
				const istd::IChangeable& data,
				const QString& filePath = QString(),
				ibase::IProgressManager* progressManagerPtr = NULL) const override;

	// reimplemented (ifile::IFileTypeInfo)
	virtual bool GetFileExtensions(QStringList& result, const istd::IChangeable* dataObjectPtr = NULL, int flags = -1, bool doAppend = false) const override;
	virtual QString GetTypeDescription(const QString* extensionPtr = NULL) const override;

private:
	void WriteIncludes(QTextStream& textStream) const;
	void WriteNamespaceHeader(QTextStream& textStream, const QString& productName) const;
	void WriteNamespaceFooter(QTextStream& textStream) const;
	void WriteFunction(QTextStream& textStream, imtlic::IProductInfo& productInfo) const;
	void WriteFeatureInfo(
				QTextStream& textStream,
				const imtlic::IFeatureInfo& featureInfo,
				const QByteArray& objectUuid) const;
	void WriteNewLine(QTextStream& textStream, int count = 1) const;
	void WriteTab(QTextStream& textStream, int count = 1) const;
	QString CreateFeatureVarName(const QByteArray& featureId) const;
};


} // namespace imtlicgql


