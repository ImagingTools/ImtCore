#pragma once


// ACF includes
#include <istd/TRange.h>

// ImtCore includes
#include <imt3d/imt3d.h>


namespace imt3d
{


/**
	Definition of cuboid object with double dimension in 3D space.
*/
class CCuboid
{
public:
	CCuboid(double left = 0.,
			double right = 0.,
			double bottom = 0.,
			double top = 0.,
			double near = 0.,
			double far = 0.);

	Point3d GetLeftBottomNear();
	void SetLeftBottomNear(const Point3d& point);
	Point3d GetRightBottomNear();
	void SetRightBottomNear(const Point3d& point);
	Point3d GetLeftTopNear();
	void SetLeftTopNear(const Point3d& point);
	Point3d GetRightTopNear();
	void SetRightTopNear(const Point3d& point);
	Point3d GetLeftBottomFar();
	void SetLeftBottomFar(const Point3d& point);
	Point3d GetRightBottomFar();
	void SetRightBottomFar(const Point3d& point);
	Point3d GetLeftTopFar();
	void SetLeftTopFar(const Point3d& point);
	Point3d GetRightTopFar();
	void SetRightTopFar(const Point3d& point);

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

	double GetWidth();
	double GetHeight();
	double GetDeep();

	Point3d GetCenterPoint();
	void ExpandToPoint(const Point3d& point);
	static CCuboid FromCloudPoints(const CloudPoints& cloudPoints);

	const istd::CRange& GetHorizontalRange() const;
	istd::CRange& GetHorizontalRangeRef();
	void SetHorizontalRange(const istd::CRange& range);

	const istd::CRange& GetVerticalRange() const;
	istd::CRange& GetVerticalRangeRef();
	void SetVerticalRange(const istd::CRange& range);

	const istd::CRange& GetDeepRange() const;
	istd::CRange& GetDeepRangeRef();
	void SetDeepRange(const istd::CRange& range);

	bool IsValid();

	/**
		Get empty cuboid with all values set to 0.
	*/
	static const imt3d::CCuboid& GetEmpty();
	/**
		Get invalid cuboid.
	*/
	static const imt3d::CCuboid& GetInvalid();
private:
	istd::CRange m_horizontal;
	istd::CRange m_vertical;
	istd::CRange m_deep;

