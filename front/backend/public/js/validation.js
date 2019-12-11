//Server part
const login = (form) => {
    const username = $('.login-email').val();
    const password = $('.login-password').val();
    const data = {"username": username, "password": password};
    axios({
        url: '/users/login',
        method: 'POST',
        data: data,
        baseURL: baseURL,
    }).then(res => {
        $('.email').innerText = $('.password').innerText = '';
        // alert('login successfully');
        location.href = 'home.html';
        toastr.success('login successfully');
    }).catch(e => {
        toastr.error('password incorrect!');
    })
};

const register = (form) => {
    const username = $('.reg-email').val();
    const password = $('.reg-password').val();
    const checker = $('.reg-password-checker').val();
    const data = {"username": username, "password": password};
    if (password !== checker) {
        toastr.warning('Password not match!');

        $('.email').val('');
        $('.password').val('');
        $('.password-checker').val('');
    } else {
        axios({
            url: '/users/',
            method: 'POST',
            data: data,
            baseURL: baseURL,
        }).then(res => {
            $('.email').val('');
            $('.password').val('');
            $('.reg-password-checker').val('');
            // alert('register successfully');
            toastr.success('register successfully');
            location.href = 'home.html';
        }).catch(e => {
            toastr.error('account already exist!');
        })
    }
};
$("#login-form").validate({
    rules: {
        email: {
            required: true,
        },
        password: {
            required: true,
            minlength: 5
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