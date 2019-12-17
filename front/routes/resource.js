let express = require('express');
let fs = require('fs');
let router = express.Router();

/* GET home page. */
router.get('/:path', function (req, res) {
    const path = 'resources/' + req.params.path;
    res.set('content-type', 'image/jpeg');//设置返回类型
    let stream = fs.createReadStream(path);
    let responseData = [];//存储文件流
    if (stream) {//判断状态
        stream.on('data', function (chunk) {
            responseData.push(chunk);
        });
        stream.on('end', function () {
            let finalData = Buffer.concat(responseData);
            res.write(finalData);
            res.end();
        });
    }
});

module.exports = router;