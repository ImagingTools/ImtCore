const GroupProperty = require("./GroupProperty")
const AnchorLine = require("./AnchorLine")
const Real = require("./Real")
const Var = require("./Var")
const Signal = require("./Signal")
const QtFunctions = require("../Qt/functions")

class Anchors extends GroupProperty {
    static meta = {
        fill: { type: Var, value: undefined, signalName: 'fillChanged' },
        centerIn: { type: Var, value: undefined, signalName: 'centerInChanged' },

        // left: { type: var, value: undefined, signalName: 'mainChanged' },
        // right: { type: var, value: undefined, signalName: 'mainChanged' },
        // top: { type: var, value: undefined, signalName: 'mainChanged' },
        // bottom: { type: var, value: undefined, signalName: 'mainChanged' },
        horizontalCenter: { type: Var, value: undefined, signalName: 'horizontalCenterChanged' },
        verticalCenter: { type: Var, value: undefined, signalName: 'verticalCenterChanged' },

        leftMargin: { type: Real, value: 0, signalName: 'leftMarginChanged' },
        topMargin: { type: Real, value: 0, signalName: 'topMarginChanged' },
        rightMargin: { type: Real, value: 0, signalName: 'rightMarginChanged' },
        bottomMargin: { type: Real, value: 0, signalName: 'bottomMarginChanged' },
        margins: { type: Real, value: 0, signalName: 'marginsChanged' },
        // horizontalCenterOffset: { type: real, value: 0, signalName: 'numberChanged' },
        // verticalCenterOffset: { type: real, value: 0, signalName: 'numberChanged' },

        fillChanged: {type:Signal, slotName:'onFillChanged', args:[]},
        centerInChanged: {type:Signal, slotName:'onCenterInChanged', args:[]},


        leftMarginChanged: {type:Signal, slotName:'onLeftMarginChanged', args:[]},
        topMarginChanged: {type:Signal, slotName:'onTopMarginChanged', args:[]},
        rightMarginChanged: {type:Signal, slotName:'onRightMarginChanged', args:[]},
        bottomMarginChanged: {type:Signal, slotName:'onBottomMarginChanged', args:[]},

        horizontalCenterChanged: {type:Signal, slotName:'onHorizontalCenterChanged', args:[]},
        verticalCenterChanged: {type:Signal, slotName:'onVerticalCenterChanged', args:[]},

        marginsChanged: {type:Signal, slotName:'onMarginsChanged', args:[]},
    }

    onMarginsChanged(){
        this.leftMargin = this.margins
        this.topMargin = this.margins
        this.rightMargin = this.margins
        this.bottomMargin = this.margins
    }

    onFillChanged(){
        let target = this.fill.__get()
        if(this.__parent && target){
            let updateList = []
            this.__parent.x = QtFunctions.binding(()=>{return this.leftMargin},updateList)
            this.__parent.y = QtFunctions.binding(()=>{return this.topMargin},updateList)
            this.__parent.width = QtFunctions.binding(()=>{return target.width - (this.leftMargin + this.rightMargin)},updateList)
            this.__parent.height = QtFunctions.binding(()=>{return target.height - (this.topMargin + this.bottomMargin)},updateList)

            for(let property of updateList){
                property.__update()
            }
        }
    }

    onCenterInChanged(){
        let target = this.centerIn.__get()
        if(this.__parent && target){
            let updateList = []
            this.__parent.x = QtFunctions.binding(()=>{return target.width / 2 - this.__parent.width / 2},updateList)
            this.__parent.y = QtFunctions.binding(()=>{return target.height / 2 - this.__parent.height / 2},updateList)

            for(let property of updateList){
                property.__update()
            }
        }
    }

    onHorizontalCenterChanged(){
        let target = this.horizontalCenter.__get()
        if(this.__parent && target){
            let updateList = []
            if(target.__float === AnchorLine.HorizontalCenter){
                this.__parent.x = QtFunctions.binding(()=>{return target.__parent.width / 2 - this.__parent.width / 2},updateList)
            }

            for(let property of updateList){
                property.__update()
            }
        }
    }

    onVerticalCenterChanged(){
        let target = this.verticalCenter.__get()
        if(this.__parent && target){
            let updateList = []
            if(target.__float === AnchorLine.VerticalCenter){
                this.__parent.y = QtFunctions.binding(()=>{return target.__parent.height / 2 - this.__parent.height / 2},updateList)
            }

            for(let property of updateList){
                property.__update()
            }
        }
    }
}

module.exports = Anchors