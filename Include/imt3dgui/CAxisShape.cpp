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

	// Rebuild the entire geometry with axis lines and 3D letter labels
	std::vector<imt3d::CPointCloud3d::PointXyzwRgba32> vertices;
	m_indices.clear();

	// Calculate label size and offset based on axis length
	float minAxisLength = m_axisConfigs[AT_X].axisLength;
	if (m_axisConfigs[AT_Y].axisLength < minAxisLength)
		minAxisLength = m_axisConfigs[AT_Y].axisLength;
	if (m_axisConfigs[AT_Z].axisLength < minAxisLength)
		minAxisLength = m_axisConfigs[AT_Z].axisLength;
	
	float labelSize = minAxisLength * 0.1f;
	float labelOffset = labelSize * 0.5f;

	// X axis line
	imt3d::CPointCloud3d::PointXyzwRgba32 vertex;
	vertex.data[0] = m_axisConfigs[AT_X].axisLength * m_axisConfigs[AT_X].axisRange.GetMinValue();
	vertex.data[1] = 0.0f;
	vertex.data[2] = 0.0f;
	vertex.data[3] = 1.0f;
	vertex.data[4] = 1.0f; // Red
	vertex.data[5] = 0.0f;
	vertex.data[6] = 0.0f;
	vertex.data[7] = 1.0f;
	vertices.push_back(vertex);
	
	vertex.data[0] = m_axisConfigs[AT_X].axisLength * m_axisConfigs[AT_X].axisRange.GetMaxValue();
	vertices.push_back(vertex);
	m_indices.push_back(vertices.size() - 2);
	m_indices.push_back(vertices.size() - 1);

	// Y axis line
	vertex.data[0] = 0.0f;
	vertex.data[1] = m_axisConfigs[AT_Y].axisLength * m_axisConfigs[AT_Y].axisRange.GetMinValue();
	vertex.data[2] = 0.0f;
	vertex.data[4] = 0.0f;
	vertex.data[5] = 1.0f; // Green
	vertex.data[6] = 0.0f;
	vertices.push_back(vertex);
	
	vertex.data[1] = m_axisConfigs[AT_Y].axisLength * m_axisConfigs[AT_Y].axisRange.GetMaxValue();
	vertices.push_back(vertex);
	m_indices.push_back(vertices.size() - 2);
	m_indices.push_back(vertices.size() - 1);

	// Z axis line
	vertex.data[0] = 0.0f;
	vertex.data[1] = 0.0f;
	vertex.data[2] = m_axisConfigs[AT_Z].axisLength * m_axisConfigs[AT_Z].axisRange.GetMinValue();
	vertex.data[4] = 0.0f;
	vertex.data[5] = 0.0f;
	vertex.data[6] = 1.0f; // Blue
	vertices.push_back(vertex);
	
	vertex.data[2] = m_axisConfigs[AT_Z].axisLength * m_axisConfigs[AT_Z].axisRange.GetMaxValue();
	vertices.push_back(vertex);
	m_indices.push_back(vertices.size() - 2);
	m_indices.push_back(vertices.size() - 1);

	// Add 3D letter labels using OpenGL primitives
	QVector3D xPos(m_axisConfigs[AT_X].axisLength * m_axisConfigs[AT_X].axisRange.GetMaxValue() + labelOffset, 0.0f, 0.0f);
	QVector3D yPos(0.0f, m_axisConfigs[AT_Y].axisLength * m_axisConfigs[AT_Y].axisRange.GetMaxValue() + labelOffset, 0.0f);
	QVector3D zPos(0.0f, 0.0f, m_axisConfigs[AT_Z].axisLength * m_axisConfigs[AT_Z].axisRange.GetMaxValue() + labelOffset);

	AddLetterGeometry(vertices, m_indices, m_axisConfigs[AT_X].label, xPos, labelSize, QVector3D(1.0f, 0.0f, 0.0f));
	AddLetterGeometry(vertices, m_indices, m_axisConfigs[AT_Y].label, yPos, labelSize, QVector3D(0.0f, 1.0f, 0.0f));
	AddLetterGeometry(vertices, m_indices, m_axisConfigs[AT_Z].label, zPos, labelSize, QVector3D(0.0f, 0.0f, 1.0f));

	// Recreate point cloud with new geometry
	m_data.CreateCloud(imt3d::CPointCloud3d::PF_XYZW_RGBA_32, vertices.size(), vertices.data());

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

void CAxisShape::Draw(QPainter& /*painter*/)
{
	// Labels are now rendered as 3D OpenGL primitives in DrawShapeGl
	// No 2D text rendering needed
}


// private methods

void CAxisShape::AddLetterGeometry(std::vector<imt3d::CPointCloud3d::PointXyzwRgba32>& vertices, 
                                    QVector<GLuint>& indices,
                                    const QString& letter,
                                    const QVector3D& position,
                                    float size,
                                    const QVector3D& color)
{
	// Helper lambda to add a line segment
	auto addLine = [&](float x1, float y1, float z1, float x2, float y2, float z2) {
		imt3d::CPointCloud3d::PointXyzwRgba32 v1, v2;
		
		v1.data[0] = position.x() + x1 * size;
		v1.data[1] = position.y() + y1 * size;
		v1.data[2] = position.z() + z1 * size;
		v1.data[3] = 1.0f;
		v1.data[4] = color.x();
		v1.data[5] = color.y();
		v1.data[6] = color.z();
		v1.data[7] = 1.0f;
		
		v2.data[0] = position.x() + x2 * size;
		v2.data[1] = position.y() + y2 * size;
		v2.data[2] = position.z() + z2 * size;
		v2.data[3] = 1.0f;
		v2.data[4] = color.x();
		v2.data[5] = color.y();
		v2.data[6] = color.z();
		v2.data[7] = 1.0f;
		
		vertices.push_back(v1);
		vertices.push_back(v2);
		indices.push_back(vertices.size() - 2);
		indices.push_back(vertices.size() - 1);
	};

	// Define letter geometry using simple line primitives
	// Letters are drawn in a normalized coordinate system (0-1 range)
	QString upperLetter = letter.toUpper();
	
	if (upperLetter == "X") {
		// X is two diagonal lines forming a cross
		addLine(-0.5f, -0.5f, 0.0f,  0.5f,  0.5f, 0.0f); // Bottom-left to top-right
		addLine(-0.5f,  0.5f, 0.0f,  0.5f, -0.5f, 0.0f); // Top-left to bottom-right
	}
	else if (upperLetter == "Y") {
		// Y is a vertical line with two diagonal lines at top
		addLine( 0.0f,  0.0f, 0.0f,  0.0f, -0.5f, 0.0f); // Vertical stem
		addLine( 0.0f,  0.0f, 0.0f, -0.5f,  0.5f, 0.0f); // Top-left diagonal
		addLine( 0.0f,  0.0f, 0.0f,  0.5f,  0.5f, 0.0f); // Top-right diagonal
	}
	else if (upperLetter == "Z") {
		// Z is three lines forming the letter
		addLine(-0.5f,  0.5f, 0.0f,  0.5f,  0.5f, 0.0f); // Top horizontal
		addLine( 0.5f,  0.5f, 0.0f, -0.5f, -0.5f, 0.0f); // Diagonal
		addLine(-0.5f, -0.5f, 0.0f,  0.5f, -0.5f, 0.0f); // Bottom horizontal
	}
	// Unknown letters are silently ignored and will not be rendered
}


} // namespace imt3dgui


