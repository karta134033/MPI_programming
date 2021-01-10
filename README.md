---
tags: 平行程式設計
---

# MPI Programming
## Q1 (3 points)
>How do you control the number of MPI processes on each node? (1 points)

在mpirun 加入以下Option
```t
-N <num>
```
Launch num processes per node on all allocated nodes (synonym for npernode).
來源: https://www.open-mpi.org/doc/v4.0/man1/mpirun.1.php


>Which functions do you use for retrieving the rank of an MPI process and the total number of processes? (1 points)

```c
MPI_Comm_size(MPI_COMM_WORLD, &world_size);  // total number of processes
MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);  // retrieving the rank of an MPI process
```

>We use Open MPI for this assignment. What else MPI implementation is commonly used? What is the difference between them? (1 points)
* HP MPI implementation for the entire HP product line.
* WMPI II, WMPI 1.5 and PaTENT MPI are commercial implementations of MPI. WMPI II is a full implementation of the MPI-2 standard for 32 and 64-bit versions of Windows and Linux operating systems. (May, 2004).
* MPI-BIP is an implementation of MPI using the BIP API.

來源: https://www.mcs.anl.gov/research/projects/mpi/implementations.html

## Q2 MPI Blocking Communication & Linear Reduction Algorithm(2 points)

> Why MPI_Send and MPI_Recv are called “blocking” communication? (1 points)

因為這些function會等到**通訊完成**才回傳。
在通訊完成之前，程式不會往下繼續執行。

> Measure the performance (execution time) of the code for 2, 4, 8, 12, 16 MPI processes and plot it. (1 points)


| 執行次數| 2 processes | 4 processes | 8 processes | 12 processes| 16 processes|
| -------- | -------- | -------- | -------- | -------- | -------- |
|第一次|6.222628|3.258448|1.623846|1.091141|0.813564|
|第二次|6.375166|3.304269|1.626661|1.087909|0.816727|
|第三次|6.233847|3.246653|1.626389|1.086883|0.811508|
|第四次|6.225535|3.278451|1.644032|1.093498|0.821531|
|第五次|6.216920|3.261514|1.621194|1.087857|0.822557|
|第六次|6.232597|3.246927|1.623656|1.081595|0.815475|
|第七次|6.221347|3.257267|1.623924|1.089928|0.816642|
|第八次|6.222680|3.246467|1.620209|1.083300|0.820859|
|第九次|6.229314|3.246529|1.633885|1.087345|0.823884|
|第十次|6.238867|3.251751|1.620121|1.080960|0.829379|

