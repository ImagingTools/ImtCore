// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtlicgql/CProductInfoFileGeneratorComp.h>


// Qt includes
#include <QtCore/QFile>


namespace imtlicgql
{


const QString s_featureContext = QStringLiteral("Feature");
const QString s_productContext = QStringLiteral("Product");


// public methods

// reimplemented (ifile::IFilePersistence)

bool CProductInfoFileGeneratorComp::IsOperationSupported(
			const istd::IChangeable* dataObjectPtr,
			const QString* /*filePathPtr*/,
			int /*flags*/,
			bool /*beQuiet*/) const
{
	const imtlic::IProductInfo* productInfoPtr = dynamic_cast<const imtlic::IProductInfo*>(dataObjectPtr);
	return productInfoPtr != nullptr;
}


ifile::IFilePersistence::OperationState CProductInfoFileGeneratorComp::LoadFromFile(
			istd::IChangeable& /*data*/,
			const QString& /*filePath*/,
			ibase::IProgressManager* /*progressManagerPtr*/) const
{
	return ifile::IFilePersistence::OS_FAILED;
}


ifile::IFilePersistence::OperationState CProductInfoFileGeneratorComp::SaveToFile(
			const istd::IChangeable& data,
			const QString& filePath,
			ibase::IProgressManager* /*progressManagerPtr*/) const
{
	istd::IChangeable* notConstDataPtr = const_cast<istd::IChangeable*>(&data);
	imtlic::IProductInfo* productInfoPtr = dynamic_cast<imtlic::IProductInfo*>(notConstDataPtr);
	if (productInfoPtr == nullptr) {
		return ifile::IFilePersistence::OS_FAILED;
	}

	QFile file(filePath);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
		return ifile::IFilePersistence::OS_FAILED;
	}

	QString productName = productInfoPtr->GetName();

	QTextStream textStream(&file);

	WriteIncludes(textStream);
	WriteNewLine(textStream, 2);
	WriteNamespaceHeader(textStream, productName);
	WriteNewLine(textStream, 2);
	WriteFunction(textStream, *productInfoPtr);
	WriteNewLine(textStream, 2);
	WriteNamespaceFooter(textStream);
	WriteNewLine(textStream, 1);

	file.close();

	return ifile::IFilePersistence::OS_OK;
}


// reimplemented (ifile::IFileTypeInfo)

bool CProductInfoFileGeneratorComp::GetFileExtensions(
			QStringList& /*result*/,
			const istd::IChangeable* /*dataObjectPtr*/,
			int /*flags*/,
			bool /*doAppend*/) const
{
	return false;
}


QString CProductInfoFileGeneratorComp::GetTypeDescription(
			const QString* /*extensionPtr*/) const
{
	return QString();
}


// private methods

void CProductInfoFileGeneratorComp::WriteIncludes(QTextStream& textStream) const
{
	textStream << QStringLiteral("// Qt includes");
	WriteNewLine(textStream, 1);
	textStream << QStringLiteral("#include <QtCore/qglobal.h>");
	WriteNewLine(textStream, 2);
	textStream << QStringLiteral("// ImtCore includes");
	WriteNewLine(textStream, 1);
	textStream << QStringLiteral("#include <imtlic/CProductInfo.h>");
	WriteNewLine(textStream, 1);
	textStream << QStringLiteral("#include <imtlic/CFeatureInfo.h>");
	WriteNewLine(textStream, 1);
}


void CProductInfoFileGeneratorComp::WriteNamespaceHeader(QTextStream& textStream, const QString& productName) const
{
	textStream << QStringLiteral("namespace %1").arg(productName.toLower());
	WriteNewLine(textStream, 1);
	textStream << QStringLiteral("{");
	WriteNewLine(textStream, 1);
}


void CProductInfoFileGeneratorComp::WriteNamespaceFooter(QTextStream& textStream) const
{
	textStream << QStringLiteral("};");
	WriteNewLine(textStream, 1);
}


