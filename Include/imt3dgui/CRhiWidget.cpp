// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imt3dgui/CRhiWidget.h>


#include <cmath>

// Qt includes
#include <QtCore/QDebug>
#include <QtGui/QMouseEvent>
#include <QtGui/QPainter>

// ImtCore includes
#include <imt3dgui/ISceneEventHandler.h>


namespace
{

// Infrequent timer: avoids hammering the GPU when nothing is animating.
const int s_timerIntervalMs = 1000;

} // namespace


namespace imt3dgui
{


// static members — identical values to COpenGLWidget

const float CRhiWidget::s_verticalAngle = 45.0f;
const float CRhiWidget::s_nearPlane = 0.1f;
const float CRhiWidget::s_farPlane = 100.0f;
const QVector3D CRhiWidget::s_lightPosition(1.2f, 1.0f, 2.0f);
const QVector3D CRhiWidget::s_lightColor(1.0f, 1.0f, 1.0f);


// public methods

CRhiWidget::CRhiWidget(QWidget* parentPtr)
	:QRhiWidget(parentPtr)
{
	setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
	setMouseTracking(true);
	setCursor(Qt::OpenHandCursor);
	setFocusPolicy(Qt::FocusPolicy::ClickFocus);

	connect(
		&m_cameraRotationAnimation, &QVariantAnimation::valueChanged,
		this, &CRhiWidget::OnCameraRotationAnimation);
	connect(
		&m_cameraPositionAnimation, &QVariantAnimation::valueChanged,
		this, &CRhiWidget::OnCameraPositionAnimation);

	m_timer.setInterval(s_timerIntervalMs);
	connect(&m_timer, &QTimer::timeout, this, &CRhiWidget::OnInternalTimer);
	m_timer.start();
}


CRhiWidget::~CRhiWidget()
{
	// Detach shapes from the backend before it is destroyed so that their
	// IRenderResourcePtr handles are released while the QRhi is still valid.
	if (m_backendReady){
		m_scene.OnAttachBackend(nullptr);
		m_backend.Shutdown();
		m_backendReady = false;
	}
}


void CRhiWidget::SetCamera(imt3dview::IScene3dCamera* cameraPtr)
{
	m_cameraPtr = cameraPtr;
}


void CRhiWidget::SetSceneEventHandler(ISceneEventHandler* handlerPtr)
{
	m_eventHandlerPtr = handlerPtr;
}


void CRhiWidget::UnsetSceneEventHandler()
{
	m_eventHandlerPtr = nullptr;
}


imt3dview::IScene3d* CRhiWidget::GetScene()
{
	return &m_scene;
}


void CRhiWidget::ZoomIn()
{
	if (m_cameraPtr && m_viewMode == VM_VIEW){
		m_cameraPtr->ZoomIn();
		update();
	}
}


void CRhiWidget::ZoomOut()
{
	if (m_cameraPtr && m_viewMode == VM_VIEW){
		m_cameraPtr->ZoomOut();
		update();
	}
}


void CRhiWidget::ShowGrid(bool show)
{
	if (m_eventHandlerPtr){
		m_eventHandlerPtr->OnShowGrid(show);
		update();
	}
}


void CRhiWidget::ShowAxis(bool show)
{
	if (m_eventHandlerPtr){
		m_eventHandlerPtr->OnShowAxis(show);
		update();
	}
}


void CRhiWidget::ShowRuler(bool show)
{
	if (m_eventHandlerPtr){
		m_eventHandlerPtr->OnShowRuler(show);
		update();
	}
}


bool CRhiWidget::GetRenderHint(RenderHint renderHint) const
{
	return m_renderHints & renderHint;
}


void CRhiWidget::SetRenderHint(RenderHint renderHint, bool on)
{
	if (on){
		m_renderHints |= renderHint;
	}
	else{
		m_renderHints &= ~renderHint;
	}
}


void CRhiWidget::SetCameraView(ViewDirection viewDirection, bool animated)
{
	if (!m_cameraPtr){
		return;
	}

	m_cameraRotationAnimation.stop();
	m_cameraPositionAnimation.stop();

	QQuaternion newRotation;

	switch (viewDirection){
	case VD_DEFAULT:
		newRotation = QQuaternion::fromAxisAndAngle(1.0, 0.0, 0.0, -80.0)
		            * QQuaternion::fromAxisAndAngle(0.0, 0.0, 1.0, -120)
		            * QQuaternion::fromAxisAndAngle(0.0, 1.0, 0.0, 10);
		break;
	case VD_RIGHT:
		newRotation = QQuaternion::fromAxisAndAngle(0.0, 1.0, 0.0, -90.0);
		break;
	case VD_FRONT:
		newRotation = QQuaternion::fromAxisAndAngle(0.0, 1.0, 0.0, 0.0);
		break;
	case VD_TOP:
		newRotation = QQuaternion::fromAxisAndAngle(1.0, 0.0, 0.0, 90.0);
		break;
	case VD_LEFT:
		newRotation = QQuaternion::fromAxisAndAngle(0.0, 1.0, 0.0, 90.0);
		break;
	case VD_BOTTOM:
		newRotation = QQuaternion::fromAxisAndAngle(1.0, 0.0, 0.0, -90.0);
		break;
	case VD_BACK:
		newRotation = QQuaternion::fromAxisAndAngle(0.0, 1.0, 0.0, 180.0);
		break;
	default:
		return;
	}

	if (animated){
		m_cameraRotationAnimation.setStartValue(m_cameraPtr->GetRotation());
		m_cameraRotationAnimation.setEndValue(newRotation);
		m_cameraRotationAnimation.setDuration(500);
		m_cameraRotationAnimation.start();

		m_cameraPositionAnimation.setStartValue(m_cameraPtr->GetPosition());
		m_cameraPositionAnimation.setEndValue(QVector3D(0.0, 0.0, 5.0));
		m_cameraPositionAnimation.setDuration(500);
		m_cameraPositionAnimation.start();
	}
	else{
		m_cameraPtr->RotateTo(newRotation);
		m_cameraPtr->MoveTo(QVector3D(0.0, 0.0, 5.0));
		update();
	}
}


void CRhiWidget::SetViewMode(ViewMode viewMode)
{
	m_viewMode = viewMode;
	m_selectionRect.setRect(0, 0, 0, 0);

	switch (viewMode){
	case VM_VIEW:
		setCursor(Qt::OpenHandCursor);
		break;
	case VM_SELECTION:
		SetSelectionMode(SM_POINT);
		break;
	}
}


void CRhiWidget::SetProjectionMode(ProjectionMode projectionMode)
{
	m_projectionMode = projectionMode;
	update();
}


void CRhiWidget::SetSelectionMode(SelectionMode selectionMode)
{
	m_selectionMode = selectionMode;

	switch (selectionMode){
	case SM_BOX:
	case SM_CIRCLE:
		setCursor(Qt::CrossCursor);
		break;
	case SM_POINT:
		setCursor(Qt::PointingHandCursor);
		break;
	}
}


void CRhiWidget::SetRotationMode(RotationMode rotationMode)
{
	m_rotationMode = rotationMode;
}


void CRhiWidget::ClearSelection()
{
	if (m_eventHandlerPtr){
		m_eventHandlerPtr->OnClearSelection();
	}
}


void CRhiWidget::AllSelection()
{
	if (m_eventHandlerPtr){
		m_eventHandlerPtr->OnAllSelection();
	}
}


void CRhiWidget::InvertSelection()
{
	if (m_eventHandlerPtr){
		m_eventHandlerPtr->OnInvertSelection();
	}
}


void CRhiWidget::DeleteSelection()
{
	if (m_eventHandlerPtr){
		m_eventHandlerPtr->OnDeleteSelection();
	}
}


void CRhiWidget::SetBackgroundColor(const QColor& backgroundColor)
{
	m_backgroundColor = backgroundColor;
	update();
}


// protected methods — QRhiWidget callbacks

void CRhiWidget::initialize(QRhiCommandBuffer* /*cb*/)
{
	// Called by Qt before the first render() and whenever the backing store
	// (swapchain) is recreated (e.g. after a device-lost event).
	// Perform a full reinitialization: detach the scene, shut down the old
	// backend resources, then reinitialize everything from scratch.

	if (m_backendReady){
		m_scene.OnAttachBackend(nullptr);
		m_backend.Shutdown();
		m_backendReady = false;
	}

	m_backend.SetRhiContext(
				rhi(),
				renderTarget(),
				nullptr, // commandBuffer not needed for Initialize()
				renderTarget()->renderPassDescriptor());

	if (!m_backend.Initialize()){
		qDebug() << "CRhiWidget::initialize: backend initialization failed";
		return;
	}

	m_scene.SetCamera(m_cameraPtr);
	m_scene.SetViewPort(rect());
	m_scene.SetProjection(GetProjectionMatrix());
	m_scene.OnAttachBackend(&m_backend);

	m_backendReady = true;
}


void CRhiWidget::render(QRhiCommandBuffer* cb)
{
	if (!m_backendReady){
		return;
	}

	// Keep scene matrices in sync with the current widget size (may change without
	// triggering initialize() on some backends/platforms).
	m_scene.SetViewPort(rect());
	m_scene.SetProjection(GetProjectionMatrix());

	m_backend.SetRhiContext(
				rhi(),
				renderTarget(),
				cb,
				renderTarget()->renderPassDescriptor());

	const imt3dview::SceneState state = BuildSceneState();
	m_backend.BeginFrame(state);
	m_scene.Render(m_backend);
	m_backend.EndFrame();

	// Note: QPainter overlay (m_scene.DrawOverlay) is intentionally omitted here.
	// QRhiWidget does not support mixing QPainter and RHI rendering in the same
	// surface. Place a transparent overlay QWidget on top of CRhiWidget if 2D
	// labels (axis names, info boxes) are required.
}


// protected methods — QWidget event overrides

void CRhiWidget::resizeEvent(QResizeEvent* event)
{
	QRhiWidget::resizeEvent(event);

	m_scene.SetViewPort(rect());
	m_scene.SetProjection(GetProjectionMatrix());
	update();
}


void CRhiWidget::mousePressEvent(QMouseEvent* e)
{
	m_cameraRotationAnimation.stop();
	m_cameraPositionAnimation.stop();

	m_mouseClickPosition = m_prevMousePosition = e->pos();

	switch (m_viewMode){
	case VM_VIEW:
		MousePressView(*e);
		break;
	case VM_SELECTION:
		MousePressSelection(*e);
		break;
	}
}


void CRhiWidget::mouseReleaseEvent(QMouseEvent* e)
{
	if (m_viewMode == VM_VIEW){
		setCursor(Qt::OpenHandCursor);

		if (m_eventHandlerPtr){
			m_eventHandlerPtr->OnMouseRelease(*e);
		}
	}

	m_selectionRect.setRect(0, 0, 0, 0);
}


void CRhiWidget::mouseMoveEvent(QMouseEvent* e)
{
	switch (m_viewMode){
	case VM_VIEW:
		MouseMoveView(*e);
		break;
	case VM_SELECTION:
		MouseMoveSelection(*e);
		break;
	}

	m_prevMousePosition = e->pos();
}


void CRhiWidget::closeEvent(QCloseEvent* event)
{
	m_scene.OnAttachBackend(nullptr);
	m_backend.Shutdown();
	m_backendReady = false;

	QRhiWidget::closeEvent(event);
}


void CRhiWidget::wheelEvent(QWheelEvent* event)
{
	if (event->pixelDelta().y() > 0 || event->angleDelta().y() > 0){
		OnZoomIn();
	}
	else{
		OnZoomOut();
	}
}


void CRhiWidget::keyPressEvent(QKeyEvent* e)
{
	switch (e->key()){
	case Qt::Key_PageUp:
		ZoomIn();
		break;
	case Qt::Key_PageDown:
		ZoomOut();
		break;
	case Qt::Key_Right:
		if (m_cameraPtr){
			m_cameraPtr->MoveTo(QPoint(width() / 2, height() / 2), QPoint(width() / 2 - 5, height() / 2));
			update();
		}
		break;
	case Qt::Key_Left:
		if (m_cameraPtr){
			m_cameraPtr->MoveTo(QPoint(width() / 2, height() / 2), QPoint(width() / 2 + 5, height() / 2));
			update();
		}
		break;
	case Qt::Key_Up:
		if (m_cameraPtr){
			m_cameraPtr->MoveTo(QPoint(width() / 2, height() / 2), QPoint(width() / 2, height() / 2 + 5));
			update();
		}
		break;
	case Qt::Key_Down:
		if (m_cameraPtr){
			m_cameraPtr->MoveTo(QPoint(width() / 2, height() / 2), QPoint(width() / 2, height() / 2 - 5));
			update();
		}
		break;
	default:
		break;
	}
}


// private slots

void CRhiWidget::OnZoomIn()
{
	ZoomIn();
}


void CRhiWidget::OnZoomOut()
{
	ZoomOut();
}


void CRhiWidget::OnInternalTimer()
{
	if (isVisible()){
		update();
	}
}


void CRhiWidget::OnCameraRotationAnimation(const QVariant& value)
{
	if (m_cameraPtr){
		m_cameraPtr->RotateTo(value.value<QQuaternion>());
		update();
	}
}


void CRhiWidget::OnCameraPositionAnimation(const QVariant& value)
{
	if (m_cameraPtr){
		m_cameraPtr->MoveTo(value.value<QVector3D>());
		update();
	}
}


// private methods

void CRhiWidget::PaintSelection(QPainter& painter)
{
	painter.save();

	switch (m_selectionMode){
	case SM_BOX:
		if (m_selectionRect.isValid()){
			painter.setPen(Qt::DashLine);
			painter.drawRect(m_selectionRect);
		}
		break;
	case SM_CIRCLE:
		if (m_selectionRect.isValid()){
			painter.setPen(Qt::DashLine);
			painter.drawEllipse(m_selectionRect);
		}
		break;
	default:
		break;
	}

	painter.restore();
}


void CRhiWidget::MousePressView(QMouseEvent& e)
{
	setCursor(Qt::ClosedHandCursor);

	if (m_eventHandlerPtr){
		m_eventHandlerPtr->OnMousePress(e);
	}
}


void CRhiWidget::MousePressSelection(QMouseEvent& e)
{
	if (m_selectionMode == SM_POINT && m_eventHandlerPtr){
		const bool clearPreviousSelection = !e.modifiers().testFlag(Qt::ControlModifier);
		m_eventHandlerPtr->OnPointSelection(m_mouseClickPosition, clearPreviousSelection);
	}
}


void CRhiWidget::MouseMoveView(QMouseEvent& e)
{
	if (!m_cameraPtr || (m_eventHandlerPtr && m_eventHandlerPtr->OnMouseMove(e))){
		return;
	}

	if (e.buttons() == Qt::LeftButton){
		switch (m_rotationMode){
		case RTM_FREE:
			m_cameraPtr->RotateTo(m_prevMousePosition, e.pos());
			break;
		case RTM_AROUND_X:
			m_cameraPtr->RotateTo(m_prevMousePosition, e.pos(), QVector3D(1.0, 0.0, 0.0));
			break;
		case RTM_AROUND_Y:
			m_cameraPtr->RotateTo(m_prevMousePosition, e.pos(), QVector3D(0.0, 1.0, 0.0));
			break;
		case RTM_AROUND_Z:
			m_cameraPtr->RotateTo(m_prevMousePosition, e.pos(), QVector3D(0.0, 0.0, 1.0));
			break;
		}
		update();
	}
	else if (e.buttons() == Qt::RightButton){
		m_cameraPtr->MoveTo(m_prevMousePosition, e.pos());
		update();
	}
}


void CRhiWidget::MouseMoveSelection(QMouseEvent& e)
{
	if (e.buttons() != Qt::LeftButton){
		return;
	}

	const bool clearPreviousSelection = !e.modifiers().testFlag(Qt::ControlModifier);

	switch (m_selectionMode){
	case SM_BOX:
	case SM_CIRCLE:
		if (e.pos().x() > m_mouseClickPosition.x() && e.pos().y() < m_mouseClickPosition.y()){
			m_selectionRect.setCoords(m_mouseClickPosition.x(), e.pos().y(), e.pos().x(), m_mouseClickPosition.y());
		}
		else if (e.pos().x() > m_mouseClickPosition.x() && e.pos().y() > m_mouseClickPosition.y()){
			m_selectionRect.setCoords(m_mouseClickPosition.x(), m_mouseClickPosition.y(), e.pos().x(), e.pos().y());
		}
		else if (e.pos().x() < m_mouseClickPosition.x() && e.pos().y() < m_mouseClickPosition.y()){
			m_selectionRect.setCoords(e.pos().x(), e.pos().y(), m_mouseClickPosition.x(), m_mouseClickPosition.y());
		}
		else{
			m_selectionRect.setCoords(e.pos().x(), m_mouseClickPosition.y(), m_mouseClickPosition.x(), e.pos().y());
		}

		if (m_eventHandlerPtr){
			if (m_selectionMode == SM_BOX){
				m_eventHandlerPtr->OnBoxSelection(m_selectionRect, clearPreviousSelection);
			}
			else if (m_selectionMode == SM_CIRCLE){
				m_eventHandlerPtr->OnCircleSelection(m_selectionRect, clearPreviousSelection);
			}
		}
		break;
	default:
		break;
	}
}


imt3dview::SceneState CRhiWidget::BuildSceneState() const
{
	imt3dview::SceneState state;

	if (m_cameraPtr){
		state.cameraPosition = m_cameraPtr->GetPosition();
		state.viewMatrix = m_cameraPtr->GetViewMatrix();
	}

	state.projectionMatrix = GetProjectionMatrix();
	state.viewport = rect();
	state.lightPosition = s_lightPosition;
	state.lightColor = s_lightColor;
	state.clearColor = m_backgroundColor;

	int hints = imt3dview::SceneState::RH_NONE;
	if (m_renderHints & RH_ANTIALIASING){
		hints |= imt3dview::SceneState::RH_ANTIALIASING;
	}
	if (m_renderHints & RH_CULLFACE){
		hints |= imt3dview::SceneState::RH_CULLFACE;
	}
	if (m_renderHints & RH_BLEND){
		hints |= imt3dview::SceneState::RH_BLEND;
	}
	state.renderHints = hints;

	return state;
}


QMatrix4x4 CRhiWidget::GetProjectionMatrix() const
{
	float w = static_cast<float>(width());
	float h = static_cast<float>(height());
	float aspectRatio = w / (h > 0.0f ? h : 1.0f);

	QMatrix4x4 projection;
	projection.setToIdentity();

	const double cameraZ =
		m_cameraPtr ? m_cameraPtr->GetPosition().z() : 5.0;

	switch (m_projectionMode){
	case PM_ORTHO:
		{
			float fw = 0.0f, fh = 0.0f;
			GetFovRect(aspectRatio, static_cast<float>(cameraZ), fw, fh);
			const double orthoFactor = fh / 2.0;
			projection.ortho(
				static_cast<float>(-orthoFactor * aspectRatio),
				static_cast<float>( orthoFactor * aspectRatio),
				static_cast<float>(-orthoFactor),
				static_cast<float>( orthoFactor),
				s_nearPlane, s_farPlane);
		}
		break;
	case PM_PERSPECTIVE:
	default:
		projection.perspective(s_verticalAngle, aspectRatio, s_nearPlane, s_farPlane);
		break;
	}

	return projection;
}


void CRhiWidget::GetFovRect(float aspectRatio, float nearPlane, float& outWidth, float& outHeight)
{
	const float fovY = s_verticalAngle * static_cast<float>(M_PI) / 180.0f;
	outHeight = 2.0f * nearPlane * std::tan(fovY / 2.0f);
	outWidth = outHeight * aspectRatio;
}


} // namespace imt3dgui
