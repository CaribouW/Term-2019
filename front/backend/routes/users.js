const userDao = require('../dao/user');

var express = require('express');
var router = express.Router();
//用户资源相关api

//获取所有用户
router.get('/list', (req, res, next) => {
    console.log(userDao.queryAll(req, res, next));
    res.status(200);
});

//获取指定用户信息
router.get('/:id', (req, res, next) => {
    res.status(200);
});

//用户注册接口
router.post('/', (req, res, next) => {
    res.status(200);
});

//用户登录接口
router.post('/login/:id', (req, res, next) => {
    res.status(200);
});

//用户基本信息修改接口
router.put('/:id', (req, res, next) => {
    res.status(200);
});


module.exports = router;
