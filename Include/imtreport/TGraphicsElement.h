#pragma once


// Qt includes
#include <QtCore/QMap>

// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CClassInfo.h>
#include <i2d/CRectangle.h>
#include <i2d/CCircle.h>
#include <i2d/CPolygon.h>

// ImtCore includes
#include <imtreport/CGraphicsElementBase.h>
#include <imtreport/CTextLabel.h>
#include <imtreport/CImageRectangle.h>


namespace imtreport
{


template <class Object2dType>
class TGraphicsElement: public Object2dType, public CGraphicsElementBase
{
public:
	typedef Object2dType BaseClass;
	typedef CGraphicsElementBase BaseClass2;
	
	static QByteArray GetTypeName();

	// reimplemented (iser::IObject)
	virtual QByteArray GetFactoryId() const;

	// reimplemented (iser::ISerializeable)
	virtual bool Serialize(iser::IArchive& archive);

	// reimplemented (istd::IChangeable)
	virtual bool CopyFrom(const istd::IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS);
	virtual istd::IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const;
};


template <class Object2dType>
QByteArray TGraphicsElement<Object2dType>::GetTypeName()
{
	QMap<QByteArray, QByteArray> knownTypesMap;
	knownTypesMap[istd::CClassInfo::GetName<i2d::CRectangle>()] = "Rectangle";
	knownTypesMap[istd::CClassInfo::GetName<i2d::CCircle>()] = "Circle";
	knownTypesMap[istd::CClassInfo::GetName<i2d::CLabel>()] = "Label";
	knownTypesMap[istd::CClassInfo::GetName<i2d::CPolygon>()] = "Polygon";
	knownTypesMap[istd::CClassInfo::GetName<i2d::CLine2d>()] = "Line";
	knownTypesMap[istd::CClassInfo::GetName<CTextLabel>()] = "TextLabel";
	knownTypesMap[istd::CClassInfo::GetName<CImageRectangle>()] = "Image";

	QByteArray baseTypeName = istd::CClassInfo::GetName<Object2dType>();

	if (knownTypesMap.contains(baseTypeName)){
		return knownTypesMap[baseTypeName];
	}

	return istd::CClassInfo::GetName<TGraphicsElement<Object2dType> >();
}


// reimplemented (iser::IObject)

template <class Object2dType>
QByteArray TGraphicsElement<Object2dType>::GetFactoryId() const
{
	return GetTypeName();
}


// reimplemented (iser::ISerializeable)

template <class Object2dType>
bool TGraphicsElement<Object2dType>::Serialize(iser::IArchive& archive)
{
	bool retVal = BaseClass::Serialize(archive);

	retVal = retVal && BaseClass2::Serialize(archive);

	return retVal;
}


// reimplemented (istd::IChangeable)

template <class Object2dType>
bool TGraphicsElement<Object2dType>::CopyFrom(const istd::IChangeable& object, CompatibilityMode mode)
{
	return BaseClass2::CopyFrom(object, mode) && BaseClass::CopyFrom(object, mode);
}


template <class Object2dType>
istd::IChangeable* TGraphicsElement<Object2dType>::CloneMe(CompatibilityMode mode) const
{
	istd::TDelPtr<TGraphicsElement<Object2dType> > clonePtr(new TGraphicsElement<Object2dType>);
	if (clonePtr->CopyFrom(*this, mode))
	{
		return clonePtr.PopPtr();
	}

	return nullptr;
}


typedef TGraphicsElement<i2d::CRectangle> CRectangleElement;
typedef TGraphicsElement<i2d::CCircle> CCircleElement;
typedef TGraphicsElement<i2d::CPolygon> CPolygonElement;
typedef TGraphicsElement<i2d::CLine2d> CLineElement;
typedef TGraphicsElement<CImageRectangle> CImageRectangleElement;


class CTextLabelElement : public TGraphicsElement<CTextLabel>
{
public:
	typedef TGraphicsElement<CTextLabel> BaseClass;

	CTextLabelElement()
	{
		m_graphicsAttributes.strokeWidth = -1;
	}
};


} // namespace imtreport


