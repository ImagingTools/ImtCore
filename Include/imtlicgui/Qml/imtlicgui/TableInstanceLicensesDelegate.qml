import QtQuick 2.0
import imtauthgui 1.0
import imtgui 1.0
import imtqml 1.0
import Acf 1.0

Rectangle {
    id: licensesTableDelegate;

    property bool selected: false;
    property string name;

    signal clicked;
    signal doubleClicked;
    signal checkBoxLicenseClicked(int modelIndex, int state);
    signal checkBoxExpirationClicked(int modelIndex, int state);

    signal expirationTextChanged(int modelIndex, string value);

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

    Rectangle {
        id: leftPart;

        anchors.left: parent.left;
        anchors.verticalCenter: parent.verticalCenter;
        color: "transparent";
        width: licensesTableDelegate.width / 2;
        height: parent.height;

        CheckBox {
            id: checkBoxLicense;

            anchors.verticalCenter: parent.verticalCenter;
            anchors.left: leftPart.left;
            anchors.leftMargin: 10;

            width: 10;
            height: width;

            MouseArea {
                anchors.fill: parent;

                onClicked: {
                    console.log("TableInstanceLicensesDelegate CheckBox onClicked");
                    checkBoxLicense.checkState === 2 ? checkBoxLicense.checkState = 0 : checkBoxLicense.checkState = 2;

                    licensesTableDelegate.checkBoxLicenseClicked(model.index, checkBoxLicense.checkState);
                }
            }

        }

        Text {
            id: titleLicensesTable;

            anchors.verticalCenter: parent.verticalCenter;
            anchors.left: checkBoxLicense.right;
            anchors.leftMargin: 10;

            text: licensesTableDelegate.name;

            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;
            color: Style.textColor;
            wrapMode: Text.WordWrap;
        }
    }

    Rectangle {
        id: rightPart;

        anchors.right: parent.right;
        anchors.verticalCenter: parent.verticalCenter;

        width: licensesTableDelegate.width / 2;
        height: parent.height;

        visible: checkBoxLicense.checkState === 2;

        color: "transparent";

        CheckBox {
            id: checkBoxExpiration;

            anchors.verticalCenter: parent.verticalCenter;
            anchors.left: rightPart.left;
    //            anchors.leftMargin: 10;

            width: 10;
            height: width;

            MouseArea {
                anchors.fill: parent;

                onClicked: {
                    console.log("TableInstanceLicensesDelegate CheckBox onClicked");
                    checkBoxExpiration.checkState === 2 ? checkBoxExpiration.checkState = 0 : checkBoxExpiration.checkState = 2;
                    licensesTableDelegate.checkBoxExpirationClicked(model.index, checkBoxLicense.checkState);
                }
            }
        }

        Text {
            id: textUnlimited;

            anchors.verticalCenter: parent.verticalCenter;
            anchors.left: checkBoxExpiration.right;
            anchors.leftMargin: 5;

            text: "Unlimited";
            visible: checkBoxExpiration.checkState === 0;

            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;
            color: Style.textColor;
            wrapMode: Text.WordWrap;
        }

        TextFieldCustom {
            id: tfcDate;

            anchors.verticalCenter: parent.verticalCenter;
            anchors.left: checkBoxExpiration.right;
            anchors.leftMargin: 5;

            text: "01.01.2023";
            textSize: 12;

            height: 20;
            width: 100;

            visible: checkBoxExpiration.checkState === 2;

            onInputTextChanged: {
                licensesTableDelegate.expirationTextChanged(model.index, tfcDate.text);
            }
        }
    }
}
