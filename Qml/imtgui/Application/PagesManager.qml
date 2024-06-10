import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0
import imtauthgui 1.0

Item {
    id: container;

    property TreeItemModel pageModel: TreeItemModel {};
    property Item activeItem: null;
    property int activePageIndex: -1;

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
        pagesData.model = 0;
        pageModel.clear();
    }

    function onLocalizationChanged(language){
        let loggedUserId = AuthorizationController.getLoggedUserId();
        if (loggedUserId !== ""){
            pagesProvider.updateModel();
        }
    }

    property alias modelState: pagesProvider.modelState;

    PagesProvider {
        id: pagesProvider;

        onPagesModelChanged: {
            container.pageModel = pagesProvider.pagesModel;

            let updateRepeaterModel = true;

            if (pagesData.model){
                let countPages = pagesData.model.getItemsCount();
                if (pagesProvider.pagesModel.getItemsCount() === countPages){
                    updateRepeaterModel = false;
                }
            }

            if (updateRepeaterModel){
                pagesData.model = pagesProvider.pagesModel;
            }
        }
    }

    Repeater {
        id: pagesData;

        anchors.fill: parent;

        clip: true;

        delegate: Item {
            id: pagesDeleg;

            anchors.fill: container;

            visible: container.activePageIndex === model.index;

            property bool isLoaded: false;

            onVisibleChanged: {
                if (visible){
                    load();
                }
            }

            function load(){
                if (isLoaded){
                    return;
                }

                if(pagesDeleg.visible){
                    if (!pagesLoader.item){
                        pagesLoader.source = model.Source;
                    }

                    if (pagesLoader.item){
                        Events.sendEvent("StartLoading")

                        if (pagesLoader.item.loadStartItem !== undefined){
                            pagesLoader.item.loadStartItem();
                        }

                        pagesDeleg.isLoaded = true;

                        Events.sendEvent("StopLoading")
                    }
                }
            }

            Component.onCompleted: {
                if (!container.loadByClick || container.activePageIndex === model.index){
                    pagesLoader.source = model.Source;

                    load();
                }
            }

            Loader {
                id: pagesLoader;
                anchors.fill: parent;

                visible: parent.visible;

                onLoaded: {
                    pagesLoader.item.startPageObj = {
                        "Id": model.Id,
                        "Name": model.Name,
                        "Source": model.StartItem,
                        "CommandId": model.Id
                    };
                }

                onStatusChanged: {
                    if (status == Loader.Error){
                        console.error("Loading page with source", pagesLoader.source, "was failed!");
                    }
                }
            }
        }
    }
}

