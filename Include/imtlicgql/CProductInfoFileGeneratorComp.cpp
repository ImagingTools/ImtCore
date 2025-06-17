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

void CProductInfoFileGeneratorComp::WriteIncludes(QTextStream& out) const
{
	out << QStringLiteral("// Qt includes");
	WriteNewLine(out, 1);
	out << QStringLiteral("#include <QtCore/QtTranslation>");
	WriteNewLine(out, 2);
	out << QStringLiteral("// ImtCore includes");
	WriteNewLine(out, 1);
	out << QStringLiteral("#include <imtlic/CProductInfo.h>");
	WriteNewLine(out, 1);
	out << QStringLiteral("#include <imtlic/CFeatureInfo.h>");
	WriteNewLine(out, 1);
}


void CProductInfoFileGeneratorComp::WriteNamespaceHeader(QTextStream& out, const QString& productName) const
{
	out << QStringLiteral("namespace %1").arg(productName.toLower());
	WriteNewLine(out, 1);
	out << QStringLiteral("{");
	WriteNewLine(out, 1);
}


void CProductInfoFileGeneratorComp::WriteNamespaceFooter(QTextStream& out) const
{
	out << QStringLiteral("};");
	WriteNewLine(out, 1);
}


void CProductInfoFileGeneratorComp::WriteFunction(QTextStream& out, imtlic::IProductInfo& productInfo) const
{
	out << QStringLiteral("static void FillProduct(imtlic::IProductInfo& productInfo){");

	const QByteArray productId = productInfo.GetProductId();
	const QString productName = productInfo.GetName();
	const QString productDescription = productInfo.GetProductDescription();
	const QByteArray categoryId = productInfo.GetCategoryId();

	WriteNewLine(out, 1);
	WriteTab(out, 1);
	out << QStringLiteral("productInfo.SetProductId(\"%1\");").arg(QString::fromUtf8(productId));
	WriteNewLine(out, 1);

	if (!productName.isEmpty()){
		WriteTab(out, 1);
		out << QStringLiteral("productInfo.SetName(QT_TRANSLATE_NOOP(\"%1\", \"%2\"));").arg(s_productContext, productName);
		WriteNewLine(out, 1);
	}

	if (!productDescription.isEmpty()){
		WriteTab(out, 1);
		out << QStringLiteral("productInfo.SetProductDescription(QT_TRANSLATE_NOOP(\"%1\", \"%2\"));").arg(s_productContext, productDescription);
		WriteNewLine(out, 1);
	}

	WriteTab(out, 1);
	out << QStringLiteral("productInfo.SetCategoryId(\"%1\");").arg(QString::fromUtf8(categoryId));
	WriteNewLine(out, 2);

	imtbase::IObjectCollection* featureCollectionPtr = productInfo.GetFeatures();
	if (featureCollectionPtr != nullptr) {
		imtbase::ICollectionInfo::Ids featureElementIds = featureCollectionPtr->GetElementIds();
		for (const imtbase::ICollectionInfo::Id& featureElementId : featureElementIds) {
			imtbase::IObjectCollection::DataPtr dataPtr;
			if (featureCollectionPtr->GetObjectData(featureElementId, dataPtr)) {
				const imtlic::IFeatureInfo* featureInfoPtr = dynamic_cast<const imtlic::IFeatureInfo*>(dataPtr.GetPtr());
				if (featureInfoPtr != nullptr) {
					WriteFeatureInfo(out, *featureInfoPtr, featureElementId);
					WriteNewLine(out, 1);

					const QByteArray featureId = featureInfoPtr->GetFeatureId();
					const QString featureVarName = CreateFeatureVarName(featureId);

					WriteTab(out, 1);
					out << QStringLiteral("productInfo.AddFeature(\"%1\", *%2.GetPtr());").arg(QString::fromUtf8(featureElementId), featureVarName);
					WriteNewLine(out, 2);
				}
			}
		}
	}

	out << QStringLiteral("}");
	WriteNewLine(out, 1);
}


