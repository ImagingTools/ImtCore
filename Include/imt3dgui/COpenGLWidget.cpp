#include <imt3dgui/COpenGLWidget.h>


// Qt includes
#include <QtGui/QMouseEvent>
#include <QtCore/QElapsedTimer>

// ImtCore includes
#include <imt3dgui/ISceneEventHandler.h>


namespace imt3dgui
{


// static members

const float COpenGLWidget::s_verticalAngle = 45.0;
const float COpenGLWidget::s_nearPlane = 0.1;
const float COpenGLWidget::s_farPlane = 100.0;
const float COpenGLWidget::s_defaultLineWidth = 1.0f;
const QVector3D COpenGLWidget::s_defaultCameraPosition = { 0.0, 0.0, 5.0 };
const QVector3D COpenGLWidget::s_lightPosition(1.2, 1.0, 2.0);
const QVector3D COpenGLWidget::s_lightColor(1.0, 1.0, 1.0);


// public methods

COpenGLWidget::COpenGLWidget(QWidget *parent)
	:QOpenGLWidget(parent),
	m_renderHints(0),
	m_eventHandler(nullptr),
	m_viewMode(VM_SHOW),
	m_selectionMode(SM_BOX)
{
	setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

	m_camera.SetPlanes(s_nearPlane, s_farPlane);
	m_camera.MoveTo(s_defaultCameraPosition);

	connect(&m_cameraRotationAnimation, SIGNAL(valueChanged(const QVariant&)), this, SLOT(OnCameraRotationAnimation(const QVariant&)));
	connect(&m_cameraPositionAnimation, SIGNAL(valueChanged(const QVariant&)), this, SLOT(OnCameraPositionAnimation(const QVariant&)));

	m_timer.setInterval(20);
	connect(&m_timer, SIGNAL(timeout()), this, SLOT(OnInternalTimer()));
	m_timer.start();

	m_programPtr = new QOpenGLShaderProgram(this);
}


COpenGLWidget::~COpenGLWidget()
{
	makeCurrent();
	doneCurrent();
}


void COpenGLWidget::SetSceneEventHandler(ISceneEventHandler* handlerPtr)
{
	m_eventHandler = handlerPtr;
}


void COpenGLWidget::UnsetSceneEventHandler()
{
	m_eventHandler = nullptr;
}


imt3dview::IScene3dCamera *COpenGLWidget::GetCamera()
{
	return &m_camera;
}


imt3dview::IScene3d* COpenGLWidget::GetScene()
{
	return &m_scene;
}


void COpenGLWidget::ZoomIn()
{
	if (m_viewMode == VM_SHOW){
		m_camera.ZoomIn();
	}
}


void COpenGLWidget::ZoomOut()
{
	if (m_viewMode == VM_SHOW){
		m_camera.ZoomOut();
	}
}


void COpenGLWidget::ShowGrid(bool show)
{
	if (m_eventHandler){
		m_eventHandler->OnShowGrid(show);
	}
}


void COpenGLWidget::ShowAxis(bool show)
{
	if (m_eventHandler){
		m_eventHandler->OnShowAxis(show);
	}
}


bool COpenGLWidget::GetRenderHint(RenderHint renderHint) const
{
	return m_renderHints & renderHint;
}


void COpenGLWidget::SetRenderHint(RenderHint renderHint, bool on)
{
	if (on){
		m_renderHints |= renderHint;
	}
	else{
		m_renderHints &= ~renderHint;
	}
}


void COpenGLWidget::SetCameraView(COpenGLWidget::ViewDirection viewDirection, bool animated)
{
	m_cameraRotationAnimation.stop();
	m_cameraPositionAnimation.stop();

	QQuaternion newRotation;
	switch (viewDirection){
	case VD_DEFAULT:
		newRotation = QQuaternion::fromAxisAndAngle(0., 1., 0., -45.) * QQuaternion::fromAxisAndAngle(1., 0., -1., 45.);
		break;
	case VD_RIGHT:
		newRotation = QQuaternion::fromAxisAndAngle(0., 1., 0., -90.);
		break;
	case VD_FRONT:
		newRotation = QQuaternion::fromAxisAndAngle(0., 1., 0., 0.);
		break;
	case VD_TOP:
		newRotation = QQuaternion::fromAxisAndAngle(1., 0., 0., 90.);
		break;
	case VD_LEFT:
		newRotation = QQuaternion::fromAxisAndAngle(0., 1., 0., 90.);
		break;
	case VD_BOTTOM:
		newRotation = QQuaternion::fromAxisAndAngle(1., 0., 0., -90.);
		break;
	case VD_BACK:
		newRotation = QQuaternion::fromAxisAndAngle(0., 1., 0., 180.);
		break;
	default:
		Q_ASSERT(false);
		return;
	}

	QQuaternion oldRotation = m_camera.GetRotation();
	if (animated){
		m_cameraRotationAnimation.setStartValue(oldRotation);
		m_cameraRotationAnimation.setEndValue(newRotation);
		m_cameraRotationAnimation.setDuration(500);
		m_cameraRotationAnimation.start();

		m_cameraPositionAnimation.setStartValue(m_camera.GetPosition());
		m_cameraPositionAnimation.setEndValue(s_defaultCameraPosition);
		m_cameraPositionAnimation.setDuration(500);
		m_cameraPositionAnimation.start();
	}
	else{
		m_camera.RotateTo(newRotation);
		m_camera.MoveTo(s_defaultCameraPosition);
	}

	if (m_eventHandler){
		m_eventHandler->OnCameraPoseChanged(m_camera.GetRotation(), m_camera.GetPosition());
	}
}


void COpenGLWidget::SetBoxSelection(bool on)
{
	SetSelection(SM_BOX, on);
}


void COpenGLWidget::SetCircleSelection(bool on)
{
	SetSelection(SM_CIRCLE, on);
}


void COpenGLWidget::ClearSelection()
{
	if (m_eventHandler){
		m_eventHandler->OnClearSelection();
	}
}


void COpenGLWidget::InvertSelection()
{
	if (m_eventHandler){
		m_eventHandler->OnInvertSelection();
	}
}


void COpenGLWidget::DeleteSelection()
{
	if (m_eventHandler){
		m_eventHandler->OnDeleteSelection();
	}
}


// protected methods

// reimplemented (QOpenGLWidget)

void COpenGLWidget::initializeGL()
{
	initializeOpenGLFunctions();

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_ALWAYS);
	glEnable(GL_PROGRAM_POINT_SIZE);

