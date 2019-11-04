#pragma once


// Qt includes
#include <QtGui/QOpenGLBuffer>
#include <QtGui/QOpenGLFunctions>

// ACF includes
#include <imod/CSingleModelObserverBase.h>

// ImtCore includes
#include <imt3dgui/IShape3d.h>


class QOpenGLContext;


namespace imt3dgui
{


/**
	Base thread-safe implementation for GL shapes
*/
class CShape3dBase: public imod::CSingleModelObserverBase, virtual public IShape3d
{
public:
	typedef imod::CSingleModelObserverBase BaseClass;

	CShape3dBase();
	virtual ~CShape3dBase();

	// reimplement (imt3dgui::IShape3d)
	bool IsValid() const override;
	bool IsVisible() const override;
	void SetVisible(bool isVisible) override;

	// reimplement (imt3dview::IScene3dItem)
	void SetCamera(const imt3dview::IScene3dCamera* cameraPtr) override;
	void SetProjection(const QMatrix4x4& projection) override;

	const QVector3D& GetPosition() const override;
	void SetPosition(const QVector3D& position) override;

	const QQuaternion& GetRotation() const override;
	void SetRotation(const QQuaternion& rotation) override;

	const QVector3D& GetScale() const override;
	void SetScale(const QVector3D& scale) override;

	// reimplement (imt3dview::IDrawable)
	void SetContext(QOpenGLContext* contextPtr) override;
	void DrawGl(QOpenGLShaderProgram& program) override;
	void Draw(QPainter& /*painter*/) override;

protected:
	// reimplement (IShape3d)
	virtual void OnContextInitialized(QOpenGLContext* contextPtr);
	virtual void OnContextDestroyed(QOpenGLContext* contextPtr);
	virtual ColorMode GetColorMode() const override { return IShape3d::CM_UNKNOWN; }
	virtual QVector3D GetColor() const override { return QVector3D(); }
	virtual bool HasNormals() const { return false; }

protected:
	// to be implemented in descendants
	virtual void UpdateShapeGeometry() = 0;
	virtual void DrawShapeGl(QOpenGLShaderProgram& program, QOpenGLFunctions& functions) = 0;

protected:
	void UpdateGeometry();
	QMatrix4x4 GetModelMatrix() const;
	QPoint ModelToWindow(const QVector3D& modelCoordinate, const QRect& viewPort) const;
	QVector3D WindowToModel(const QPoint& windowCoordinate, int z, const QRect& viewPort) const;

	template <typename TDataVector>
	void UploadGeometry(bool reallocate, const TDataVector& dataVector, QOpenGLBuffer& dataBuffer)
	{
		if (!dataVector.isEmpty() && dataBuffer.isCreated()){
			dataBuffer.bind();

			if (reallocate){
				dataBuffer.allocate(dataVector.data(), dataVector.count() * sizeof(dataVector.first()));
			}
			else{
				dataBuffer.write(0, dataVector.data(), dataVector.count() * sizeof(dataVector.first()));
			}
		}
	}

	// reimplemented (imod::CSingleModelObserverBase)
	void OnUpdate(const istd::IChangeable::ChangeSet& changeSet) override;

protected:
	struct Vertex
	{
		Vertex()
		{
		}

		Vertex(const QVector3D& inPosition, const QVector3D& inNormal = QVector3D(), const QVector3D& inColor = QVector3D())
			:position(inPosition),
			normal(inNormal),
			color(inColor)
		{
		}

		QVector3D position;
		QVector3D normal;
		QVector3D color;
	};

	QVector<Vertex> m_vertices;
	QVector<GLuint> m_indices;

	QVector3D m_position;
	QQuaternion m_rotation;
	QVector3D m_scale;

	QOpenGLContext* m_contextPtr;
	const imt3dview::IScene3dCamera* m_cameraPtr;
	QOpenGLBuffer m_vertexBuffer;
	QOpenGLBuffer m_indexBuffer;
	QMatrix4x4 m_projection;

	bool m_isVisible;
};


} // namespace imt3dgui


