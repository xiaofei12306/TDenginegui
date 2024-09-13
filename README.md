# 介绍

## 版本 Tdengine v1.0.0

免费开源的TDengine时序数据库图形用户界面工具，为用户提供更专业流畅的数据库管理工具

## 主要功能

1.数据连接维护（新增、修改、查询、删除）

2.数据库维护（新增、修改、查询、删除）

3.超级表（查询）

4.普通表（查询）

5.支持分页查询，自定义字段筛选

6.更美观的自定义sql查询界面

# 软件架构

软件基于c++ qt6.0开发，更流畅的软件体验感，更低的内存消耗

# 使用说明

## 提示

软件采用原生连接，没有采用6041端口，目前只适合版本没有差异的使用

默认taos.dll版本是3.1.0

如果是别的版本请从官网下载tdengine客户端，下载后从C:\TDengine目录下把taos.dll复制到软件目录下替换

![image-20240913151835358](https://gitee.com/xiaofei12306/tdengine-cgui/raw/main/docs/image-20240913151835358.png)

## 数据库下面默认展示普通表和超级表(不含子表)

![image-20240913152211083](https://gitee.com/xiaofei12306/tdengine-cgui/raw/main/docs/image-20240913152211083.png)

## 子表查看

![image-20240913152429288](https://gitee.com/xiaofei12306/tdengine-cgui/raw/main/docs/image-20240913152429288.png)

## 查看子表数据

选中行点击右键，弹出菜单查看子表数据

![image-20240913152510839](https://gitee.com/xiaofei12306/tdengine-cgui/raw/main/docs/image-20240913152510839.png)

## 自定义筛选数据

![image-20240913152611564](https://gitee.com/xiaofei12306/tdengine-cgui/raw/main/docs/image-20240913152611564.png)

## 自定义sql查询

![image-20240913152641403](https://gitee.com/xiaofei12306/tdengine-cgui/raw/main/docs/image-20240913152641403.png)

# 未来版本

1.适配差异版本用户使用

2.支持创建表、数据库