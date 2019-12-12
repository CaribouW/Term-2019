## Uncover系统

### 1. 本地运行说明

数据库源采用 `mysql` , 可以通过 docker 直接进行启动

在根目录下

```
docker-compose up -d
```

即可启动目标数据库 ，随后在根目录下依次执行

```
npm install
npm start
```

然后打开浏览器，输入 `localhost:8000` 即可进入相对应的前端页面。UI如下图所示：

![](https://i.loli.net/2019/12/11/m8X6kEU4Vi2ypoh.png)

当前支持的功能

- 表单验证
- 图片校验码验证与刷新
- 密码加密存储

