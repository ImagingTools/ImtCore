// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtlicgql/CProductInfoFileGeneratorComp.h>


// STL includes
#include <map>
#include <memory>

// Qt includes
#include <QtCore/QFile>
#include <QtCore/QRegularExpression>
#include <QtCore/QUuid>

// ImtCore includes
#include <imtlic/CFeatureInfo.h>


namespace imtlicgql
{


const QString s_featureContext = QStringLiteral("Feature");
const QString s_productContext = QStringLiteral("Product");


namespace
{


// A C string literal as the generator writes it: quotes and backslashes are escaped.
const QString s_string = QStringLiteral(R"re("((?:[^"\\]|\\.)*)")re");
const QString s_translated = QStringLiteral(R"re(QT_TRANSLATE_NOOP\("\w+", %1\))re").arg(s_string);


QString Unescape(const QString& text)
{
	QString retVal;
	retVal.reserve(text.size());
	for (qsizetype i = 0; i < text.size(); ++i){
		if (text[i] == '\\' && i + 1 < text.size()){
			++i;
		}
		retVal += text[i];
	}

	return retVal;
}


QRegularExpressionMatch MatchStatement(const QString& pattern, const QString& statement)
{
	return QRegularExpression(QStringLiteral("^") + pattern + QStringLiteral("$")).match(statement);
}


} // anonymous namespace


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
	istd::IChangeable& data,
	const QString& filePath,
	ibase::IProgressManager* /*progressManagerPtr*/) const
{
	imtlic::IProductInfo* productInfoPtr = dynamic_cast<imtlic::IProductInfo*>(&data);
	if (productInfoPtr == nullptr){
		return ifile::IFilePersistence::OS_FAILED;
	}

	QFile file(filePath);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
		SendErrorMessage(0, QStringLiteral("Unable to open the product file '%1'").arg(filePath), "CProductInfoFileGeneratorComp");

		return ifile::IFilePersistence::OS_FAILED;
	}

	const QString text = QString::fromUtf8(file.readAll());
	file.close();

	data.ResetData();

	QString errorMessage;
	if (!ReadProduct(text, *productInfoPtr, errorMessage)){
		SendErrorMessage(0, QStringLiteral("The product file '%1' cannot be read: %2").arg(filePath, errorMessage), "CProductInfoFileGeneratorComp");

		return ifile::IFilePersistence::OS_FAILED;
	}

	// The header does not carry the uuid of the product, a read product is a new one.
	imtlic::CIdentifiableProductInfo* identifiableProductPtr = dynamic_cast<imtlic::CIdentifiableProductInfo*>(&data);
	if (identifiableProductPtr != nullptr){
		identifiableProductPtr->SetObjectUuid(QUuid::createUuid().toByteArray(QUuid::WithoutBraces));
	}

	return ifile::IFilePersistence::OS_OK;
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
	QStringList& result,
	const istd::IChangeable* /*dataObjectPtr*/,
	int /*flags*/,
	bool doAppend) const
{
	if (!doAppend){
		result.clear();
	}

	result << QStringLiteral("h");

	return true;
}


QString CProductInfoFileGeneratorComp::GetTypeDescription(
	const QString* /*extensionPtr*/) const
{
	return QStringLiteral("C++ Header Files");
}


// private methods

bool CProductInfoFileGeneratorComp::ReadProduct(const QString& text, imtlic::IProductInfo& productInfo, QString& errorMessage) const
{
	static const QString variable = QStringLiteral(R"re((\w+))re");
	static const QString featureClass = QStringLiteral(R"re(imtlic::(CIdentifiableFeatureInfo|CFeatureInfo))re");

	// Features still being filled; a feature leaves when it is inserted into its parent or added to the product.
	std::map<QString, std::unique_ptr<imtlic::CFeatureInfo>> features;
	bool hasProductId = false;
	bool insideFunction = false;

	const QStringList lines = text.split('\n');
	for (qsizetype lineIndex = 0; lineIndex < lines.size(); ++lineIndex){
		const QString statement = lines[lineIndex].trimmed();
		auto fail = [&errorMessage, lineIndex, &statement](const QString& reason){
			errorMessage = QStringLiteral("line %1: %2 ('%3')").arg(lineIndex + 1).arg(reason, statement);

			return false;
		};

		if (statement.isEmpty() || statement.startsWith(QStringLiteral("//")) || statement.startsWith('#')){
			continue;
		}

		if (!insideFunction){
			if (statement == QStringLiteral("static void FillProduct(imtlic::IProductInfo& productInfo){")){
				insideFunction = true;
			}
			else if (!statement.startsWith(QStringLiteral("namespace ")) && statement != QStringLiteral("{") && statement != QStringLiteral("}") && statement != QStringLiteral("};")){
				return fail(QStringLiteral("unexpected statement outside of FillProduct"));
			}

			continue;
		}

		if (statement == QStringLiteral("}")){
			insideFunction = false;

			continue;
		}

		QRegularExpressionMatch match;
		if ((match = MatchStatement(QStringLiteral(R"re(productInfo\.SetProductId\(%1\);)re").arg(s_string), statement)).hasMatch()){
			productInfo.SetProductId(Unescape(match.captured(1)).toUtf8());
			hasProductId = !productInfo.GetProductId().isEmpty();
		}
		else if ((match = MatchStatement(QStringLiteral(R"re(productInfo\.SetName\(%1\);)re").arg(s_translated), statement)).hasMatch()){
			productInfo.SetName(Unescape(match.captured(1)));
		}
		else if ((match = MatchStatement(QStringLiteral(R"re(productInfo\.SetProductDescription\(%1\);)re").arg(s_translated), statement)).hasMatch()){
			productInfo.SetProductDescription(Unescape(match.captured(1)));
		}
		else if ((match = MatchStatement(QStringLiteral(R"re(productInfo\.SetCategoryId\(%1\);)re").arg(s_string), statement)).hasMatch()){
			productInfo.SetCategoryId(Unescape(match.captured(1)).toUtf8());
		}
		else if ((match = MatchStatement(QStringLiteral(R"re(istd::TDelPtr<%1> %2;)re").arg(featureClass, variable), statement)).hasMatch()){
			const QString variableName = match.captured(2);
			if ((features.count(variableName) > 0)){
				return fail(QStringLiteral("the feature variable is declared twice"));
			}

			if (match.captured(1) == QStringLiteral("CIdentifiableFeatureInfo")){
				features[variableName] = std::make_unique<imtlic::CIdentifiableFeatureInfo>();
			}
			else{
				features[variableName] = std::make_unique<imtlic::CFeatureInfo>();
			}
		}
		else if ((match = MatchStatement(QStringLiteral(R"re(%1\.SetPtr\(new %2\);)re").arg(variable, featureClass), statement)).hasMatch()){
			if (!(features.count(match.captured(1)) > 0)){
				return fail(QStringLiteral("unknown feature variable"));
			}
		}
		else if ((match = MatchStatement(QStringLiteral(R"re(%1->InsertSubFeature\(%2\.PopPtr\(\)\);)re").arg(variable, variable), statement)).hasMatch()){
			const QString parentName = match.captured(1);
			const QString childName = match.captured(2);
			if (!(features.count(parentName) > 0) || !(features.count(childName) > 0) || parentName == childName){
				return fail(QStringLiteral("unknown feature variable"));
			}

			imtlic::CFeatureInfo* subFeaturePtr = features.extract(childName).mapped().release();
			if (!features[parentName]->InsertSubFeature(imtlic::IFeatureInfo::FeatureInfoPtr(subFeaturePtr))){
				return fail(QStringLiteral("the sub-feature cannot be inserted, its id is already used under this parent"));
			}
		}
		else if ((match = MatchStatement(QStringLiteral(R"re(productInfo\.AddFeature\(%1, \*%2\.GetPtr\(\)\);)re").arg(s_string, variable), statement)).hasMatch()){
			const QString variableName = match.captured(2);
			if (!(features.count(variableName) > 0)){
				return fail(QStringLiteral("unknown feature variable"));
			}

			std::unique_ptr<imtlic::CFeatureInfo> featurePtr = std::move(features[variableName]);
			features.erase(variableName);

			const QByteArray elementId = Unescape(match.captured(1)).toUtf8();
			const imtlic::CIdentifiableFeatureInfo* identifiablePtr = dynamic_cast<const imtlic::CIdentifiableFeatureInfo*>(featurePtr.get());
			if (elementId.isEmpty() || identifiablePtr == nullptr || identifiablePtr->GetObjectUuid() != elementId){
				return fail(QStringLiteral("a feature of the product must be added under its own uuid"));
			}

			if (featurePtr->GetFeatureId().isEmpty()){
				return fail(QStringLiteral("the feature has no id"));
			}

			if (!productInfo.AddFeature(elementId, *featurePtr)){
				return fail(QStringLiteral("the feature cannot be added, its uuid is already used"));
			}
		}
		else{
			match = MatchStatement(QStringLiteral(R"re(%1->(\w+)\((.*)\);)re").arg(variable), statement);
			if (!match.hasMatch() || !(features.count(match.captured(1)) > 0)){
				return fail(QStringLiteral("unexpected statement"));
			}

			imtlic::CFeatureInfo& featureInfo = *features[match.captured(1)];
			const QString method = match.captured(2);
			const QString argument = match.captured(3);
			const QRegularExpressionMatch stringMatch = MatchStatement(s_string, argument);
			const QRegularExpressionMatch translatedMatch = MatchStatement(s_translated, argument);
			const bool isBoolean = (argument == QStringLiteral("true")) || (argument == QStringLiteral("false"));

			if (method == QStringLiteral("SetObjectUuid") && stringMatch.hasMatch()){
				imtlic::CIdentifiableFeatureInfo* identifiablePtr = dynamic_cast<imtlic::CIdentifiableFeatureInfo*>(&featureInfo);
				if (identifiablePtr == nullptr){
					return fail(QStringLiteral("only a feature of the product has a uuid"));
				}

				identifiablePtr->SetObjectUuid(Unescape(stringMatch.captured(1)).toUtf8());
			}
			else if (method == QStringLiteral("SetFeatureId") && stringMatch.hasMatch()){
				featureInfo.SetFeatureId(Unescape(stringMatch.captured(1)).toUtf8());
			}
			else if (method == QStringLiteral("SetFeatureName") && translatedMatch.hasMatch()){
				featureInfo.SetFeatureName(Unescape(translatedMatch.captured(1)));
			}
			else if (method == QStringLiteral("SetFeatureDescription") && translatedMatch.hasMatch()){
				featureInfo.SetFeatureDescription(Unescape(translatedMatch.captured(1)));
			}
			else if (method == QStringLiteral("SetOptional") && isBoolean){
				featureInfo.SetOptional(argument == QStringLiteral("true"));
			}
			else if (method == QStringLiteral("SetIsPermission") && isBoolean){
				featureInfo.SetIsPermission(argument == QStringLiteral("true"));
			}
			else if (method == QStringLiteral("SetRequirements")){
				const QRegularExpressionMatch requirementsMatch = MatchStatement(QStringLiteral(R"re(QByteArray\(%1\)\.split\(';'\))re").arg(s_string), argument);
				if (!requirementsMatch.hasMatch()){
					return fail(QStringLiteral("unexpected requirements"));
				}

				featureInfo.SetRequirements(Unescape(requirementsMatch.captured(1)).toUtf8().split(';'));
			}
			else{
				return fail(QStringLiteral("unexpected feature statement"));
			}
		}
	}

	if (!hasProductId){
		errorMessage = QStringLiteral("the file sets no product id");

		return false;
	}

	if (!features.empty()){
		QStringList variableNames;
		for (const auto& feature : features){
			variableNames << feature.first;
		}

		errorMessage = QStringLiteral("features are declared but never added to the product: %1").arg(variableNames.join(QStringLiteral(", ")));

		return false;
	}

	return true;
}


QString CProductInfoFileGeneratorComp::EscapeString(const QString& text)
{
	QString retVal = text;
	retVal.replace(QStringLiteral("\\"), QStringLiteral("\\\\"));
	retVal.replace(QStringLiteral("\""), QStringLiteral("\\\""));

	return retVal;
}


void CProductInfoFileGeneratorComp::WriteIncludes(QTextStream& textStream) const
{
	textStream << QStringLiteral("// Qt includes");
	WriteNewLine(textStream, 1);
	textStream << QStringLiteral("#include <QtCore/qglobal.h>");
	WriteNewLine(textStream, 2);
	textStream << QStringLiteral("// ACF includes");
	WriteNewLine(textStream, 1);
	textStream << QStringLiteral("#include <istd/TDelPtr.h>");
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
	textStream << QStringLiteral("productInfo.SetProductId(\"%1\");").arg(productId);
	WriteNewLine(textStream, 1);

	if (!productName.isEmpty()){
		WriteTab(textStream, 1);
		textStream << QStringLiteral("productInfo.SetName(QT_TRANSLATE_NOOP(\"%1\", \"%2\"));").arg(s_productContext, EscapeString(productName));
		WriteNewLine(textStream, 1);
	}

	if (!productDescription.isEmpty()){
		WriteTab(textStream, 1);
		textStream << QStringLiteral("productInfo.SetProductDescription(QT_TRANSLATE_NOOP(\"%1\", \"%2\"));").arg(s_productContext, EscapeString(productDescription));
		WriteNewLine(textStream, 1);
	}

	WriteTab(textStream, 1);
	textStream << QStringLiteral("productInfo.SetCategoryId(\"%1\");").arg(categoryId);
	WriteNewLine(textStream, 2);

	imtbase::IObjectCollection* featureCollectionPtr = productInfo.GetFeatures();
	if (featureCollectionPtr != nullptr) {
		imtbase::ICollectionInfo::Ids featureElementIds = featureCollectionPtr->GetElementIds();
		for (const imtbase::ICollectionInfo::Id& featureElementId : featureElementIds) {
			imtbase::IObjectCollection::DataPtr dataPtr;
			if (featureCollectionPtr->GetObjectData(featureElementId, dataPtr)) {
				const imtlic::IFeatureInfo* featureInfoPtr = dynamic_cast<const imtlic::IFeatureInfo*>(dataPtr.GetPtr());
				if (featureInfoPtr != nullptr) {
					WriteFeatureInfo(textStream, *featureInfoPtr, productInfo, featureElementId);
					WriteNewLine(textStream, 1);

					const QByteArray featurePath = imtlic::CalculateFeaturePath(*featureInfoPtr);
					const QString featureVarName = CreateFeatureVarName(featurePath);

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
	imtlic::IProductInfo &productInfo,
	const QByteArray& objectUuid) const
{
	const QByteArray featureId = featureInfo.GetFeatureId();
	const QString featureName = featureInfo.GetFeatureName();
	const QString featureDescription = featureInfo.GetFeatureDescription();
	const bool isOptional = featureInfo.IsOptional();
	const bool isPermission = featureInfo.IsPermission();

	// Feature-IDs may repeat across the tree as long as the parent path differs, so the
	// variable name must be derived from the full path to stay unique within the function.
	const QByteArray featurePath = imtlic::CalculateFeaturePath(featureInfo);
	const QString featureVarName = CreateFeatureVarName(featurePath);

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
						  .arg(featureVarName, s_featureContext, EscapeString(featureName));
		WriteNewLine(textStream, 1);
	}

	if (!featureDescription.isEmpty()){
		WriteTab(textStream, 1);
		textStream << QStringLiteral("%1->SetFeatureDescription(QT_TRANSLATE_NOOP(\"%2\", \"%3\"));")
						  .arg(featureVarName, s_featureContext, EscapeString(featureDescription));
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

	const QByteArrayList requirementPaths = ResolveRequirementPaths(productInfo, featureInfo.GetRequirements());
	if (!requirementPaths.isEmpty()){
		WriteTab(textStream, 1);
		textStream << QStringLiteral("%1->SetRequirements(QByteArray(\"%2\").split(';'));")
						  .arg(featureVarName, QString::fromUtf8(requirementPaths.join(';')));
		WriteNewLine(textStream, 1);
	}

	const imtlic::IFeatureInfo::FeatureInfoList& subFeatures = featureInfo.GetSubFeatures();
	if (!subFeatures.isEmpty()){
		for (int i = 0; i < subFeatures.count(); i++){
			imtlic::IFeatureInfoSharedPtr subFeatureInfoPtr = subFeatures.at(i);
			if (subFeatureInfoPtr.IsValid()){
				WriteNewLine(textStream, 1);
				WriteFeatureInfo(textStream, *subFeatureInfoPtr, productInfo, "");

				const QByteArray subFeaturePath = imtlic::CalculateFeaturePath(*subFeatureInfoPtr);
				const QString subFeatureVarName = CreateFeatureVarName(subFeaturePath);

				WriteNewLine(textStream, 1);
				WriteTab(textStream, 1);
				textStream << QStringLiteral("%1->InsertSubFeature(%2.PopPtr());").arg(featureVarName, subFeatureVarName);
				WriteNewLine(textStream, 1);
			}
		}
	}
}


QByteArrayList CProductInfoFileGeneratorComp::ResolveRequirementPaths(
	imtlic::IProductInfo& productInfo,
	const QByteArrayList& requirements) const
{
	QByteArrayList retVal;

	imtbase::IObjectCollection* featureCollectionPtr = productInfo.GetFeatures();
	if (featureCollectionPtr == nullptr){
		return retVal;
	}

	const imtbase::ICollectionInfo::Ids featureElementIds = featureCollectionPtr->GetElementIds();

	for (const QByteArray& requirement : requirements){
		// A requirement is the full path of a node; ids repeat across the tree, so it is never looked up by its last segment.
		const QByteArrayList segments = requirement.startsWith('/') ? requirement.mid(1).split('/') : QByteArrayList();

		QByteArray requirementPath;
		for (const imtbase::ICollectionInfo::Id& featureElementId : featureElementIds){
			imtbase::IObjectCollection::DataPtr dataPtr;
			if (segments.isEmpty() || !featureCollectionPtr->GetObjectData(featureElementId, dataPtr)){
				continue;
			}

			const imtlic::IFeatureInfo* featureInfoPtr = dynamic_cast<const imtlic::IFeatureInfo*>(dataPtr.GetPtr());
			if (featureInfoPtr == nullptr || featureInfoPtr->GetFeatureId() != segments.first()){
				continue;
			}

			for (qsizetype segmentIndex = 1; segmentIndex < segments.size() && featureInfoPtr != nullptr; ++segmentIndex){
				const imtlic::IFeatureInfo* childPtr = nullptr;
				for (const imtlic::IFeatureInfo::FeatureInfoPtr& subFeaturePtr : featureInfoPtr->GetSubFeatures()){
					if (subFeaturePtr.IsValid() && subFeaturePtr->GetFeatureId() == segments[segmentIndex]){
						childPtr = subFeaturePtr.GetPtr();
						break;
					}
				}
				featureInfoPtr = childPtr;
			}

			if (featureInfoPtr != nullptr){
				requirementPath = requirement;
			}
			break;
		}

		if (requirementPath.isEmpty()){
			SendWarningMessage(0, QString("Requirement '%1' does not match any feature of product '%2'.")
					.arg(QString::fromUtf8(requirement), QString::fromUtf8(productInfo.GetProductId())),
					"CProductInfoFileGeneratorComp");

			continue;
		}

		retVal.append(requirementPath);
	}

	return retVal;
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


QString CProductInfoFileGeneratorComp::CreateFeatureVarName(const QByteArray& featurePath) const
{
	if (featurePath.isEmpty()){
		return QString();
	}

	QByteArray sanitizedPath = featurePath;
	if (sanitizedPath.startsWith('/')){
		sanitizedPath.remove(0, 1);
	}
	sanitizedPath.replace('/', '_');

	QString retVal = QString::fromUtf8(sanitizedPath);
	retVal[0] = retVal[0].toLower();

	return retVal + "FeatureInfo";
}


} // namespace imtlicgql


