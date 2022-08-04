import QtQuick 2.12
import Acf 1.0
import imtqml 1.0

Item {
    id: container;

    property TreeItemModel pageModel;
    property Item activeItem;
    property int activePageIndex: -1;

//    onModelChanged: {
//        console.log("PagesManager onModelChanged");
//    }

    Repeater {
        id: pagesData;

        anchors.fill: parent;

        clip: true;
        model: container.pageModel;

        delegate: Item {
            id: pagesDeleg;

            anchors.fill: parent;

            visible: activePageIndex === model.index;

            Component.onCompleted: {
                console.log("pagesData Item onCompleted");

                console.log("width", width);
                console.log("height", height);
            }

            /**
                The page will be loaded only by click if it hasn't loaded yet
             */

            onVisibleChanged: {
                console.log("pagesData onVisibleChanged");
                if(pagesDeleg.visible){
//                    activeItem = pagesLoader.item;
                    console.log("pagesDeleg.visible", pagesDeleg.visible);
                    if (!pagesLoader.item){
                        var source = pageModel.GetData("Source", model.index);
                        console.log("source", source);
                        pagesLoader.source = source;
                    }
                }
            }

            Loader {
                id: pagesLoader;
                anchors.fill: parent;

                onItemChanged: {
                    console.log("ThumbnailDecorator Repeater Loader onItemChanged", pagesLoader.source)
                    if (pagesLoader.item){
                        pagesLoader.item.startPageObj = {"Id": model.PageId,
                                                         "Name": model.Name,
                                                         "Source": model.StartItem,
                                                         "CommandsId": model.PageId};
                    }
                }
            }
        }
    }
}

