var user = {
    insert: 'insert into user(id, name, age) values(0,?,?);',
    update: 'update user set name=?, age=? where id=?;',
    delete: 'delete from user where id=?;',
    queryById: 'select * from user where id=?;',
    queryAll: 'select * from user;'
};

// dao/userDao.js
// 实现与MySQL交互
var mysql = require('mysql');
var $conf = require('../db/config');

// 使用连接池，提升性能
//var pool  = mysql.createPool($util.extend({}, $conf.mysql));
const pool = mysql.createPool($conf.mysql);

const jsonWrite = function (res, ret) {
    if (typeof ret === 'undefined') {
        res.json({
            code: '1',
            msg: '操作失败'
        });
    } else {
        res.json(ret);
    }
};

module.exports = {
    queryAll: function (req, res, next) {
        pool.getConnection(function (err, connection) {
            connection.query(user.queryAll, function (err, result) {
                jsonWrite(res, result);
                connection.release();
            });
        });
    }

};