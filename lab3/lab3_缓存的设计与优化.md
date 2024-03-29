# 实验三 缓存的设计与优化

## 实验目的

1.	了解gem5存储结构和替换策略的实现，学习创建SimObject。
2.	练习缓存设计。

## 实验要求

### 准备工作

实验三修改了gem5源码，调整gem5的功能。本次Cache实验将实现一个SimObject，以此为例说明如何在gem5上实现一个自定义的新组件。SimObject对象是gem5中所有对象的基本类。我们将使用`se.py`运行测试。

快速浏览learning gem5文档第二部分中的一个小节[gem5: Creating a very simple SimObject](https://www.gem5.org/documentation/learning_gem5/part2/helloobject/)，理解如何创建SimObject对象并加入编译。

### 实现NMRU策略

gem5中Cache替换策略也是一个SimObject对象。为了实现新策略，参考`/src/mem/cache/replacement_policies/`中已经实现的策略。

1.  仿照LRU策略，复制一份源文件，把其中任何LRU前缀重命名为 NMRU(区分大小写)。

    ```
    cp lru_rp.cc nmru_rp.cc
    cp lru_rp.hh nmru_rp.hh
    ```

2.  在python中使用C++类

    SimObject底层是C++类。为了在python中使用这些类， 需要创建python对象，和C++类联系起来。对于NMRU替换策略，我们首先继承`BaseReplacementPolicy`类，然后设置类的名称和C++文件路径（该文件定义了底层C++类）。

    <br>

    编辑`/src/mem/cache/replacement_policies`路径下的`ReplacementPolicies.py`，添加：

    ```python
    class NMRURP(BaseReplacementPolicy):
        type = 'NMRURP'
        cxx_class = 'gem5::replacement_policy::NMRU'
        cxx_header = "mem/cache/replacement_policies/nmru_rp.hh"
    ```

3.  注册C++文件

    gem5 使用scons协助编译。每个 SimObject 的对应C++文件都必须向 scons 注册，以便在编译时创建 Params 对象和 Python 包装器。

    <br>

    修改 SimObject 所在目录中的 SConscipt 文件。对于每个 SimObject，用 source 添加源文件。在本例中，将以下语句添加到 `/src/mem/cache/replacement_policies/SConscript`，并在Sim_object中添加相关项：

    ```C++
    Source('nmru_rp.cc')
    ```

    此时，你应该可以编译代码。

4.  修改源文件代码实现NMRU（MRU的反面，**从最近没有使用的块中随机选择一个进行替换**）。*Tips：建议理解getVictim函数，可参考random策略的实现，仅几行代码*。

5.  在`se.py`中配置替换策略

    你可以手动在合适的位置改变replacement_policy。调用方法：
    
    ```python
    dcache_class( ... , replacement_policy=NMRURP())
    dcache_class.replacement_policy = NMRURP()
    ```

    你也可以修改`se.py`或相关文件使得替换策略作为一个命令行参数传入，这样更加易于模拟。

### 设计基于乱序 O3CPU 处理器的 cache

只考虑dcache，使用实验二的测试程序（推荐`mm.c`）。

1.  探究cache相联度、替换策略对访存的影响。替换策略至少要模拟Random、NMRU、LRU、LIP（BIP的特例，gem5已经实现）。cache大小适量调小，便于分析。其他参数使用默认值。

    描述你模拟的所有配置和结果，分析变量的影响。给出性能最佳的配置，分析性能提升原因。

    <br>

2.  考虑一个实际情况，O3CPU 2.2GHz，issuewidth = 8。策略限制如下：

    |             | Random | NMRU | LIP |
    | ----------- | ------ | ---- | --- |
    |  Max assoc. |   16   |  8   |  8  |
    | Lookup time |  100ps | 500ps|555ps|

    *注意lookup time（ps）和tag_latency（cycles，取整）的转化。*

    哪一种替换策略更优？说明理由。

## 提交

1.  实验报告。内容包括NMRU的实现方法和修改配置的方法；关键代码；模拟结果展示；分析；实验过程中的问题（可选）。
2.  提交文件。所有经过修改的代码文件（可选，直接在报告中展示关键代码），模拟结果的配置和统计文件。

## 评分标准

1.  完成实验要求，5分。
2.  实验报告占5分。
