// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonValue>


/// These are useless classes that were created as expected from the generator. \todo They MUST be erased.


namespace sdl::imtsdl2d::imt2d
{


class CPoint2d
{
public:
	double m_x = 0.;
	double m_y = 0.;

	[[nodiscard]] double GetX() const;
	virtual void SetX(double newX);
	[[nodiscard]] double GetY() const;
	virtual void SetY(double newY);
};


class CLine2d
{

public:
	CPoint2d m_point1;
	CPoint2d m_point2;

	[[nodiscard]] CPoint2d GetPoint1() const;
	virtual void SetPoint1(CPoint2d newPoint1);
	[[nodiscard]] CPoint2d GetPoint2() const;
	virtual void SetPoint2(CPoint2d newPoint2);
};


class CPolyline2d
{
public:
	QList<CPoint2d> m_points;

	[[nodiscard]] QList<CPoint2d> GetPoints() const;
	virtual void SetPoints(const QList<CPoint2d>& newPoints);
};


class CPolygon2d
{
public:
	QList<CPoint2d> m_points;
	bool m_isClosed = false;

	[[nodiscard]] QList<CPoint2d> GetPoints() const;
	virtual void SetPoints(const QList<CPoint2d>& newPoints);
	[[nodiscard]] bool GetIsClosed() const;
	virtual void SetIsClosed(bool newIsClosed);
};


class CRectangle2d
{
public:
	CPoint2d m_topLeft;
	CPoint2d m_bottomRight;

	[[nodiscard]] CPoint2d GetTopLeft() const;
	virtual void SetTopLeft(CPoint2d newTopLeft);
	[[nodiscard]] CPoint2d GetBottomRight() const;
	virtual void SetBottomRight(CPoint2d newBottomRight);
};


class CCircle
{
public:
	CPoint2d m_center;
	double m_radius = 0.;

	[[nodiscard]] CPoint2d GetCenter() const;
	virtual void SetCenter(CPoint2d newCenter);
	[[nodiscard]] double GetRadius() const;
	virtual void SetRadius(double newRadius);
};


class CAnnulus
{
public:
	CPoint2d m_center;
	double m_innerRadius = 0.;
	double m_outerRadius = 0.;

	[[nodiscard]] CPoint2d GetCenter() const;
	virtual void SetCenter(CPoint2d newCenter);
	[[nodiscard]] double GetInnerRadius() const;
	virtual void SetInnerRadius(double newInnerRadius);
	[[nodiscard]] double GetOuterRadius() const;
	virtual void SetOuterRadius(double newOuterRadius);
};

inline double CPoint2d::GetY() const
{
	return m_y;
}

inline void CPoint2d::SetY(double newY)
{
	m_y = newY;
}

inline double CPoint2d::GetX() const
{
	return m_x;
}

inline void CPoint2d::SetX(double newX)
{
	m_x = newX;
}

inline CPoint2d CLine2d::GetPoint2() const
{
	return m_point2;
}

inline void CLine2d::SetPoint2(CPoint2d newPoint2)
{
	m_point2 = newPoint2;
}

inline CPoint2d CLine2d::GetPoint1() const
{
	return m_point1;
}

inline  void CLine2d::SetPoint1(CPoint2d newPoint1)
{
	m_point1 = newPoint1;
}

inline QList<CPoint2d> CPolyline2d::GetPoints() const
{
	return m_points;
}

inline  void CPolyline2d::SetPoints(const QList<CPoint2d>& newPoints)
{
	m_points = newPoints;
}

inline bool CPolygon2d::GetIsClosed() const
{
	return m_isClosed;
}

inline  void CPolygon2d::SetIsClosed(bool newIsClosed)
{
	m_isClosed = newIsClosed;
}

inline QList<CPoint2d> CPolygon2d::GetPoints() const
{
	return m_points;
}

inline  void CPolygon2d::SetPoints(const QList<CPoint2d>& newPoints)
{
	m_points = newPoints;
}

inline CPoint2d CRectangle2d::GetBottomRight() const
{
	return m_bottomRight;
}

inline  void CRectangle2d::SetBottomRight(CPoint2d newBottomRight)
{
	m_bottomRight = newBottomRight;
}

inline CPoint2d CRectangle2d::GetTopLeft() const
{
	return m_topLeft;
}

inline  void CRectangle2d::SetTopLeft(CPoint2d newTopLeft)
{
	m_topLeft = newTopLeft;
}

inline double CCircle::GetRadius() const
{
	return m_radius;
}

inline  void CCircle::SetRadius(double newRadius)
{
	m_radius = newRadius;
}

inline CPoint2d CCircle::GetCenter() const
{
	return m_center;
}

inline  void CCircle::SetCenter(CPoint2d newCenter)
{
	m_center = newCenter;
}

inline double CAnnulus::GetInnerRadius() const
{
	return m_innerRadius;
}

inline  void CAnnulus::SetInnerRadius(double newInnerRadius)
{
	m_innerRadius = newInnerRadius;
}

inline double CAnnulus::GetOuterRadius() const
{
	return m_outerRadius;
}

inline  void CAnnulus::SetOuterRadius(double newOuterRadius)
{
	m_outerRadius = newOuterRadius;
}

inline CPoint2d CAnnulus::GetCenter() const
{
	return m_center;
}

inline  void CAnnulus::SetCenter(CPoint2d newCenter)
{
	m_center = newCenter;
}


}
