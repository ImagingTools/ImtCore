// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtreport/CGraphicsElementFactory.h>


// ImtCore includes
#include <imtreport/TGraphicsElement.h>
#include <imtreport/CTextTable.h>


namespace imtreport
{


// reimplemented (istd::TIFactory<iser::IObject2d>)

istd::TUniqueInterfacePtr<i2d::IObject2d> CGraphicsElementFactory::CreateInstance(const QByteArray& keyId) const
{
	if (keyId == CRectangleElement::GetTypeName())
		return istd::TUniqueInterfacePtr<i2d::IObject2d>(new CRectangleElement());
	else if (keyId == CCircleElement::GetTypeName())
		return istd::TUniqueInterfacePtr<i2d::IObject2d>(new CCircleElement());
	else if (keyId == CTextLabelElement::GetTypeName())
		return istd::TUniqueInterfacePtr<i2d::IObject2d>(new CTextLabelElement());
	else if (keyId == CPolygonElement::GetTypeName())
		return istd::TUniqueInterfacePtr<i2d::IObject2d>(new CPolygonElement());
	else if (keyId == CLineElement::GetTypeName())
		return istd::TUniqueInterfacePtr<i2d::IObject2d>(new CLineElement());
	else if (keyId == CImageRectangleElement::GetTypeName())
		return istd::TUniqueInterfacePtr<i2d::IObject2d>(new CImageRectangleElement());
	else if (keyId == CTextTable::GetTypeName())
		return istd::TUniqueInterfacePtr<i2d::IObject2d>(new CTextTable());
	else
		return istd::TUniqueInterfacePtr<i2d::IObject2d>();
}


// reimplemented (istd::IFactoryInfo)

CGraphicsElementFactory::KeyList CGraphicsElementFactory::GetFactoryKeys() const
{
	return {
				CRectangleElement::GetTypeName(),
				CCircleElement::GetTypeName(),
				CTextLabelElement::GetTypeName(),
				CPolygonElement::GetTypeName(),
				CLineElement::GetTypeName(),
				CImageRectangleElement::GetTypeName()
	};
}


} // namespace imtreport



