// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
//
// Imt3dDemo — Minimal pure-QML desktop application showing a 3D cube.
//
// This demo uses CScene3DItem (the generic scene-based QQuickItem) together
// with a CCubeSceneItem that is added to the scene, demonstrating how any
// IScene3dItem can be rendered through the generic pipeline.  No QWidget code
// is involved — the entire application is driven by QQmlApplicationEngine +
// main.qml.
//

#include <QtCore/QDebug>
#include <QtGui/QGuiApplication>
#include <QtQml/QQmlApplicationEngine>
#include <QtQml/QQmlContext>

#include <imt3dgui/CScene3DItem.h>
#include <imt3dgui/CCubeSceneItem.h>


int main(int argc, char* argv[])
{
	QGuiApplication app(argc, argv);
	app.setApplicationName("Imt3dDemo");

	// Register the generic Scene3DView QML type
	imt3dgui::CScene3DItem::RegisterQmlType();

	QQmlApplicationEngine engine;
	engine.load(QUrl("qrc:/main.qml"));

	if (engine.rootObjects().isEmpty()){
		return -1;
	}

	// Get the Scene3DView item and add a cube to its scene.
	QObject* rootPtr = engine.rootObjects().first();
	auto* viewPtr = rootPtr->findChild<imt3dgui::CScene3DItem*>("view3d");
	if (!viewPtr){
		qWarning() << "Imt3dDemo: Scene3DView 'view3d' not found — cube will not be displayed";
		return app.exec();
	}

	imt3dgui::CCubeSceneItem cube;
	cube.SetColor(QVector3D(0.91f, 0.27f, 0.38f));
	viewPtr->GetScene()->AddShapeToScene(&cube);

	return app.exec();
}
