#include <imtreport/CGraphicsElementFactory.h>

// ImtCore includes
#include <imtreport/TGraphicsElement.h>

namespace imtreport
{


// reimplemented (istd::TIFactory<iser::IObject>)

IGraphicsElement* CGraphicsElementFactory::CreateInstance(const QByteArray& keyId) const
{
	if (keyId == CRectangleElement::GetTypeName())
		return new CRectangleElement();
	else if (keyId == CCircleElement::GetTypeName())
		return new CCircleElement();
	else if (keyId == CTextLabelElement::GetTypeName())
		return new CTextLabelElement();
	else if (keyId == CPolygonElement::GetTypeName())
		return new CPolygonElement();
	else if (keyId == CLineElement::GetTypeName())
		return new CLineElement();
	else if (keyId == CImageRectangleElement::GetTypeName())
		return new CImageRectangleElement();
	else
		return NULL;
}


// reimplemented (istd::IFactoryInfo)

CGraphicsElementFactory::KeyList CGraphicsElementFactory::GetFactoryKeys() const
{
	return
	{
		CRectangleElement::GetTypeName(),
		CCircleElement::GetTypeName(),
		CTextLabelElement::GetTypeName(),
		CPolygonElement::GetTypeName(),
		CLineElement::GetTypeName(),
		CImageRectangleElement::GetTypeName()
	};
}


} // namespace imtreport



