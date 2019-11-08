#pragma once


// Qt includes
#include <QtGui/QOpenGLFunctions>
#include <QtWidgets/QOpenGLWidget>
#include <QtCore/QTimer>
#include <QtCore/QVariantAnimation>

// ACF includes
#include <istd/istd.h>

// ImtCore includes
#include <imt3dview/CTrackballCamera.h>
#include <imt3dview/CScene3d.h>


namespace imt3dgui
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

	enum SelectionMode
	{
		SM_NONE,
		SM_POINT,
		SM_BOX,
		SM_CIRCLE
	};

	enum RotationMode
	{
		RM_FREE,
		RM_AROUND_X,
		RM_AROUND_Y,
		RM_AROUND_Z
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
	imt3dview::IScene3dCamera* GetCamera();
	imt3dview::IScene3d* GetScene();
	void ZoomIn();
	void ZoomOut();
	void ShowGrid(bool show);
	void ShowAxis(bool show);
	bool GetRenderHint(RenderHint renderHint) const;
	void SetRenderHint(RenderHint renderHint, bool on = true);
	void SetCameraView(ViewDirection viewDirection, bool animated = true);
	void SetSelectionMode(SelectionMode selectionMode);
	void SetRotationMode(RotationMode rotationMode);
	void ClearSelection();
	void AllSelection();
	void InvertSelection();
	void DeleteSelection();
	void BoxFromSelection();

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
	void MouseMoveEventNoSelection(QMouseEvent& e);
	void MouseMoveEventSelection(QMouseEvent& e);
	void SetGlFlags();
	void SetGlUniformValues();
	QMatrix4x4 GetProjectionMatrix() const;

private:
	QPoint m_mouseClickPosition;
	QPoint m_prevMousePosition;
	QRect m_selectionRect;
	imt3dgui::ISceneEventHandler* m_eventHandler;
	imt3dview::CTrackballCamera m_camera;
	imt3dview::CScene3d m_scene;
	QTimer m_timer;
	QVariantAnimation m_cameraRotationAnimation;
	QVariantAnimation m_cameraPositionAnimation;
	int m_renderHints;
	SelectionMode m_selectionMode;
	RotationMode m_rotationMode;
	QOpenGLShaderProgram* m_programPtr;

	static const float s_verticalAngle;
	static const float s_nearPlane;
	static const float s_farPlane;
	static const float s_defaultLineWidth;
	static const QVector3D s_defaultCameraPosition;
	static const QVector3D s_lightPosition;
	static const QVector3D s_lightColor;
};


} // namespace imt3dgui


