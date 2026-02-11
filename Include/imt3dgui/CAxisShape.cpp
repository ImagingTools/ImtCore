// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imt3dgui/CAxisShape.h>
#include <cmath>


namespace imt3dgui
{


// static members
const float CAxisShape::s_zoomFontFactor = 50.0f;


// public methods

CAxisShape::CAxisShape()
	:m_doUpdate(true)
{
	std::vector<imt3d::CPointCloud3d::PointXyzwRgba32> vertexes(6);

	// x (red)
	vertexes[0].data[4] = 1.0;
	vertexes[0].data[5] = 0.0;
	vertexes[0].data[6] = 0.0;
	vertexes[1].data[4] = 1.0;
	vertexes[1].data[5] = 0.0;
	vertexes[1].data[6] = 0.0;

	// y (green)
	vertexes[2].data[4] = 0.0;
	vertexes[2].data[5] = 1.0;
	vertexes[2].data[6] = 0.0;
	vertexes[3].data[4] = 0.0;
	vertexes[3].data[5] = 1.0;
	vertexes[3].data[6] = 0.0;

	// z (blue)
	vertexes[4].data[4] = 0.0;
	vertexes[4].data[5] = 0.0;
	vertexes[4].data[6] = 1.0;
	vertexes[5].data[4] = 0.0;
	vertexes[5].data[5] = 0.0;
	vertexes[5].data[6] = 1.0;

	m_data.CreateCloud(imt3d::CPointCloud3d::PF_XYZW_RGBA_32, 6, vertexes.data());

	m_indices = { 0, 1, 2, 3, 4, 5 };
	m_pointsDataPtr = &m_data;

	SetAxisLength(AT_X, 1.0);
	SetAxisRange(AT_X, istd::CRange(0, 1.0));
	SetAxisLineWidth(AT_X, 4);
	SetAxisLabel(AT_X, "X");

	SetAxisLength(AT_Y, 1.0);
	SetAxisRange(AT_Y, istd::CRange(0, 1.0));
	SetAxisLineWidth(AT_Y, 4);
	SetAxisLabel(AT_Y, "Y");

	SetAxisLength(AT_Z, 1.0);
	SetAxisRange(AT_Z, istd::CRange(0, 1.0));
	SetAxisLineWidth(AT_Z, 4);
	SetAxisLabel(AT_Z, "Z");
}


void CAxisShape::SetAxisLength(AxisType axis, double axisLength)
{
	Q_ASSERT(axisLength > 0.0);

	m_axisConfigs[axis].axisLength = axisLength;

	m_doUpdate = true;
}


void CAxisShape::SetAxisRange(AxisType axis, const istd::CRange& range)
{
	m_axisConfigs[axis].axisRange = range;

	m_doUpdate = true;
}


void CAxisShape::SetAxisLineWidth(AxisType axis, double lineWidth)
{
	m_axisConfigs[axis].lineWidth = lineWidth;

	m_doUpdate = true;
}


void CAxisShape::SetAxisLabel(AxisType axis, const QString& label)
{
	m_axisConfigs[axis].label = label;

	m_doUpdate = true;
}


// protected methods

// reimplement (imt3dgui::CShape3dBase)

void CAxisShape::UpdateShapeGeometry(const istd::IChangeable::ChangeSet & /*changeSet*/)
{
	if (!m_doUpdate){
		return;
	}

	// X
	imt3d::CPointCloud3d::PointXyzwRgba32* a = (imt3d::CPointCloud3d::PointXyzwRgba32*)m_data.GetPointData(AT_X * 2);
	a->data[AT_X] = m_axisConfigs[AT_X].axisLength * m_axisConfigs[AT_X].axisRange.GetMinValue();
	imt3d::CPointCloud3d::PointXyzwRgba32* b = (imt3d::CPointCloud3d::PointXyzwRgba32*)m_data.GetPointData(AT_X * 2 + 1);
	b->data[AT_X] = m_axisConfigs[AT_X].axisLength * m_axisConfigs[AT_X].axisRange.GetMaxValue();

	// Y
	a = (imt3d::CPointCloud3d::PointXyzwRgba32*)m_data.GetPointData(AT_Y * 2);
	a->data[AT_Y] = m_axisConfigs[AT_Y].axisLength * m_axisConfigs[AT_Y].axisRange.GetMinValue();
	b = (imt3d::CPointCloud3d::PointXyzwRgba32*)m_data.GetPointData(AT_Y * 2 + 1);
	b->data[AT_Y] = m_axisConfigs[AT_Y].axisLength * m_axisConfigs[AT_Y].axisRange.GetMaxValue();

	// Z
	a = (imt3d::CPointCloud3d::PointXyzwRgba32*)m_data.GetPointData(AT_Z * 2);
	a->data[AT_Z] = m_axisConfigs[AT_Z].axisLength * m_axisConfigs[AT_Z].axisRange.GetMinValue();
	b = (imt3d::CPointCloud3d::PointXyzwRgba32*)m_data.GetPointData(AT_Z * 2 + 1);
	b->data[AT_Z] = m_axisConfigs[AT_Z].axisLength * m_axisConfigs[AT_Z].axisRange.GetMaxValue();

	m_doUpdate = false;
}


void CAxisShape::DrawShapeGl(QOpenGLShaderProgram& /*program*/, QOpenGLFunctions& functions)
{
	double lineWidth = m_axisConfigs[AT_X].lineWidth;

	functions.glLineWidth(lineWidth);

	functions.glDrawElements(GL_LINES, m_indices.count(), GL_UNSIGNED_INT, 0);
}


// reimplement (imt3dgui::IDrawable)

void CAxisShape::Draw(QPainter& painter)
{
	if (!IsVisible()){
		return;
	}

	painter.save();

	painter.setRenderHint(QPainter::TextAntialiasing, true);
	painter.setPen(Qt::darkGray);
	QFont font = GetAxeLabelFont();
	painter.setFont(font);
	
	QFontMetrics fontMetrics(font);
	
	// Calculate text offset that scales with zoom to work correctly in orthographic projection
	// Use a small world-space offset (percentage of axis length) and project it to screen space
	const double offsetFactor = 0.02; // 2% of axis length
	const int minPixelOffset = 3; // Minimum offset in pixels
	
	QPoint origin = ModelToWindow(QVector3D(0.0, 0.0, 0.0));

	// X axis labels
	QString xLabel = m_axisConfigs[AT_X].label;
	QPoint windowCoordinate = ModelToWindow(QVector3D(m_axisConfigs[AT_X].axisLength * m_axisConfigs[AT_X].axisRange.GetMaxValue(), 0.0, 0.0));
	QRect textRect = fontMetrics.boundingRect(xLabel);
	
	// Calculate offset based on X axis length - project a small offset in X direction
	double baseOffset = m_axisConfigs[AT_X].axisLength * offsetFactor;
	QPoint offsetPoint = ModelToWindow(QVector3D(baseOffset, 0.0, 0.0));
	int textOffset = qMax(qAbs(offsetPoint.x() - origin.x()), minPixelOffset);
	
	windowCoordinate.setX(windowCoordinate.x() + textOffset);
	windowCoordinate.setY(windowCoordinate.y() + textRect.height() / 2);
	painter.drawText(windowCoordinate, xLabel);

	if (m_axisConfigs[AT_X].axisRange.GetMinValue() < 0.0){
		QString negXLabel = QString("-") + xLabel;
		windowCoordinate = ModelToWindow(QVector3D(m_axisConfigs[AT_X].axisLength * m_axisConfigs[AT_X].axisRange.GetMinValue(), 0.0, 0.0));
		textRect = fontMetrics.boundingRect(negXLabel);
		windowCoordinate.setX(windowCoordinate.x() - textRect.width() - textOffset);
		windowCoordinate.setY(windowCoordinate.y() + textRect.height() / 2);
		painter.drawText(windowCoordinate, negXLabel);
	}

	// Y axis labels
	QString yLabel = m_axisConfigs[AT_Y].label;
	windowCoordinate = ModelToWindow(QVector3D(0.0, m_axisConfigs[AT_Y].axisLength * m_axisConfigs[AT_Y].axisRange.GetMaxValue(), 0.0));
	textRect = fontMetrics.boundingRect(yLabel);
	
	// Calculate offset based on Y axis length - project a small offset in Y direction and use screen-space distance
	baseOffset = m_axisConfigs[AT_Y].axisLength * offsetFactor;
	offsetPoint = ModelToWindow(QVector3D(0.0, baseOffset, 0.0));
	QPoint delta = offsetPoint - origin;
	textOffset = qMax(static_cast<int>(std::sqrt(delta.x() * delta.x() + delta.y() * delta.y())), minPixelOffset);
	
	windowCoordinate.setX(windowCoordinate.x() + textOffset);
	windowCoordinate.setY(windowCoordinate.y() + textRect.height() / 2);
	painter.drawText(windowCoordinate, yLabel);

	if (m_axisConfigs[AT_Y].axisRange.GetMinValue() < 0.0){
		QString negYLabel = QString("-") + yLabel;
		windowCoordinate = ModelToWindow(QVector3D(0.0, m_axisConfigs[AT_Y].axisLength * m_axisConfigs[AT_Y].axisRange.GetMinValue(), 0.0));
		textRect = fontMetrics.boundingRect(negYLabel);
		windowCoordinate.setX(windowCoordinate.x() - textRect.width() - textOffset);
		windowCoordinate.setY(windowCoordinate.y() + textRect.height() / 2);
		painter.drawText(windowCoordinate, negYLabel);
	}

	// Z axis labels
	QString zLabel = m_axisConfigs[AT_Z].label;
	windowCoordinate = ModelToWindow(QVector3D(0.0, 0.0, m_axisConfigs[AT_Z].axisLength * m_axisConfigs[AT_Z].axisRange.GetMaxValue()));
	textRect = fontMetrics.boundingRect(zLabel);
	
	// Calculate offset based on Z axis length - project a small offset in Z direction and use screen-space distance
	baseOffset = m_axisConfigs[AT_Z].axisLength * offsetFactor;
	offsetPoint = ModelToWindow(QVector3D(0.0, 0.0, baseOffset));
	delta = offsetPoint - origin;
	textOffset = qMax(static_cast<int>(std::sqrt(delta.x() * delta.x() + delta.y() * delta.y())), minPixelOffset);
	
	windowCoordinate.setX(windowCoordinate.x() + textOffset);
	windowCoordinate.setY(windowCoordinate.y() + textRect.height() / 2);
	painter.drawText(windowCoordinate, zLabel);

	if (m_axisConfigs[AT_Z].axisRange.GetMinValue() < 0.0){
		QString negZLabel = QString("-") + zLabel;
		windowCoordinate = ModelToWindow(QVector3D(0.0, 0.0, m_axisConfigs[AT_Z].axisLength * m_axisConfigs[AT_Z].axisRange.GetMinValue()));
		textRect = fontMetrics.boundingRect(negZLabel);
		windowCoordinate.setX(windowCoordinate.x() - textRect.width() - textOffset);
		windowCoordinate.setY(windowCoordinate.y() + textRect.height() / 2);
		painter.drawText(windowCoordinate, negZLabel);
	}

	painter.restore();
}


// private methods

QFont CAxisShape::GetAxeLabelFont() const
{
	if (m_cameraPtr == nullptr){
		return QFont();
	}

	float fontSize = qAbs(s_zoomFontFactor / m_cameraPtr->GetPosition().z());
	fontSize = qMin(fontSize, 20.0f);

	QFont font("Verdana");
	font.setPointSizeF(fontSize);
	font.setWeight(QFont::Weight(80));

	return font;
}


} // namespace imt3dgui


