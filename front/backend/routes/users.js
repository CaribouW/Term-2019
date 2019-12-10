var express = require('express');
var router = express.Router();
//用户资源相关api

//获取所有用户
router.get('/list', (req, res) => {
    res.status(200).end();
});

//获取指定用户信息
router.get('/:id', (req, res) => {
    res.status(200).end();
});

//用户注册接口
router.post('/', (req, res) => {
    res.status(200).end();
});

//用户登录接口
router.post('/login', (req, res) => {
    const user = {
        username: 'admin',
        password: 'admin'
    };
    console.log(req.body);
    if (req.body.username === user.username &&
        req.body.password === user.password) {
        res.sendStatus(200).end();
    } else {
        //not match
        res.sendStatus(401).end();
    }
});

//用户基本信息修改接口
router.put('/:id', (req, res) => {
    res.status(200).end();
});


module.exports = router;
