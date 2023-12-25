import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtauthgui 1.0

Item {
    id: container;

    property TreeItemModel pageModel: TreeItemModel {};
    property Item activeItem: null;
    property int activePageIndex: 0;
    property var documentManager: null;
    property AuthorizationPage authorizationStatusProvider: null;

    /**
        The page will be loaded only by click if it hasn't loaded yet
    */
    property bool loadByClick: true;

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
        pagesData.model = 0;
        pageModel.Clear();
    }

    function onLocalizationChanged(language){
        console.log("pagesProvider onLocalizationChanged", language);
        if (container.authorizationStatusProvider != null){
            let loggedUserId = container.authorizationStatusProvider.getLoggedUserId();
            if (loggedUserId !== ""){
                pagesProvider.updateModel();
            }
        }
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

            onVisibleChanged: {
                if(pagesDeleg.visible && container.loadByClick){
                    if (!pagesLoader.item){
                        pagesLoader.source = model.Source;
                    }

                    container.activeItem = pagesLoader.item;
                }
            }

            Component.onCompleted: {
                if (!container.loadByClick){
                    pagesLoader.source = model.Source;
                }
            }

            Loader {
                id: pagesLoader;
                anchors.fill: parent;

                visible: parent.visible;

                onItemChanged: {
                    if (pagesLoader.item){
                        pagesLoader.item.startPageObj =
                        {
                            "Id": model.Id,
                            "Name": model.Name,
                            "Source": model.StartItem,
                            "CommandId": model.Id
                        };
                    }
                }

                onStatusChanged: {
                    console.log("pagesLoader onStatusChanged", pagesLoader.source);

                    if (status == Loader.Error){
                        console.error("Loading page with source", pagesLoader.source, "was failed!");
                    }
                }
            }
        }
    }
}

