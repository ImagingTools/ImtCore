// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtlicgql/ProductRepresentation.h>


// ACF includes
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtbase/CComplexCollectionFilter.h>
#include <imtlic/IFeatureInfo.h>
#include <imtlicgql/imtlicgql.h>


namespace imtlicgql
{


namespace
{


// IDs of the other products whose field has the value.
QByteArray FindProductByField(
			const imtbase::IObjectCollection& productCollection,
			const QByteArray& fieldId,
			const QVariant& value,
			const QByteArray& objectId)
{
	imtbase::IComplexCollectionFilter::FieldFilter fieldFilter;
	fieldFilter.fieldId = fieldId;
	fieldFilter.filterValue = value;

	imtbase::IComplexCollectionFilter::FilterExpression groupFilter;
	groupFilter.fieldFilters << fieldFilter;

	imtbase::CComplexCollectionFilter complexFilter;
	complexFilter.SetFilterExpression(groupFilter);

	iprm::CParamsSet filterParam;
	filterParam.SetEditableParameter("ComplexFilter", &complexFilter);

	const imtbase::ICollectionInfo::Ids ids = productCollection.GetElementIds(0, -1, &filterParam);
	for (const QByteArray& id : ids){
		if (id != objectId){
			return id;
		}
	}

	return QByteArray();
}


} // namespace


void FillProductFromRepresentation(
			const sdl::V1_0::imtlic::CProductData& representation,
			const imtbase::IObjectCollection& featureCollection,
			imtlic::IProductInfo& productInfo,
			QByteArrayList* unresolvedRequirementsPtr)
{
	productInfo.SetProductId(representation.productId ? QByteArray(*representation.productId) : QByteArray());
	productInfo.SetName(representation.productName ? *representation.productName : QString());
	productInfo.SetProductDescription(representation.description ? *representation.description : QString());
	productInfo.SetCategoryId(representation.categoryId ? QByteArray(*representation.categoryId) : QByteArray());

	QByteArray features;
	if (representation.features){
		features = *representation.features;
	}

	QByteArrayList featureIds;
	if (!features.isEmpty()){
		featureIds = features.split(';');
		featureIds.removeAll(QByteArray());
	}

	// Every id in "features" is a feature document id and is looked up as one;
	// the ids below are nodes inside such a document and are never resolved
	// against the collection. featureId is only matched against what the product
	// actually has, to drop stale entries.
	imtlic::IProductInfo::OptionalFeatureInfos optionalFeatures;
	if (representation.optionalFeatures){
		for (const sdl::V1_0::imtlic::CProductOptionalFeature& optionalFeature : representation.optionalFeatures->ToList()){
			if (!optionalFeature.featureId){
				continue;
			}

			const QByteArray featureId = *optionalFeature.featureId;
			if (featureId.isEmpty() || !featureIds.contains(featureId)){
				continue;
			}

			imtlic::IProductInfo::OptionalFeatureInfo optionalFeatureInfo;
			optionalFeatureInfo.featureId = featureId;

			if (optionalFeature.subFeatureIds){
				optionalFeatureInfo.subFeatureIds = optionalFeature.subFeatureIds->ToList();
				optionalFeatureInfo.subFeatureIds.removeAll(QByteArray());
			}

			if (!optionalFeatureInfo.subFeatureIds.isEmpty()){
				optionalFeatures << optionalFeatureInfo;
			}
		}
	}

	// A feature is worth nothing without what it requires, so the product is
	// completed here rather than trusted to arrive complete: the editor resolves
	// the same requirements while the user works, but a product also arrives from
	// an older client, from another one, or from a direct request - and a product
	// missing a required feature is one whose generated product info silently
	// drops that requirement (\see CProductInfoFileGeneratorComp).
	const ProductFeatureClosure featureClosure = CompleteProductFeatures(
			featureCollection,
			featureIds,
			optionalFeatures);

	if (unresolvedRequirementsPtr != nullptr){
		*unresolvedRequirementsPtr = featureClosure.unresolvedRequirements;
	}

	imtbase::IObjectCollection* productFeaturesPtr = productInfo.GetFeatures();
	if (productFeaturesPtr != nullptr){
		productFeaturesPtr->ResetData();
	}

	for (const QByteArray& featureId : featureClosure.featureIds){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (featureCollection.GetObjectData(featureId, dataPtr)){
			const imtlic::IFeatureInfo* featureInfoPtr = dynamic_cast<const imtlic::IFeatureInfo*>(dataPtr.GetPtr());
			if (featureInfoPtr != nullptr){
				productInfo.AddFeature(featureId, *featureInfoPtr);
			}
		}
	}

	productInfo.SetOptionalFeatures(featureClosure.optionalFeatures);
}


void FillRepresentationFromProduct(
			imtlic::IProductInfo& productInfo,
			sdl::V1_0::imtlic::CProductData& representation)
{
	representation.productId = productInfo.GetProductId();
	representation.name = productInfo.GetName();
	representation.productName = productInfo.GetName();
	representation.description = productInfo.GetProductDescription();
	representation.categoryId = productInfo.GetCategoryId();

	imtbase::IObjectCollection* productFeaturesPtr = productInfo.GetFeatures();
	representation.features = productFeaturesPtr != nullptr ? productFeaturesPtr->GetElementIds().toList().join(';') : QByteArray();
	representation.optionalFeatures = CreateOptionalFeaturesRepresentation(productInfo);
}


imtsdl::TElementList<sdl::V1_0::imtlic::CProductOptionalFeature> CreateOptionalFeaturesRepresentation(
			const imtlic::IProductInfo& productInfo)
{
	imtsdl::TElementList<sdl::V1_0::imtlic::CProductOptionalFeature> retVal;

	const imtlic::IProductInfo::OptionalFeatureInfos optionalFeatures = productInfo.GetOptionalFeatures();
	for (const imtlic::IProductInfo::OptionalFeatureInfo& optionalFeature : optionalFeatures){
		sdl::V1_0::imtlic::CProductOptionalFeature representation;
		representation.featureId = optionalFeature.featureId;
		representation.subFeatureIds.Emplace();
		representation.subFeatureIds->FromList(optionalFeature.subFeatureIds);

		retVal << representation;
	}

	return retVal;
}


bool CheckProductIdentity(
			const imtlic::IProductInfo& productInfo,
			const QByteArray& objectId,
			const imtbase::IObjectCollection& productCollection,
			QString& errorMessage)
{
	const QByteArray productId = productInfo.GetProductId();
	if (productId.isEmpty()){
		errorMessage = QT_TR_NOOP("Product Name cannot be empty");

		return false;
	}

	if (!FindProductByField(productCollection, "ProductId", productId, objectId).isEmpty()){
		errorMessage = QT_TR_NOOP(QStringLiteral("Product '%1' already exists")).arg(QString::fromUtf8(productId));

		return false;
	}

	const QString name = productInfo.GetName();
	if (name.trimmed().isEmpty()){
		errorMessage = QT_TR_NOOP("Product Name is empty");

		return false;
	}

	if (!FindProductByField(productCollection, "ProductName", name, objectId).isEmpty()){
		errorMessage = QT_TR_NOOP(QStringLiteral("Product name '%1' already exists, please rename")).arg(name);

		return false;
	}

	return true;
}


} // namespace imtlicgql


