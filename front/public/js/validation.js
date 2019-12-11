//Server part
const login = (form) => {
    const username = $('.login-email').val();
    const password = $('.login-password').val();
    const cat = $('#captcha').val();
    const data = {
        "username": username,
        "password": password,
        "captcha": cat
    };
    axios({
        url: '/users/login',
        method: 'POST',
        data: data,
        baseURL: baseURL,
    }).then((ans, err) => {
        console.log(err);
        $('.email').innerText = $('.password').innerText = '';
        location.href = 'home.html';
        toastr.success('login successfully');
    }).catch(error => {
        toastr.error(error.response.data);
    });
};

const register = (form) => {
    const username = $('.reg-email').val();
    const password = $('.reg-password').val();
    const checker = $('.reg-password-checker').val();
    const data = {"username": username, "password": password};
    axios({
        url: '/users/',
        method: 'POST',
        data: data,
        baseURL: baseURL,
    }).then(res => {
        document.getElementById("login-form").reset();
        document.getElementById("reg-form").reset();
        // alert('register successfully');
        toastr.success('register successfully');
        location.href = 'home.html';
    }).catch(error => {
        toastr.error(error.response.data);
    })
};
$("#login-form").validate({
    rules: {
        email: {
            required: true,
        },
        password: {
            required: true,
            minlength: 5
        },
        captcha: {
            required: true,
        }
    },
    messages: {
        password: {
            required: "Please input the password",
            minlength: "The length of password should be greater than 5"
        },
        email: {
            required: "Please input the email"
        },
        captcha: {
            required: "Please input the captcha"
        }
    },
    errorElement: "em",
    success: function (label) {
        label.text(" ")                //清空错误提示消息
            .addClass("success");    //加上自定义的success类
    },
    submitHandler: (form) => {
        login(form);
    }
});

$("#reg-form").validate({
    rules: {
        email: {
            required: true
        },
        password: {
            required: true,
            minlength: 5
        },
        checker: {
            required: true,
            equalTo: "#reg-psw"
        }
    },
    messages: {
        email: {
            required: "Please input the email"
        },
        password: {
            required: "Please input the password",
            minlength: "The length of password should be greater than 5"
        },
        checker: {
            required: "Please input the password again",
            equalTo: "Please input the same password"
        }
    },
    errorElement: "em",
    success: function (label) {
        label.text(" ")                //清空错误提示消息
            .addClass("success");    //加上自定义的success类
    },
    submitHandler: (form) => {
        register(form);
    }
});