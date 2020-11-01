router  = require("@system.router")

export default {
    data: {
        title: 'Hello,Developer!'
    },
    onInit: function () {
        print("==========onInit index==========")
    },

    onReady: function () {
        print("==========onReady index==========")
    },

    onShow: function () {
        print("==========onShow index==========")
        setTimeout(function(){
            router.push({ uri: "pages/dock/index" })
        }, 2000)
    },

    onHide: function () {
        print("==========onHide index==========")
    },
    
    onDestroy: function () {
        print("==========onDestroy index==========")
    },
}
