var express = require('express');
var router = express.Router();
var http = require('http');

/* GET users listing. */
router.get('/', function (req, res, next) {
    res.send('respond with a resource');
});

var server = http.createServer(function (req, res) {
    fs.createReadStream(Path.resolve(__dirname, "." + req.url)).pipe(res);
});
module.exports = router;
