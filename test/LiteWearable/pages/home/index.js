router  = require("@system.router")

export default {
    data: {
        title: 'Home',
        pageIndex: 0,
    },
    
    onInit: function () {
        print("==========onInit==========")
    },

    onReady: function () {
        this.data.pageIndex = 20;
        // this.data.user.age = 30;
        print("==========onReady==========")
        print(this.data)
        print(this.data.pageIndex)
    },

    onShow: function () {
        print("==========onShow==========")
    },

    onHide: function () {
        print("==========onHide==========")
    },
    
    onDestroy: function () {
        print("==========onDestroy==========")
    },

    onPageRedirect: function (obj, x, y) {
        print("==========onclick==========")
        print(obj.attributes.url);
        router.push({
            uri: obj.attributes.url
        })
    }
}