import QtQuick
import QtQuick.Window

Window {
	id: splashWindow

	width: 480
	height: 270
	color: "transparent"
	flags: Qt.SplashScreen | Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint
	minimumWidth: width
	maximumWidth: width
	minimumHeight: height
	maximumHeight: height
}
