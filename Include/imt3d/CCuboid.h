#pragma once


// ACF includes
#include <istd/TRange.h>
#include <i3d/CVector3d.h>


namespace imt3d
{


/**
	Definition of cuboid object with double dimension in 3D space.
*/
class CCuboid
{
public:
	CCuboid(double left = 0.0,
			double right = 0.0,
			double bottom = 0.0,
			double top = 0.0,
			double near = 0.0,
			double far = 0.0);

	i3d::CVector3d GetLeftBottomNear();
	void SetLeftBottomNear(const i3d::CVector3d& point);

	i3d::CVector3d GetRightBottomNear();
	void SetRightBottomNear(const i3d::CVector3d& point);

	i3d::CVector3d GetLeftTopNear();
	void SetLeftTopNear(const i3d::CVector3d& point);

	i3d::CVector3d GetRightTopNear();
	void SetRightTopNear(const i3d::CVector3d& point);

	i3d::CVector3d GetLeftBottomFar();
	void SetLeftBottomFar(const i3d::CVector3d& point);

	i3d::CVector3d GetRightBottomFar();
	void SetRightBottomFar(const i3d::CVector3d& point);

	i3d::CVector3d GetLeftTopFar();
	void SetLeftTopFar(const i3d::CVector3d& point);

	i3d::CVector3d GetRightTopFar();
	void SetRightTopFar(const i3d::CVector3d& point);

	double GetLeft() const;
	void SetLeft(double left);

	double GetTop() const;
	void SetTop(double top);

	double GetRight() const;
	void SetRight(double right);

	double GetBottom() const;
	void SetBottom(double bottom);

	double GetNear() const;
	void SetNear(double near);

	double GetFar() const;
	void SetFar(double far);

	double GetWidth() const;
	double GetHeight() const;
	double GetDepth() const;

	i3d::CVector3d GetCenterPoint();
	void ExpandToPoint(const i3d::CVector3d& point);

	const istd::CRange& GetHorizontalRange() const;
	istd::CRange& GetHorizontalRangeRef();
	void SetHorizontalRange(const istd::CRange& range);

	const istd::CRange& GetVerticalRange() const;
	istd::CRange& GetVerticalRangeRef();
	void SetVerticalRange(const istd::CRange& range);

	const istd::CRange& GetDeepRange() const;
	istd::CRange& GetDeepRangeRef();
	void SetDeepRange(const istd::CRange& range);

	bool IsValid() const;

	/**
		Get empty cuboid with all values set to 0.0.
	*/
	static const imt3d::CCuboid& GetEmpty();

	/**
		Get invalid cuboid.
	*/
	static const imt3d::CCuboid& GetInvalid();

	bool operator==(const CCuboid &object);

	bool operator!=(const CCuboid &object);

private:
	istd::CRange m_horizontalRange;
	istd::CRange m_verticalRange;
	istd::CRange m_depthRange;

