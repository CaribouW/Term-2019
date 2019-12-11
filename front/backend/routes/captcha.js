const express = require('express');
const session = require('express-session');
const bodyParser = require('body-parser');
const svgCaptcha = require('svg-captcha');
const crypter = require('../utils/crypter');
const router = express.Router();

// 获取验证码
router.get('/', function getCaptcha(req, res, next) {
    const captcha = svgCaptcha.create({
        // 翻转颜色
        inverse: false,
        // 字体大小
        fontSize: 36,
        // 噪声线条数
        noise: 2,
        // 宽度
        width: 80,
        // 高度
        height: 30,
    });
    // 保存到session,忽略大小写
    req.session = captcha.text.toLowerCase();
    console.log(req.session); //0xtg 生成的验证码
    //保存到cookie 方便前端调用验证
    //信息需要预先加密
    req.session = crypter.encrypt(req.session, 'captcha');
    res.cookie('captcha', req.session);
    res.setHeader('Content-Type', 'image/svg+xml');
    res.write(String(captcha.data));
    res.end();
});

//
router.get('/send', (req, res, next) => {
    const capSec = crypter.encrypt(req.body.captcha, 'captcha');
    if (capSec === req.headers.cookie) {

    }
    res.status(200).end();
});

module.exports = router;
