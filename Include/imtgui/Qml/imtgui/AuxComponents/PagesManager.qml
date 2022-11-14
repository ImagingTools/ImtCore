import QtQuick 2.12
import Acf 1.0
import imtqml 1.0

Item {
    id: container;

    property TreeItemModel pageModel: TreeItemModel {};
    property Item activeItem;
    property int activePageIndex: menuPanel.activePageIndex;

    onActiveItemChanged: {
        console.log("onActiveItemChanged", activeItem);
    }

    onPageModelChanged: {
        console.log("onPageModelChanged", pageModel);
        pagesData.model = container.pageModel;
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
            container.pageModel = pagesModel;
        }
    }

    Repeater {
        id: pagesData;

        anchors.fill: parent;

        clip: true;

        delegate: Item {
            id: pagesDeleg;

            anchors.fill: parent;
            anchors.margins: thumbnailDecoratorContainer.mainMargin;

            visible: activePageIndex === model.index;

            /**
                The page will be loaded only by click if it hasn't loaded yet
            */
            onVisibleChanged: {
                if(pagesDeleg.visible){
                    if (!pagesLoader.item){
                        var source = pageModel.GetData("Source", model.index);
                        pagesLoader.source = source;
                    }

                    activeItem = pagesLoader.item;
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

