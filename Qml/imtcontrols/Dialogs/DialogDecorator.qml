import QtQuick 2.12
import Qt5Compat.GraphicalEffects 6.0
import QtGraphicalEffects 1.12

import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0


DecoratorBase {
    id: dialogContainer;

    width: dialogColumn ? dialogColumn.width : 0;
    height: dialogColumn ? dialogColumn.height : 0;
    z:-1;

    property string title : baseElement ? baseElement.title : "";
    property string bodySource: baseElement ? baseElement.bodySource : "";
    property string topPanelSource: baseElement ? baseElement.topPanelSource : "";
    property bool baseElementFocus: baseElement ? baseElement.focus : false;
    property Component topPanelComp : !baseElement ? undefined : baseElement.topPanelComp;

    Component.onCompleted: {
        Events.subscribeEvent("OnLocalizationChanged", dialogContainer.onLocalizationChanged);
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("OnLocalizationChanged", dialogContainer.onLocalizationChanged);
    }

    function onLocalizationChanged(language){
        if(loaderTopPanel.item){
            loaderTopPanel.item.title = dialogContainer.title;
        }
    }

    onBaseElementChanged: {
        baseElement.buttons = buttonsDialog;
    }

    onBaseElementFocusChanged: {
        if (dialogContainer.focus && loaderBodyDialog.item){
            loaderBodyDialog.item.focus = true;
        }

    }

    onWidthChanged: {
        if (loaderTopPanel && loaderTopPanel.item){
            loaderTopPanel.item.width = dialogContainer.width;
        }

        if (loaderBodyDialog && loaderBodyDialog.item){
        }

		if(baseElement && baseElement.canResize && width > baseElement.width){
			baseElement.width = width;
		}
    }

	onHeightChanged: {
		if(baseElement && baseElement.canResize && height > baseElement.height){
			baseElement.height = height;
		}
	}

    onTitleChanged: {
        if(loaderTopPanel.item){
            loaderTopPanel.item.title = dialogContainer.title;
        }
    }

    MouseArea {
        anchors.fill: parent;
        onClicked: {}
    }

    Rectangle{
        id: background;

        anchors.fill: parent;

        color: dialogContainer.baseElement.backgroundColor;
        radius: 4;
    }

    DropShadow {
        id: dropShadow;

        anchors.fill: background;

        horizontalOffset: 2;
        verticalOffset: 2;

        radius: background.radius;
        color: Style.shadowColor;

        source: background;
    }

    Column {
        id: dialogColumn;

        width: Math.max(buttonsWidth, bodyWidth);

        property real bodyWidth: !loaderBodyDialog.item ? 1 : loaderBodyDialog.item.width;
        property real buttonsWidth: buttonsContainer.width + 2 * buttonsContainer.anchors.rightMargin;

        onWidthChanged: {
            dialogContainer.width = width
        }
        onHeightChanged: {
            if(dialogColumn.height > topPanelContainer.height){
                dialogContainer.height = height
            }
        }

        Item {
            id: topPanelContainer;

            width: loaderTopPanel.item ? loaderTopPanel.item.width : 0;
            height: loaderTopPanel.item ? loaderTopPanel.item.height : 0;

            MovingItem {
                visible: !dialogContainer.baseElement ? false : dialogContainer.baseElement.canMove;
                containerItem: topPanelContainer;
                movingItem: !dialogContainer.baseElement ? null : dialogContainer.baseElement;
            }

            Loader {
                id: loaderTopPanel;

                sourceComponent: dialogContainer.topPanelComp;

                onLoaded:  {
                    loaderTopPanel.item.width = dialogContainer.width;

                    if(loaderTopPanel.item.title !== undefined){
                        loaderTopPanel.item.title = dialogContainer.baseElement.title;
                    }

                    loaderTopPanel.width = loaderTopPanel.item.width;
                    loaderTopPanel.height = loaderTopPanel.item.height;

                    if (loaderTopPanel.item.closeButtonClicked){
                        loaderTopPanel.item.closeButtonClicked.connect(dialogContainer.baseElement.finished);
                    }
                    dialogContainer.baseElement.topPanel = loaderTopPanel.item;
                }
                onItemChanged: {
                    if(loaderTopPanel.item){
                        loaderTopPanel.item.width = dialogContainer.width;
                        if(loaderTopPanel.item.title !== undefined){
                            loaderTopPanel.item.title = dialogContainer.baseElement.title;
                        }
                    }
                }
            }

        }

        Loader {
            id: loaderBodyDialog;

            anchors.horizontalCenter: !parent ? undefined : parent.horizontalCenter;

            sourceComponent: dialogContainer.baseElement ? dialogContainer.baseElement.contentComp : undefined;
            onLoaded: {
                if(loaderBodyDialog.item.rootItem !== undefined){
                    loaderBodyDialog.item.rootItem = dialogContainer.baseElement;
                }
                dialogContainer.baseElement.contentItem = loaderBodyDialog.item;
                dialogContainer.baseElement.bodyItem = loaderBodyDialog.item;
            }

            onItemChanged: {
                if (loaderBodyDialog.item){
                }
            }
        }

        Item {
            id: buttonsContainer;

            anchors.right: !parent ? undefined : parent.right;
            anchors.rightMargin: Style.sizeMainMargin;

            width: buttonsDialog.width;
            height: buttonsDialog.height + 2* buttonsDialog.anchors.bottomMargin;

            DialogButtons {
                id: buttonsDialog;

                anchors.right: !parent ? undefined : parent.right;
                anchors.bottom: !parent ? undefined : parent.bottom;
                anchors.bottomMargin: Style.sizeMainMargin;

                buttons: !dialogContainer.baseElement ? 0 : dialogContainer.baseElement.buttonsModel;

                onButtonClicked: {
                    dialogContainer.baseElement.finished(buttonId);
                }
            }
        }
    }
}//dialog
