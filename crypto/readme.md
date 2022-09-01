##  加密库

-	`aes_cbc_256.c`：基于openssl-API实现的aes-cbc-256加密函数


编译方法：
```bash
gcc -o main aes_cbc_256.c aes.h  -lssl -lcrypto
```
