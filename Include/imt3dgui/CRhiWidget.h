// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QLine>
#include <QtCore/QTimer>
#include <QtCore/QVariantAnimation>
#include <QtWidgets/QRhiWidget>

// ACF includes
#include <istd/istd.h>

// ImtCore includes
#include <imt3dview/CScene3d.h>
#include <imt3dgui/CRhiRenderBackend.h>


namespace imt3dgui
{

class ISceneEventHandler;


/**
	Qt RHI host widget for the 3D scene.

	Mirrors the public API of COpenGLWidget so that callers can switch between the
	two backends with minimal code changes. The widget itself inherits QRhiWidget
	(available since Qt 6.7) and drives CRhiRenderBackend through the standard
	IRenderBackend interface.

	### Differences from COpenGLWidget
	* The 2D QPainter overlay (axis labels, mesh info boxes) is **not** rendered inside
	  this widget because QRhiWidget does not support QPainter drawing over the RHI
	  surface. Applications that need the overlay should place a transparent sibling
	  widget on top and drive m_scene.DrawOverlay() from it, or switch to
	  COpenGLWidget.
	* Line width is fixed at 1 pixel on most RHI backends; the RH_ANTIALIASING hint
	  enables MSAA (4×) at construction time via QRhiWidget::setSampleCount().
*/
class CRhiWidget: public QRhiWidget
{
	Q_OBJECT

public:
	enum ProjectionMode { PM_PERSPECTIVE, PM_ORTHO };
	enum ViewDirection  { VD_DEFAULT = 0, VD_RIGHT, VD_FRONT, VD_TOP, VD_LEFT, VD_BOTTOM, VD_BACK };
	enum ViewMode       { VM_VIEW, VM_SELECTION };
	enum SelectionMode  { SM_POINT, SM_BOX, SM_CIRCLE };
	enum RotationMode   { RTM_FREE, RTM_AROUND_X, RTM_AROUND_Y, RTM_AROUND_Z };

	enum RenderHint
	{
		RH_ANTIALIASING = 0x1,
		RH_CULLFACE     = 0x2,
		RH_BLEND        = 0x4
	};

	I_DECLARE_FLAGS(RenderHints, RH_ANTIALIASING, RH_CULLFACE, RH_BLEND);

	explicit CRhiWidget(QWidget* parentPtr = nullptr);
	~CRhiWidget() override;

	// Scene / camera
	void SetCamera(imt3dview::IScene3dCamera* cameraPtr);
	void SetSceneEventHandler(ISceneEventHandler* handlerPtr);
	void UnsetSceneEventHandler();
	imt3dview::IScene3d* GetScene();

	// Navigation helpers
	void ZoomIn();
	void ZoomOut();

	// Scene-content visibility
	void ShowGrid(bool show);
	void ShowAxis(bool show);
	void ShowRuler(bool show);

	// Render hints
	bool GetRenderHint(RenderHint renderHint) const;
	void SetRenderHint(RenderHint renderHint, bool on = true);

	// View presets
	void SetCameraView(ViewDirection viewDirection, bool animated = true);

	// Widget mode
	void SetViewMode(ViewMode viewMode);
	void SetProjectionMode(ProjectionMode projectionMode);
	void SetSelectionMode(SelectionMode selectionMode);
	void SetRotationMode(RotationMode rotationMode);

	// Selection helpers
	void ClearSelection();
	void AllSelection();
	void InvertSelection();
	void DeleteSelection();

	// Appearance
	void SetBackgroundColor(const QColor& backgroundColor);

protected:
	// reimplemented (QRhiWidget)
	virtual void initialize(QRhiCommandBuffer* cb) override;
	virtual void render(QRhiCommandBuffer* cb) override;

	// reimplemented (QWidget)
	virtual void resizeEvent(QResizeEvent* event) override;
	virtual void mousePressEvent(QMouseEvent* e) override;
	virtual void mouseReleaseEvent(QMouseEvent* e) override;
	virtual void mouseMoveEvent(QMouseEvent* e) override;
	virtual void closeEvent(QCloseEvent* event) override;
	virtual void wheelEvent(QWheelEvent* event) override;
	virtual void keyPressEvent(QKeyEvent* e) override;

private Q_SLOTS:
	void OnZoomIn();
	void OnZoomOut();
	void OnInternalTimer();
	void OnCameraRotationAnimation(const QVariant& value);
	void OnCameraPositionAnimation(const QVariant& value);

private:
	void PaintSelection(QPainter& painter);
	void MousePressView(QMouseEvent& e);
	void MousePressSelection(QMouseEvent& e);
	void MouseMoveView(QMouseEvent& e);
	void MouseMoveSelection(QMouseEvent& e);
	imt3dview::SSceneState BuildSceneState() const;
	QMatrix4x4 GetProjectionMatrix() const;
	static void GetFovRect(float aspectRatio, float nearPlane, float& width, float& height);

private:
	QPoint m_mouseClickPosition;
	QPoint m_prevMousePosition;
	QRect  m_selectionRect;

	imt3dgui::ISceneEventHandler* m_eventHandlerPtr  = nullptr;
	imt3dview::CScene3d           m_scene;
	imt3dgui::CRhiRenderBackend   m_backend;
	imt3dview::IScene3dCamera*    m_cameraPtr         = nullptr;

	QTimer            m_timer;
	QVariantAnimation m_cameraRotationAnimation;
	QVariantAnimation m_cameraPositionAnimation;

	int            m_renderHints   = 0;
	ViewMode       m_viewMode      = VM_VIEW;
	SelectionMode  m_selectionMode = SM_POINT;
	RotationMode   m_rotationMode  = RTM_FREE;
	ProjectionMode m_projectionMode = PM_PERSPECTIVE;

	QColor m_backgroundColor{ Qt::black };

	bool m_backendReady = false;

	static const float      s_verticalAngle;
	static const float      s_nearPlane;
	static const float      s_farPlane;
	static const QVector3D  s_lightPosition;
	static const QVector3D  s_lightColor;
};


} // namespace imt3dgui
