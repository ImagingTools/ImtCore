import QtQuick 2.12
import Acf 1.0
import imtcontrols 2.0

/*!
    \qmltype ComboBox
    \inqmlmodule imtcontrols

    Drop-down selector, API-compatible with Qt Quick Controls \c ComboBox.

    ComboBox shows the currently selected entry and, when clicked, opens a
    \l Popup listing the entries of its \l model. Selecting an entry updates
    \l currentIndex / \l currentText and emits \l activated(). Use \l textRole
    to pick the display field of object models.

    \qml
    ComboBox {
        model: [ qsTr("Low"), qsTr("Medium"), qsTr("High") ]
        onActivated: priority = currentText
    }
    \endqml

    \sa Popup
*/
Control {
    id: control
    objectName: "ImtControlsComboBox"

    /*! The model providing the entries (array, integer or ListModel). */
    property var model: []
    /*! Index of the selected entry. */
    property int currentIndex: 0
    /*! Field of object models used for display. */
    property string textRole: ""

    /*! Display text of the selected entry (read-only). */
    readonly property string currentText: _display(currentIndex, _rawAt(currentIndex))
    /*! Text shown in the closed combo box. */
    property string displayText: currentText

    /*! Whether the drop-down is open (read-only). */
    readonly property bool popupVisible: listPopup.opened

    /*! Emitted when the user selects \a index. */
    signal activated(int index)

    padding: Style.spacingS
    leftPadding: Style.spacingM
    rightPadding: Style.spacingS

    implicitWidth:  Style.buttonWidthL
    implicitHeight: Style.controlHeightL

    function _count() {
        if (typeof model === "number")
            return model;
        if (model && model.count !== undefined)
            return model.count;
        if (model && model.length !== undefined)
            return model.length;
        return 0;
    }

    function _rawAt(index) {
        if (index < 0 || index >= _count())
            return undefined;
        if (typeof model === "number")
            return index;
        if (model && model.get !== undefined)
            return model.get(index);
        return model[index];
    }

    function _display(index, raw) {
        if (raw === undefined || raw === null)
            return "";
        if (textRole !== "" && typeof raw === "object")
            return "" + raw[textRole];
        return "" + raw;
    }

    background: Rectangle {
        radius: Style.comboBoxRadius
        color: Style.backgroundColor
        border.width: Style.buttonBorderWidth
        border.color: control.popupVisible ? Style.imaginToolsAccentColor : Style.borderColor
    }

    contentItem: Item {
        implicitWidth:  Style.buttonWidthL
        implicitHeight: Style.controlHeightL

        Text {
            anchors.left: parent.left
            anchors.right: arrow.left
            anchors.verticalCenter: parent.verticalCenter
            text: control.displayText
            elide: Text.ElideRight
            verticalAlignment: Text.AlignVCenter
            color: control.enabled ? Style.textColor : Style.inactiveTextColor
            font.family: Style.fontFamily
            font.pixelSize: Style.fontSizeM
        }

        Text {
            id: arrow
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            text: "\u25BE"
            color: Style.textColor
            font.family: Style.fontFamily
            font.pixelSize: Style.fontSizeM
        }

        MouseArea {
            anchors.fill: parent
            enabled: control.enabled
            onClicked: {
                if (listPopup.opened) {
                    listPopup.close();
                } else {
                    listPopup.parent = control;
                    listPopup.x = 0;
                    listPopup.y = control.height;
                    listPopup.open();
                }
            }
        }
    }

    Popup {
        id: listPopup
        modal: false
        padding: 0
        width: control.width
        height: Math.min(list.contentHeight + topPadding + bottomPadding, Style.controlHeightL * 5)

        ListView {
            id: list
            anchors.fill: parent
            clip: true
            model: control.model

            delegate: Rectangle {
                width: list.width
                height: Style.controlHeightL
                color: hoverArea.containsMouse ? Style.highlightBackgroundColor
                       : index === control.currentIndex ? Style.selectedColor
                       : "transparent"

                Text {
                    anchors.fill: parent
                    anchors.leftMargin: Style.spacingM
                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideRight
                    text: control._display(index, modelData)
                    color: Style.textColor
                    font.family: Style.fontFamily
                    font.pixelSize: Style.fontSizeM
                }

                MouseArea {
                    id: hoverArea
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: {
                        control.currentIndex = index;
                        control.activated(index);
                        listPopup.close();
                    }
                }
            }
        }
    }
}
