import QtQuick 2.12
import Acf 1.0
import imtqml 1.0

Item {
    id: container;

    property TreeItemModel pageModel: TreeItemModel {};
    property Item activeItem: null;
    property int activePageIndex;

    onActiveItemChanged: {
        console.log("onActiveItemChanged", container.activeItem);
    }

    function updateModel(){
        pagesProvider.updateModel();
    }

    function clearModels(){
        pageModel.Clear();
        pagesData.model = 0;
    }

    PagesProvider {
        id: pagesProvider;

        onPagesModelChanged: {
            console.log("pagesProvider onPagesModelChanged");
            pagesData.model = pagesProvider.pagesModel;
            container.pageModel = pagesProvider.pagesModel;
        }
    }

    Repeater {
        id: pagesData;

        anchors.fill: parent;

        clip: true;

        delegate: Item {
            id: pagesDeleg;

            anchors.fill: parent;
            //anchors.margins: thumbnailDecoratorContainer.mainMargin;
            anchors.margins: Style.size_mainMargin !== undefined ? Style.size_mainMargin : 0;

            visible: container.activePageIndex === model.index;

            Component.onCompleted: {
                console.log("pagesDeleg onCompleted", model.Source);
            }

            /**
                The page will be loaded only by click if it hasn't loaded yet
            */
            onVisibleChanged: {
                if(pagesDeleg.visible){
                    if (!pagesLoader.item){
                        var source = container.pageModel.GetData("Source", model.index);
                        console.log('DEBUG::', pagesLoader)
                        pagesLoader.source = source;
                    }

                    container.activeItem = pagesLoader.item;
                }
            }

            Loader {
                id: pagesLoader;
                anchors.fill: parent;

                onItemChanged: {
                    console.log("ThumbnailDecorator Repeater Loader onItemChanged", pagesLoader.source)
                    console.log("model.PageId", model.Id)
                    if (pagesLoader.item){
                        pagesLoader.item.startPageObj = {"Id": model.Id,
                                                         "Name": model.Name,
                                                         "Source": model.StartItem,
                                                         "CommandsId": model.Id};
                    }
                }
            }
        }
    }
}

