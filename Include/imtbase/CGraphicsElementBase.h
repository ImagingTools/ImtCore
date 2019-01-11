#pragma once


// ImtCore includes
#include <imtbase/IGraphicsElement.h>


namespace imtbase
{


/**
	Interface for describing a graphic element.
*/
class CGraphicsElementBase: virtual public IGraphicsElement
{
public:
	CGraphicsElementBase();

	// reimplemented (IGraphicsElement)
	virtual QColor GetFillColor() const;
	virtual void SetFillColor(const QColor& fillColor);
	virtual QColor GetStrokeColor() const;
	virtual void SetStrokeColor(const QColor& strokeColor);
	virtual double GetStrokeWidth() const;
	virtual void SetStrokeWidth(double strokeWidth);
	virtual void SetId(const QByteArray& id);
	virtual QByteArray GetId() const;

	// reimplemented (iser::ISerializeable)
	virtual bool Serialize(iser::IArchive& archive);

	// reimplemented (istd::IChangeable)
	virtual bool CopyFrom(const istd::IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS);

private:
	QColor m_fillColor;
	QColor m_strokeColor;
	double m_strokeWidth;
	QByteArray m_elementId;
};


} // namespace imtbase


