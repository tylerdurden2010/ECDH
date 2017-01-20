# ECDH
环境：
Centos 6 ， openssl 1.1.0 的库

编译：
gcc GenerateECDH.c -L/usr/local/lib64 -I/usr/local/include -lcrypto
-lssl -o GenECDH
gcc CalSecret.c.c -L/usr/local/lib64 -I/usr/local/include -lcrypto
-lssl -o CalSecret

使用：

生成公私钥

Alice 那一端
./GenECDH AlicePub.pem AlicePri.pem

Bob 那一端
./GenECDH BobPub.pem BobPri.pem

计算公私钥：

Alice 那一端：

./CalSecret BobPub.pem  AlicePri.pem

Bob那一端：

./CalSecret AlicePub.pem BobPri.pem

及其粗糙的原理：

该算法和椭圆曲线没有任何关系，只是将(x,y) 对应到了椭圆曲线的公式中。

如果Alice和Bob使用ECDH通信，需要双方知道的是G（即椭圆曲线对应的有限域）和对方公钥

1、
Alice 生成 私钥a
Alice 生成公钥 A=aG

2、
Bob 生成私钥b
Bob 生成 公钥 B=bG

3、
Alice 计算共享密钥： a * B  = abG
Bob 计算共享密钥： b*A = baG

因为满足交换律 所以 abG = baG ，即为共享密钥；
