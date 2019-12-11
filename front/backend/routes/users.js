var express = require('express');
var router = express.Router();
var user = require('../dao/user');
var uuid = require('node-uuid');
const {encrypt, decrypt} = require('../utils/crypter')
//用户资源相关api

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
router.post('/', (req, res) => {
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
router.use('/login', (req, res, next) => {
    user.findUserByName(req.body.username).then(ans => {
        if (ans.length !== 1) {
            res.sendStatus(404).end();
        } else {
            const fetchPsw = ans[0].password;
            req.body.check = decrypt(fetchPsw, ans[0].uid);
            next();
        }
    });
}, (req, res) => {
    if (req.body.check === req.body.password) {
        res.status(200).send(
            'login successfully'
        ).end();
    } else {
        res.status(403).send('password incorrect').end();
    }
});


//用户基本信息修改接口
router.put('/:id', (req, res) => {
    res.status(200).end();
});


module.exports = router;
