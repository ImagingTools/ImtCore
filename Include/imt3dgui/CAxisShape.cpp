#include <imt3dgui/CAxisShape.h>
#include <cmath>


namespace imt3dgui
{


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
	// Use the maximum line width from all axes since glLineWidth applies to all lines
	double lineWidth = qMax(m_axisConfigs[AT_X].lineWidth,
	                        qMax(m_axisConfigs[AT_Y].lineWidth,
	                             m_axisConfigs[AT_Z].lineWidth));

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

	// Use a clear, readable font for axis labels
	QFont font("Arial", 10);
	font.setBold(true);
	painter.setFont(font);
	painter.setRenderHint(QPainter::TextAntialiasing, true);
	
	QFontMetrics fontMetrics(font);
	const int textOffset = 5; // Fixed pixel offset from axis endpoint
	
	// X axis label
	QString xLabel = m_axisConfigs[AT_X].label;
	QPoint xPos = ModelToWindow(QVector3D(m_axisConfigs[AT_X].axisLength * m_axisConfigs[AT_X].axisRange.GetMaxValue(), 0.0, 0.0));
	QRect xTextRect = fontMetrics.boundingRect(xLabel);
	xPos.setX(xPos.x() + textOffset);
	xPos.setY(xPos.y() + xTextRect.height() / 2);
	painter.setPen(QColor(255, 0, 0)); // Red for X axis
	painter.drawText(xPos, xLabel);

	// Y axis label
	QString yLabel = m_axisConfigs[AT_Y].label;
	QPoint yPos = ModelToWindow(QVector3D(0.0, m_axisConfigs[AT_Y].axisLength * m_axisConfigs[AT_Y].axisRange.GetMaxValue(), 0.0));
	QRect yTextRect = fontMetrics.boundingRect(yLabel);
	yPos.setX(yPos.x() + textOffset);
	yPos.setY(yPos.y() + yTextRect.height() / 2);
	painter.setPen(QColor(0, 255, 0)); // Green for Y axis
	painter.drawText(yPos, yLabel);

	// Z axis label
	QString zLabel = m_axisConfigs[AT_Z].label;
	QPoint zPos = ModelToWindow(QVector3D(0.0, 0.0, m_axisConfigs[AT_Z].axisLength * m_axisConfigs[AT_Z].axisRange.GetMaxValue()));
	QRect zTextRect = fontMetrics.boundingRect(zLabel);
	zPos.setX(zPos.x() + textOffset);
	zPos.setY(zPos.y() + zTextRect.height() / 2);
	painter.setPen(QColor(0, 0, 255)); // Blue for Z axis
	painter.drawText(zPos, zLabel);

	painter.restore();
}


} // namespace imt3dgui