	// static members
	static imt3d::CCuboid s_empty;
	static imt3d::CCuboid s_invalid;
};


// public methods

CCuboid::CCuboid(double left, double right, double bottom, double top, double near, double far)
	:m_horizontal(left, right),
	  m_vertical(bottom, top),
	  m_deep(far, near)
{
}


inline Point3d CCuboid::GetLeftBottomNear()
{
	return Point3d(m_horizontal.GetMinValue(), m_vertical.GetMinValue(), m_deep.GetMaxValue());
}


inline void CCuboid::SetLeftBottomNear(const Point3d& point)
{
	m_horizontal.SetMinValue(point.GetX());
	m_vertical.SetMinValue(point.GetY());
	m_deep.SetMaxValue(point.GetZ());
}


inline Point3d CCuboid::GetRightBottomNear()
{
	return Point3d(m_horizontal.GetMaxValue(), m_vertical.GetMinValue(), m_deep.GetMaxValue());
}


inline void CCuboid::SetRightBottomNear(const Point3d& point)
{
	m_horizontal.SetMaxValue(point.GetX());
	m_vertical.SetMinValue(point.GetY());
	m_deep.SetMaxValue(point.GetZ());
}


inline Point3d CCuboid::GetLeftTopNear()
{
	return Point3d(m_horizontal.GetMinValue(), m_vertical.GetMaxValue(), m_deep.GetMaxValue());
}


inline void CCuboid::SetLeftTopNear(const Point3d& point)
{
	m_horizontal.SetMinValue(point.GetX());
	m_vertical.SetMaxValue(point.GetY());
	m_deep.SetMaxValue(point.GetZ());
}


inline Point3d CCuboid::GetRightTopNear()
{
	return Point3d(m_horizontal.GetMaxValue(), m_vertical.GetMaxValue(), m_deep.GetMaxValue());
}


inline void CCuboid::SetRightTopNear(const Point3d& point)
{
	m_horizontal.SetMaxValue(point.GetX());
	m_vertical.SetMaxValue(point.GetY());
	m_deep.SetMaxValue(point.GetZ());
}


inline Point3d CCuboid::GetLeftBottomFar()
{
	return Point3d(m_horizontal.GetMinValue(), m_vertical.GetMinValue(), m_deep.GetMinValue());
}


inline void CCuboid::SetLeftBottomFar(const Point3d& point)
{
	m_horizontal.SetMinValue(point.GetX());
	m_vertical.SetMinValue(point.GetY());
	m_deep.SetMinValue(point.GetZ());
}


inline Point3d CCuboid::GetRightBottomFar()
{
	return Point3d(m_horizontal.GetMaxValue(), m_vertical.GetMinValue(), m_deep.GetMinValue());
}


inline void CCuboid::SetRightBottomFar(const Point3d& point)
{
	m_horizontal.SetMaxValue(point.GetX());
	m_vertical.SetMinValue(point.GetY());
	m_deep.SetMinValue(point.GetZ());
}


inline Point3d CCuboid::GetLeftTopFar()
{
	return Point3d(m_horizontal.GetMinValue(), m_vertical.GetMaxValue(), m_deep.GetMinValue());
}


inline void CCuboid::SetLeftTopFar(const Point3d& point)
{
	m_horizontal.SetMinValue(point.GetX());
	m_vertical.SetMaxValue(point.GetY());
	m_deep.SetMinValue(point.GetZ());
}


inline Point3d CCuboid::GetRightTopFar()
{
	return Point3d(m_horizontal.GetMaxValue(), m_vertical.GetMaxValue(), m_deep.GetMinValue());
}


inline void CCuboid::SetRightTopFar(const Point3d& point)
{
	m_horizontal.SetMaxValue(point.GetX());
	m_vertical.SetMaxValue(point.GetY());
	m_deep.SetMinValue(point.GetZ());
}


double CCuboid::GetLeft() const
{
	return m_horizontal.GetMinValue();
}


void CCuboid::SetLeft(double left)
{
	m_horizontal.SetMinValue(left);
}


double CCuboid::GetTop() const
{
	return m_vertical.GetMaxValue();
}


void CCuboid::SetTop(double top)
{
	m_vertical.SetMaxValue(top);
}


double CCuboid::GetRight() const
{
	return m_horizontal.GetMaxValue();
}


void CCuboid::SetRight(double right)
{
	m_horizontal.SetMaxValue(right);
}


double CCuboid::GetBottom() const
{
	return m_vertical.GetMinValue();
}


void CCuboid::SetBottom(double bottom)
{
	m_vertical.SetMinValue(bottom);
}


double CCuboid::GetNear() const
{
	return m_deep.GetMaxValue();
}


void CCuboid::SetNear(double near)
{
	m_deep.SetMaxValue(near);
}


double CCuboid::GetFar() const
{
	return m_deep.GetMinValue();
}


void CCuboid::SetFar(double far)
{
	m_deep.SetMinValue(far);
}


inline double CCuboid::GetWidth()
{
	return m_horizontal.GetLength();
}


inline double CCuboid::GetHeight()
{
	return m_vertical.GetLength();
}


inline double CCuboid::GetDeep()
{
	return m_deep.GetLength();
}

Point3d CCuboid::GetCenterPoint()
{
	return Point3d((m_horizontal.GetMaxValue() - m_horizontal.GetMinValue())/ 2,
					(m_vertical.GetMaxValue() - m_vertical.GetMinValue())/ 2,
					(m_deep.GetMaxValue() - m_deep.GetMinValue())/ 2);
}


inline void CCuboid::ExpandToPoint(const Point3d& point)
{
	if ((point.GetX() > m_horizontal.GetMaxValue())){
		m_horizontal.SetMaxValue(point.GetX());
	} else if ((point.GetX() < m_horizontal.GetMinValue())){
		m_horizontal.SetMinValue(point.GetX());
	}
	if ((point.GetY() > m_vertical.GetMaxValue())){
		m_vertical.SetMaxValue(point.GetY());
	} else if ((point.GetY() < m_vertical.GetMinValue())){
		m_vertical.SetMinValue(point.GetY());
	}
	if ((point.GetZ() > m_deep.GetMaxValue())){
		m_deep.SetMaxValue(point.GetZ());
	} else if ((point.GetZ() < m_deep.GetMinValue())){
		m_deep.SetMinValue(point.GetZ());
	}
}



CCuboid CCuboid::FromCloudPoints(const CloudPoints& cloudPoints)
{

	if (cloudPoints.count() > 0){
		double left = cloudPoints.first().GetX();
		double right = cloudPoints.first().GetX();
		double bottom = cloudPoints.first().GetY();
		double top = cloudPoints.first().GetY();
		double near = cloudPoints.first().GetZ();
		double far = cloudPoints.first().GetZ();

		for (CloudPoints::const_iterator pointIter = cloudPoints.constBegin(); pointIter != cloudPoints.constEnd(); pointIter++){
			if ((pointIter->GetX() > right)){
				right = pointIter->GetX();
			} else if ((pointIter->GetX() < left)){
				left = pointIter->GetX();
			}
			if ((pointIter->GetY() > top)){
				top = pointIter->GetY();
			} else if ((pointIter->GetY() < bottom)){
				bottom = pointIter->GetY();
			}
			if ((pointIter->GetZ() > near)){
				near = pointIter->GetZ();
			} else if ((pointIter->GetZ() < far)){
				far = pointIter->GetZ();
			}
		}

		return CCuboid(left, right, bottom, top, near, far);
	}
	else{
		return s_invalid;
	}

	return s_invalid;
}


inline const istd::CRange& CCuboid::GetHorizontalRange() const
{
	return m_horizontal;
}


inline istd::CRange& CCuboid::GetHorizontalRangeRef()
{
	return m_horizontal;
}


inline void CCuboid::SetHorizontalRange(const istd::CRange& range)
{
	m_horizontal = range;
}


inline const istd::CRange& CCuboid::GetVerticalRange() const
{
	return m_vertical;
}


inline istd::CRange& CCuboid::GetVerticalRangeRef()
{
	return m_vertical;
}


inline void CCuboid::SetVerticalRange(const istd::CRange& range)
{
	m_vertical = range;
}


inline const istd::CRange& CCuboid::GetDeepRange() const
{
	return m_deep;
}


inline istd::CRange& CCuboid::GetDeepRangeRef()
{
	return m_deep;
}


inline void CCuboid::SetDeepRange(const istd::CRange& range)
{
	m_deep = range;
}


inline bool CCuboid::IsValid()
{
	return (m_horizontal.GetLength() > I_BIG_EPSILON) && (m_vertical.GetLength() > I_BIG_EPSILON) && (m_deep.GetLength() > I_BIG_EPSILON);
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


} // namespace imt3d


