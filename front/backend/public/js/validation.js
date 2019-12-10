$('.password').validate({
    rules: {
        name: {
            required: true,
            rangelength: [2, 10],
            remote: {
                url: "/validate_reg"
            }
        },
        password: {
            required: true,
            rangelength: [6, 16]
        },
        // 这里加双引号是因为有'-'，会把字符串转换为数值然后相减
        "password-repeat": {
            required: true,
            // password 的id属性，这里是一个选择器
            equalTo: "#regpassword"
        },
        email: {
            required: true,
            email: true
        }
    },
})