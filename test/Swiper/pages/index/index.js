router = require("@system.router")

export default {
    data: {
        title: 'show-abcedfghijklmn',
        pageIndex: 0,
        address: {
            city: 'beijing'
        }
    },

    onInit: function() {
        print("==========onInit==========")
    },

    onReady: function() {
        // this.data.pageIndex = 20;
        // this.data.title = '30000000';
        print("==========onReady==========")
        print(this.data)
        print(this.data.pageIndex)
    },

    onShow: function() {
        print("==========onShow==========")
    },

    onHide: function() {
        print("==========onHide==========")
    },

    onDestroy: function() {
        print("==========onDestroy==========")
    },

    onPage1: function(obj, x, y) {
        print("==========onclick==========")
        print(x);
        print(x);
        router.replace({
            uri: "pages/slider/index"
        })
    },

    onPage2: function(obj, x, y) {
        print("==========onclick==========")
        print(x);
        print(x);
        router.replace({
            uri: "pages/switch/index"
        })
    },

    onPage3: function(obj, x, y) {
        print("==========onclick==========")
        print(x);
        print(x);
        router.replace({
            uri: "pages/progress/index"
        })
    }
}