	if (!m_programPtr->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/Shaders/vshader.glsl")){
		qDebug() << "Vertex shader could not be loaded";
		return;
	}

	if (!m_programPtr->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/Shaders/fshader.glsl")){
		qDebug() << "Fragment shader could not be loaded";
		return;
	}

	if (!m_programPtr->link()){
		qDebug() << "Failed to link shader program: " << m_programPtr->log();
		return;
	}

	m_scene.SetContext(QOpenGLWidget::context());
	m_scene.SetCamera(&m_camera);
	m_scene.SetProjection(GetProjectionMatrix());
}


void COpenGLWidget::resizeGL(int w, int h)
{
	m_camera.SetViewPortSize(QSize(w, h));

	m_scene.SetProjection(GetProjectionMatrix());
}


void COpenGLWidget::paintGL()
{
	QPainter painter(this);

	painter.beginNativePainting();
	PaintGl();
	painter.endNativePainting();

	Paint(painter);
}


// reimplemented (QWidget)

void COpenGLWidget::mousePressEvent(QMouseEvent* e)
{
	m_cameraRotationAnimation.stop();
	m_cameraPositionAnimation.stop();

	m_mouseClickPosition = m_prevMousePosition = e->pos();
}


void COpenGLWidget::mouseReleaseEvent(QMouseEvent* /*e*/)
{
	m_selectionRect.setRect(0.0, 0.0, 0.0, 0.0);
}


void COpenGLWidget::mouseMoveEvent(QMouseEvent* e)
{
	if (m_viewMode == VM_SHOW){
		MouseMoveEventShow(*e);
	}
	else if (m_viewMode == VM_SELECTION){
		MouseMoveEventSelection(*e);
	}

	m_prevMousePosition = e->pos();
}


void COpenGLWidget::closeEvent(QCloseEvent* eventPtr)
{
	m_scene.SetContext(nullptr);

	QOpenGLWidget::closeEvent(eventPtr);
}


void COpenGLWidget::wheelEvent(QWheelEvent* eventPtr)
{
	if (eventPtr->delta() > 0){
		OnZoomIn();
	}
	else{
		OnZoomOut();
	}
}


// private slots

void COpenGLWidget::OnZoomIn()
{
	ZoomIn();
}


void COpenGLWidget::OnZoomOut()
{
	ZoomOut();
}


void COpenGLWidget::OnInternalTimer()
{
	QElapsedTimer timer;
	timer.start();

	update();
}


