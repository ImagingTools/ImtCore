import QtQuick 2.0
import imtauthgui 1.0
import Acf 1.0

Rectangle {
    id: containerInstallation;
    anchors.fill: parent;
    width: 500;
    height: 500;
    color: Style.backgroundColor;
    Rectangle {
        id: container;
        width: 500;
        height: parent.height;
        color: parent.color;
        Text {
            id: titleInstance;
            text: qsTr("Instance-ID");
            color: Style.textColor;
            font.family: Style.fontFamily;
            anchors.top: container.top;
            anchors.topMargin: 10;
            anchors.left: container.left;
            anchors.leftMargin: 10;
            font.pixelSize: Style.fontSize_common;
        }

        Rectangle {
            id: tfcInstance;
            width: container.width - 20;
            anchors.horizontalCenter: container.horizontalCenter;
            height: 45;
            anchors.top: titleInstance.bottom;

            color: Style.imagingToolsGradient1;
            border.color: Style.theme == "Light" ? "#d0d0d2" : "#3a3b3b" ;

            TextFieldCustom {
                width: tfcInstance.width - 22;
                height: 23;
                anchors.horizontalCenter: tfcInstance.horizontalCenter;
                anchors.verticalCenter: tfcInstance.verticalCenter;
            }
        }

        Text {
            id: titleCustomer;
            text: qsTr("Customer");
            color: Style.textColor;
            font.family: Style.fontFamily;
            anchors.top: tfcInstance.bottom;
            anchors.topMargin: 7;
            anchors.left: container.left;
            anchors.leftMargin: 10;
            font.pixelSize: Style.fontSize_common;
        }

        Rectangle {
            id: tfcCustomer;
            width: container.width - 20;
            anchors.horizontalCenter: container.horizontalCenter;
            height: 45;
            anchors.top: titleCustomer.bottom;

            color: Style.imagingToolsGradient1;
            border.color: Style.theme == "Light" ? "#d0d0d2" : "#3a3b3b" ;

            TextFieldCustom {
                width: tfcCustomer.width - 22;
                height: 23;
                anchors.horizontalCenter: tfcCustomer.horizontalCenter;
                anchors.verticalCenter: tfcCustomer.verticalCenter;
            }
        }

        Text {
            id: titleProduct;
            text: qsTr("Product");
            color: Style.textColor;
            font.family: Style.fontFamily;
            anchors.top: tfcCustomer.bottom;
            anchors.topMargin: 7;
            anchors.left: container.left;
            anchors.leftMargin: 10;
            font.pixelSize: Style.fontSize_common;
        }

        Rectangle {
            id: tfcProduct;
            width: container.width - 20;
            anchors.horizontalCenter: container.horizontalCenter;
            height: 45;
            anchors.top: titleProduct.bottom;

            color: Style.imagingToolsGradient1;
            border.color: Style.theme == "Light" ? "#d0d0d2" : "#3a3b3b" ;

            TextFieldCustom {
                width: tfcCustomer.width - 22;
                height: 23;
                anchors.horizontalCenter: tfcProduct.horizontalCenter;
                anchors.verticalCenter: tfcProduct.verticalCenter;
            }
        }
    }


}