void CProductInfoFileGeneratorComp::WriteFeatureInfo(
			QTextStream& out,
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
		WriteTab(out, 1);
		out << QStringLiteral("istd::TDelPtr<imtlic::CFeatureInfo> %1;").arg(featureVarName);
		WriteNewLine(out, 1);
		WriteTab(out, 1);
		out << QStringLiteral("%1.SetPtr(new imtlic::CFeatureInfo);").arg(featureVarName);
		WriteNewLine(out, 1);
	}
	else{
		WriteTab(out, 1);
		out << QStringLiteral("istd::TDelPtr<imtlic::CIdentifiableFeatureInfo> %1;").arg(featureVarName);
		WriteNewLine(out, 1);
		WriteTab(out, 1);
		out << QStringLiteral("%1.SetPtr(new imtlic::CIdentifiableFeatureInfo);").arg(featureVarName);
		WriteNewLine(out, 1);

		WriteTab(out, 1);
		out << QStringLiteral("%1->SetObjectUuid(\"%2\");").arg(featureVarName, QString::fromUtf8(objectUuid));
		WriteNewLine(out, 1);
	}

	WriteTab(out, 1);
	out << QStringLiteral("%1->SetFeatureId(\"%2\");").arg(featureVarName, QString::fromUtf8(featureId));
	WriteNewLine(out, 1);

	if (!featureName.isEmpty()){
		WriteTab(out, 1);
		out << QStringLiteral("%1->SetFeatureName(QT_TRANSLATE_NOOP(\"%2\", \"%3\"));")
					.arg(featureVarName, s_featureContext, featureName);
		WriteNewLine(out, 1);
	}

	if (!featureDescription.isEmpty()){
		WriteTab(out, 1);
		out << QStringLiteral("%1->SetFeatureDescription(QT_TRANSLATE_NOOP(\"%2\", \"%3\"));")
					.arg(featureVarName, s_featureContext, featureDescription);
		WriteNewLine(out, 1);
	}

	WriteTab(out, 1);
	out << QStringLiteral("%1->SetOptional(%2);")
				.arg(featureVarName, isOptional ? QStringLiteral("true") : QStringLiteral("false"));
	WriteNewLine(out, 1);

	WriteTab(out, 1);
	out << QStringLiteral("%1->SetIsPermission(%2);")
				.arg(featureVarName, isPermission ? QStringLiteral("true") : QStringLiteral("false"));
	WriteNewLine(out, 1);

	if (!dependencyList.isEmpty()){
		QByteArray dependencies = dependencyList.join(';');
		if (!dependencies.isEmpty()){
			WriteTab(out, 1);
			out << QStringLiteral("%1->SetDependencies(%2);").arg(featureVarName, QString("QByteArray(\"%1\").split(';')").arg(QString::fromUtf8(dependencies)));
			WriteNewLine(out, 1);
		}
	}
	
	const imtlic::FeatureInfoList& subFeatures = featureInfo.GetSubFeatures();
	if (!subFeatures.IsEmpty()){
		for (int i = 0; i < subFeatures.GetCount(); i++){
			const imtlic::IFeatureInfo* subFeatureInfoPtr = subFeatures.GetAt(i);
			if (subFeatureInfoPtr != nullptr){
				WriteNewLine(out, 1);
				WriteFeatureInfo(out, *subFeatureInfoPtr, "");

				const QByteArray subFeatureId = subFeatureInfoPtr->GetFeatureId();
				const QString subFeatureVarName = CreateFeatureVarName(subFeatureId);

				WriteNewLine(out, 1);
				WriteTab(out, 1);
				out << QStringLiteral("%1->InsertSubFeature(%2.PopPtr());").arg(featureVarName, subFeatureVarName);
				WriteNewLine(out, 1);
			}
		}
	}
}


void CProductInfoFileGeneratorComp::WriteNewLine(QTextStream& out, int count) const
{
	for (int i = 0; i < count; ++i){
		out << '\n';
	}
}


void CProductInfoFileGeneratorComp::WriteTab(QTextStream& out, int count) const
{
	for (int i = 0; i < count; ++i){
		out << '\t';
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


