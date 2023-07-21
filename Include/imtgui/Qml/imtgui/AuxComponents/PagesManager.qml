import QtQuick 2.12
import Acf 1.0
import imtqml 1.0

Item {
    id: container;

    property TreeItemModel pageModel: TreeItemModel {};
    property Item activeItem: null;
    property int activePageIndex: -1;
    property MainDocumentManager documentManager: null;

    Component.onCompleted: {
        Events.subscribeEvent("OnLocalizationChanged", container.onLocalizationChanged);
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("OnLocalizationChanged", container.onLocalizationChanged);
    }

    function updateModel(){
        pagesProvider.updateModel();
    }

    function clearModels(){
        console.log("clearModels");
        pageModel.Clear();
        pagesData.model = 0;
    }

    function onLocalizationChanged(language){
        console.log("pagesProvider onLocalizationChanged", language);

        pagesProvider.updateModel();
    }

    property alias modelState: pagesProvider.modelState;

    PagesProvider {
        id: pagesProvider;

        onPagesModelChanged: {
            console.log("pagesProvider onPagesModelChanged", pagesProvider.pagesModel.toJSON());
            let updateRepeaterModel = true;

            if (pagesData.model){
                let countPages = pagesData.model.GetItemsCount();
                if (pagesProvider.pagesModel.GetItemsCount() === countPages){
                    updateRepeaterModel = false;
                }
            }

            console.log("updateRepeaterModel", updateRepeaterModel);

            if (updateRepeaterModel){
                pagesData.model = pagesProvider.pagesModel;
            }

            container.pageModel = pagesProvider.pagesModel;
        }
    }

    Repeater {
        id: pagesData;

        anchors.fill: parent;

        clip: true;

        delegate: Item {
            id: pagesDeleg;

            anchors.fill: container;
            anchors.margins: Style.size_mainMargin !== undefined ? Style.size_mainMargin : 0;

            visible: container.activePageIndex === model.index;

            Component.onCompleted: {
                console.log("pagesDeleg onCompleted", model.Source);

                if (container.documentManager != null){
                    container.documentManager.registerDocumentManager(model.Id, null);
                }
            }

            /**
                The page will be loaded only by click if it hasn't loaded yet
            */
            onVisibleChanged: {
                if(pagesDeleg.visible){
                    if (!pagesLoader.item){
                        if (container.pageModel && container.pageModel.ContainsKey("Source", model.index)){
                            var source = container.pageModel.GetData("Source", model.index);
                            if (source){
                                pagesLoader.source = source;
                            }
                        }
                    }

                    container.activeItem = pagesLoader.item;
                }
            }

            Loader {
                id: pagesLoader;
                anchors.fill: parent;

                onItemChanged: {
                    if (pagesLoader.item){
                        if (pagesLoader.item.mainDocumentManager !== undefined){
                            pagesLoader.item.mainDocumentManager = container.documentManager;
                        }

//                        if (pagesLoader.item.startPageObj){
                            pagesLoader.item.startPageObj = {"Id": model.Id,
                                                             "Name": model.Name,
                                                             "Source": model.StartItem,
                                                             "CommandsId": model.Id};
//                        }
                    }
                }
            }
        }
    }
}

