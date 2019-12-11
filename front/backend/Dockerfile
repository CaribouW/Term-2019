#node镜像版本
FROM node:8-alpine
RUN mkdir -p /home/Service
#将该文件夹作为工作目录
WORKDIR /home/Service

# 将node工程下所有文件拷贝到Image下的文件夹中
COPY . /home/Service

#使用RUN命令执行npm install安装工程依赖库
RUN npm install --registry=https://registry.npm.taobao.org
#暴露给主机的端口号
EXPOSE 3000
#执行npm start命令，启动Node工程
CMD [ "npm", "start" ]

