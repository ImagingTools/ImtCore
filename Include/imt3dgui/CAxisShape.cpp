/********************************************************************************
**
**	Copyright (C) 2017-2020 ImagingTools GmbH
**
**	This file is part of the ImagingTools SDK.
**
**	This file may be used under the terms of the GNU Lesser
**	General Public License version 2.1 as published by the Free Software
**	Foundation and appearing in the file LicenseLGPL.txt included in the
**	packaging of this file.  Please review the following information to
**	ensure the GNU Lesser General Public License version 2.1 requirements
**	will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
**	If you are unsure which license is appropriate for your use, please
**	contact us at info@imagingtools.de.
**
**
********************************************************************************/


#include <imt3dgui/CAxisShape.h>


namespace imt3dgui
{


// static members
const float CAxisShape::s_zoomFontFactor = 50.0f;


// public methods

CAxisShape::CAxisShape()
	:m_axisLength(0.0),
	m_axisRange(0.0, 1.0),
	m_doUpdate(true)
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
}


void CAxisShape::SetAxisLength(double axisLength)
{
	Q_ASSERT(axisLength > 0.0);

	m_axisLength = axisLength;
	m_doUpdate = true;
}


void CAxisShape::SetAxisRange(const istd::CRange& range)
{
	m_axisRange = range;
	m_doUpdate = true;
}


// protected methods

// reimplement (imt3dgui::CShape3dBase)

// protected methods

// reimplement (imt3dgui::CShape3dBase)

void CAxisShape::UpdateShapeGeometry(const istd::IChangeable::ChangeSet & /*changeSet*/)
{
	if (!m_doUpdate) {
		return;
	}

	for (int i = 0; i < 3; i++) {
		imt3d::CPointCloud3d::PointXyzwRgba32* a = (imt3d::CPointCloud3d::PointXyzwRgba32*)m_data.GetPointData(i*2);
		a->data[i] = m_axisLength * m_axisRange.GetMinValue();
		imt3d::CPointCloud3d::PointXyzwRgba32* b = (imt3d::CPointCloud3d::PointXyzwRgba32*)m_data.GetPointData(i * 2 + 1);
		b->data[i] = m_axisLength * m_axisRange.GetMaxValue();
	}

	m_doUpdate = false;
}

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


