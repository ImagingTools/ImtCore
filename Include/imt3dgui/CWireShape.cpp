

#include <imt3dgui/CWireShape.h>


// Qt includes
#include <QtGui/QTextDocument>

// ACF includes
#include <imod/IModel.h>


namespace imt3dgui
{


// public methods

CWireShape::CWireShape()
	:m_color(QVector3D(0.0, 1.0, 1.0)),
	m_isInfoBoxEnabled(false)
{
}


void CWireShape::SetColor(const QVector3D& color)
{
	m_color = color;
}

void CWireShape::SetInfoBoxEnabled(bool isEnabled)
{
	m_isInfoBoxEnabled = isEnabled;
}


// protected methods

// reimplement (imt3dgui::CShape3dBase)

void CWireShape::UpdateShapeGeometry(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	const imt3d::IMesh3d* meshPtr = dynamic_cast<imt3d::IMesh3d*>(GetObservedModel());

	if (meshPtr == nullptr || meshPtr->IsEmpty()) {
		m_indices.clear();
		return;
	}

	m_pointsDataPtr = meshPtr;

	const imt3d::IMesh3d::Indices& indices = meshPtr->GetIndices();

	m_indices.clear();

	// update indices
	for (size_t i = 0; i < indices.size(); ++i) {
		const std::vector<uint32_t>& index = indices[i];

		for (size_t j = 0; j < index.size(); ++j) {
			m_indices.push_back(index[j]);
		}
	}
}


void CWireShape::DrawShapeGl(QOpenGLShaderProgram& /*program*/, QOpenGLFunctions& functions)
{
	functions.glDrawElements(GL_LINES, m_indices.size(), GL_UNSIGNED_INT, (GLuint*)0);
}


// reimplemented (imt3dview::IDrawable)

void CWireShape::Draw(QPainter& painter)
{
	if (m_isInfoBoxEnabled && m_pointsDataPtr != nullptr){
		QString text = QString("<b><p>Total vertices: %1</p>").arg(m_pointsDataPtr->GetPointsCount());
		text += QString("<p>Total faces: %1</p>").arg(m_indices.size() / 3);

		painter.save();

		painter.setBrush(QBrush(QColor(240, 240, 240)));
		painter.drawRoundedRect(10, 10, 300, 90, 3.0, 3.0);
		painter.translate(15.0, 15.0);

		QTextDocument doc;
		doc.setHtml(text);
		doc.drawContents(&painter);

		painter.restore();
	}
}


QVector3D CWireShape::GetColor() const
{
	return m_color;
}


} // namespace imt3dgui


