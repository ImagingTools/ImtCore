// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imt3dgui/CCompositeShapeViewComp.h>


namespace imt3dgui
{


// protected methods

// reimplemented (iqtui::CComponentBase)

void CCompositeShapeViewComp::OnGuiCreated()
{
	BaseClass::OnGuiCreated();

	COpenGLWidget* widgetPtr = GetQtWidget();
	Q_ASSERT(widgetPtr != NULL);

	widgetPtr->SetSceneEventHandler(this);

	imt3dview::IScene3d* scenePtr = GetScene();
	if (scenePtr != NULL){
		for (int i = 0; i < m_shapesCompPtr.GetCount(); ++i){
			IShape3d* shapePtr = m_shapesCompPtr[i];
			if (shapePtr != nullptr){
				scenePtr->AddShapeToScene(shapePtr);
			}
		}
	}
}


void CCompositeShapeViewComp::OnGuiDestroyed()
{
	imt3dview::IScene3d* scenePtr = GetScene();
	if (scenePtr != NULL){
		for (int i = 0; i < m_shapesCompPtr.GetCount(); ++i){
			IShape3d* shapePtr = m_shapesCompPtr[i];
			if (shapePtr != nullptr){
				scenePtr->RemoveShapeFromScene(shapePtr);
			}
		}
	}

	BaseClass::OnGuiDestroyed();
}


} // namespace imt3dgui


