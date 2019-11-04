#pragma once


// Qt includes
#include <QtGui/QOpenGLFunctions>
#include <QtWidgets/QOpenGLWidget>
#include <QtCore/QTimer>
#include <QtCore/QVariantAnimation>

// ACF includes
#include <istd/istd.h>

// ImtCore includes
#include <imtview3d/CTrackballCamera.h>
#include <imtview3d/CScene3d.h>


namespace imtgui3d
{

class ISceneEventHandler;
class IScene3dCamera;

class COpenGLWidget: public QOpenGLWidget, protected QOpenGLFunctions
{
	Q_OBJECT

public:
	enum ViewDirection
	{
		VD_DEFAULT = 0,
		VD_RIGHT,
		VD_FRONT,
		VD_TOP,
		VD_LEFT,
		VD_BOTTOM,
		VD_BACK
	};

	enum ViewMode
	{
		VM_SHOW,
		VM_SELECTION
	};

	enum SelectionMode
	{
		SM_BOX,
		SM_CIRCLE
	};

	enum RenderHint
	{
		RH_ANTIALIASING = 0x1,
		RH_CULLFACE = 0x2
	};

	I_DECLARE_FLAGS(RenderHints, RH_ANTIALIASING, RH_CULLFACE);

	COpenGLWidget(QWidget *parent = 0);
	~COpenGLWidget();
	
	void SetSceneEventHandler(ISceneEventHandler* handlerPtr);
	void UnsetSceneEventHandler();
	imtview3d::IScene3dCamera* GetCamera();
	imtview3d::IScene3d* GetScene();
	void ZoomIn();
	void ZoomOut();
	void ShowGrid(bool show);
	void ShowAxis(bool show);
	bool GetRenderHint(RenderHint renderHint) const;
	void SetRenderHint(RenderHint renderHint, bool on = true);
	void SetCameraView(ViewDirection viewDirection, bool animated = true);
	void SetBoxSelection(bool on);
	void SetCircleSelection(bool on);
	void ClearSelection();
	void InvertSelection();
	void DeleteSelection();

protected:
	// reimplemented (QOpenGLWidget)
	void initializeGL() override;
	void resizeGL(int w, int h) override;
	void paintGL() override;

	// reimplemented (QWidget)
	void mousePressEvent(QMouseEvent *e) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent *e) override;
	void closeEvent(QCloseEvent* event) override;
	void wheelEvent(QWheelEvent* event) override;

private Q_SLOTS:
	void OnZoomIn();
	void OnZoomOut();
	void OnInternalTimer();
	void OnCameraRotationAnimation(const QVariant& value);
	void OnCameraPositionAnimation(const QVariant& value);

private:
	void PaintGl();
	void Paint(QPainter& painter);
	void PaintSelection(QPainter& painter);
	void MouseMoveEventShow(QMouseEvent& e);
	void MouseMoveEventSelection(QMouseEvent& e);
	void SetSelection(SelectionMode selectionMode, bool on);
	void SetGlFlags();
	void SetGlUniformValues();
	QMatrix4x4 GetProjectionMatrix() const;

private:
	QPoint m_mouseClickPosition;
	QPoint m_prevMousePosition;
	QRect m_selectionRect;
	imtgui3d::ISceneEventHandler* m_eventHandler;
	imtview3d::CTrackballCamera m_camera;
	imtview3d::CScene3d m_scene;
	QTimer m_timer;
	QVariantAnimation m_cameraRotationAnimation;
	QVariantAnimation m_cameraPositionAnimation;
	int m_renderHints;
	ViewMode m_viewMode;
	SelectionMode m_selectionMode;
	QOpenGLShaderProgram* m_programPtr;

	static const float s_verticalAngle;
	static const float s_nearPlane;
	static const float s_farPlane;
	static const float s_defaultLineWidth;
	static const QVector3D s_defaultCameraPosition;
	static const QVector3D s_lightPosition;
	static const QVector3D s_lightColor;
};


} // namespace imtgui3d


