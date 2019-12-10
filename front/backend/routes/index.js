let express = require('express');
let router = express.Router();

/* GET home page. */
router.get('/', function (req, res, next) {
    res.redirect('/login.html').end();
});

//多级回调
router.get('/example/b', function (req, res, next) {
    console.log('the response will be sent by the next function ...');
    next();
}, function (req, res) {
    res.send('Hello from B!');
});

router.get('/getReq', function (req, res) {
    res.sendStatus(404);
});

module.exports = router;
