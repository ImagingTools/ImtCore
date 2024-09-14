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

        left: { type: Var, value: undefined, signalName: 'leftChanged' },
        right: { type: Var, value: undefined, signalName: 'rightChanged' },
        top: { type: Var, value: undefined, signalName: 'topChanged' },
        bottom: { type: Var, value: undefined, signalName: 'bottomChanged' },
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

        leftChanged: {type:Signal, slotName:'onLeftChanged', args:[]},
        topChanged: {type:Signal, slotName:'onTopChanged', args:[]},
        rightChanged: {type:Signal, slotName:'onRightChanged', args:[]},
        bottomChanged: {type:Signal, slotName:'onBottomChanged', args:[]},
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
        let target = this.fill
        let self = this.__parent

        if(self){
            self.__getObject('x').__unfreeze()
            self.__getObject('y').__unfreeze()
            self.__getObject('width').__unfreeze()
            self.__getObject('height').__unfreeze()
        }

        if(self && target){
            let lazy = !self.__completed

            if(self.parent === target){
                self.__getObject('AX').__subscribe(this.__getObject('leftMargin'))
                self.__getObject('AY').__subscribe(this.__getObject('topMargin'))
                self.__getObject('AWidth').__subscribe(target.__getObject('AWidth'),this.__getObject('leftMargin'),this.__getObject('rightMargin'))
                self.__getObject('AHeight').__subscribe(target.__getObject('AHeight'),this.__getObject('topMargin'),this.__getObject('bottomMargin'))

                self.AX = QtFunctions.binding(()=>{return this.leftMargin},lazy)
                self.AY = QtFunctions.binding(()=>{return this.topMargin},lazy)
                self.AWidth = QtFunctions.binding(()=>{return target.AWidth - (this.leftMargin + this.rightMargin)},lazy)
                self.AHeight = QtFunctions.binding(()=>{return target.AHeight - (this.topMargin + this.bottomMargin)},lazy)
            } else {
                self.__getObject('AX').__subscribe(target.__getObject('AX'),this.__getObject('leftMargin'))
                self.__getObject('AY').__subscribe(target.__getObject('AY'),this.__getObject('topMargin'))
                self.__getObject('AWidth').__subscribe(target.__getObject('AWidth'),this.__getObject('leftMargin'),this.__getObject('rightMargin'))
                self.__getObject('AHeight').__subscribe(target.__getObject('AHeight'),this.__getObject('topMargin'),this.__getObject('bottomMargin'))

                self.AX = QtFunctions.binding(()=>{return target.AX + this.leftMargin},lazy)
                self.AY = QtFunctions.binding(()=>{return target.AY + this.topMargin},lazy)
                self.AWidth = QtFunctions.binding(()=>{return target.AWidth - (this.leftMargin + this.rightMargin)},lazy)
                self.AHeight = QtFunctions.binding(()=>{return target.AHeight - (this.topMargin + this.bottomMargin)},lazy)
            }
            
            self.__getObject('x').__freeze()
            self.__getObject('y').__freeze()
            self.__getObject('width').__freeze()
            self.__getObject('height').__freeze()
        }
    }

    onCenterInChanged(){
        let target = this.centerIn
        let self = this.__parent

        if(self){
            self.__getObject('x').__unfreeze()
            self.__getObject('y').__unfreeze()
        }

        if(self && target){
            let lazy = !self.__completed

            if(self.parent === target){
                self.__getObject('AX').__subscribe(target.__getObject('AWidth'),self.__getObject('AWidth'))
                self.__getObject('AY').__subscribe(target.__getObject('AHeight'),self.__getObject('AHeight'))

                self.AX = QtFunctions.binding(()=>{return target.AWidth / 2 - self.AWidth / 2},lazy)
                self.AY = QtFunctions.binding(()=>{return target.AHeight / 2 - self.AHeight / 2},lazy)
            } else {
                self.__getObject('AX').__subscribe(target.__getObject('AX'),target.__getObject('AWidth'),self.__getObject('AWidth'))
                self.__getObject('AY').__subscribe(target.__getObject('AY'),target.__getObject('AHeight'),self.__getObject('AHeight'))

                self.AX = QtFunctions.binding(()=>{return target.AX + target.AWidth / 2 - self.AWidth / 2},lazy)
                self.AY = QtFunctions.binding(()=>{return target.AY + target.AHeight / 2 - self.AHeight / 2},lazy)
            }

            self.__getObject('x').__freeze()
            self.__getObject('y').__freeze()
        }
    }

    onHorizontalCenterChanged(){
        let target = this.horizontalCenter
        let self = this.__parent

        if(self){
            self.__getObject('x').__unfreeze()
        }

        if(self && target){
            let targetFloat = target.__float
            target = target.__parent

            let lazy = !self.__completed

            if(self.parent === target){
                if(targetFloat === AnchorLine.Left){
                    self.__getObject('AX').__subscribe(self.__getObject('AWidth'))

                    self.AX = QtFunctions.binding(()=>{return -self.AWidth / 2},lazy)
                } else if(targetFloat === AnchorLine.Right){
                    self.__getObject('AX').__subscribe(target.__getObject('AWidth'),self.__getObject('AWidth'))

                    self.AX = QtFunctions.binding(()=>{return target.AWidth - self.AWidth / 2},lazy)
                } else if(targetFloat === AnchorLine.HorizontalCenter){
                    self.__getObject('AX').__subscribe(target.__getObject('AWidth'),self.__getObject('AWidth'))

                    self.AX = QtFunctions.binding(()=>{return target.AWidth / 2 - self.AWidth / 2},lazy)
                }
            } else {
                if(targetFloat === AnchorLine.Left){
                    self.__getObject('AX').__subscribe(target.__getObject('AX'),self.__getObject('AWidth'))

                    self.AX = QtFunctions.binding(()=>{return target.AX - self.AWidth / 2},lazy)
                } else if(targetFloat === AnchorLine.Right){
                    self.__getObject('AX').__subscribe(target.__getObject('AX'),target.__getObject('AWidth'),self.__getObject('AWidth'))

                    self.AX = QtFunctions.binding(()=>{return target.AX + target.AWidth - self.AWidth / 2},lazy)
                } else if(targetFloat === AnchorLine.HorizontalCenter){
                    self.__getObject('AX').__subscribe(target.__getObject('AX'),target.__getObject('AWidth'),self.__getObject('AWidth'))

                    self.AX = QtFunctions.binding(()=>{return target.AX + target.AWidth / 2 - self.AWidth / 2},lazy)
                }
            }

            self.__getObject('x').__freeze()
        }
    }

    onVerticalCenterChanged(){
        let target = this.verticalCenter
        let self = this.__parent

        if(self){
            self.__getObject('y').__unfreeze()
        }

        if(self && target){
            let targetFloat = target.__float
            target = target.__parent

            let lazy = !self.__completed

            if(self.parent === target){
                if(targetFloat === AnchorLine.Top){
                    self.__getObject('AY').__subscribe(self.__getObject('AHeight'))

                    self.AY = QtFunctions.binding(()=>{return -self.AHeight / 2},lazy)
                } else if(targetFloat === AnchorLine.Bottom){
                    self.__getObject('AY').__subscribe(target.__getObject('AHeight'),self.__getObject('AHeight'))

                    self.AY = QtFunctions.binding(()=>{return target.AHeight - self.AHeight / 2},lazy)
                } else if(targetFloat === AnchorLine.VerticalCenter){
                    self.__getObject('AY').__subscribe(target.__getObject('AHeight'),self.__getObject('AHeight'))

                    self.AY = QtFunctions.binding(()=>{return target.AHeight / 2 - self.AHeight / 2},lazy)
                }
            } else {
                if(targetFloat === AnchorLine.Top){
                    self.__getObject('AY').__subscribe(target.__getObject('AY'),self.__getObject('AHeight'))

                    self.AY = QtFunctions.binding(()=>{return target.AY - self.AHeight / 2},lazy)
                } else if(targetFloat === AnchorLine.Bottom){
                    self.__getObject('AY').__subscribe(target.__getObject('AY'),target.__getObject('AHeight'),self.__getObject('AHeight'))

                    self.AY = QtFunctions.binding(()=>{return target.AY + target.AHeight - self.AHeight / 2},lazy)
                } else if(targetFloat === AnchorLine.VerticalCenter){
                    self.__getObject('AY').__subscribe(target.__getObject('AY'),target.__getObject('AHeight'),self.__getObject('AHeight'))

                    self.AY = QtFunctions.binding(()=>{return target.AY + target.AHeight / 2 - self.AHeight / 2},lazy)
                }               
            }

            self.__getObject('y').__freeze()
        }
    }

    onLeftChanged(){
        let target = this.left
        let self = this.__parent

        if(target && this.right) {
            this.onLeftRightChanged()
            return
        }

        if(self){
            self.__getObject('x').__unfreeze()
        }

        if(self && target){
            let targetFloat = target.__float
            target = target.__parent

            let lazy = !self.__completed

            if(self.parent === target){
                if(targetFloat === AnchorLine.Left){
                    self.AX = QtFunctions.binding(()=>{return this.leftMargin},lazy)
                } else if(targetFloat === AnchorLine.Right){
                    self.__getObject('AX').__subscribe(target.__getObject('AWidth'))

                    self.AX = QtFunctions.binding(()=>{return target.AWidth + this.leftMargin},lazy)
                } else if(targetFloat === AnchorLine.HorizontalCenter){
                    self.__getObject('AX').__subscribe(target.__getObject('AWidth'))

                    self.AX = QtFunctions.binding(()=>{return target.AWidth / 2 + this.leftMargin},lazy)
                }
            } else {
                if(targetFloat === AnchorLine.Left){
                    self.__getObject('AX').__subscribe(target.__getObject('AX'))

                    self.AX = QtFunctions.binding(()=>{return target.AX + this.leftMargin},lazy)
                } else if(targetFloat === AnchorLine.Right){
                    self.__getObject('AX').__subscribe(target.__getObject('AX'),target.__getObject('AWidth'))

                    self.AX = QtFunctions.binding(()=>{return target.AX + target.AWidth + this.leftMargin},lazy)
                } else if(targetFloat === AnchorLine.HorizontalCenter){
                    self.__getObject('AX').__subscribe(target.__getObject('AX'),target.__getObject('AWidth'))

                    self.AX = QtFunctions.binding(()=>{return target.AX + target.AWidth / 2 + this.leftMargin},lazy)
                }            
            }

            self.__getObject('x').__freeze()
        }
    }

    onRightChanged(){
        let target = this.right
        let self = this.__parent

        if(target && this.left) {
            this.onLeftRightChanged()
            return
        }

        if(self){
            self.__getObject('x').__unfreeze()
        }

        if(self && target){
            let targetFloat = target.__float
            target = target.__parent

            let lazy = !self.__completed

            if(self.parent === target){
                if(targetFloat === AnchorLine.Left){
                    self.__getObject('AX').__subscribe(self.__getObject('AWidth'))

                    self.AX = QtFunctions.binding(()=>{return -self.AWidth - this.rightMargin},lazy)
                } else if(targetFloat === AnchorLine.Right){
                    self.__getObject('AX').__subscribe(target.__getObject('AWidth'), self.__getObject('AWidth'))

                    self.AX = QtFunctions.binding(()=>{return target.AWidth - self.AWidth - this.rightMargin},lazy)
                } else if(targetFloat === AnchorLine.HorizontalCenter){
                    self.__getObject('AX').__subscribe(target.__getObject('AWidth'), self.__getObject('AWidth'))

                    self.AX = QtFunctions.binding(()=>{return target.AWidth / 2 - self.AWidth - this.rightMargin},lazy)
                }
            } else {
                if(targetFloat === AnchorLine.Left){
                    self.__getObject('AX').__subscribe(target.__getObject('AX'), self.__getObject('AWidth'))

                    self.AX = QtFunctions.binding(()=>{return target.AX - self.AWidth - this.rightMargin},lazy)
                } else if(targetFloat === AnchorLine.Right){
                    self.__getObject('AX').__subscribe(target.__getObject('AX'), target.__getObject('AWidth'), self.__getObject('AWidth'))

                    self.AX = QtFunctions.binding(()=>{return target.AX + target.AWidth - self.AWidth - this.rightMargin},lazy)
                } else if(targetFloat === AnchorLine.HorizontalCenter){
                    self.__getObject('AX').__subscribe(target.__getObject('AX'), target.__getObject('AWidth'), self.__getObject('AWidth'))

                    self.AX = QtFunctions.binding(()=>{return target.AX + target.AWidth / 2 - self.AWidth - this.rightMargin},lazy)
                }              
            }

            self.__getObject('x').__freeze()
        }
    }

    onLeftRightChanged(){
        let left = this.left
        let right = this.right
        let self = this.__parent

        if(self){
            self.__getObject('x').__unfreeze()
            self.__getObject('width').__unfreeze()
        }

        if(self && left && right){
            let leftFloat = left.__float
            left = left.__parent

            let rightFloat = right.__float
            right = right.__parent

            let lazy = !self.__completed

            if(self.parent === left){
                if(leftFloat === AnchorLine.Left){
                    self.AX = QtFunctions.binding(()=>{return this.leftMargin},lazy)
                } else if(leftFloat === AnchorLine.Right){
                    self.__getObject('AX').__subscribe(left.__getObject('AWidth'))

                    self.AX = QtFunctions.binding(()=>{return left.AWidth + this.leftMargin},lazy)
                } else if(leftFloat === AnchorLine.HorizontalCenter){
                    self.__getObject('AX').__subscribe(left.__getObject('AWidth'))

                    self.AX = QtFunctions.binding(()=>{return left.AWidth / 2 + this.leftMargin},lazy)
                }
            } else {
                if(leftFloat === AnchorLine.Left){
                    self.__getObject('AX').__subscribe(left.__getObject('AX'))

                    self.AX = QtFunctions.binding(()=>{return left.AX + this.leftMargin},lazy)
                } else if(leftFloat === AnchorLine.Right){
                    self.__getObject('AX').__subscribe(left.__getObject('AX'), left.__getObject('AWidth'))

                    self.AX = QtFunctions.binding(()=>{return left.AX + left.AWidth + this.leftMargin},lazy)
                } else if(leftFloat === AnchorLine.HorizontalCenter){
                    self.__getObject('AX').__subscribe(left.__getObject('AX'), left.__getObject('AWidth'))

                    self.AX = QtFunctions.binding(()=>{return left.AX + left.AWidth / 2 + this.leftMargin},lazy)
                }
            }

            if(self.parent === right){
                if(rightFloat === AnchorLine.Left){
                    self.__getObject('AWidth').__subscribe(self.__getObject('AX'))

                    self.AWidth = QtFunctions.binding(()=>{return -this.rightMargin - self.x},lazy)
                } else if(rightFloat === AnchorLine.Right){
                    self.__getObject('AWidth').__subscribe(right.__getObject('AWidth'), self.__getObject('AX'))

                    self.AWidth = QtFunctions.binding(()=>{return right.AWidth - this.rightMargin - self.x},lazy)
                } else if(rightFloat === AnchorLine.HorizontalCenter){
                    self.__getObject('AWidth').__subscribe(right.__getObject('AWidth'), self.__getObject('AX'))

                    self.AWidth = QtFunctions.binding(()=>{return right.AWidth / 2 - this.rightMargin - self.x},lazy)
                }
            } else {
                if(rightFloat === AnchorLine.Left){
                    self.__getObject('AWidth').__subscribe(right.__getObject('AX'), self.__getObject('AX'))

                    self.AWidth = QtFunctions.binding(()=>{return right.AX - this.rightMargin - self.AX},lazy)
                } else if(rightFloat === AnchorLine.Right){
                    self.__getObject('AWidth').__subscribe(right.__getObject('AX'), right.__getObject('AWidth'), self.__getObject('AX'))

                    self.AWidth = QtFunctions.binding(()=>{return right.AX + right.AWidth - this.rightMargin - self.AX},lazy)
                } else if(rightFloat === AnchorLine.HorizontalCenter){
                    self.__getObject('AWidth').__subscribe(right.__getObject('AX'), right.__getObject('AWidth'), self.__getObject('AX'))

                    self.AWidth = QtFunctions.binding(()=>{return right.AX + right.AWidth / 2 - this.rightMargin - self.AX},lazy)
                }
            }

            self.__getObject('x').__freeze()
            self.__getObject('width').__freeze()
        }
    }

    onTopChanged(){
        let target = this.top
        let self = this.__parent

        if(target && this.bottom) {
            this.onTopBottomChanged()
            return
        }

        if(self){
            self.__getObject('y').__unfreeze()
        }

        if(self && target){
            let targetFloat = target.__float
            target = target.__parent

            let lazy = !self.__completed

            if(self.parent === target){
                if(targetFloat === AnchorLine.Top){
                    self.AY = QtFunctions.binding(()=>{return this.topMargin},lazy)
                } else if(targetFloat === AnchorLine.Bottom){
                    self.__getObject('AY').__subscribe(target.__getObject('AHeight'))

                    self.AY = QtFunctions.binding(()=>{return target.AHeight + this.topMargin},lazy)
                } else if(targetFloat === AnchorLine.VerticalCenter){
                    self.__getObject('AY').__subscribe(target.__getObject('AHeight'))

                    self.AY = QtFunctions.binding(()=>{return target.AHeight / 2 + this.topMargin},lazy)
                }
            } else {
                if(targetFloat === AnchorLine.Top){
                    self.__getObject('AY').__subscribe(target.__getObject('AY'))

                    self.AY = QtFunctions.binding(()=>{return target.AY + this.topMargin},lazy)
                } else if(targetFloat === AnchorLine.Bottom){
                    self.__getObject('AY').__subscribe(target.__getObject('AY'), target.__getObject('AHeight'))

                    self.AY = QtFunctions.binding(()=>{return target.AY + target.AHeight + this.topMargin},lazy)
                } else if(targetFloat === AnchorLine.VerticalCenter){
                    self.__getObject('AY').__subscribe(target.__getObject('AY'), target.__getObject('AHeight'))

                    self.AY = QtFunctions.binding(()=>{return target.AY + target.AHeight / 2 + this.topMargin},lazy)
                }               
            }

            self.__getObject('y').__freeze()
        }
    }

    onBottomChanged(){
        let target = this.bottom
        let self = this.__parent
        
        if(target && this.top) {
            this.onTopBottomChanged()
            return
        }

        if(self){
            self.__getObject('y').__unfreeze()
        }

        if(self && target){
            let targetFloat = target.__float
            target = target.__parent

            let lazy = !self.__completed

            if(self.parent === target){
                if(targetFloat === AnchorLine.Top){
                    self.__getObject('AY').__subscribe(self.__getObject('AHeight'))

                    self.AY = QtFunctions.binding(()=>{return -self.AHeight - this.bottomMargin},lazy)
                } else if(targetFloat === AnchorLine.Bottom){
                    self.__getObject('AY').__subscribe(target.__getObject('AHeight'), self.__getObject('AHeight'))

                    self.AY = QtFunctions.binding(()=>{return target.AHeight - self.AHeight - this.bottomMargin},lazy)
                } else if(targetFloat === AnchorLine.VerticalCenter){
                    self.__getObject('AY').__subscribe(target.__getObject('AHeight'), self.__getObject('AHeight'))

                    self.AY = QtFunctions.binding(()=>{return target.AHeight / 2 - self.AHeight - this.bottomMargin},lazy)
                }
            } else {
                if(targetFloat === AnchorLine.Top){
                    self.__getObject('AY').__subscribe(target.__getObject('AY'), self.__getObject('AHeight'))

                    self.AY = QtFunctions.binding(()=>{return target.AY - self.AHeight - this.bottomMargin},lazy)
                } else if(targetFloat === AnchorLine.Bottom){
                    self.__getObject('AY').__subscribe(target.__getObject('AY'), target.__getObject('AHeight'), self.__getObject('AHeight'))

                    self.AY = QtFunctions.binding(()=>{return target.AY + target.AHeight - self.AHeight - this.bottomMargin},lazy)
                } else if(targetFloat === AnchorLine.VerticalCenter){
                    self.__getObject('AY').__subscribe(target.__getObject('AY'), target.__getObject('AHeight'), self.__getObject('AHeight'))

                    self.AY = QtFunctions.binding(()=>{return target.AY + target.AHeight / 2 - self.AHeight - this.bottomMargin},lazy)
                }               
            }

            self.__getObject('y').__freeze()
        }
    }

    onTopBottomChanged(){
        let top = this.top
        let bottom = this.bottom
        let self = this.__parent

        if(self){
            self.__getObject('y').__unfreeze()
            self.__getObject('height').__unfreeze()
        }

        if(self && top && bottom){
            let topFloat = top.__float
            top = top.__parent

            let bottomFloat = bottom.__float
            bottom = bottom.__parent

            let lazy = !self.__completed

            if(self.parent === top){
                if(topFloat === AnchorLine.Top){
                    self.AY = QtFunctions.binding(()=>{return this.topMargin},lazy)
                } else if(topFloat === AnchorLine.Bottom){
                    self.__getObject('AY').__subscribe(top.__getObject('AHeight'))

                    self.AY = QtFunctions.binding(()=>{return top.AHeight + this.topMargin},lazy)
                } else if(topFloat === AnchorLine.VerticalCenter){
                    self.__getObject('AY').__subscribe(top.__getObject('AHeight'))

                    self.AY = QtFunctions.binding(()=>{return top.AHeight / 2 + this.topMargin},lazy)
                }
            } else {
                if(topFloat === AnchorLine.Top){
                    self.__getObject('AY').__subscribe(top.__getObject('AY'))

                    self.AY = QtFunctions.binding(()=>{return top.AY + this.topMargin},lazy)
                } else if(topFloat === AnchorLine.Bottom){
                    self.__getObject('AY').__subscribe(top.__getObject('AY'), top.__getObject('AHeight'))

                    self.AY = QtFunctions.binding(()=>{return top.AY + top.AHeight + this.topMargin},lazy)
                } else if(topFloat === AnchorLine.VerticalCenter){
                    self.__getObject('AY').__subscribe(top.__getObject('AY'), top.__getObject('AHeight'))

                    self.AY = QtFunctions.binding(()=>{return top.AY + top.AHeight / 2 + this.topMargin},lazy)
                }
            }

            if(self.parent === bottom){
                if(bottomFloat === AnchorLine.Top){
                    self.__getObject('AHeight').__subscribe(self.__getObject('AY'))

                    self.AHeight = QtFunctions.binding(()=>{return -this.bottomMargin - self.AY},lazy)
                } else if(bottomFloat === AnchorLine.Bottom){
                    self.__getObject('AHeight').__subscribe(bottom.__getObject('AHeight'), self.__getObject('AY'))

                    self.AHeight = QtFunctions.binding(()=>{return bottom.AHeight - this.bottomMargin - self.AY},lazy)
                } else if(bottomFloat === AnchorLine.VerticalCenter){
                    self.__getObject('AHeight').__subscribe(bottom.__getObject('AHeight'), self.__getObject('AY'))

                    self.AHeight = QtFunctions.binding(()=>{return bottom.AHeight / 2 - this.bottomMargin - self.AY},lazy)
                }
            } else {
                if(bottomFloat === AnchorLine.Top){
                    self.__getObject('AHeight').__subscribe(bottom.__getObject('AY'), self.__getObject('AY'))

                    self.AHeight = QtFunctions.binding(()=>{return bottom.AY - this.bottomMargin - self.AY},lazy)
                } else if(bottomFloat === AnchorLine.Bottom){
                    self.__getObject('AHeight').__subscribe(bottom.__getObject('AY'), bottom.__getObject('AHeight'), self.__getObject('AY'))

                    self.AHeight = QtFunctions.binding(()=>{return bottom.AY + bottom.AHeight - this.bottomMargin - self.AY},lazy)
                } else if(bottomFloat === AnchorLine.VerticalCenter){
                    self.__getObject('AHeight').__subscribe(bottom.__getObject('AY'), bottom.__getObject('AHeight'), self.__getObject('AY'))

                    self.AHeight = QtFunctions.binding(()=>{return bottom.AY + bottom.AHeight / 2 - this.bottomMargin - self.AY},lazy)
                }
            }

            self.__getObject('y').__freeze()
            self.__getObject('height').__freeze()
        }
    }
}

module.exports = Anchors