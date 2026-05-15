const Popup = require("./Popup")
const Real = require("../../QtQml/Real")
const Int = require("../../QtQml/Int")
const Var = require("../../QtQml/Var")
const String = require("../../QtQml/String")
const Signal = require("../../QtQml/Signal")

class Menu extends Popup {
    static meta = Object.assign({}, Popup.meta, {
        cascade: { type: Var, value: true },
        contentData: { type: Var, value: [] },
        contentModel: { type: Var },
        count: { type: Int, value: 0 },
        currentIndex: { type: Int, value: -1 },
        delegate: { type: Var },
        displayMargin: { type: Real, value: 0 },
        focus: { type: Var, value: true },
        icon: { type: Var },
        implicitHeight: { type: Real, value: 0 },
        implicitWidth: { type: Real, value: 0 },
        overlap: { type: Int, value: 0 },
        title: { type: String, value: '' },
        
        cascadeChanged: { type: Signal, args: [] },
        contentDataChanged: { type: Signal, args: [] },
        contentModelChanged: { type: Signal, args: [] },
        countChanged: { type: Signal, args: [] },
        currentIndexChanged: { type: Signal, args: [] },
        delegateChanged: { type: Signal, args: [] },
        displayMarginChanged: { type: Signal, args: [] },
        focusChanged: { type: Signal, args: [] },
        iconChanged: { type: Signal, args: [] },
        implicitHeightChanged: { type: Signal, args: [] },
        implicitWidthChanged: { type: Signal, args: [] },
        overlapChanged: { type: Signal, args: [] },
        titleChanged: { type: Signal, args: [] },
        
        triggered: { type: Signal, args: ['action'] },
        aboutToHide: { type: Signal, args: [] },
        aboutToShow: { type: Signal, args: [] }
    })

    static create(parent = null, properties = {}){
        let obj = super.create(parent, properties)
        obj.__getDOM().classList.add('Menu')

        return obj
    }

}



module.exports = Menu
