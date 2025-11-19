import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtcolgui 1.0

ControlBase {
    id: filterContainer;
    objectName: "FilterMenu"
    
    decorator: Style.filterPanelDecorator
    
    heightFromDecorator: true
    
    property bool canResetFilters: true
    property CollectionFilter complexFilter: CollectionFilter {}
    property DocCollectionFilter documentFilter: DocCollectionFilter {}
    property var enabledFilters: ({})
    
    signal close()
    signal clear()
    signal filterChanged()

    property var filterDependencies: ({})
    property ListModel filtersModel: ListModel {}
    
    Component.onCompleted: {
        setFilterIsEnabled("ComplexFilter", true)
        setFilterIsEnabled("DocumentFilter", documentFilter !== null)
    }

    onDocumentFilterChanged: {
        setFilterIsEnabled("DocumentFilter", documentFilter !== null)
    }

    function setFilterIsEnabled(filterId, enabled){
        enabledFilters[filterId] = enabled
    }

    function registerFieldFilterDelegate(filterId, filterDelegateComp){
        filtersModel.append({id: filterId, comp:filterDelegateComp})
    }

    function removeFieldFilterDelegate(filterId){
        for (let i = 0; i < filtersModel.count; i++){
            if (filtersModel.get(i).id === filterId){
                filtersModel.remove(i)
                return;
            }
        }
    }

    function getFilterDelegate(filterId){
        for (let i = 0; i < filtersModel.count; i++){
            if (filtersModel.get(i).id === filterId){
                return filtersModel.get(i).item
            }
        }

        return null
    }

    function setFilterDependency(filterId, dependsOnFilterId){
        if (filterId in filterDependencies){
            filterDependencies[filterId].push(dependsOnFilterId)
        }
        else{
            filterDependencies[filterId] = [dependsOnFilterId]
        }
    }

    function hasDependsOn(filterId, dependsOnFilterId){
        if (filterId in filterDependencies){
            return filterDependencies[filterId].includes(dependsOnFilterId)
        }

        return false
    }
}

