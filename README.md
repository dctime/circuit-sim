# 一個只用 eigen 函式庫 與 SFML2 的 電路模擬器
## 試用AI的強大和智慧 有問題就問它
## 怎跑?
```
git clone 這東西
./run.sh
./execute.sh
報錯? 裝module (如果用 ubuntu sudo apt install windows 的話 慢慢改 cmake 吧拉資料夾 lib 耶)
```
### 如何使用這個模擬器
![image](https://github.com/user-attachments/assets/f07d33e3-190c-40be-a3eb-e17e9422aae8)

按鈕未完成 UI 部份只須知道網格交叉點為元件可能的中心位置與接點的位置 FPS 為 這模擬器目前動畫的禎數 t 為目前模擬的秒數

按住r並且對網格點左鍵可以放置電阻 目前電阻不可在UI界面更改需要到main.cpp裡修改 所有元件都不可旋轉 也不可刪除

![image](https://github.com/user-attachments/assets/5e2d5bf5-45f3-49d3-bafc-248e0f5445ac)

按住g並且對網格點左鍵可以放置ground

![image](https://github.com/user-attachments/assets/b91fb4f8-d816-4873-8e73-05c81fe3724f)

按住w並且按住一個網格 拖拽後放開可以畫一條電線

![image](https://github.com/user-attachments/assets/8da1559a-809f-476b-bd04-6b161513d852)

按住v並且對網格點左鍵可以放置電壓原

![image](https://github.com/user-attachments/assets/80c9f25b-2a0d-43fc-a253-96b6b103554c)

確保每一個元件 每個節點都有一條路徑可以到地 不能有浮動節點

按住s後對視窗點左鍵即可開始模擬

![image](https://github.com/user-attachments/assets/272ec462-0185-42cc-a408-e6ca58057b22)

如果UI的電流太快可以去改 UICircuit.h 的 currentScale

這模擬器支援 電阻(r) 地(g) 電線(w + drag) NMOS(n) PMOS(p) 二極體(d) 電壓源(v) 可變電壓原(a) (sin, cos, step等等自定義v(t)) 電流原(i) 電容(c) 電感(SHIFT + I)

計算與渲染是兩個不同的thread 有個 buffer 儲存計算好的電路 如果計算根不上渲染可以從UI t值 的變化減慢做判斷

terminal 顯示 目前計算所使用的 iteration 數量 預設最大是 1000 如果terminal 顯示一堆1000 代表這電路難以收斂計算出來的不準確

![image](https://github.com/user-attachments/assets/6ffed365-c2f2-41a8-b212-bc554a3eda39)

線性電路準度高但是非線性電路準度不可作為商業用途

terminal 可能會顯示 eigen 的錯誤訊息 與計算出來的結果 (很多 inf) 微分出問題

非商業用途 單純業餘作品

## 歡迎爆改code (發 pull request 有功能就加)
http://dev.hypertriton.com/edacious/trunk/doc/lec.pdf 主要"參考"論文

https://acrobat.adobe.com/id/urn:aaid:sc:AP:9c98ed59-76ea-454a-821b-281e4a7006d9?viewer%21megaVerb=group-discover 
主模擬用到的數學 Circuit.h Circuit Class and iterate method (許多AI給的點子)

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

