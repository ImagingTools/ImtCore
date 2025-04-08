import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

ControlBase {
    id: container;

    decorator: Style.menuPanelButtonDecorator;

    property string text: "";
    property string iconSource: "";
    property string textColor: "#335777";//"#191970"
    property string fontName: "";

    property bool enabled:  true;
    property bool selected: false;
    property bool highlighted: ma.containsMouse && container.enabled;

    property real imageDecrease: 0.63;
    property real imageSelectedCoeff: 0.73;
    property real fontSize: 11;

    property int contentWidth: 80;
    property int contentHeight: 80;

    property string decoratorSource;

    property alias subPagesCount: subPagesRepeater.count;

    property Item menuPanelRef: null;
    property alias mouseArea: ma;

    signal clicked;

    Rectangle {
        id: mainRect;

        anchors.top: container.top;

        width: container.width;
        height: container.height;

        color: "transparent";

        MouseArea{
            id: ma;

            anchors.fill: parent;

            hoverEnabled: enabled;
            cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor;

            onClicked: {
                container.clicked();
            }
        }
    }

    Column {
        id: subPagesColumn;

        anchors.top: mainRect.bottom;

        visible: container.selected;

        width: parent.width;

        property int currentIndex: 0;

        Repeater {
            id: subPagesRepeater;

            delegate: SubMenuPanelButtonDecorator {
                id: subPageDelegate;

                width: subPagesColumn.width;
                height: container.height

                selected: subPagesColumn.currentIndex == model.index;
                title: model["name"]

                MouseArea{
                    anchors.fill: parent;

                    hoverEnabled: enabled;
                    cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor;

                    onClicked: {
                        subPagesColumn.currentIndex = model.index;
                    }
                }
            }
        }
    }
}
