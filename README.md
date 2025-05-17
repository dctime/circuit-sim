# sources:
## 怎跑?
```
git clone 這東西
./run.sh
./execute.sh
報錯? 裝module (如果用 ubuntu sudo apt install windows 的話 慢慢改 cmake 吧拉資料夾 lib 耶)
```
## 歡迎爆改code (發 pull request 有功能就加)
http://dev.hypertriton.com/edacious/trunk/doc/lec.pdf 主要"參考"論文

https://acrobat.adobe.com/id/urn:aaid:sc:AP:9c98ed59-76ea-454a-821b-281e4a7006d9?viewer%21megaVerb=group-discover 
主模擬用到的數學 Circuit.h Circuit Class and iterate method

這 method 後面有一部分是將用jacobian得到的 deltaV 做一些處理 使其不會讓模擬震盪或是卡住 (非正規的方法需要改進)

https://aice.sjtu.edu.cn/eLearning/Lecture/Lecture5/Lecture5.html MNA 論文沒說到但很重要
![image](https://github.com/user-attachments/assets/d4fd5df3-9256-4567-a6d0-4b3ea16f15cd)

## while 在哪裡???? || 為何我的程式卡住了?
https://youtube.com/playlist?list=PLvv0ScY6vfd_ocTP2ZLicgqKnvq50OCXM&si=W4fUKjixYTU_Af90

https://www.geeksforgeeks.org/std-mutex-in-cpp/ Mutex 防止 thread 衝在一起

### 現在不想寫 doc 和註解 也真的沒時間寫 呵呵

#### 可以幫忙加的功能:

第一規則:
電流源錯誤問題
![image](https://github.com/user-attachments/assets/35755f98-f009-4667-b0c1-2d07c45d8c47)

第二個規則: 任一個迴圈不能全部都是電壓原 （地與地是連起來的）

第三個規則: 每個節點一定要有路徑可以到地 (那些輸入電流為0的節點沒跟同元件其他節點連在一起 像是 ideal op amp 的 ＋ -  與 mos 的 ground) (這好像加了)

元件旋轉, UI, 好玩的關卡(https://www.youtube.com/watch?v=NS3C8i8kfWY), 極度非線性電路優化(cascode mos, CMOS differential amp等等)