void CProductInfoFileGeneratorComp::WriteFunction(QTextStream& textStream, imtlic::IProductInfo& productInfo) const
{
	textStream << QStringLiteral("static void FillProduct(imtlic::IProductInfo& productInfo){");

	const QByteArray productId = productInfo.GetProductId();
	const QString productName = productInfo.GetName();
	const QString productDescription = productInfo.GetProductDescription();
	const QByteArray categoryId = productInfo.GetCategoryId();

	WriteNewLine(textStream, 1);
	WriteTab(textStream, 1);
	textStream << QStringLiteral("productInfo.SetProductId(\"%1\");").arg(QString::fromUtf8(productId));
	WriteNewLine(textStream, 1);

	if (!productName.isEmpty()){
		WriteTab(textStream, 1);
		textStream << QStringLiteral("productInfo.SetName(QT_TRANSLATE_NOOP(\"%1\", \"%2\"));").arg(s_productContext, productName);
		WriteNewLine(textStream, 1);
	}

	if (!productDescription.isEmpty()){
		WriteTab(textStream, 1);
		textStream << QStringLiteral("productInfo.SetProductDescription(QT_TRANSLATE_NOOP(\"%1\", \"%2\"));").arg(s_productContext, productDescription);
		WriteNewLine(textStream, 1);
	}

	WriteTab(textStream, 1);
	textStream << QStringLiteral("productInfo.SetCategoryId(\"%1\");").arg(QString::fromUtf8(categoryId));
	WriteNewLine(textStream, 2);

	imtbase::IObjectCollection* featureCollectionPtr = productInfo.GetFeatures();
	if (featureCollectionPtr != nullptr) {
		imtbase::ICollectionInfo::Ids featureElementIds = featureCollectionPtr->GetElementIds();
		for (const imtbase::ICollectionInfo::Id& featureElementId : featureElementIds) {
			imtbase::IObjectCollection::DataPtr dataPtr;
			if (featureCollectionPtr->GetObjectData(featureElementId, dataPtr)) {
				const imtlic::IFeatureInfo* featureInfoPtr = dynamic_cast<const imtlic::IFeatureInfo*>(dataPtr.GetPtr());
				if (featureInfoPtr != nullptr) {
					WriteFeatureInfo(textStream, *featureInfoPtr, featureElementId);
					WriteNewLine(textStream, 1);

					const QByteArray featureId = featureInfoPtr->GetFeatureId();
					const QString featureVarName = CreateFeatureVarName(featureId);

					WriteTab(textStream, 1);
					textStream << QStringLiteral("productInfo.AddFeature(\"%1\", *%2.GetPtr());").arg(QString::fromUtf8(featureElementId), featureVarName);
					WriteNewLine(textStream, 2);
				}
			}
		}
	}

	textStream << QStringLiteral("}");
	WriteNewLine(textStream, 1);
}


