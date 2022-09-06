import QtQuick 2.0
import imtgui 1.0
import Acf 1.0

Rectangle {
    id: licensesTableDelegate;

    color: Style.baseColor;

    property bool selected: false;
    property string name;

    signal clicked;
    signal doubleClicked;

    Rectangle{
        id: selectionBackGround;

        anchors.fill: parent;
        color: "#4682B4";

        opacity: 0.2;
        radius: 2;
        visible: licensesTableDelegate.selected;
    }

    MouseArea {
        id: ma;

        anchors.fill: parent;

        onClicked: {
            licensesTableDelegate.clicked();
        }

        onDoubleClicked: {
            licensesTableDelegate.doubleClicked();
        }
    }

    Item {
        id: leftPart;

        anchors.left: parent.left;
        anchors.verticalCenter: parent.verticalCenter;

        width: licensesTableDelegate.width / 2;
        height: parent.height;

        clip: true;

        CheckBox {
            id: checkBoxLicense;

            anchors.verticalCenter: parent.verticalCenter;
            anchors.left: leftPart.left;
            anchors.leftMargin: 10;

            checkState: model.LicenseState;

            MouseArea {
                anchors.fill: parent;

                onClicked: {
                    console.log("TableInstanceLicensesDelegate CheckBox onClicked");
                    model.LicenseState = 2 - checkBoxLicense.checkState;
                }
            }
        }

        Text {
            id: titleLicensesTable;

            anchors.verticalCenter: parent.verticalCenter;
            anchors.left: checkBoxLicense.right;
            anchors.leftMargin: 10;

            text: model.Name;

            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;
            color: Style.textColor;
            wrapMode: Text.WordWrap;
            elide: Text.ElideRight;
        }
    }

    Item {
        id: rightPart;

        anchors.right: parent.right;
        anchors.verticalCenter: parent.verticalCenter;

        width: licensesTableDelegate.width / 2;
        height: parent.height;

        visible: checkBoxLicense.checkState === 2;

        CheckBox {
            id: checkBoxExpiration;

            anchors.verticalCenter: parent.verticalCenter;
            anchors.left: rightPart.left;

            checkState: model.ExpirationState;

            MouseArea {
                anchors.fill: parent;

                onClicked: {
                    console.log("TableInstanceLicensesDelegate CheckBox onClicked");
                    model.ExpirationState = 2 - checkBoxExpiration.checkState;
                }
            }
        }

        Text {
            id: textUnlimited;

            anchors.verticalCenter: parent.verticalCenter;
            anchors.left: checkBoxExpiration.right;
            anchors.leftMargin: 5;

            text: qsTr("Unlimited");
            visible: checkBoxExpiration.checkState === 0;

            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;
            color: Style.textColor;
        }

        RegExpValidator {
            id: dateRegex;
            regExp: /^\d{4}\-(0?[1-9]|1[012])\-(0?[1-9]|[12][0-9]|3[01])$/;
        }

        DatePicker {
            id: datePicker;

            anchors.verticalCenter: parent.verticalCenter;
            anchors.left: checkBoxExpiration.right;
            anchors.leftMargin: 5;

            visible: checkBoxExpiration.checkState === 2;

            width: 100;
            height: 20;

            currentDayButtonVisible: false;

            Component.onCompleted: {
                console.log("onCompleted");
                let date = model.Expiration;
                let data = date.split("-");
                datePicker.setDate(Number(data[0]), Number(data[1]) - 1, Number(data[2]));
            }

            onDateChanged: {
                console.log("onDateChanged", getDate());
                model.Expiration = getDate();
            }
        }
    }
}
