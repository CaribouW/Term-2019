//cookie字符串转换为dic
const parseCookie = (cookieStr) => {
    const Cookies = {};
    cookieStr && cookieStr.split(';').forEach(function (Cookie) {
        const parts = Cookie.split('=');
        Cookies[parts[0].trim()] = (parts[1] || '').trim();
    });
    console.log(Cookies);
    return Cookies;
};
module.exports = {parseCookie};