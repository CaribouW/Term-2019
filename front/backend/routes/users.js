const userDao = require('../dao/user');

var express = require('express');
var router = express.Router();

router.get('/list', (req, res, next) => {
    console.log(userDao.queryAll(req, res, next));
    res.status(201);
});

module.exports = router;
