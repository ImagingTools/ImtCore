// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtlic/IProductInfo.h>
#include <GeneratedFiles/imtlicsdl/SDL/1.0/CPP/Products.h>


namespace imtlicgql
{


// The conversions between a product and its GraphQL representation and the checks of a product,
// shared by the product collection controller and the product document service.

/**
	Fills the product from its representation; the object ID of the product is not touched.
	The features of the representation are completed with the features they require, which are
	read from \a featureCollection; requirements no feature carries go to \a unresolvedRequirementsPtr.
*/
void FillProductFromRepresentation(
			const sdl::V1_0::imtlic::CProductData& representation,
			const imtbase::IObjectCollection& featureCollection,
			imtlic::IProductInfo& productInfo,
			QByteArrayList* unresolvedRequirementsPtr = nullptr);

void FillRepresentationFromProduct(
			imtlic::IProductInfo& productInfo,
			sdl::V1_0::imtlic::CProductData& representation);

/**
	Turn the product's optional-part record into its wire form: one entry per
	feature, each carrying the ids of the parts taken from it.
*/
imtsdl::TElementList<sdl::V1_0::imtlic::CProductOptionalFeature> CreateOptionalFeaturesRepresentation(
			const imtlic::IProductInfo& productInfo);

/**
	Checks that the product ID and the name of the product are set and that no other product
	of \a productCollection has them. \a objectId is the product itself, empty for a new one.
*/
bool CheckProductIdentity(
			const imtlic::IProductInfo& productInfo,
			const QByteArray& objectId,
			const imtbase::IObjectCollection& productCollection,
			QString& errorMessage);


} // namespace imtlicgql


