//Connection pool
const Sequelize = require('sequelize');
const {dbName, host, port, user, password} = require('./config').mysql;

const seq = new Sequelize(dbName, user, password, {
    dialect: 'mysql',
    host,
    port,
    logging: true,
    timezone: '+08:00',
    pool: {
        max: 5,
        min: 0,
        idle: 10000
    },
});

/**
 * 定义数据模型
 *
 * @param {any} name 模型名称【数据库表名】
 * @param {any} attributes 数据字段集合
 * @returns 数据模型对象
 */
function defineModel (name, attributes) {
    var attrs = {};

    for (let key in attributes) {
        let value = attributes[key];
        if (typeof value === 'object' && value['type']) {
            value.allowNull = value.allowNull || false;
            attrs[key] = value;
        } else {
            attrs[key] = {
                type: value,
                allowNull: false
            };
        }
    }

    // 附加公共字段
    // attrs.id = {
    //     type: ID_TYPE,
    //     primaryKey: true
    // };
    // 调用seq的方法定义模型并返回
    return seq.define(name, attrs, {
        tableName: name,
        timestamps: false,
    });
}

module.exports = {defineModel};