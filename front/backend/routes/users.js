var express = require('express');
var router = express.Router();
var http = require('http');

//这里的中间件匹配不是完全匹配，而是一种前缀匹配, 使用 < app / router >.use()
router.use('/user', (req, res, next) => {
    console.log('the url is', req.originalUrl);
    next()
}, (req, res, next) => {
    console.log('now start the user');
    next('route');
});

//这里的中间件栈, 是一个完全匹配
router.get('/user/:id', (req, res, next) => {
    console.log('Req url is', req.originalUrl);
    next();
});

router.get('/user/:id', (req, res, next) => {
    if (req.params.id === 0) next('route');
    else next();
}, (req, res) => {
    res.status(200).json({
        'names': req.params['id']
    })
});


var server = http.createServer(function (req, res) {
    fs.createReadStream(Path.resolve(__dirname, "." + req.url)).pipe(res);
});
module.exports = router;