	// static members
	static imt3d::CCuboid s_empty;
	static imt3d::CCuboid s_invalid;
};


// public methods

inline i3d::CVector3d CCuboid::GetLeftBottomNear()
{
	return i3d::CVector3d(m_horizontalRange.GetMinValue(), m_verticalRange.GetMinValue(), m_depthRange.GetMaxValue());
}


inline void CCuboid::SetLeftBottomNear(const i3d::CVector3d& point)
{
	m_horizontalRange.SetMinValue(point.GetX());
	m_verticalRange.SetMinValue(point.GetY());
	m_depthRange.SetMaxValue(point.GetZ());
}


inline i3d::CVector3d CCuboid::GetRightBottomNear()
{
	return i3d::CVector3d(m_horizontalRange.GetMaxValue(), m_verticalRange.GetMinValue(), m_depthRange.GetMaxValue());
}


inline void CCuboid::SetRightBottomNear(const i3d::CVector3d& point)
{
	m_horizontalRange.SetMaxValue(point.GetX());
	m_verticalRange.SetMinValue(point.GetY());
	m_depthRange.SetMaxValue(point.GetZ());
}


inline i3d::CVector3d CCuboid::GetLeftTopNear()
{
	return i3d::CVector3d(m_horizontalRange.GetMinValue(), m_verticalRange.GetMaxValue(), m_depthRange.GetMaxValue());
}


inline void CCuboid::SetLeftTopNear(const i3d::CVector3d& point)
{
	m_horizontalRange.SetMinValue(point.GetX());
	m_verticalRange.SetMaxValue(point.GetY());
	m_depthRange.SetMaxValue(point.GetZ());
}


inline i3d::CVector3d CCuboid::GetRightTopNear()
{
	return i3d::CVector3d(m_horizontalRange.GetMaxValue(), m_verticalRange.GetMaxValue(), m_depthRange.GetMaxValue());
}


inline void CCuboid::SetRightTopNear(const i3d::CVector3d& point)
{
	m_horizontalRange.SetMaxValue(point.GetX());
	m_verticalRange.SetMaxValue(point.GetY());
	m_depthRange.SetMaxValue(point.GetZ());
}


inline i3d::CVector3d CCuboid::GetLeftBottomFar()
{
	return i3d::CVector3d(m_horizontalRange.GetMinValue(), m_verticalRange.GetMinValue(), m_depthRange.GetMinValue());
}


inline void CCuboid::SetLeftBottomFar(const i3d::CVector3d& point)
{
	m_horizontalRange.SetMinValue(point.GetX());
	m_verticalRange.SetMinValue(point.GetY());
	m_depthRange.SetMinValue(point.GetZ());
}


inline i3d::CVector3d CCuboid::GetRightBottomFar()
{
	return i3d::CVector3d(m_horizontalRange.GetMaxValue(), m_verticalRange.GetMinValue(), m_depthRange.GetMinValue());
}


inline void CCuboid::SetRightBottomFar(const i3d::CVector3d& point)
{
	m_horizontalRange.SetMaxValue(point.GetX());
	m_verticalRange.SetMinValue(point.GetY());
	m_depthRange.SetMinValue(point.GetZ());
}


inline i3d::CVector3d CCuboid::GetLeftTopFar()
{
	return i3d::CVector3d(m_horizontalRange.GetMinValue(), m_verticalRange.GetMaxValue(), m_depthRange.GetMinValue());
}


inline void CCuboid::SetLeftTopFar(const i3d::CVector3d& point)
{
	m_horizontalRange.SetMinValue(point.GetX());
	m_verticalRange.SetMaxValue(point.GetY());
	m_depthRange.SetMinValue(point.GetZ());
}


inline i3d::CVector3d CCuboid::GetRightTopFar()
{
	return i3d::CVector3d(m_horizontalRange.GetMaxValue(), m_verticalRange.GetMaxValue(), m_depthRange.GetMinValue());
}


inline void CCuboid::SetRightTopFar(const i3d::CVector3d& point)
{
	m_horizontalRange.SetMaxValue(point.GetX());
	m_verticalRange.SetMaxValue(point.GetY());
	m_depthRange.SetMinValue(point.GetZ());
}


inline double CCuboid::GetLeft() const
{
	return m_horizontalRange.GetMinValue();
}


inline void CCuboid::SetLeft(double left)
{
	m_horizontalRange.SetMinValue(left);
}


inline double CCuboid::GetTop() const
{
	return m_verticalRange.GetMaxValue();
}


inline void CCuboid::SetTop(double top)
{
	m_verticalRange.SetMaxValue(top);
}


inline double CCuboid::GetRight() const
{
	return m_horizontalRange.GetMaxValue();
}


inline void CCuboid::SetRight(double right)
{
	m_horizontalRange.SetMaxValue(right);
}


inline double CCuboid::GetBottom() const
{
	return m_verticalRange.GetMinValue();
}


inline void CCuboid::SetBottom(double bottom)
{
	m_verticalRange.SetMinValue(bottom);
}


inline double CCuboid::GetNear() const
{
	return m_depthRange.GetMaxValue();
}


inline void CCuboid::SetNear(double nearValue)
{
	m_depthRange.SetMaxValue(nearValue);
}


inline double CCuboid::GetFar() const
{
	return m_depthRange.GetMinValue();
}


inline void CCuboid::SetFar(double farValue)
{
	m_depthRange.SetMinValue(farValue);
}


inline double CCuboid::GetWidth() const
{
	return m_horizontalRange.GetLength();
}


inline double CCuboid::GetHeight() const
{
	return m_verticalRange.GetLength();
}


inline double CCuboid::GetDepth() const
{
	return m_depthRange.GetLength();
}


inline i3d::CVector3d CCuboid::GetCenterPoint()
{
	return i3d::CVector3d(
				(m_horizontalRange.GetMaxValue() - m_horizontalRange.GetMinValue()) / 2,
				(m_verticalRange.GetMaxValue() - m_verticalRange.GetMinValue()) / 2,
				(m_depthRange.GetMaxValue() - m_depthRange.GetMinValue()) / 2);
}


inline void CCuboid::ExpandToPoint(const i3d::CVector3d& point)
{
	if ((point.GetX() > m_horizontalRange.GetMaxValue())){
		m_horizontalRange.SetMaxValue(point.GetX());
	} else if ((point.GetX() < m_horizontalRange.GetMinValue())){
		m_horizontalRange.SetMinValue(point.GetX());
	}
	if ((point.GetY() > m_verticalRange.GetMaxValue())){
		m_verticalRange.SetMaxValue(point.GetY());
	} else if ((point.GetY() < m_verticalRange.GetMinValue())){
		m_verticalRange.SetMinValue(point.GetY());
	}
	if ((point.GetZ() > m_depthRange.GetMaxValue())){
		m_depthRange.SetMaxValue(point.GetZ());
	} else if ((point.GetZ() < m_depthRange.GetMinValue())){
		m_depthRange.SetMinValue(point.GetZ());
	}
}


inline const istd::CRange& CCuboid::GetHorizontalRange() const
{
	return m_horizontalRange;
}


inline istd::CRange& CCuboid::GetHorizontalRangeRef()
{
	return m_horizontalRange;
}


inline void CCuboid::SetHorizontalRange(const istd::CRange& range)
{
	m_horizontalRange = range;
}


inline const istd::CRange& CCuboid::GetVerticalRange() const
{
	return m_verticalRange;
}


inline istd::CRange& CCuboid::GetVerticalRangeRef()
{
	return m_verticalRange;
}


inline void CCuboid::SetVerticalRange(const istd::CRange& range)
{
	m_verticalRange = range;
}


inline const istd::CRange& CCuboid::GetDeepRange() const
{
	return m_depthRange;
}


inline istd::CRange& CCuboid::GetDeepRangeRef()
{
	return m_depthRange;
}


inline void CCuboid::SetDeepRange(const istd::CRange& range)
{
	m_depthRange = range;
}


inline bool CCuboid::IsValid() const
{
	return (m_horizontalRange.GetLength() > I_BIG_EPSILON) && (m_verticalRange.GetLength() > I_BIG_EPSILON) && (m_depthRange.GetLength() > I_BIG_EPSILON);
}


// static methods

inline const imt3d::CCuboid& CCuboid::GetEmpty()
{
	return s_empty;
}


inline const imt3d::CCuboid& CCuboid::GetInvalid()
{
	return s_invalid;
}

inline bool CCuboid::operator==(const CCuboid & object)
{
	return ((m_horizontalRange == object.m_horizontalRange)
			&& (m_verticalRange == object.m_verticalRange)
			&& (m_depthRange == object.m_depthRange));
}

inline bool CCuboid::operator!=(const CCuboid & object)
{
	return !(operator==(object));
}


} // namespace imt3d


