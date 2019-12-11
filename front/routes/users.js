const express = require('express');
const router = express.Router();
const user = require('../dao/user');
const uuid = require('node-uuid');
const {parseCookie} = require('../utils/cookie');
const {encrypt, decrypt} = require('../utils/crypter');
//用户资源相关api
const findUserByName = (name) => {
    return user.findUserByName(name);
};

//获取所有用户
router.get('/list', async (req, res) => {
    user.findAll().then(ans => {
        res.status(200).send(ans);
    });
});

//获取指定用户信息
router.get('/:id', (req, res) => {
    res.status(200).end();
});

//用户注册接口
router.post('/', (req, res, next) => {
    //如果用户已存在
    findUserByName(req.body.username).then(ans => {
        if (ans.length > 0) {
            res.status(405).end('user already exist');
        } else {
            next();
        }
    });
}, (req, res) => {
    const uid = uuid.v1();
    const psw = encrypt(req.body.password, uid);        //let uid to be secret
    user.insertUser({
        uid: uid,
        username: req.body.username,
        password: psw
    }).then(ans => {
        return {
            uid: ans.uid,
            email: ans.username,
        }
    }).then(ans => {
        res.status(200).send(ans);
    });
});

//预先的密码加密
//用户登录接口
router.post('/login', (req, res, next) => {
    user.findUserByName(req.body.username).then(ans => {
        if (ans.length === 0) {
            res.status(404).end('user not exist');
        } else {
            const cookie = parseCookie(req.headers.cookie);
            const checkPsw = decrypt(ans[0].password, ans[0].uid);
            const checkCaptcha = decrypt(cookie.captcha, 'captcha');
            if (checkCaptcha !== req.body.captcha) {
                res.status(403).send('captcha incorrect').end();
            } else if (checkPsw !== req.body.password) {
                res.status(403).send('password incorrect').end();
            }
            next();
        }
    });
}, (req, res) => {
    res.status(200).send(
        'login successfully');
});


//用户基本信息修改接口
router.put('/:id', (req, res) => {
    res.status(200).end();
});

module.exports = router;
