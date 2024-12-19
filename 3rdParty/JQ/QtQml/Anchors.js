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
            self.__getDataQml('x').__unfreeze()
            self.__getDataQml('y').__unfreeze()
            self.__getDataQml('width').__unfreeze()
            self.__getDataQml('height').__unfreeze()
        }

        if(self && target){
            let lazy = !self.__completed

            if(self.parent === target){
                self.__getDataQml('x').__subscribe(this.__getDataQml('leftMargin'))
                self.__getDataQml('y').__subscribe(this.__getDataQml('topMargin'))
                self.__getDataQml('width').__subscribe(target.__getDataQml('width'),this.__getDataQml('leftMargin'),this.__getDataQml('rightMargin'))
                self.__getDataQml('height').__subscribe(target.__getDataQml('height'),this.__getDataQml('topMargin'),this.__getDataQml('bottomMargin'))

                self.x = QtFunctions.binding(()=>{return this.leftMargin},lazy)
                self.y = QtFunctions.binding(()=>{return this.topMargin},lazy)
                self.width = QtFunctions.binding(()=>{return target.width - (this.leftMargin + this.rightMargin)},lazy)
                self.height = QtFunctions.binding(()=>{return target.height - (this.topMargin + this.bottomMargin)},lazy)
            } else {
                self.__getDataQml('x').__subscribe(target.__getDataQml('x'),this.__getDataQml('leftMargin'))
                self.__getDataQml('y').__subscribe(target.__getDataQml('y'),this.__getDataQml('topMargin'))
                self.__getDataQml('width').__subscribe(target.__getDataQml('width'),this.__getDataQml('leftMargin'),this.__getDataQml('rightMargin'))
                self.__getDataQml('height').__subscribe(target.__getDataQml('height'),this.__getDataQml('topMargin'),this.__getDataQml('bottomMargin'))

                self.x = QtFunctions.binding(()=>{return target.x + this.leftMargin},lazy)
                self.y = QtFunctions.binding(()=>{return target.y + this.topMargin},lazy)
                self.width = QtFunctions.binding(()=>{return target.width - (this.leftMargin + this.rightMargin)},lazy)
                self.height = QtFunctions.binding(()=>{return target.height - (this.topMargin + this.bottomMargin)},lazy)
            }
            
            self.__getDataQml('x').__freeze()
            self.__getDataQml('y').__freeze()
            self.__getDataQml('width').__freeze()
            self.__getDataQml('height').__freeze()
        }
    }

    onCenterInChanged(){
        let target = this.centerIn
        let self = this.__parent

        if(self){
            self.__getDataQml('x').__unfreeze()
            self.__getDataQml('y').__unfreeze()
        }

        if(self && target){
            let lazy = !self.__completed

            if(self.parent === target){
                self.__getDataQml('x').__subscribe(target.__getDataQml('width'),self.__getDataQml('width'))
                self.__getDataQml('y').__subscribe(target.__getDataQml('height'),self.__getDataQml('height'))

                self.x = QtFunctions.binding(()=>{return target.width / 2 - self.width / 2},lazy)
                self.y = QtFunctions.binding(()=>{return target.height / 2 - self.height / 2},lazy)
            } else {
                self.__getDataQml('x').__subscribe(target.__getDataQml('x'),target.__getDataQml('width'),self.__getDataQml('width'))
                self.__getDataQml('y').__subscribe(target.__getDataQml('y'),target.__getDataQml('height'),self.__getDataQml('height'))

                self.x = QtFunctions.binding(()=>{return target.x + target.width / 2 - self.width / 2},lazy)
                self.y = QtFunctions.binding(()=>{return target.y + target.height / 2 - self.height / 2},lazy)
            }

            self.__getDataQml('x').__freeze()
            self.__getDataQml('y').__freeze()
        }
    }

    onHorizontalCenterChanged(){
        let target = this.horizontalCenter
        let self = this.__parent

        if(self){
            self.__getDataQml('x').__unfreeze()
        }

        if(self && target){
            let targetFloat = target.__float
            target = target.__parent

            let lazy = !self.__completed

            if(self.parent === target){
                if(targetFloat === AnchorLine.Left){
                    self.__getDataQml('x').__subscribe(self.__getDataQml('width'))

                    self.x = QtFunctions.binding(()=>{return -self.width / 2},lazy)
                } else if(targetFloat === AnchorLine.Right){
                    self.__getDataQml('x').__subscribe(target.__getDataQml('width'),self.__getDataQml('width'))

                    self.x = QtFunctions.binding(()=>{return target.width - self.width / 2},lazy)
                } else if(targetFloat === AnchorLine.HorizontalCenter){
                    self.__getDataQml('x').__subscribe(target.__getDataQml('width'),self.__getDataQml('width'))

                    self.x = QtFunctions.binding(()=>{return target.width / 2 - self.width / 2},lazy)
                }
            } else {
                if(targetFloat === AnchorLine.Left){
                    self.__getDataQml('x').__subscribe(target.__getDataQml('x'),self.__getDataQml('width'))

                    self.x = QtFunctions.binding(()=>{return target.x - self.width / 2},lazy)
                } else if(targetFloat === AnchorLine.Right){
                    self.__getDataQml('x').__subscribe(target.__getDataQml('x'),target.__getDataQml('width'),self.__getDataQml('width'))

                    self.x = QtFunctions.binding(()=>{return target.x + target.width - self.width / 2},lazy)
                } else if(targetFloat === AnchorLine.HorizontalCenter){
                    self.__getDataQml('x').__subscribe(target.__getDataQml('x'),target.__getDataQml('width'),self.__getDataQml('width'))

                    self.x = QtFunctions.binding(()=>{return target.x + target.width / 2 - self.width / 2},lazy)
                }
            }

            self.__getDataQml('x').__freeze()
        }
    }

    onVerticalCenterChanged(){
        let target = this.verticalCenter
        let self = this.__parent

        if(self){
            self.__getDataQml('y').__unfreeze()
        }

        if(self && target){
            let targetFloat = target.__float
            target = target.__parent

            let lazy = !self.__completed

            if(self.parent === target){
                if(targetFloat === AnchorLine.Top){
                    self.__getDataQml('y').__subscribe(self.__getDataQml('height'))

                    self.y = QtFunctions.binding(()=>{return -self.height / 2},lazy)
                } else if(targetFloat === AnchorLine.Bottom){
                    self.__getDataQml('y').__subscribe(target.__getDataQml('height'),self.__getDataQml('height'))

                    self.y = QtFunctions.binding(()=>{return target.height - self.height / 2},lazy)
                } else if(targetFloat === AnchorLine.VerticalCenter){
                    self.__getDataQml('y').__subscribe(target.__getDataQml('height'),self.__getDataQml('height'))

                    self.y = QtFunctions.binding(()=>{return target.height / 2 - self.height / 2},lazy)
                }
            } else {
                if(targetFloat === AnchorLine.Top){
                    self.__getDataQml('y').__subscribe(target.__getDataQml('y'),self.__getDataQml('height'))

                    self.y = QtFunctions.binding(()=>{return target.y - self.height / 2},lazy)
                } else if(targetFloat === AnchorLine.Bottom){
                    self.__getDataQml('y').__subscribe(target.__getDataQml('y'),target.__getDataQml('height'),self.__getDataQml('height'))

                    self.y = QtFunctions.binding(()=>{return target.y + target.height - self.height / 2},lazy)
                } else if(targetFloat === AnchorLine.VerticalCenter){
                    self.__getDataQml('y').__subscribe(target.__getDataQml('y'),target.__getDataQml('height'),self.__getDataQml('height'))

                    self.y = QtFunctions.binding(()=>{return target.y + target.height / 2 - self.height / 2},lazy)
                }               
            }

            self.__getDataQml('y').__freeze()
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
            self.__getDataQml('x').__unfreeze()
        }

        if(self && target){
            let targetFloat = target.__float
            target = target.__parent

            let lazy = !self.__completed

            if(self.parent === target){
                if(targetFloat === AnchorLine.Left){
                    self.x = QtFunctions.binding(()=>{return this.leftMargin},lazy)
                } else if(targetFloat === AnchorLine.Right){
                    self.__getDataQml('x').__subscribe(target.__getDataQml('width'))

                    self.x = QtFunctions.binding(()=>{return target.width + this.leftMargin},lazy)
                } else if(targetFloat === AnchorLine.HorizontalCenter){
                    self.__getDataQml('x').__subscribe(target.__getDataQml('width'))

                    self.x = QtFunctions.binding(()=>{return target.width / 2 + this.leftMargin},lazy)
                }
            } else {
                if(targetFloat === AnchorLine.Left){
                    self.__getDataQml('x').__subscribe(target.__getDataQml('x'))

                    self.x = QtFunctions.binding(()=>{return target.x + this.leftMargin},lazy)
                } else if(targetFloat === AnchorLine.Right){
                    self.__getDataQml('x').__subscribe(target.__getDataQml('x'),target.__getDataQml('width'))

                    self.x = QtFunctions.binding(()=>{return target.x + target.width + this.leftMargin},lazy)
                } else if(targetFloat === AnchorLine.HorizontalCenter){
                    self.__getDataQml('x').__subscribe(target.__getDataQml('x'),target.__getDataQml('width'))

                    self.x = QtFunctions.binding(()=>{return target.x + target.width / 2 + this.leftMargin},lazy)
                }            
            }

            self.__getDataQml('x').__freeze()
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
            self.__getDataQml('x').__unfreeze()
        }

        if(self && target){
            let targetFloat = target.__float
            target = target.__parent

            let lazy = !self.__completed

            if(self.parent === target){
                if(targetFloat === AnchorLine.Left){
                    self.__getDataQml('x').__subscribe(self.__getDataQml('width'))

                    self.x = QtFunctions.binding(()=>{return -self.width - this.rightMargin},lazy)
                } else if(targetFloat === AnchorLine.Right){
                    self.__getDataQml('x').__subscribe(target.__getDataQml('width'), self.__getDataQml('width'))

                    self.x = QtFunctions.binding(()=>{return target.width - self.width - this.rightMargin},lazy)
                } else if(targetFloat === AnchorLine.HorizontalCenter){
                    self.__getDataQml('x').__subscribe(target.__getDataQml('width'), self.__getDataQml('width'))

                    self.x = QtFunctions.binding(()=>{return target.width / 2 - self.width - this.rightMargin},lazy)
                }
            } else {
                if(targetFloat === AnchorLine.Left){
                    self.__getDataQml('x').__subscribe(target.__getDataQml('x'), self.__getDataQml('width'))

                    self.x = QtFunctions.binding(()=>{return target.x - self.width - this.rightMargin},lazy)
                } else if(targetFloat === AnchorLine.Right){
                    self.__getDataQml('x').__subscribe(target.__getDataQml('x'), target.__getDataQml('width'), self.__getDataQml('width'))

                    self.x = QtFunctions.binding(()=>{return target.x + target.width - self.width - this.rightMargin},lazy)
                } else if(targetFloat === AnchorLine.HorizontalCenter){
                    self.__getDataQml('x').__subscribe(target.__getDataQml('x'), target.__getDataQml('width'), self.__getDataQml('width'))

                    self.x = QtFunctions.binding(()=>{return target.x + target.width / 2 - self.width - this.rightMargin},lazy)
                }              
            }

            self.__getDataQml('x').__freeze()
        }
    }

    onLeftRightChanged(){
        let left = this.left
        let right = this.right
        let self = this.__parent

        if(self){
            self.__getDataQml('x').__unfreeze()
            self.__getDataQml('width').__unfreeze()
        }

        if(self && left && right){
            let leftFloat = left.__float
            left = left.__parent

            let rightFloat = right.__float
            right = right.__parent

            let lazy = !self.__completed

            if(self.parent === left){
                if(leftFloat === AnchorLine.Left){
                    self.x = QtFunctions.binding(()=>{return this.leftMargin},lazy)
                } else if(leftFloat === AnchorLine.Right){
                    self.__getDataQml('x').__subscribe(left.__getDataQml('width'))

                    self.x = QtFunctions.binding(()=>{return left.width + this.leftMargin},lazy)
                } else if(leftFloat === AnchorLine.HorizontalCenter){
                    self.__getDataQml('x').__subscribe(left.__getDataQml('width'))

                    self.x = QtFunctions.binding(()=>{return left.width / 2 + this.leftMargin},lazy)
                }
            } else {
                if(leftFloat === AnchorLine.Left){
                    self.__getDataQml('x').__subscribe(left.__getDataQml('x'))

                    self.x = QtFunctions.binding(()=>{return left.x + this.leftMargin},lazy)
                } else if(leftFloat === AnchorLine.Right){
                    self.__getDataQml('x').__subscribe(left.__getDataQml('x'), left.__getDataQml('width'))

                    self.x = QtFunctions.binding(()=>{return left.x + left.width + this.leftMargin},lazy)
                } else if(leftFloat === AnchorLine.HorizontalCenter){
                    self.__getDataQml('x').__subscribe(left.__getDataQml('x'), left.__getDataQml('width'))

                    self.x = QtFunctions.binding(()=>{return left.x + left.width / 2 + this.leftMargin},lazy)
                }
            }

            if(self.parent === right){
                if(rightFloat === AnchorLine.Left){
                    self.__getDataQml('width').__subscribe(self.__getDataQml('x'))

                    self.width = QtFunctions.binding(()=>{return -this.rightMargin - self.x},lazy)
                } else if(rightFloat === AnchorLine.Right){
                    self.__getDataQml('width').__subscribe(right.__getDataQml('width'), self.__getDataQml('x'))

                    self.width = QtFunctions.binding(()=>{return right.width - this.rightMargin - self.x},lazy)
                } else if(rightFloat === AnchorLine.HorizontalCenter){
                    self.__getDataQml('width').__subscribe(right.__getDataQml('width'), self.__getDataQml('x'))

                    self.width = QtFunctions.binding(()=>{return right.width / 2 - this.rightMargin - self.x},lazy)
                }
            } else {
                if(rightFloat === AnchorLine.Left){
                    self.__getDataQml('width').__subscribe(right.__getDataQml('x'), self.__getDataQml('x'))

                    self.width = QtFunctions.binding(()=>{return right.x - this.rightMargin - self.x},lazy)
                } else if(rightFloat === AnchorLine.Right){
                    self.__getDataQml('width').__subscribe(right.__getDataQml('x'), right.__getDataQml('width'), self.__getDataQml('x'))

                    self.width = QtFunctions.binding(()=>{return right.x + right.width - this.rightMargin - self.x},lazy)
                } else if(rightFloat === AnchorLine.HorizontalCenter){
                    self.__getDataQml('width').__subscribe(right.__getDataQml('x'), right.__getDataQml('width'), self.__getDataQml('x'))

                    self.width = QtFunctions.binding(()=>{return right.x + right.width / 2 - this.rightMargin - self.x},lazy)
                }
            }

            self.__getDataQml('x').__freeze()
            self.__getDataQml('width').__freeze()
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
            self.__getDataQml('y').__unfreeze()
        }

        if(self && target){
            let targetFloat = target.__float
            target = target.__parent

            let lazy = !self.__completed

            if(self.parent === target){
                if(targetFloat === AnchorLine.Top){
                    self.y = QtFunctions.binding(()=>{return this.topMargin},lazy)
                } else if(targetFloat === AnchorLine.Bottom){
                    self.__getDataQml('y').__subscribe(target.__getDataQml('height'))

                    self.y = QtFunctions.binding(()=>{return target.height + this.topMargin},lazy)
                } else if(targetFloat === AnchorLine.VerticalCenter){
                    self.__getDataQml('y').__subscribe(target.__getDataQml('height'))

                    self.y = QtFunctions.binding(()=>{return target.height / 2 + this.topMargin},lazy)
                }
            } else {
                if(targetFloat === AnchorLine.Top){
                    self.__getDataQml('y').__subscribe(target.__getDataQml('y'))

                    self.y = QtFunctions.binding(()=>{return target.y + this.topMargin},lazy)
                } else if(targetFloat === AnchorLine.Bottom){
                    self.__getDataQml('y').__subscribe(target.__getDataQml('y'), target.__getDataQml('height'))

                    self.y = QtFunctions.binding(()=>{return target.y + target.height + this.topMargin},lazy)
                } else if(targetFloat === AnchorLine.VerticalCenter){
                    self.__getDataQml('y').__subscribe(target.__getDataQml('y'), target.__getDataQml('height'))

                    self.y = QtFunctions.binding(()=>{return target.y + target.height / 2 + this.topMargin},lazy)
                }               
            }

            self.__getDataQml('y').__freeze()
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
            self.__getDataQml('y').__unfreeze()
        }

        if(self && target){
            let targetFloat = target.__float
            target = target.__parent

            let lazy = !self.__completed

            if(self.parent === target){
                if(targetFloat === AnchorLine.Top){
                    self.__getDataQml('y').__subscribe(self.__getDataQml('height'))

                    self.y = QtFunctions.binding(()=>{return -self.height - this.bottomMargin},lazy)
                } else if(targetFloat === AnchorLine.Bottom){
                    self.__getDataQml('y').__subscribe(target.__getDataQml('height'), self.__getDataQml('height'))

                    self.y = QtFunctions.binding(()=>{return target.height - self.height - this.bottomMargin},lazy)
                } else if(targetFloat === AnchorLine.VerticalCenter){
                    self.__getDataQml('y').__subscribe(target.__getDataQml('height'), self.__getDataQml('height'))

                    self.y = QtFunctions.binding(()=>{return target.height / 2 - self.height - this.bottomMargin},lazy)
                }
            } else {
                if(targetFloat === AnchorLine.Top){
                    self.__getDataQml('y').__subscribe(target.__getDataQml('y'), self.__getDataQml('height'))

                    self.y = QtFunctions.binding(()=>{return target.y - self.height - this.bottomMargin},lazy)
                } else if(targetFloat === AnchorLine.Bottom){
                    self.__getDataQml('y').__subscribe(target.__getDataQml('y'), target.__getDataQml('height'), self.__getDataQml('height'))

                    self.y = QtFunctions.binding(()=>{return target.y + target.height - self.height - this.bottomMargin},lazy)
                } else if(targetFloat === AnchorLine.VerticalCenter){
                    self.__getDataQml('y').__subscribe(target.__getDataQml('y'), target.__getDataQml('height'), self.__getDataQml('height'))

                    self.y = QtFunctions.binding(()=>{return target.y + target.height / 2 - self.height - this.bottomMargin},lazy)
                }               
            }

            self.__getDataQml('y').__freeze()
        }
    }

    onTopBottomChanged(){
        let top = this.top
        let bottom = this.bottom
        let self = this.__parent

        if(self){
            self.__getDataQml('y').__unfreeze()
            self.__getDataQml('height').__unfreeze()
        }

        if(self && top && bottom){
            let topFloat = top.__float
            top = top.__parent

            let bottomFloat = bottom.__float
            bottom = bottom.__parent

            let lazy = !self.__completed

            if(self.parent === top){
                if(topFloat === AnchorLine.Top){
                    self.y = QtFunctions.binding(()=>{return this.topMargin},lazy)
                } else if(topFloat === AnchorLine.Bottom){
                    self.__getDataQml('y').__subscribe(top.__getDataQml('height'))

                    self.y = QtFunctions.binding(()=>{return top.height + this.topMargin},lazy)
                } else if(topFloat === AnchorLine.VerticalCenter){
                    self.__getDataQml('y').__subscribe(top.__getDataQml('height'))

                    self.y = QtFunctions.binding(()=>{return top.height / 2 + this.topMargin},lazy)
                }
            } else {
                if(topFloat === AnchorLine.Top){
                    self.__getDataQml('y').__subscribe(top.__getDataQml('y'))

                    self.y = QtFunctions.binding(()=>{return top.y + this.topMargin},lazy)
                } else if(topFloat === AnchorLine.Bottom){
                    self.__getDataQml('y').__subscribe(top.__getDataQml('y'), top.__getDataQml('height'))

                    self.y = QtFunctions.binding(()=>{return top.y + top.height + this.topMargin},lazy)
                } else if(topFloat === AnchorLine.VerticalCenter){
                    self.__getDataQml('y').__subscribe(top.__getDataQml('y'), top.__getDataQml('height'))

                    self.y = QtFunctions.binding(()=>{return top.y + top.height / 2 + this.topMargin},lazy)
                }
            }

            if(self.parent === bottom){
                if(bottomFloat === AnchorLine.Top){
                    self.__getDataQml('height').__subscribe(self.__getDataQml('y'))

                    self.height = QtFunctions.binding(()=>{return -this.bottomMargin - self.y},lazy)
                } else if(bottomFloat === AnchorLine.Bottom){
                    self.__getDataQml('height').__subscribe(bottom.__getDataQml('height'), self.__getDataQml('y'))

                    self.height = QtFunctions.binding(()=>{return bottom.height - this.bottomMargin - self.y},lazy)
                } else if(bottomFloat === AnchorLine.VerticalCenter){
                    self.__getDataQml('height').__subscribe(bottom.__getDataQml('height'), self.__getDataQml('y'))

                    self.height = QtFunctions.binding(()=>{return bottom.height / 2 - this.bottomMargin - self.y},lazy)
                }
            } else {
                if(bottomFloat === AnchorLine.Top){
                    self.__getDataQml('height').__subscribe(bottom.__getDataQml('y'), self.__getDataQml('y'))

                    self.height = QtFunctions.binding(()=>{return bottom.y - this.bottomMargin - self.y},lazy)
                } else if(bottomFloat === AnchorLine.Bottom){
                    self.__getDataQml('height').__subscribe(bottom.__getDataQml('y'), bottom.__getDataQml('height'), self.__getDataQml('y'))

                    self.height = QtFunctions.binding(()=>{return bottom.y + bottom.height - this.bottomMargin - self.y},lazy)
                } else if(bottomFloat === AnchorLine.VerticalCenter){
                    self.__getDataQml('height').__subscribe(bottom.__getDataQml('y'), bottom.__getDataQml('height'), self.__getDataQml('y'))

                    self.height = QtFunctions.binding(()=>{return bottom.y + bottom.height / 2 - this.bottomMargin - self.y},lazy)
                }
            }

            self.__getDataQml('y').__freeze()
            self.__getDataQml('height').__freeze()
        }
    }
}

Anchors.initialize()

module.exports = Anchors