void CProductInfoFileGeneratorComp::WriteFeatureInfo(
			QTextStream& textStream,
			const imtlic::IFeatureInfo& featureInfo,
			const QByteArray& objectUuid) const
{
	const QByteArray featureId = featureInfo.GetFeatureId();
	const QString featureName = featureInfo.GetFeatureName();
	const QString featureDescription = featureInfo.GetFeatureDescription();
	const bool isOptional = featureInfo.IsOptional();
	const bool isPermission = featureInfo.IsPermission();
	QByteArrayList dependencyList = featureInfo.GetDependencies();

	const QString featureVarName = CreateFeatureVarName(featureId);

	if (objectUuid.isEmpty()){
		WriteTab(textStream, 1);
		textStream << QStringLiteral("istd::TDelPtr<imtlic::CFeatureInfo> %1;").arg(featureVarName);
		WriteNewLine(textStream, 1);
		WriteTab(textStream, 1);
		textStream << QStringLiteral("%1.SetPtr(new imtlic::CFeatureInfo);").arg(featureVarName);
		WriteNewLine(textStream, 1);
	}
	else{
		WriteTab(textStream, 1);
		textStream << QStringLiteral("istd::TDelPtr<imtlic::CIdentifiableFeatureInfo> %1;").arg(featureVarName);
		WriteNewLine(textStream, 1);
		WriteTab(textStream, 1);
		textStream << QStringLiteral("%1.SetPtr(new imtlic::CIdentifiableFeatureInfo);").arg(featureVarName);
		WriteNewLine(textStream, 1);

		WriteTab(textStream, 1);
		textStream << QStringLiteral("%1->SetObjectUuid(\"%2\");").arg(featureVarName, QString::fromUtf8(objectUuid));
		WriteNewLine(textStream, 1);
	}

	WriteTab(textStream, 1);
	textStream << QStringLiteral("%1->SetFeatureId(\"%2\");").arg(featureVarName, QString::fromUtf8(featureId));
	WriteNewLine(textStream, 1);

	if (!featureName.isEmpty()){
		WriteTab(textStream, 1);
		textStream << QStringLiteral("%1->SetFeatureName(QT_TRANSLATE_NOOP(\"%2\", \"%3\"));")
					.arg(featureVarName, s_featureContext, featureName);
		WriteNewLine(textStream, 1);
	}

	if (!featureDescription.isEmpty()){
		WriteTab(textStream, 1);
		textStream << QStringLiteral("%1->SetFeatureDescription(QT_TRANSLATE_NOOP(\"%2\", \"%3\"));")
					.arg(featureVarName, s_featureContext, featureDescription);
		WriteNewLine(textStream, 1);
	}

	WriteTab(textStream, 1);
	textStream << QStringLiteral("%1->SetOptional(%2);")
				.arg(featureVarName, isOptional ? QStringLiteral("true") : QStringLiteral("false"));
	WriteNewLine(textStream, 1);

	WriteTab(textStream, 1);
	textStream << QStringLiteral("%1->SetIsPermission(%2);")
				.arg(featureVarName, isPermission ? QStringLiteral("true") : QStringLiteral("false"));
	WriteNewLine(textStream, 1);

	if (!dependencyList.isEmpty()){
		QByteArray dependencies = dependencyList.join(';');
		if (!dependencies.isEmpty()){
			WriteTab(textStream, 1);
			textStream << QStringLiteral("%1->SetDependencies(%2);").arg(featureVarName, QString("QByteArray(\"%1\").split(';')").arg(QString::fromUtf8(dependencies)));
			WriteNewLine(textStream, 1);
		}
	}
	
	const imtlic::IFeatureInfo::FeatureInfoList& subFeatures = featureInfo.GetSubFeatures();
	if (!subFeatures.isEmpty()){
		for (int i = 0; i < subFeatures.count(); i++){
			imtlic::IFeatureInfoSharedPtr subFeatureInfoPtr = subFeatures.at(i);
			if (subFeatureInfoPtr.IsValid()){
				WriteNewLine(textStream, 1);
				WriteFeatureInfo(textStream, *subFeatureInfoPtr, "");

				const QByteArray subFeatureId = subFeatureInfoPtr->GetFeatureId();
				const QString subFeatureVarName = CreateFeatureVarName(subFeatureId);

				WriteNewLine(textStream, 1);
				WriteTab(textStream, 1);
				textStream << QStringLiteral("%1->InsertSubFeature(%2.PopPtr());").arg(featureVarName, subFeatureVarName);
				WriteNewLine(textStream, 1);
			}
		}
	}
}


void CProductInfoFileGeneratorComp::WriteNewLine(QTextStream& textStream, int count) const
{
	for (int i = 0; i < count; ++i){
		textStream << '\n';
	}
}


void CProductInfoFileGeneratorComp::WriteTab(QTextStream& textStream, int count) const
{
	for (int i = 0; i < count; ++i){
		textStream << '\t';
	}
}


QString CProductInfoFileGeneratorComp::CreateFeatureVarName(const QByteArray& featureId) const
{
	if (featureId.isEmpty()){
		return QString();
	}

	QString retVal = QString::fromUtf8(featureId);
	retVal[0] = retVal[0].toLower();

	return retVal + "FeatureInfo";
}


} // namespace imtlicgql


