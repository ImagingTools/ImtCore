import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

ControlBase {
    id: tabDelegate;

    property bool selected: false;
    property bool firstElement: false;
    property bool lastElement: false;
    property bool autoWidth: true;
    property bool isCloseEnable: true;

    property string text;
    property string firstElementImageSource:  "../../../" + "Icons/" + Style.theme + "/" + "Workflow" + "_On_Normal.svg";
    property int index: model.index;
    property int selectedIndex: -1;

    property int maxWidth: 250;
    property int minWidth: 100;

    property ListView listView: null;
    property alias mouseArea: ma;

    decorator: Style.tabPanelDecorator

    widthFromDecorator: true;

	signal clicked(var mouse);
    signal closeSignal;

    MouseArea {
        id: ma;
        anchors.fill: tabDelegate;
        cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor;
        hoverEnabled: true;
		acceptedButtons: Qt.LeftButton | Qt.RightButton;

        onClicked: {
			tabDelegate.clicked(mouse);
        }
    }
}


