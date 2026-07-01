import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
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

	LocalizationEvent {
		id: localizationEvent;

		onLocalizationChanged: {
			let loggedUserId = AuthorizationController.getLoggedUserId();
			if (loggedUserId !== ""){

				container.updateModel();
			}
		}
	}

    function updateModel(){
        pagesProvider.updateModel();
    }

    function clearModels(){
        pagesData.model = 0;
        pageModel.clear();
    }

    property alias modelState: pagesProvider.modelState;

    PagesProvider {
        id: pagesProvider;

        onPagesModelChanged: {
            container.pageModel = pagesProvider.pagesModel;
            // Always assign the current model to the Repeater so that
            // the page delegates (and their Loaders) are created from
            // the exact same page list that MenuPanel is using.
            // This prevents desync where menu shows page A but Loader
            // shows stale content from page B (e.g. after tenant switch
            // even when page count happens to be the same).
            pagesData.model = pagesProvider.pagesModel;

            // Explicitly ensure the active page content is loaded from
            // the fresh model (in case visible/onCompleted timing is
            // affected by the model replacement).
            if (container.activePageIndex >= 0) {
                container.loadPageContent(container.activePageIndex);
            }
        }
    }

    function loadPageContent(index){
        let page = pagesData.itemAt(index)
        if (page){
            page.load();
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

            Component.onCompleted: {
                if (visible){
                    load();
                }
            }

            onVisibleChanged: {
                if (visible){
                    load();
                }
            }

            Loader {
                id: loader;
                anchors.fill: parent;
                onLoaded: {
                    if (item.pageId !== undefined){
                        item.pageId = model.id;
                    }

                    if (item.pageName !== undefined){
                        item.pageName = model.name;
                    }

                    if (item.startItemSource !== undefined){
                        item.startItemSource = model.startItem;
                    }
                }
                onStatusChanged: {
                    if (status == Loader.Error){
                        console.error("Unable to create page component. Please check the source path:", loader.source);
                    }
                }
            }

            function load(){
                if (loader && !loader.item){
                    loader.source = model.source;
                }
            }
        }
    }
}

