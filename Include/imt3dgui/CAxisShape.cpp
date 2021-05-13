#include <imt3dgui/CAxisShape.h>


namespace imt3dgui
{


// static members
const float CAxisShape::s_zoomFontFactor = 50.0f;


// public methods

CAxisShape::CAxisShape()
	:m_axisLength(0.0),
	m_axisRange(0.0, 1.0)
{
}


void CAxisShape::SetAxisLength(double axisLength)
{
	Q_ASSERT(axisLength > 0.0);

	m_axisLength = axisLength;
}


void CAxisShape::SetAxisRange(const istd::CRange& range)
{
	m_axisRange = range;
}


// protected methods

// reimplement (imt3dgui::CShape3dBase)

void CAxisShape::UpdateShapeGeometry(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	m_vertices.clear();
	m_vertices.resize(6);

	// x (red)
	m_vertices[0] = Vertex(QVector3D(m_axisLength * m_axisRange.GetMinValue(), 0.0, 0.0), QVector3D(), QVector3D(1.0, 0.0, 0.0));
	m_vertices[1] = Vertex(QVector3D(m_axisLength * m_axisRange.GetMaxValue(), 0.0, 0.0), QVector3D(), QVector3D(1.0, 0.0, 0.0));

	// y (green)
	m_vertices[2] = Vertex(QVector3D(0.0, m_axisLength * m_axisRange.GetMinValue(), 0.0), QVector3D(), QVector3D(0.0, 1.0, 0.0));
	m_vertices[3] = Vertex(QVector3D(0.0, m_axisLength * m_axisRange.GetMaxValue(), 0.0), QVector3D(), QVector3D(0.0, 1.0, 0.0));

	// z (blue)
	m_vertices[4] = Vertex(QVector3D(0.0, 0.0, m_axisLength * m_axisRange.GetMinValue()), QVector3D(), QVector3D(0.0, 0.0, 1.0));
	m_vertices[5] = Vertex(QVector3D(0.0, 0.0, m_axisLength * m_axisRange.GetMaxValue()), QVector3D(), QVector3D(0.0, 0.0, 1.0));
	
	m_indices = { 0, 1, 2, 3, 4, 5 };
}


// protected methods

// reimplement (imt3dgui::CShape3dBase)

void CAxisShape::DrawShapeGl(QOpenGLShaderProgram& /*program*/, QOpenGLFunctions& functions)
{
	functions.glLineWidth(2.0f);
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
	painter.setFont(GetAxeLabelFont());

	QPoint windowCoordinate = ModelToWindow(QVector3D(m_axisLength * m_axisRange.GetMaxValue(), 0.0, 0.0));
	painter.drawText(windowCoordinate, QString("X"));

	if (m_axisRange.GetMinValue() < 0.0){
		painter.drawText(ModelToWindow(QVector3D(m_axisLength * m_axisRange.GetMinValue(), 0.0, 0.0)), QString("-X"));
	}

	windowCoordinate = ModelToWindow(QVector3D(0.0, m_axisLength * m_axisRange.GetMaxValue(), 0.0));
	painter.drawText(windowCoordinate, "Y");

	if (m_axisRange.GetMinValue() < 0.0){
		painter.drawText(ModelToWindow(QVector3D(0.0, m_axisLength * m_axisRange.GetMinValue(), 0.0)), QString("-Y"));
	}

	windowCoordinate = ModelToWindow(QVector3D(0.0, 0.0, m_axisLength * m_axisRange.GetMaxValue()));
	painter.drawText(windowCoordinate, "Z");

	if (m_axisRange.GetMinValue() < 0.0){
		painter.drawText(ModelToWindow(QVector3D(0.0, 0.0, m_axisLength * m_axisRange.GetMinValue())), QString("-Z"));
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
	font.setWeight(80);

	return font;
}


} // namespace imt3dgui