void COpenGLWidget::OnCameraRotationAnimation(const QVariant& value)
{
	m_camera.RotateTo(value.value<QQuaternion>());
	float x = 0.;
	float y = 0.;
	float z = 0.;
	value.value<QQuaternion>().getEulerAngles(&x, &y, &z);
}


void COpenGLWidget::OnCameraPositionAnimation(const QVariant& value)
{
	m_camera.MoveTo(value.value<QVector3D>());
}


void COpenGLWidget::PaintGl()
{
	if (!m_programPtr->isLinked() || !QOpenGLWidget::context()){
		return;
	}

	m_programPtr->bind();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	SetGlFlags();
	SetGlUniformValues();

	m_scene.DrawGl(*m_programPtr);

	glLineWidth(s_defaultLineWidth);

	m_programPtr->release();
}


void COpenGLWidget::Paint(QPainter& painter)
{
	m_scene.Draw(painter);

	if (m_viewMode == VM_SELECTION){
		PaintSelection(painter);
	}
}


void COpenGLWidget::PaintSelection(QPainter& painter)
{
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
	}
}


void COpenGLWidget::MouseMoveEventShow(QMouseEvent& e)
{
	if (e.buttons() == Qt::LeftButton){
		m_camera.RotateTo(m_prevMousePosition, e.pos());
	}
	else if (e.buttons() == Qt::RightButton){
		m_camera.MoveTo(m_prevMousePosition, e.pos());
	}

	if (m_eventHandler){
		m_eventHandler->OnCameraPoseChanged(m_camera.GetRotation(), m_camera.GetPosition());
	}
}


void COpenGLWidget::MouseMoveEventSelection(QMouseEvent& e)
{
	bool clearPreviousSelection = !e.modifiers().testFlag(Qt::ControlModifier);

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
		else if (e.pos().x() < m_mouseClickPosition.x() && e.pos().y() > m_mouseClickPosition.y()){
			m_selectionRect.setCoords(e.pos().x(), m_mouseClickPosition.y(), m_mouseClickPosition.x(), e.pos().y());
		}

		if (m_eventHandler){
			if (m_selectionMode == SM_BOX){
				m_eventHandler->OnBoxSelection(m_selectionRect, clearPreviousSelection);
			}
			else if (m_selectionMode == SM_CIRCLE){
				m_eventHandler->OnCircleSelection(m_selectionRect, clearPreviousSelection);
			}
		}

		break;
	}
}


void COpenGLWidget::SetSelection(SelectionMode selectionMode, bool on)
{
	if (on){
		m_viewMode = VM_SELECTION;
		m_selectionMode = selectionMode;

		setCursor(Qt::CrossCursor);
	}
	else{
		m_viewMode = VM_SHOW;
		m_selectionRect.setRect(0.0, 0.0, 0.0, 0.0);

		unsetCursor();
	}
}


void COpenGLWidget::SetGlFlags()
{
	glEnable(GL_DEPTH_TEST);

	if (m_renderHints & RH_ANTIALIASING){
		glEnable(GL_LINE_SMOOTH);
		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
		glEnable(GL_POINT_SMOOTH);
		glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	else{
		glDisable(GL_LINE_SMOOTH);
		glHint(GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
		glDisable(GL_POINT_SMOOTH);
		glHint(GL_POINT_SMOOTH_HINT, GL_DONT_CARE);
		glDisable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ZERO);
	}

	if (m_renderHints & RH_CULLFACE){
		glCullFace(GL_BACK);
		glFrontFace(GL_CCW);
	}
	else{
		glDisable(GL_CULL_FACE);
	}
}


void COpenGLWidget::SetGlUniformValues()
{
	m_programPtr->setUniformValue("viewPosition", m_camera.GetPosition());
	m_programPtr->setUniformValue("viewMatrix", m_camera.GetViewMatrix());
	m_programPtr->setUniformValue("projectionMatrix", GetProjectionMatrix());
	m_programPtr->setUniformValue("lightPosition", s_lightPosition);
	m_programPtr->setUniformValue("lightColor", s_lightColor);
}


QMatrix4x4 COpenGLWidget::GetProjectionMatrix() const
{
	float width = static_cast<float>(QOpenGLWidget::width());
	float height = static_cast<float>(QOpenGLWidget::height());
	float aspectRatio = width / (height > 0.0f ? height : 1.0f);

	QMatrix4x4 projectionMatrix;
	projectionMatrix.perspective(s_verticalAngle, aspectRatio, s_nearPlane, s_farPlane);

	return projectionMatrix;
}


} // namespace imt3dgui


