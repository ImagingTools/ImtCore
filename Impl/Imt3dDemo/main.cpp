// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
//
// Imt3dDemo — Minimal pure-QML desktop application showing a 3D cube.
//
// This demo uses CRender3DItem (a QQuickItem that renders via CRhiRenderBackend)
// to display a GPU-accelerated 3D cube.  No QWidget code is involved — the
// entire application is driven by QQmlApplicationEngine + main.qml.
//

#include <QtGui/QGuiApplication>
#include <QtQml/QQmlApplicationEngine>

#include <imt3dgui/CRender3DItem.h>


int main(int argc, char* argv[])
{
	QGuiApplication app(argc, argv);
	app.setApplicationName("Imt3dDemo");

	// Register the C++ QML type
	imt3dgui::CRender3DItem::RegisterQmlType();

	QQmlApplicationEngine engine;
	engine.load(QUrl("qrc:/main.qml"));

	if (engine.rootObjects().isEmpty()){
		return -1;
	}

	return app.exec();
}