![](https://i.imgur.com/VijJkBA.png)


## Q3 MPI Blocking Communication & Binary Tree Reduction Communication Algorithm(6 points)

> Measure the performance (execution time) of the code for 2, 4, 8, 16, 32 MPI processes and plot it. (1 points)


| 執行次數| 2 processes | 4 processes | 8 processes | 16 processes|
| -------- | -------- | -------- | -------- | -------- |
|第一次|6.383110|3.330043|1.659048|0.847468|
|第二次|6.412798|3.328772|1.660425|0.836124|
|第三次|6.386674|3.356846|1.671011|0.840810|
|第四次|6.392714|3.342694|1.663842|0.849458|
|第五次|6.386712|3.381254|1.683435|0.846065|
|第六次|6.387532|3.350520|1.681222|0.840655|
|第七次|6.390035|3.328853|1.657916|0.862402|
|第八次|6.387016|3.825363|1.691969|0.840834|
|第九次|6.406965|3.345638|1.659699|0.847283|
|第十次|6.401789|3.333314|1.667420|0.832717|
![](https://i.imgur.com/BDlvl7T.png)


> How does the performance of binary tree reduction compare to the performance of linear reduction? (2 points)

根據實驗結果來看，速度上看似差不多的，但仔細看會發現大部分的情況下linear reduction執行速度是優於binary tree reduction。

> Increasing the number of processes, which approach (linear/tree) is going to perform better? Why? Think about the number of messages and their costs. (3 points)

分別來比較看看兩個算法在"傳輸"上的次數
(紅線代表 Send + Receive 的傳輸)
* linear
    ![](https://i.imgur.com/KVieQtc.png)

* binary tree
    ![](https://i.imgur.com/nbrUCuw.png)
    
兩張圖的紅線個數都是"Processes - 1"的個數，因為binary tree reduction的傳輸次數是 
$2^0 + 2^1 + .... 2^{(log_2 Processes) - 1} = Processes - 1$與linear reduction的傳輸次數相等。
演算法的實做上binary tree reduction需要 遞迴 或是 bottom up 的方式去分配各個Process對應到的傳送與接收，相對於linear reduction的方式需要多一道手續，理論上是會比較耗時的。
由執行時間的結果來看確實會有影響(執行速度大於linear reduction)。

### --推測--
當Process數量夠龐大時，binary tree reduction與linear reduction的執行時間差異會更加顯著，因為Process數量會影響binary tree reduction的遞迴或bottom up的執行時間。

### --實驗--
對binary tree reduction與linear reduction做交叉測試，
測試方法為: 
* 第一回合:測試binary tree reduction後再測試linear reduction
* 第二回合:測試binary tree reduction後再測試linear reduction
* ...
* 第N回合:測試binary tree reduction後再測試linear reduction

觀察哪個方法會在該回合勝出並記錄下來。
為了驗證推測的想法，實驗以"16"個Process做測試


 執行次數| binary tree reduction| linear reduction| winner |
| -------- | -------- | -------- |-------- |
| 第一回合 | 0.848134 | 0.811449 |linear reduction|
| 第二回合 | 0.839108 | 0.816715 |linear reduction|
| 第三回合 | 0.840159 | 0.824015 |linear reduction|
| 第四回合 | 0.858253 | 0.821956 |linear reduction|
| 第五回合 | 0.849453 | 0.815526 |linear reduction|
| 第六回合 | 0.836354 | 0.817536 |linear reduction|
| 第七回合 | 0.849248 | 0.829014 |linear reduction|
| 第八回合 | 0.839074 | 0.820755 |linear reduction|
| 第九回合 | 0.849211 | 0.824378 |linear reduction|
| 第十回合 | 0.863041 | 0.827162 |linear reduction|


由實驗結果可以觀察到，linear reduction的執行時間皆是比binary tree reduction還快，binary tree reduction的執行速度確實受到遞迴或bottom up的影響。

### --結論--
在傳輸數量相等的情況下，更改成binary tree reduction的形式並沒有產生加速。

## Q4 MPI Non-Blocking Communication & Linear Reduction Algorithm(5 points)
>Measure the performance (execution time) of the code for 2, 4, 8, 12, 16 MPI processes and plot it. (1 points)

| 執行次數| 2 processes | 4 processes | 8 processes | 12 processes| 16 processes|
| -------- | -------- | -------- | -------- | -------- | -------- |
|第一次|6.288943|3.252174|1.653662|1.090103|0.820807|
|第二次|6.288198|3.286300|1.644987|1.105617|0.824977|
|第三次|6.329171|3.250971|1.659410|1.105851|0.825756|
|第四次|6.300694|3.265915|1.655671|1.096556|0.820877|
|第五次|6.297765|3.305555|1.651724|1.093906|0.832667|
|第六次|6.296913|3.282880|1.650262|1.082119|0.816960|
|第七次|6.300789|3.282007|1.636786|1.100730|0.822147|
|第八次|6.287053|3.293441|1.642541|1.096798|0.822052|
|第九次|6.299614|3.276060|1.627048|1.091189|0.829576|
|第十次|6.299393|3.284514|1.637760|1.095370|0.829212|
![](https://i.imgur.com/jqkKhDz.png)

>What are the MPI functions for non-blocking communication? (1 points)

執行完non-blocking的function不必等待回傳即可執行接下來的程式碼，可以將原本需要等待訊息拷貝到system buffer或是從system buffer拷貝訊息的時間節省下來，再利用這節省下來的時間做計算，藉此減少整個程式的總執行時間。

>How the performance of non-blocking communication compares to the performance of blocking communication? (3 points)

* Sender
    我們在計算pi的耗時是在隨機數的產生，在計算pi的結果前一定得需要利用隨機數的產生來知道到底有多少的"投擲數"，造成投擲數一定得在MPI_Isend()之前計算完成才能傳送，因此在Sender端無法利用到non-blocking的優勢(因為MPI_Isend()做完就沒事可以做了，浪費掉節省下來的時間)。
    
* Receiver
    可以在MPI_Irecv()節省的時間去做計算，最主要計算耗時部分依然在隨機數的產生並計算投擲數，因此可以把此計算放在MPI_Irecv()與MPI_Waitall()之間，理論上執行的總時間會下降，但觀測之前的執行時間可以發現，與block_linear的差距並不大，且能執行到的最優時間大部分是由block_linear囊括。
    
### --推測--
蒙地卡羅方法計算pi的應用中non-blocking 的計算時間與blocking差異不大。

### --實驗--
跟之前的實驗作法一樣，以交叉測試的方式比較non-block_linear與block_linear的差異。

 執行次數| non-block_linear | block_linear | winner |
| -------- | -------- | -------- |-------- |
| 第一回合 | 0.829059 | 0.821459 |block |
| 第二回合 | 0.827706 | 0.824499 |block |
| 第三回合 | 0.822493 | 0.826243 |non-block |
| 第四回合 | 0.820953 | 0.816093 |block |
| 第五回合 | 0.828087 | 0.819204 |block |
| 第六回合 | 0.839368 | 0.826518 |block |
| 第七回合 | 0.816040 | 0.814587 |block |
| 第八回合 | 0.828909 | 0.820001 |block |
| 第九回合 | 0.827066 | 0.816145 |block |
| 第十回合 | 0.827172 | 0.818487 |block |


由實驗結果可以得知，non-block_linear與block_linear差距不大，但大致上block_linear的速度優於non-block_linear。

### --結論--
non-blocking的優勢的確存在，但需要根據應用的需求來做取捨，sender傳送後或Receiver接收前若有大量閒置時間可以挪做應用則能放大它的優勢，以此作業的應用來說，能夠得到的優勢有限。

## Q5 MPI Collective: MPI_Gather(1 points)
>Measure the performance (execution time) of the code for 2, 4, 8, 12, 16 MPI processes and plot it. (1 points)

| 執行次數| 2 processes | 4 processes | 8 processes | 12 processes| 16 processes|
| -------- | -------- | -------- | -------- | -------- | -------- |
|第一次|6.178350|3.245216|1.645818|1.109531|0.837313|
|第二次|6.188486|3.256749|1.664749|1.129469|0.833729|
|第三次|6.198797|3.243519|1.644100|1.117390|0.831283|
|第四次|6.197042|3.251223|1.645701|1.119659|0.843971|
|第五次|6.175262|3.222052|1.640643|1.118594|0.849956|
|第六次|6.171730|3.258035|1.632914|1.108996|0.839391|
|第七次|6.177070|3.240237|1.666762|1.104416|0.830972|
|第八次|6.185465|3.231172|1.639484|1.106466|0.839815|
|第九次|6.174514|3.247507|1.640669|1.119828|0.846441|
|第十次|6.190247|3.226133|1.660900|1.164238|0.827807|
![](https://i.imgur.com/pHX9nap.png)



## Q6 MPI Collective: MPI_Reduce(1 points)
> Measure the performance (execution time) of the code for 2, 4, 8, 12, 16 MPI processes and plot it. (1 points)

| 執行次數| 2 processes | 4 processes | 8 processes | 12 processes| 16 processes|
| -------- | -------- | -------- | -------- | -------- | -------- |
|第一次|6.523341|3.475318|1.705478|1.160775|0.861392|
|第二次|6.530686|3.501203|1.712756|1.174153|0.860969|
|第三次|6.518648|3.448250|1.728486|1.142506|0.877411|
|第四次|6.542977|3.418788|1.729991|1.158167|0.874623|
|第五次|6.525786|3.425553|1.723256|1.150544|0.861214|
|第六次|6.527951|3.473632|1.714713|1.166732|0.872226|
|第七次|6.532479|3.458567|1.724982|1.179247|0.867682|
|第八次|6.554199|3.412340|1.707461|1.147920|0.861806|
|第九次|6.545611|3.416386|1.719805|1.149390|0.879354|
|第十次|6.557933|3.458989|1.706763|1.149922|0.868944|
![](https://i.imgur.com/HGEnknM.png)


## Q7 MPI Windows and One-Sided Communication & Linear Reduction Algorithm(10 points)

>Measure the performance (execution time) of the code for 2, 4, 8, 12, 16 MPI processes and plot it. (1 points)

| 執行次數| 2 processes | 4 processes | 8 processes | 12 processes| 16 processes|
| -------- | -------- | -------- | -------- | -------- | -------- |
|第一次|6.599396|3.488961|1.782240|1.209667|0.916551|
|第二次|6.599631|3.447976|1.783086|1.189953|0.906083|
|第三次|6.606558|3.465806|1.785468|1.189376|0.902062|
|第四次|6.608931|3.474297|1.792435|1.199922|0.911515|
|第五次|6.600111|3.459822|1.774521|1.193032|0.899961|
|第六次|6.631013|3.475611|1.790236|1.206099|0.907665|
|第七次|6.605497|3.467315|1.763902|1.185203|0.918799|
|第八次|6.612874|3.507694|1.789531|1.203681|0.897148|
|第九次|6.593390|3.512142|1.780475|1.195639|0.890708|
|第十次|6.588922|3.486846|1.776043|1.199411|0.899455|
![](https://i.imgur.com/edaHXiP.png)

>Which approach gives the best performance among the 1.2.1-1.2.6 
cases? What is the reason for that? (3 points)

![](https://i.imgur.com/AQwelzU.png)
2 processes中 "**gather**" 表現較好

![](https://i.imgur.com/nbzUwQw.png)
4 processes中 "**gather**" 表現最好

![](https://i.imgur.com/O2WrURb.png)
8 processes中 "**blocklinear**" 表現最好

![](https://i.imgur.com/LMoq1fB.png)
12 processes中 "**block_linear**" 表現較好

![](https://i.imgur.com/Rs1q8Zf.png)
16 processes中 "**block_linear**" 表現最好

### --Best Performance--
綜合以上的結果"**block_linear**"的表現是比較好的。

### --Reason--
作業的實作主要在分配計算投擲數給各個processes，而主process一定得等大家都計算完投擲數後才能做pi的計算，在這種架構下能夠做的變化就比較侷限，其中block_linear, nonblock_linear的做法比較符合這種**多點發送單點接收**的架構方式，表現上也比較優異。

<br>

>Which algorithm or algorithms do MPI implementations use for reduction operations? You can research this on the WEB focusing on one MPI implementation. (1 points)

在[這篇論文](https://www.researchgate.net/publication/298286563_An_Efficient_MPI_Reduce_Algorithm_for_OpenFlow-Enabled_Network) 的RELATED WORK有提到，有些reduction演算法在communicator size比較小的時候比較有效率
>*Many reduction algorithms are used in MPICH [4] and
Open MPI [5]. Some algorithms are efficient when the
communicator size is small. Many algorithms are designed to
work well when communication time is bandwidth or latency
dominant. The following is a brief discussion of a few
standard reduction algorithms in MPICH and Open MPI [6, 7].*

根據不同的communicator size大小，reduction operations會有不同種的實作方式，其中包含:
>*1 Linear: All processes send data directly to root process
and apply reduction operator for p-1 times (p is the number of
processes). Open MPI use this algorithm when communicator
size is less than 8 and message size less than 512 bytes.
 2 Binomial Tree: First step, half of p processes send the
data to another half. Operate reduction in receiving processes.
Then divide receiving processes into two halves and send
again. Repeat these steps until one process remaining. This
algorithm is used in both MPICH and Open MPI to reduce
small size message. It operates just in log p number of steps.
 3 Binary Tree: Assume that there are $2^n-1$ processes,assign $2^{n-1}$ processes to be sending processes and assign $2^{n-2}$ processes to be receiving processes. For each two sending
processes, send the message to a receiving process and reduce.
Repeat these steps until one process remains.*

比較特殊的方法為***Rabenseifner’s Algorithm or Butterfly Algorithm***。
>*4 Rabenseifner’s Algorithm or Butterfly Algorithm: 
This algorithm is the combination of reduction-scatter and gathers operation designed to decrease the communication bandwidth.MPICH selected this algorithm when the message larger than 2 KB*

## Q8 (9 points)

>Plot ping-pong time in function of the message size for cases 1 and 2, respectively. (2 points)
* ### case 1
    <div style="display: flex;">
        <img src="https://i.imgur.com/FEssu0s.png">
        <img src="https://i.imgur.com/VRI27hg.png" height="50%">
    </div>
* ### case 2
    <div style="display: flex;">
        <img src="https://i.imgur.com/EQsudUQ.png">
        <img src="https://i.imgur.com/SWXilgo.png" height="50%">
    </div>
 
>Calculate the bandwidth and latency for cases 1 and 2, respectively. (3 points)

T(n) = s + rn
latency = s
bandwidth = 1/r
* ### case1  
    T(n) = 1.47467417e-10 n + 0.000045088058925304997
    latency = 0.045088058925305 ms
    bandwidth = 1/1.47467417e-10/1E9 = 6.781158986462751 GB/s
* ### case2
    以下為pp6與pp8的測試結果
    T(n) = 8.571e-09 n + 0.00016816105215378612
    latency = 0.16816105215378613 ms
    bandwidth = 1/8.571e-09/1E9 = 0.11667250029168126 GB/s
>For case 2, how do the obtained values of bandwidth and latency compare to the nominal network bandwidth and latency of the NCTU-PP workstations. What are the differences and what could be the explanation of the differences if any? (4 points)

* ### Bandwidth
    由ethtool指令檢查工作站的網卡
    ![](https://i.imgur.com/FeaZQRs.png)
    可以發現網卡的最大速率為1000Mb/s，而在case2測量到的頻寬為
    0.11667250029168126 GB/s = 933.3800023334501Mb/s
    接近工作站可傳輸的最大速率。
    其中，無法達到最佳速率的原因可能為
    1. 實體線路或硬體設備可達的極速不及網卡的極速造成瓶頸
        *資料來源: https://www.howtogeek.com/341538/why-is-my-internet-so-slow/*
    3. 工作站在多個使用者同時使用下使系統負載上升、傳輸效率降低
        *資料來源: https://smallbusiness.chron.com/can-affect-bandwidth-68780.html*

    ### --實驗--
    將原本的hosts pp6與pp8改為較多人使用的pp1 pp2，並在系統負載高的情況下去觀察頻寬是否會跟著下降。
    
    > [name=Liu An Chi]
    > Good try!
    
    ### --結果--
    ![](https://i.imgur.com/bsBu807.png)
    T(n) = 9.028e-09 n + 0.02434
    bandwidth = 1/9.028e-09/1E9 = 
    0.11076650420912716 GB/s = 886.1320336730173Mb/s
    根據實驗結果可得知系統負載(多個使用者同時使用)是造成頻寬下降的主因。
    
* ### latency
    可以直接用ping的方式觀察封包延遲的時間。
    *資料來源: https://maxrohde.com/2018/01/05/test-latency-between-two-servers-linux/*
    ### --實驗--
    為了模擬當初程式執行的環境，以ssh到pp6去ping pp8的ip位置做觀測。
    
    ### --結果--
    ![](https://i.imgur.com/16LJ57T.png)    
    測試結果平均(avg)的latency為0.202，與之前算出的結果0.16816105215378613 ms有約0.05~0.06ms左右的差距，但最小的(min)的latency為0.168與之前算出來的數值接近，推測當時的程式執行環境是在延遲比較小的時間傳送的。


## Q9 (5 points)

> Describe what approach(es) were used in your MPI matrix multiplication for each data set.

### --矩陣相乘--
1. 原本的矩陣相乘做法
    ![](https://i.imgur.com/wxCSRF2.png)
    * a00~a03 與 b00 b03 b06 b09 組合成c00的結果
    * a00~a03 與 b01 b04 b07 b10 組合成c01的結果
    * a00~a03 與 b02 b05 b08 b11 組合成c02的結果
    * ... 以此類推
    
    但這個做法有個問題，因為B矩陣在這種作法下**記憶體訪問不連續**。這會導致cache命中率不高，必須頻繁地從memory取得資料。所以為了加速就要盡可能使A, B, C矩陣在取資料時是根據記憶的的順序來訪問，取代原本跳躍式的訪問方式。
    
2. 改良後的矩陣相乘做法
    ![](https://i.imgur.com/EXxKMZ1.png)
    * a00 與 b00 b01 b02 依序組合成 c00_1 c01_1 c02_1 的結果
    * a01 與 b03 b04 b05 依序組合成 c00_2 c01_2 c02_2 的結果
    * a02 與 b06 b07 b08 依序組合成 c00_3 c01_3 c02_3 的結果
    * a03 與 b09 b10 b11 依序組合成 c00_4 c01_4 c02_4 的結果
    * ... 以此類推
    
    根據以上的圖片可以得知此作法在訪問A, B, C矩陣都是連續的，唯獨C矩陣需要在同一個row訪問四次。實測確實比原本的版本快上許多(2倍以上的加速)。
    
### --平行化--
1. main process先將A, B, C矩陣與矩陣大小的資訊傳送給各個process
2. 各個process根據自己的rank與process的數目做矩陣運算，假設process數目為4，則:
    * rank0 做A矩陣第0, 4, 8, 12...row 搭配B矩陣的運算，算出C矩陣第0, 4, 8, 12...row的結果 
    * rank1 做A矩陣第1, 5, 9, 13...row 搭配B矩陣的運算，算出C矩陣第1, 5, 9, 13...row的結果
    * rank2 做A矩陣第2, 6, 10, 14...row 搭配B矩陣的運算，算出C矩陣第2, 6, 10, 14...row的結果
    * rank3 做A矩陣第3, 7, 11, 15...row 搭配B矩陣的運算，算出C矩陣第3, 7, 11, 15...row的結果

3. 最後再將各個rank算出的C矩陣結果傳回main process，main process再將結果印出。

### --Hybrid--
MPI在資料傳輸上需要一段時間(根據矩陣大小而定)，當資料量小的時候，傳輸的時間會比單一個process去做矩陣相乘還慢，因此在
* 矩陣的n * m * l < 2900000000 (經實驗後得出)
    會將矩陣相乘的工作全權交給main process做
* 矩陣的n * m * l >= 2900000000
    會將矩陣相乘的工作分給多個process做