const User = require('../model/user');
const crypto = require('crypto');

const insertUser = (user) => {
    //create the user model
    return User.create({
        uid: user.uid,
        username: user.username,
        password: user.password
    }).then(res => {
        return JSON.parse(JSON.stringify(res))
    });
};

const updateUser = (user) => {
    return User.update({
        id: user.id,
        uid: user.uid,
        username: user.username,
        password: user.password
    })
};

const findUserByName = (name) => {
    return User.findAll({
        where: {
            username: name,
        }
    }).then(res => {
        return JSON.parse(JSON.stringify(res));
    });
};

const findAll = () => {
    return User.findAll().then(ans => {
        return JSON.parse(JSON.stringify(ans));
    });
};

module.exports = {insertUser, updateUser, findAll,findUserByName};