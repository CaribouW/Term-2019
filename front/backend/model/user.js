const db = require('../db/db');
const seq = require('sequelize');

const User = db.defineModel('User', {
    uid: seq.STRING,
    username: seq.STRING,
    password: seq.STRING
});

User.sync();
// 导出模型对象
module.exports = User;
