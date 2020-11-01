router  = require("@system.router")

export default {
    data: {
        title: 'World'
    },
    onBack: function() {
        router.replace({
            uri: "pages/index/index"
        })
    }
}
