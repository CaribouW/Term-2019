import axios from 'axios'

const login = () => {
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
        toastr.success('login successfully');
        location.href = 'home.html';
    }).catch(e => {
        toastr.error('password incorrect!');
    })
};