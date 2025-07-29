import QtQuick 2.12
import Acf 1.0
import imtbaseImtBaseTypesSdl 1.0

OptionsList {
    id: root
    
    property var collectionModel: null
    
    Component {
        id: optionFactory
        Option {}
    }
    
    onCollectionModelChanged: {
        if (!hasOptions()){
           createOptions()
        }
        
        m_options.clear()
        
        if (collectionModel){
            for (let i = 0; i < collectionModel.getItemsCount(); i++){
                let optionObj = optionFactory.createObject(root)
                if (collectionModel.containsKey("id", i)){
                    let id = collectionModel.getData("id", i)
                    optionObj.m_id = id
                }
                
                if (collectionModel.containsKey("name", i)){
                    let name = collectionModel.getData("name", i)
                    optionObj.m_name = name
                }
                
                if (collectionModel.containsKey("description", i)){
                    let description = collectionModel.getData("description", i)
                    optionObj.m_description = description
                }
                
                optionObj.m_enabled = true
                
                m_options.addElement(optionObj)
            }
        }
    }
}
