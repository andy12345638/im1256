# ireader
使用C 語法 編寫
im1256 12路電量偵測模組
可同時上傳到 mysql 伺服器

im1256
http://www.ireader-opto.com/Products/im138wxqrs.html
可紀錄12路的電壓(V)、電流(A)、有功功率(W)、累計使用電量(KWH)、功率因數、頻率
透過RS485將訊號傳出，用RS485轉USB的晶片傳入樹梅派


---

# 材料
1. ireader-im1256 淘寶購買 $3600
2. RS485 TO USB 露天購買 $60
3. Rpi3 露天購買 $1600

---

# 架構
1. ireader.c   
用於讀取PZEM-004T(V1.0)  
使用方式  
`gcc ireader4.c -O3 -lmysqlclient -I/usr/include/mysql/ -o a4.out`  
`./ireader`  
有使用到mysql.h的資料編譯時都需要補上 `-lmysqlclient -I/usr/include/mysql/`  

2. plot.r  
用於將 mysql server 裡的資料讀出並畫圖。  
`Rscript plot.r`  

3. plot.php  
用於把 R 畫出來的圖變成網頁  


---

# mysql
`sudo apt-get install mysql-server mysql-client`  
  
`mysql -u root -p`  
  
`CREATE DATABASE ireader;`  
  
`USE ireader;`  
  
`CREATE TABLE powerdata (`  
`timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP, `  
`v1 DECIMAL(5,2),`  
`v2 DECIMAL(5,2),`  
`v3 DECIMAL(5,2),`  
`v4 DECIMAL(5,2),`  
`v5 DECIMAL(5,2),`  
`v6 DECIMAL(5,2),`  
`v7 DECIMAL(5,2),`  
`v8 DECIMAL(5,2),`  
`v9 DECIMAL(5,2),`  
`v10 DECIMAL(5,2),`  
`v11 DECIMAL(5,2),`  
`v12 DECIMAL(5,2),`  
`a1 DECIMAL(5,2),`  
`a2 DECIMAL(5,2),`  
`a3 DECIMAL(5,2),`  
`a4 DECIMAL(5,2),`  
`a5 DECIMAL(5,2),`  
`a6 DECIMAL(5,2),`  
`a7 DECIMAL(5,2),`  
`a8 DECIMAL(5,2),`  
`a9 DECIMAL(5,2),`  
`a10 DECIMAL(5,2),`  
`a11 DECIMAL(5,2),`  
`a12 DECIMAL(5,2),`  
`w1 SMALLINT(6),`  
`w2 SMALLINT(6),`  
`w3 SMALLINT(6),`  
`w4 SMALLINT(6),`  
`w5 SMALLINT(6),`  
`w6 SMALLINT(6),`  
`w7 SMALLINT(6),`  
`w8 SMALLINT(6),`  
`w9 SMALLINT(6),`  
`w10 SMALLINT(6),`  
`w11 SMALLINT(6),`  
`w12 SMALLINT(6),`  
`wh1 INT(11) UNSIGNED,`  
`wh2 INT(11) UNSIGNED,`  
`wh3 INT(11) UNSIGNED,`  
`wh4 INT(11) UNSIGNED,`  
`wh5 INT(11) UNSIGNED,`  
`wh6 INT(11) UNSIGNED,`  
`wh7 INT(11) UNSIGNED,`  
`wh8 INT(11) UNSIGNED,`  
`wh9 INT(11) UNSIGNED,`  
`wh10 INT(11) UNSIGNED,`  
`wh11 INT(11) UNSIGNED,`  
`wh12 INT(11) UNSIGNED,`  
`pf1 DECIMAL(3,2),`  
`pf2 DECIMAL(3,2),`  
`pf3 DECIMAL(3,2),`  
`pf4 DECIMAL(3,2),`  
`pf5 DECIMAL(3,2),`  
`pf6 DECIMAL(3,2),`  
`pf7 DECIMAL(3,2),`  
`pf8 DECIMAL(3,2),`  
`pf9 DECIMAL(3,2),`  
`pf10 DECIMAL(3,2),`  
`pf11 DECIMAL(3,2),`  
`pf12 DECIMAL(3,2),`  
`fq1 DECIMAL(4,2),`  
`fq2 DECIMAL(4,2),`  
`fq3 DECIMAL(4,2),`  
`fq4 DECIMAL(4,2),`  
`fq5 DECIMAL(4,2),`  
`fq6 DECIMAL(4,2),`  
`fq7 DECIMAL(4,2),`  
`fq8 DECIMAL(4,2),`  
`fq9 DECIMAL(4,2),`  
`fq10 DECIMAL(4,2),`  
`fq11 DECIMAL(4,2),`  
`fq12 DECIMAL(4,2),`  
`PRIMARY KEY (timestamp)`  
`);`  
  
---

`use mysql;`  
  
`INSERT INTO user(host,user,password) VALUES('%.%.%.%','rpi',password('paswd'));`  
  
`GRANT SELECT,INSERT,UPDATE,CREATE ON ireader.* TO 'rpi' IDENTIFIED BY 'paswd';`  

`FLUSH PRIVILEGES;`  
  
---  
