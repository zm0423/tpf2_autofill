<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="zh_CN">
<context>
    <name>MarkdownLanguageManager</name>
    <message>
        <location filename="src/MarkdownLanguageManager.cpp" line="110"/>
        <source># Error
Cannot load document.</source>
        <translation># Error
Cannot load document.</translation>
    </message>
</context>
<context>
    <name>QObject</name>
    <message>
        <source>说明：

游戏底层代码仅识别编号，所以需要进入游戏获取id

先打开游戏，进入设置-高级，打开调试模式，并重启游戏，然后进入存档
按`打开控制台，也可在设置里自行设置按键
复制代码，在控制台里粘贴并回车，可以看到出来了很多数据
复制所有uil上面一直到你的代码那一行的数据，应该符合一行名字一行空格、一行数字一行空格的格式
名字有乱码是正常的
复制好了文本，粘贴进界面左侧，选择站点导入或者线路导入
线路允许进行截断，具体说明看截断选项

注意：同步时刻表时所有的数据要和这里导入的相匹配，如果有不匹配请自己修改数据
不管是修改时刻表的数据还是_station、_line的数据，总之软件要正确的识别你的对应id
</source>
        <translation type="vanished">说明：

游戏底层代码仅识别编号，所以需要进入游戏获取id

先打开游戏，进入设置-高级，打开调试模式，并重启游戏，然后进入存档
按`打开控制台，也可在设置里自行设置按键
复制代码，在控制台里粘贴并回车，可以看到出来了很多数据
复制所有uil上面一直到你的代码那一行的数据，应该符合一行名字一行空格、一行数字一行空格的格式
名字有乱码是正常的
复制好了文本，粘贴进界面左侧，选择站点导入或者线路导入
线路允许进行截断，具体说明看截断选项

注意：同步时刻表时所有的数据要和这里导入的相匹配，如果有不匹配请自己修改数据
不管是修改时刻表的数据还是_station、_line的数据，总之软件要正确的识别你的对应id
</translation>
    </message>
    <message>
        <source>说明：

先选择根目录文件夹，这里是所有时刻表数据以及系统数据的存放地，
再选择存档，它一般在“C:\Program Files (x86)\Steam\user
data\XXXX\1066780\local\save\xxx.lua”，取决于steam安装位置，
如果找不到，可以进入游戏-设置-高级-打开用户数据文件夹

然后进入游戏，获取存档的站点信息和线路信息，方法见“站点、线路数据录入”内的说明，
存在站点、线路数据表示已成功导入，
请根据需要修改他们，为存档名_station(line).xlsx，尤其是覆盖选项

随后进入数据录入，接下来介绍模式：
简单模式：根据_line内的线路数据依次寻找文件
         文件命名规则：例：Z1.xlsx，Z1_2.xlsx，Z1_3.xlsx ...诸如此类，
         同一线路不同时刻表请按照这个规则命名，
         自动采用文件里的所有表单。
列表模式：程序已自动生成存档名_list.xlsx，第一行数据请随意更改
         从第二行开始，每一行一个线路，第一列为线路名称，
         第二列开始，每两列分别为文件名、表单名，
         多表单请用空格分隔，需要所有表单请空着，
         可无限往后加，也可相同线路再单开一行。

其余选项说明：
xlsx，csv：顾名思义。csv仅支持UTF8格式，请在另存为界面寻找相关编码，csv列表模式忽略表单数据

数据最后一行为无效数据：顾名思义。最后一行无效比较方便拉表。

时刻表堆叠：举例：一班车在60min内跑了一个交路三次，那么打开这个选项就会记录3组时刻表，否则是1组

仅覆盖：仅覆盖检测到的所有时刻表，比如数据里仅有一个车次的时刻表那就只覆盖这个
清空line后导入：检测所有line文件里的线路，删除这些线路的时刻表（如果存在）并导入
               适合有其他非火车线路的时刻表时，设置好line里仅保存火车线路，清空火车线路时刻表并覆盖
全部清空后导入：删除所有存在的时刻表并导入

其他注意事项：
所有时刻表格式应符合第二列为站点名称，第三、四列为到发时刻数据，其余数据并不读取，
请保证站点和线路名称匹配，
如果监测到有重复的时刻表数据，或者两组数据所有间隔小于5s，会默认合并重复的项目，并且在导入前提示，
每次导入都会为上一次做一个备份，位置为存档文件夹，
如需回档请直接修改文件名（请在文件夹界面查看-显示里打开文件名扩展），并copy回存档文件夹
导入时刻表时请关闭游戏存档

如果遇到了任何问题，请打开关于界面和我反馈，谢谢！b站私信和github均可
</source>
        <translation type="vanished">说明：

先选择根目录文件夹，这里是所有时刻表数据以及系统数据的存放地，
再选择存档，它一般在“C:\Program Files (x86)\Steam\user
data\XXXX\1066780\local\save\xxx.lua”，取决于steam安装位置，
如果找不到，可以进入游戏-设置-高级-打开用户数据文件夹

然后进入游戏，获取存档的站点信息和线路信息，方法见“站点、线路数据录入”内的说明，
存在站点、线路数据表示已成功导入，
请根据需要修改他们，为存档名_station(line).xlsx，尤其是覆盖选项

随后进入数据录入，接下来介绍模式：
简单模式：根据_line内的线路数据依次寻找文件
         文件命名规则：例：Z1.xlsx，Z1_2.xlsx，Z1_3.xlsx ...诸如此类，
         同一线路不同时刻表请按照这个规则命名，
         自动采用文件里的所有表单。
列表模式：程序已自动生成存档名_list.xlsx，第一行数据请随意更改
         从第二行开始，每一行一个线路，第一列为线路名称，
         第二列开始，每两列分别为文件名、表单名，
         多表单请用空格分隔，需要所有表单请空着，
         可无限往后加，也可相同线路再单开一行。

其余选项说明：
xlsx，csv：顾名思义。csv仅支持UTF8格式，请在另存为界面寻找相关编码，csv列表模式忽略表单数据

数据最后一行为无效数据：顾名思义。最后一行无效比较方便拉表。

时刻表堆叠：举例：一班车在60min内跑了一个交路三次，那么打开这个选项就会记录3组时刻表，否则是1组

仅覆盖：仅覆盖检测到的所有时刻表，比如数据里仅有一个车次的时刻表那就只覆盖这个
清空line后导入：检测所有line文件里的线路，删除这些线路的时刻表（如果存在）并导入
               适合有其他非火车线路的时刻表时，设置好line里仅保存火车线路，清空火车线路时刻表并覆盖
全部清空后导入：删除所有存在的时刻表并导入

其他注意事项：
所有时刻表格式应符合第二列为站点名称，第三、四列为到发时刻数据，其余数据并不读取，
请保证站点和线路名称匹配，
如果监测到有重复的时刻表数据，或者两组数据所有间隔小于5s，会默认合并重复的项目，并且在导入前提示，
每次导入都会为上一次做一个备份，位置为存档文件夹，
如需回档请直接修改文件名（请在文件夹界面查看-显示里打开文件名扩展），并copy回存档文件夹
导入时刻表时请关闭游戏存档

如果遇到了任何问题，请打开关于界面和我反馈，谢谢！b站私信和github均可
</translation>
    </message>
    <message>
        <location filename="src/mainui.cpp" line="1707"/>
        <source>作者：今天学高代了吗&lt;br/&gt;b站视频教程：&lt;a href=&quot;https://www.bilibili.com/video/BV1xxaZ6WE2E&quot;&gt;https://www.bilibili.com/video/BV1xxaZ6WE2E&lt;/a&gt; &lt;br/&gt;github：&lt;a href=&quot;https://github.com/zm0423/tpf2_autofill&quot;&gt; https://github.com/zm0423/tpf2_autofill&lt;/a&gt; &lt;br/&gt;邮箱：15800733391@163.com &lt;br/&gt;2025.12.14</source>
        <translation>作者：今天学高代了吗&lt;br/&gt;b站视频教程：&lt;a href=&quot;https://www.bilibili.com/video/BV1xxaZ6WE2E&quot;&gt;https://www.bilibili.com/video/BV1xxaZ6WE2E&lt;/a&gt; &lt;br/&gt;github：&lt;a href=&quot;https://github.com/zm0423/tpf2_autofill&quot;&gt; https://github.com/zm0423/tpf2_autofill&lt;/a&gt; &lt;br/&gt;邮箱：15800733391@163.com &lt;br/&gt;2025.12.14</translation>
    </message>
    <message>
        <location filename="src/util.cpp" line="71"/>
        <location filename="src/util.cpp" line="100"/>
        <location filename="src/util.cpp" line="141"/>
        <location filename="src/util.cpp" line="972"/>
        <location filename="src/util.cpp" line="1299"/>
        <source>错误</source>
        <translation>错误</translation>
    </message>
    <message>
        <location filename="src/util.cpp" line="71"/>
        <location filename="src/util.cpp" line="100"/>
        <source>站点或线路文件第二列含有非数字，请修改后重新打开软件</source>
        <translation>站点或线路文件第二列含有非数字，请修改后重新打开软件</translation>
    </message>
    <message>
        <location filename="src/util.cpp" line="115"/>
        <source>宋体</source>
        <translation>宋体</translation>
    </message>
    <message>
        <location filename="src/util.cpp" line="141"/>
        <source>文件存储失败，请检查文件夹权限、是否被其他应用打开、或磁盘空间</source>
        <translation>文件存储失败，请检查文件夹权限、是否被其他应用打开、或磁盘空间</translation>
    </message>
    <message>
        <source>站点或线路输入信息的某一组内的第二行含有非数字</source>
        <translation type="vanished">站点或线路输入信息的某一组内的第二行含有非数字</translation>
    </message>
    <message>
        <location filename="src/util.cpp" line="196"/>
        <location filename="src/util.cpp" line="862"/>
        <source>确定</source>
        <translation>确定</translation>
    </message>
    <message>
        <location filename="src/util.cpp" line="458"/>
        <location filename="src/util.cpp" line="1429"/>
        <source>成功</source>
        <translation>成功</translation>
    </message>
    <message>
        <location filename="src/util.cpp" line="458"/>
        <source>时刻表已成功导入，已生成备份文件 %1</source>
        <translation>时刻表已成功导入，已生成备份文件 %1</translation>
    </message>
    <message>
        <location filename="src/util.cpp" line="844"/>
        <source>确认</source>
        <translation>确认</translation>
    </message>
    <message>
        <source>时刻表表单数据如下：</source>
        <translation type="vanished">时刻表表单数据如下：</translation>
    </message>
    <message>
        <location filename="src/util.cpp" line="863"/>
        <source>取消</source>
        <translation>取消</translation>
    </message>
    <message>
        <location filename="src/util.cpp" line="890"/>
        <source>未发现线路id数据</source>
        <translation>未发现线路id数据</translation>
    </message>
    <message>
        <location filename="src/util.cpp" line="894"/>
        <source>未发现站点id数据</source>
        <translation>未发现站点id数据</translation>
    </message>
    <message>
        <location filename="src/util.cpp" line="898"/>
        <source>列表模式未发现列表</source>
        <translation>列表模式未发现列表</translation>
    </message>
    <message>
        <location filename="src/util.cpp" line="902"/>
        <source>列表模式下，线路%1不存在</source>
        <translation>列表模式下，线路%1不存在</translation>
    </message>
    <message>
        <location filename="src/util.cpp" line="906"/>
        <source>列表模式下，文件%1不存在</source>
        <translation>列表模式下，文件%1不存在</translation>
    </message>
    <message>
        <location filename="src/util.cpp" line="910"/>
        <source>列表模式下，%1表单不存在</source>
        <translation>列表模式下，%1表单不存在</translation>
    </message>
    <message>
        <location filename="src/util.cpp" line="914"/>
        <source>列表模式下无时刻表或简单模式未检测到任何时刻表</source>
        <translation>列表模式下无时刻表或简单模式未检测到任何时刻表</translation>
    </message>
    <message>
        <location filename="src/util.cpp" line="918"/>
        <source>站点%1不存在</source>
        <oldsource>站点%1对应id不存在</oldsource>
        <translation>站点%1不存在</translation>
    </message>
    <message>
        <location filename="src/util.cpp" line="922"/>
        <source>表单%1内无数据</source>
        <translation>表单%1内无数据</translation>
    </message>
    <message>
        <location filename="src/util.cpp" line="926"/>
        <source>线路%1内存在多个时刻表的站点不对应</source>
        <translation>线路%1内存在多个时刻表的站点不对应</translation>
    </message>
    <message>
        <location filename="src/util.cpp" line="930"/>
        <source>文件%1内含有无效时间数据</source>
        <translation>文件%1内含有无效时间数据</translation>
    </message>
    <message>
        <location filename="src/util.cpp" line="934"/>
        <source>文件%1内无表单</source>
        <translation>文件%1内无表单</translation>
    </message>
    <message>
        <location filename="src/util.cpp" line="938"/>
        <source>存档文件无法打开，请检查权限等</source>
        <translation>存档文件无法打开，请检查权限等</translation>
    </message>
    <message>
        <location filename="src/util.cpp" line="942"/>
        <source>存档文件无法保存，请检查权限或者是否在别的应用打开等</source>
        <translation>存档文件无法保存，请检查权限或者是否在别的应用打开等</translation>
    </message>
    <message>
        <location filename="src/util.cpp" line="946"/>
        <source>未安装时刻表mod</source>
        <translation>未安装时刻表mod</translation>
    </message>
    <message>
        <location filename="src/util.cpp" line="950"/>
        <source>时刻表内的%1站点存在重名</source>
        <translation>时刻表内的%1站点存在重名</translation>
    </message>
    <message>
        <location filename="src/util.cpp" line="954"/>
        <source>时刻表内的%1线路存在重名</source>
        <translation>时刻表内的%1线路存在重名</translation>
    </message>
    <message>
        <location filename="src/util.cpp" line="958"/>
        <source>时刻表超出所选周期范围：%1</source>
        <translation>时刻表超出所选周期范围：%1</translation>
    </message>
    <message>
        <location filename="src/util.cpp" line="962"/>
        <source>与游戏存档数据不一致：%1
可能存档已变动（增删或改动了线路、站点），或站点、线路数据未刷新。
请重新执行“站点、线路数据导入”后再试，必要时在游戏中核对线路站点</source>
        <translation>与游戏存档数据不一致：%1
可能存档已变动（增删或改动了线路、站点），或站点、线路数据未刷新。
请重新执行“站点、线路数据导入”后再试，必要时在游戏中核对线路站点</translation>
    </message>
    <message>
        <location filename="src/util.cpp" line="968"/>
        <source>其他</source>
        <translation>其他</translation>
    </message>
    <message>
        <location filename="src/util.cpp" line="1082"/>
        <source>%1 小时</source>
        <translation>%1 小时</translation>
    </message>
    <message>
        <location filename="src/util.cpp" line="1300"/>
        <source>未能读取游戏数据文件 %1
请先启动一次游戏，让桥接 mod 导出数据</source>
        <translation>未能读取游戏数据文件 %1
请先启动一次游戏，让桥接 mod 导出数据</translation>
    </message>
    <message>
        <location filename="src/util.cpp" line="1328"/>
        <source>线路%1 在存档中不存在</source>
        <translation>线路%1 在存档中不存在</translation>
    </message>
    <message>
        <location filename="src/util.cpp" line="1334"/>
        <location filename="src/util.cpp" line="1341"/>
        <source>线路%1 的站序与存档不一致</source>
        <translation>线路%1 的站序与存档不一致</translation>
    </message>
    <message>
        <location filename="src/util.cpp" line="1362"/>
        <source>出发 %1</source>
        <translation>出发 %1</translation>
    </message>
    <message>
        <location filename="src/util.cpp" line="1363"/>
        <source>到达 %1</source>
        <translation>到达 %1</translation>
    </message>
    <message>
        <location filename="src/util.cpp" line="1366"/>
        <source>线路%1 站点%2（%3，周期上限 %4）</source>
        <translation>线路%1 站点%2（%3，周期上限 %4）</translation>
    </message>
    <message>
        <location filename="src/util.cpp" line="1430"/>
        <source>时刻表数据已生成：
%1

进入游戏后打开 AutoFill 窗口，点击“将导入数据应用至时刻表”即可生效。</source>
        <oldsource>时刻表数据已生成：
%1

进入游戏后打开 AutoFill 窗口，点击“应用数据文件”即可生效。</oldsource>
        <translation>时刻表数据已生成：
%1

进入游戏后打开 AutoFill 窗口，点击“将导入数据应用至时刻表”即可生效。</translation>
    </message>
</context>
<context>
    <name>data_add</name>
    <message>
        <location filename="ui/data_add.ui" line="14"/>
        <source>Form</source>
        <translation>站点、线路数据录入</translation>
    </message>
    <message>
        <location filename="ui/data_add.ui" line="36"/>
        <source>车站数据
导入</source>
        <oldsource>游戏内获取的数据输入区域：</oldsource>
        <translation>车站数据
导入</translation>
    </message>
    <message>
        <location filename="ui/data_add.ui" line="81"/>
        <source>线路数据
导入</source>
        <oldsource>车站数据代码：</oldsource>
        <translation>线路数据
导入</translation>
    </message>
    <message>
        <source>复制</source>
        <translation type="vanished">复制</translation>
    </message>
    <message>
        <source>线路数据代码：</source>
        <translation type="vanished">线路数据代码：</translation>
    </message>
    <message>
        <location filename="ui/data_add.ui" line="49"/>
        <location filename="ui/data_add.ui" line="94"/>
        <source>覆盖（推荐）</source>
        <translation>覆盖（推荐）</translation>
    </message>
    <message>
        <location filename="ui/data_add.ui" line="56"/>
        <location filename="ui/data_add.ui" line="101"/>
        <location filename="src/data_add.cpp" line="116"/>
        <location filename="src/data_add.cpp" line="211"/>
        <source>仅添加</source>
        <translation>仅添加</translation>
    </message>
    <message>
        <location filename="ui/data_add.ui" line="112"/>
        <source>截断&quot;/&quot;以后内容</source>
        <translation>截断&quot;/&quot;以后内容</translation>
    </message>
    <message>
        <location filename="ui/data_add.ui" line="119"/>
        <source>自定义截断字符</source>
        <translation>自定义截断字符</translation>
    </message>
    <message>
        <location filename="ui/data_add.ui" line="144"/>
        <source>请安装“时刻表自动录入程序辅助工具”mod
将它应用至存档内并保存至存档。
mod可在创意工坊内找到，GitHub上也有备份
三代请安装Timetable AutoFill，请在游戏打开时录入线路和站点</source>
        <translation>请安装“时刻表自动录入程序辅助工具”mod
将它应用至存档内并保存至存档。
mod可在创意工坊内找到，GitHub上也有备份
三代请安装Timetable AutoFill，请在游戏打开时录入线路和站点</translation>
    </message>
    <message>
        <location filename="ui/data_add.ui" line="65"/>
        <location filename="src/data_add.cpp" line="337"/>
        <source>说明</source>
        <translation>说明</translation>
    </message>
    <message>
        <location filename="src/data_add.cpp" line="33"/>
        <location filename="src/data_add.cpp" line="330"/>
        <source>截断&quot;%1&quot;以后内容</source>
        <translation>截断&quot;%1&quot;以后内容</translation>
    </message>
    <message>
        <location filename="src/data_add.cpp" line="34"/>
        <source>站点、线路数据导入</source>
        <translation>站点、线路数据导入</translation>
    </message>
    <message>
        <location filename="src/data_add.cpp" line="89"/>
        <location filename="src/data_add.cpp" line="169"/>
        <source>未能读取 export.lua，请先启动一次游戏导出数据</source>
        <translation>未能读取 export.lua，请先启动一次游戏导出数据</translation>
    </message>
    <message>
        <location filename="src/data_add.cpp" line="103"/>
        <source>export.lua 内站点数据为空，请先启动一次游戏导出数据</source>
        <translation>export.lua 内站点数据为空，请先启动一次游戏导出数据</translation>
    </message>
    <message>
        <location filename="src/data_add.cpp" line="104"/>
        <source>未找到存档内站点数据，请检查辅助mod是否安装</source>
        <translation>未找到存档内站点数据，请检查辅助mod是否安装</translation>
    </message>
    <message>
        <location filename="src/data_add.cpp" line="114"/>
        <source>读取的数据如下</source>
        <translation>读取的数据如下</translation>
    </message>
    <message>
        <location filename="src/data_add.cpp" line="116"/>
        <location filename="src/data_add.cpp" line="211"/>
        <source>模式为%1，是否继续？</source>
        <translation>模式为%1，是否继续？</translation>
    </message>
    <message>
        <location filename="src/data_add.cpp" line="116"/>
        <location filename="src/data_add.cpp" line="211"/>
        <source>覆盖</source>
        <translation>覆盖</translation>
    </message>
    <message>
        <location filename="src/data_add.cpp" line="119"/>
        <location filename="src/data_add.cpp" line="214"/>
        <source>%1: %2
</source>
        <translation>%1: %2
</translation>
    </message>
    <message>
        <location filename="src/data_add.cpp" line="153"/>
        <location filename="src/data_add.cpp" line="241"/>
        <source>提示</source>
        <translation>提示</translation>
    </message>
    <message>
        <location filename="src/data_add.cpp" line="153"/>
        <source>站点数据导入成功，若需更改请查看 %1</source>
        <translation>站点数据导入成功，若需更改请查看 %1</translation>
    </message>
    <message>
        <location filename="src/data_add.cpp" line="182"/>
        <source>export.lua 内线路数据为空，请先启动一次游戏导出数据</source>
        <translation>export.lua 内线路数据为空，请先启动一次游戏导出数据</translation>
    </message>
    <message>
        <location filename="src/data_add.cpp" line="241"/>
        <source>线路数据导入成功，若需更改请查看 %1</source>
        <translation>线路数据导入成功，若需更改请查看 %1</translation>
    </message>
    <message>
        <source>站点复制代码已复制至粘贴板！</source>
        <translation type="vanished">站点复制代码已复制至粘贴板！</translation>
    </message>
    <message>
        <source>线路复制代码已复制至粘贴板！</source>
        <translation type="vanished">线路复制代码已复制至粘贴板！</translation>
    </message>
    <message>
        <location filename="src/data_add.cpp" line="89"/>
        <location filename="src/data_add.cpp" line="102"/>
        <location filename="src/data_add.cpp" line="169"/>
        <location filename="src/data_add.cpp" line="181"/>
        <location filename="src/data_add.cpp" line="280"/>
        <location filename="src/data_add.cpp" line="292"/>
        <location filename="src/data_add.cpp" line="321"/>
        <source>错误</source>
        <translation>错误</translation>
    </message>
    <message>
        <source>数据过少</source>
        <translation type="vanished">数据过少</translation>
    </message>
    <message>
        <source>站点数据导入成功，若需更改请查看 存档名_station.xlsx</source>
        <translation type="vanished">站点数据导入成功，若需更改请查看 存档名_station.xlsx</translation>
    </message>
    <message>
        <location filename="src/data_add.cpp" line="183"/>
        <source>未找到存档内线路数据，请检查辅助mod是否安装</source>
        <translation>未找到存档内线路数据，请检查辅助mod是否安装</translation>
    </message>
    <message>
        <location filename="src/data_add.cpp" line="209"/>
        <source>读取的（截断后）数据如下</source>
        <translation>读取的（截断后）数据如下</translation>
    </message>
    <message>
        <source>线路数据导入成功，若需更改请查看 存档名_line.xlsx</source>
        <translation type="vanished">线路数据导入成功，若需更改请查看 存档名_line.xlsx</translation>
    </message>
    <message>
        <location filename="src/data_add.cpp" line="256"/>
        <source>截断</source>
        <translation>截断</translation>
    </message>
    <message>
        <location filename="src/data_add.cpp" line="257"/>
        <source>输入截断字符 默认/ 
例如&quot;G1/2 上海-北京&quot;会被截断成&quot;G1&quot;，用于时刻表匹配 
被截断的线路在列表里会排序在相对上侧的位置

请输入一个ASCII字符（允许空格）：
• 可见字符: A-Z, a-z, 0-9, !@#$%^&amp;*()等
• 允许空格
• 不允许其他控制字符（Tab、换行等）</source>
        <translation>输入截断字符 默认/ 
例如&quot;G1/2 上海-北京&quot;会被截断成&quot;G1&quot;，用于时刻表匹配 
被截断的线路在列表里会排序在相对上侧的位置

请输入一个ASCII字符（允许空格）：
• 可见字符: A-Z, a-z, 0-9, !@#$%^&amp;*()等
• 允许空格
• 不允许其他控制字符（Tab、换行等）</translation>
    </message>
    <message>
        <location filename="src/data_add.cpp" line="265"/>
        <source>确认</source>
        <translation>确认</translation>
    </message>
    <message>
        <location filename="src/data_add.cpp" line="266"/>
        <source>取消</source>
        <translation>取消</translation>
    </message>
    <message>
        <location filename="src/data_add.cpp" line="281"/>
        <source>输入不能为空！</source>
        <translation>输入不能为空！</translation>
    </message>
    <message>
        <location filename="src/data_add.cpp" line="281"/>
        <source>只能输入一个字符！</source>
        <translation>只能输入一个字符！</translation>
    </message>
    <message>
        <location filename="src/data_add.cpp" line="293"/>
        <source>&apos;%1&apos; 不是ASCII字符！
请输入0-127范围内的字符。</source>
        <translation>&apos;%1&apos; 不是ASCII字符！
请输入0-127范围内的字符。</translation>
    </message>
    <message>
        <location filename="src/data_add.cpp" line="314"/>
        <source>不允许输入制表符(Tab)！</source>
        <translation>不允许输入制表符(Tab)！</translation>
    </message>
    <message>
        <location filename="src/data_add.cpp" line="316"/>
        <source>不允许输入换行符！</source>
        <translation>不允许输入换行符！</translation>
    </message>
    <message>
        <location filename="src/data_add.cpp" line="318"/>
        <source>字符 0x%1 是不可见的控制字符！</source>
        <translation>字符 0x%1 是不可见的控制字符！</translation>
    </message>
</context>
<context>
    <name>mainui</name>
    <message>
        <location filename="ui/mainui.ui" line="14"/>
        <source>mainui</source>
        <translation>狂热运输2/3 时刻表自动输入 V2.0</translation>
    </message>
    <message>
        <location filename="ui/mainui.ui" line="413"/>
        <source>站点、线路数据录入</source>
        <translation>站点、线路数据录入</translation>
    </message>
    <message>
        <source>全部导入（覆盖）</source>
        <translation type="vanished">全部导入（覆盖）</translation>
    </message>
    <message>
        <location filename="ui/mainui.ui" line="502"/>
        <source>存在站点数据：</source>
        <translation>存在站点数据：</translation>
    </message>
    <message>
        <location filename="ui/mainui.ui" line="509"/>
        <location filename="ui/mainui.ui" line="523"/>
        <location filename="src/mainui.cpp" line="809"/>
        <location filename="src/mainui.cpp" line="810"/>
        <source>否</source>
        <translation>否</translation>
    </message>
    <message>
        <location filename="ui/mainui.ui" line="516"/>
        <source>存在线路数据：</source>
        <translation>存在线路数据：</translation>
    </message>
    <message>
        <location filename="ui/mainui.ui" line="83"/>
        <source>根目录文件夹：</source>
        <translation>根目录文件夹：</translation>
    </message>
    <message>
        <location filename="ui/mainui.ui" line="115"/>
        <source>更改根目录</source>
        <translation>更改根目录</translation>
    </message>
    <message>
        <location filename="ui/mainui.ui" line="122"/>
        <location filename="src/mainui.cpp" line="907"/>
        <source>存档名称：</source>
        <translation>存档名称：</translation>
    </message>
    <message>
        <location filename="ui/mainui.ui" line="154"/>
        <location filename="src/mainui.cpp" line="908"/>
        <source>更改存档</source>
        <translation>更改存档</translation>
    </message>
    <message>
        <location filename="ui/mainui.ui" line="27"/>
        <location filename="src/mainui.cpp" line="872"/>
        <source>狂热运输2/3 时刻表mod自动录入</source>
        <translation>狂热运输2/3 时刻表mod自动录入</translation>
    </message>
    <message>
        <location filename="ui/mainui.ui" line="61"/>
        <location filename="src/mainui.cpp" line="835"/>
        <source>切换到三代</source>
        <translation>切换到三代</translation>
    </message>
    <message>
        <location filename="ui/mainui.ui" line="161"/>
        <source>当前steam用户（如多账户请手动切换）</source>
        <translation>当前steam用户（如多账户请手动切换）</translation>
    </message>
    <message>
        <location filename="ui/mainui.ui" line="187"/>
        <source>简单模式</source>
        <translation>简单模式</translation>
    </message>
    <message>
        <location filename="ui/mainui.ui" line="194"/>
        <source>列表匹配模式</source>
        <translation>列表匹配模式</translation>
    </message>
    <message>
        <location filename="ui/mainui.ui" line="223"/>
        <source>XLSX</source>
        <translation>XLSX</translation>
    </message>
    <message>
        <location filename="ui/mainui.ui" line="230"/>
        <source>CSV-UTF8</source>
        <translation>CSV-UTF8</translation>
    </message>
    <message>
        <source>csv模式不支持多表单</source>
        <translation type="vanished">csv模式不支持多表单</translation>
    </message>
    <message>
        <source>忽略最后
一行数据</source>
        <oldsource>数据最后一行
为无效数据</oldsource>
        <translation type="vanished">忽略最后
一行数据</translation>
    </message>
    <message>
        <location filename="ui/mainui.ui" line="543"/>
        <location filename="src/mainui.cpp" line="1697"/>
        <source>说明</source>
        <translation>说明</translation>
    </message>
    <message>
        <location filename="ui/mainui.ui" line="495"/>
        <source>时刻表堆叠</source>
        <translation>时刻表堆叠</translation>
    </message>
    <message>
        <location filename="ui/mainui.ui" line="271"/>
        <source>仅覆盖</source>
        <translation>仅覆盖</translation>
    </message>
    <message>
        <source>清空line并导入</source>
        <translation type="vanished">清空line并导入</translation>
    </message>
    <message>
        <location filename="ui/mainui.ui" line="285"/>
        <source>全部清空后导入</source>
        <translation>全部清空后导入</translation>
    </message>
    <message>
        <location filename="ui/mainui.ui" line="550"/>
        <location filename="src/mainui.cpp" line="1717"/>
        <source>关于</source>
        <translation>关于</translation>
    </message>
    <message>
        <location filename="ui/mainui.ui" line="557"/>
        <source>English</source>
        <translation></translation>
    </message>
    <message>
        <location filename="src/mainui.cpp" line="65"/>
        <source>狂热运输2/3 时刻表自动输入</source>
        <oldsource>狂热运输2 时刻表自动输入</oldsource>
        <translation>狂热运输2/3 时刻表自动输入</translation>
    </message>
    <message>
        <source> V1.3</source>
        <translation type="vanished"> V1.3</translation>
    </message>
    <message>
        <location filename="src/mainui.cpp" line="250"/>
        <location filename="src/mainui.cpp" line="704"/>
        <location filename="src/mainui.cpp" line="760"/>
        <source>宋体</source>
        <translation>宋体</translation>
    </message>
    <message>
        <location filename="src/mainui.cpp" line="255"/>
        <location filename="src/mainui.cpp" line="709"/>
        <location filename="src/mainui.cpp" line="765"/>
        <source>线路</source>
        <translation>线路</translation>
    </message>
    <message>
        <location filename="src/mainui.cpp" line="256"/>
        <location filename="src/mainui.cpp" line="710"/>
        <location filename="src/mainui.cpp" line="766"/>
        <source>文件1</source>
        <translation>文件1</translation>
    </message>
    <message>
        <location filename="src/mainui.cpp" line="257"/>
        <location filename="src/mainui.cpp" line="259"/>
        <location filename="src/mainui.cpp" line="711"/>
        <location filename="src/mainui.cpp" line="713"/>
        <location filename="src/mainui.cpp" line="767"/>
        <location filename="src/mainui.cpp" line="769"/>
        <source>表单名称</source>
        <translation>表单名称</translation>
    </message>
    <message>
        <location filename="src/mainui.cpp" line="258"/>
        <location filename="src/mainui.cpp" line="712"/>
        <location filename="src/mainui.cpp" line="768"/>
        <source>文件2</source>
        <translation>文件2</translation>
    </message>
    <message>
        <location filename="src/mainui.cpp" line="264"/>
        <location filename="src/mainui.cpp" line="718"/>
        <location filename="src/mainui.cpp" line="774"/>
        <source>未检测到列表文件，已自动生成</source>
        <translation>未检测到列表文件，已自动生成</translation>
    </message>
    <message>
        <location filename="src/mainui.cpp" line="265"/>
        <location filename="src/mainui.cpp" line="719"/>
        <location filename="src/mainui.cpp" line="775"/>
        <source>，如采用列表模式请编辑该文件
格式见文档，每行一个线路，如有更多文件请向后加。对于文件中的某些表单，请以空格分隔。如果需要一个文件里的所有表单请空置“表单名称”栏目，第一行仅做说明，可随意更改。</source>
        <translation>，如采用列表模式请编辑该文件
格式见文档，每行一个线路，如有更多文件请向后加。对于文件中的某些表单，请以空格分隔。如果需要一个文件里的所有表单请空置“表单名称”栏目，第一行仅做说明，可随意更改。</translation>
    </message>
    <message>
        <location filename="src/mainui.cpp" line="268"/>
        <location filename="src/mainui.cpp" line="325"/>
        <location filename="src/mainui.cpp" line="330"/>
        <location filename="src/mainui.cpp" line="722"/>
        <location filename="src/mainui.cpp" line="778"/>
        <source>提示</source>
        <translation>提示</translation>
    </message>
    <message>
        <location filename="src/mainui.cpp" line="325"/>
        <source>请先选取工作文件夹</source>
        <translation>请先选取工作文件夹</translation>
    </message>
    <message>
        <location filename="src/mainui.cpp" line="331"/>
        <source>尚未设置桥接数据目录，请点击右上角“切换到三代”按钮自动检测</source>
        <translation>尚未设置桥接数据目录，请点击右上角“切换到三代”按钮自动检测</translation>
    </message>
    <message>
        <location filename="src/mainui.cpp" line="332"/>
        <source>请先选取存档</source>
        <translation>请先选取存档</translation>
    </message>
    <message>
        <location filename="src/mainui.cpp" line="380"/>
        <location filename="src/mainui.cpp" line="384"/>
        <location filename="src/mainui.cpp" line="1040"/>
        <source>错误</source>
        <translation>错误</translation>
    </message>
    <message>
        <location filename="src/mainui.cpp" line="518"/>
        <source>未找到数据目录</source>
        <translation>未找到数据目录</translation>
    </message>
    <message>
        <location filename="src/mainui.cpp" line="519"/>
        <source>未能自动找到桥接数据目录（tpf3_timetable_bridge）。
如果还没出现过，请先进一次游戏；现在请手动选择。</source>
        <translation>未能自动找到桥接数据目录（tpf3_timetable_bridge）。
如果还没出现过，请先进一次游戏；现在请手动选择。</translation>
    </message>
    <message>
        <location filename="src/mainui.cpp" line="538"/>
        <source>切换到狂热运输3</source>
        <translation>切换到狂热运输3</translation>
    </message>
    <message>
        <location filename="src/mainui.cpp" line="540"/>
        <source>&lt;b&gt;注意！请仔细阅读以下内容，三代逻辑与二代差异较大&lt;/b&gt;&lt;ol&gt;&lt;li&gt;如要使用三代，请先在 mod.io 里安装“Timetable AutoFill”模组；建议使用方法：先打开游戏进入存档，然后打开本程序——导入是实时的，不需要像二代一样导入后重新进游戏。&lt;/li&gt;&lt;li&gt;三代无存档文件概念，程序会实时检测当前游戏存档；如更换存档，请加载完后点“刷新”。请确保当前存档与文件夹内数据一致——文件夹内数据名均为 tpf3_timetable_xxx；请确保不同存档使用不同的文件夹。&lt;/li&gt;&lt;li&gt;使用流程：打开游戏存档、打开程序、进行线路和站点的导入（还是和二代一样，不用每次都导入）、数据导入，最后在游戏内 mod 界面点“将导入数据应用至时刻表”即可完成。&lt;/li&gt;&lt;li&gt;如果游戏中途增加了站点或者线路，请按游戏内的“导出存档站点线路信息”按钮，再在程序内重新导入站点线路数据。&lt;/li&gt;&lt;li&gt;没有存档文件，所以没有备份，请务必确认数据正确后再保存你的存档！&lt;/li&gt;&lt;li&gt;三代有全局周期小时数概念，如有小时数据溢出会报错。&lt;/li&gt;&lt;li&gt;如果你有多个 Steam 账号同时玩《狂热运输》并玩时刻表，请自行选择账号（在“路径”栏的“当前steam用户”行切换）。如果只有一个账号，或者只有一个账号玩《狂热运输3》时刻表，那么系统会自动锁定账号，无需手动选择。&lt;/li&gt;&lt;li&gt;如果碰到任何问题，请立刻联系作者。&lt;/li&gt;&lt;/ol&gt;</source>
        <translation>&lt;b&gt;注意！请仔细阅读以下内容，三代逻辑与二代差异较大&lt;/b&gt;&lt;ol&gt;&lt;li&gt;如要使用三代，请先在 mod.io 里安装“Timetable AutoFill”模组；建议使用方法：先打开游戏进入存档，然后打开本程序——导入是实时的，不需要像二代一样导入后重新进游戏。&lt;/li&gt;&lt;li&gt;三代无存档文件概念，程序会实时检测当前游戏存档；如更换存档，请加载完后点“刷新”。请确保当前存档与文件夹内数据一致——文件夹内数据名均为 tpf3_timetable_xxx；请确保不同存档使用不同的文件夹。&lt;/li&gt;&lt;li&gt;使用流程：打开游戏存档、打开程序、进行线路和站点的导入（还是和二代一样，不用每次都导入）、数据导入，最后在游戏内 mod 界面点“将导入数据应用至时刻表”即可完成。&lt;/li&gt;&lt;li&gt;如果游戏中途增加了站点或者线路，请按游戏内的“导出存档站点线路信息”按钮，再在程序内重新导入站点线路数据。&lt;/li&gt;&lt;li&gt;没有存档文件，所以没有备份，请务必确认数据正确后再保存你的存档！&lt;/li&gt;&lt;li&gt;三代有全局周期小时数概念，如有小时数据溢出会报错。&lt;/li&gt;&lt;li&gt;如果你有多个 Steam 账号同时玩《狂热运输》并玩时刻表，请自行选择账号（在“路径”栏的“当前steam用户”行切换）。如果只有一个账号，或者只有一个账号玩《狂热运输3》时刻表，那么系统会自动锁定账号，无需手动选择。&lt;/li&gt;&lt;li&gt;如果碰到任何问题，请立刻联系作者。&lt;/li&gt;&lt;/ol&gt;</translation>
    </message>
    <message>
        <location filename="src/mainui.cpp" line="605"/>
        <source>检测中…</source>
        <translation>检测中…</translation>
    </message>
    <message>
        <location filename="src/mainui.cpp" line="640"/>
        <source>未检测到游戏</source>
        <translation>未检测到游戏</translation>
    </message>
    <message>
        <location filename="src/mainui.cpp" line="641"/>
        <source>已向游戏发送刷新请求，但游戏内没有响应。
请确认游戏正在运行、已进入存档，并且新版桥接模组已启用。
当前显示的是上次导出的信息。</source>
        <translation>已向游戏发送刷新请求，但游戏内没有响应。
请确认游戏正在运行、已进入存档，并且新版桥接模组已启用。
当前显示的是上次导出的信息。</translation>
    </message>
    <message>
        <location filename="src/mainui.cpp" line="650"/>
        <location filename="src/mainui.cpp" line="654"/>
        <location filename="src/mainui.cpp" line="689"/>
        <source>选择目录</source>
        <translation>选择目录</translation>
    </message>
    <message>
        <location filename="src/mainui.cpp" line="650"/>
        <source>请选取所有数据文件的根目录，即保存所有时刻表文件的目录。随后的车站和线路编号信息也都会存放于此</source>
        <translation>请选取所有数据文件的根目录，即保存所有时刻表文件的目录。随后的车站和线路编号信息也都会存放于此</translation>
    </message>
    <message>
        <location filename="src/mainui.cpp" line="684"/>
        <source>选择数据目录</source>
        <translation>选择数据目录</translation>
    </message>
    <message>
        <location filename="src/mainui.cpp" line="685"/>
        <source>“狂热运输3”模式下请选取桥接数据目录（tpf3_timetable_bridge），通常在：
C:\Program Files (x86)\Steam\userdata\&lt;你的ID&gt;\3493540\local\tpf3_timetable_bridge</source>
        <translation>“狂热运输3”模式下请选取桥接数据目录（tpf3_timetable_bridge），通常在：
C:\Program Files (x86)\Steam\userdata\&lt;你的ID&gt;\3493540\local\tpf3_timetable_bridge</translation>
    </message>
    <message>
        <location filename="src/mainui.cpp" line="714"/>
        <source>...</source>
        <translation>...</translation>
    </message>
    <message>
        <location filename="src/mainui.cpp" line="737"/>
        <source>选择存档lua文件</source>
        <translation>选择存档lua文件</translation>
    </message>
    <message>
        <location filename="src/mainui.cpp" line="737"/>
        <source>选取存档，默认应该为“C:\Program Files (x86)\Steam\userdata\XXXX\1066780\local\save\xxx.lua”，取决于steam安装位置</source>
        <translation>选取存档，默认应该为“C:\Program Files (x86)\Steam\userdata\XXXX\1066780\local\save\xxx.lua”，取决于steam安装位置</translation>
    </message>
    <message>
        <location filename="src/mainui.cpp" line="741"/>
        <location filename="src/mainui.cpp" line="1176"/>
        <source>打开文件</source>
        <translation>打开文件</translation>
    </message>
    <message>
        <location filename="src/mainui.cpp" line="744"/>
        <source>lua文件 (*.lua);;所有文件 (*.*)</source>
        <translation>lua文件 (*.lua);;所有文件 (*.*)</translation>
    </message>
    <message>
        <location filename="src/mainui.cpp" line="1591"/>
        <source>确认</source>
        <translation>确认</translation>
    </message>
    <message>
        <location filename="src/mainui.cpp" line="1592"/>
        <source>取消</source>
        <translation>取消</translation>
    </message>
    <message>
        <location filename="ui/mainui.ui" line="90"/>
        <location filename="ui/mainui.ui" line="129"/>
        <location filename="src/mainui.cpp" line="808"/>
        <location filename="src/mainui.cpp" line="906"/>
        <source>无</source>
        <translation>无</translation>
    </message>
    <message>
        <location filename="src/mainui.cpp" line="809"/>
        <location filename="src/mainui.cpp" line="810"/>
        <source>是</source>
        <translation>是</translation>
    </message>
    <message>
        <location filename="src/mainui.cpp" line="829"/>
        <source>切换回二代</source>
        <translation>切换回二代</translation>
    </message>
    <message>
        <location filename="src/mainui.cpp" line="855"/>
        <source>未读取</source>
        <translation>未读取</translation>
    </message>
    <message>
        <location filename="src/mainui.cpp" line="856"/>
        <source>当前游戏中存档：</source>
        <oldsource>当前游戏中存档（三代无存档文件，详情见下）：</oldsource>
        <translation>当前游戏中存档：</translation>
    </message>
    <message>
        <location filename="src/mainui.cpp" line="858"/>
        <source>刷新</source>
        <translation>刷新</translation>
    </message>
    <message>
        <location filename="src/mainui.cpp" line="863"/>
        <source>原存档：%1</source>
        <translation>原存档：%1</translation>
    </message>
    <message>
        <location filename="src/mainui.cpp" line="864"/>
        <source>原存档：未读取</source>
        <translation>原存档：未读取</translation>
    </message>
    <message>
        <location filename="src/mainui.cpp" line="880"/>
        <source>未检测到</source>
        <translation>未检测到</translation>
    </message>
    <message>
        <location filename="src/mainui.cpp" line="916"/>
        <source>周期</source>
        <translation>周期</translation>
    </message>
    <message>
        <location filename="src/mainui.cpp" line="1177"/>
        <source>正在打开文件...</source>
        <translation>正在打开文件...</translation>
    </message>
    <message>
        <location filename="src/mainui.cpp" line="1201"/>
        <location filename="src/mainui.cpp" line="1257"/>
        <source>正在打开文件：%1</source>
        <translation>正在打开文件：%1</translation>
    </message>
    <message>
        <location filename="src/mainui.cpp" line="1528"/>
        <source>时刻表数据如下</source>
        <translation>时刻表表单数据如下：</translation>
    </message>
    <message>
        <location filename="src/mainui.cpp" line="1534"/>
        <source>%1 : %2组数据</source>
        <translation>%1 : %2组数据</translation>
    </message>
    <message>
        <location filename="src/mainui.cpp" line="1538"/>
        <source>     包含相近或重复数据</source>
        <translation>     包含相近或重复数据</translation>
    </message>
    <message>
        <location filename="src/mainui.cpp" line="1563"/>
        <source>当前为覆盖模式，即仅添加或更新现有时刻表
</source>
        <translation>当前为覆盖模式，即仅添加或更新现有时刻表
</translation>
    </message>
    <message>
        <location filename="src/mainui.cpp" line="1566"/>
        <source>当前为列表清空，即仅清空_line文件内包含的列表
</source>
        <translation>当前为列表清空，即仅清空_line文件内包含的列表
</translation>
    </message>
    <message>
        <location filename="src/mainui.cpp" line="1569"/>
        <source>当前为全部清空模式，即删除所有原时刻表
</source>
        <translation>当前为全部清空模式，即删除所有原时刻表
</translation>
    </message>
    <message>
        <location filename="src/mainui.cpp" line="1576"/>
        <source>当前时刻表存在重复或者相近（两组或多组时刻表所有站点时刻&lt;5s），
按确认则随机保留一组并继续，按取消则返回</source>
        <translation>当前时刻表存在重复或者相近（两组或多组时刻表所有站点时刻&lt;5s），
按确认则随机保留一组并继续，按取消则返回</translation>
    </message>
    <message>
        <location filename="src/mainui.cpp" line="1578"/>
        <source>请核对时刻表数量以及表单，按确认继续</source>
        <translation>请核对时刻表数量以及表单，按确认继续</translation>
    </message>
    <message>
        <location filename="src/mainui.cpp" line="562"/>
        <location filename="src/mainui.cpp" line="1722"/>
        <source>确定</source>
        <translation>确定</translation>
    </message>
    <message>
        <location filename="ui/mainui.ui" line="77"/>
        <source>路径</source>
        <translation>路径</translation>
    </message>
    <message>
        <location filename="ui/mainui.ui" line="181"/>
        <source>数据读取方式</source>
        <translation>数据读取方式</translation>
    </message>
    <message>
        <location filename="ui/mainui.ui" line="217"/>
        <source>文件格式</source>
        <translation>文件格式</translation>
    </message>
    <message>
        <location filename="ui/mainui.ui" line="265"/>
        <source>导入方式</source>
        <translation>导入方式</translation>
    </message>
    <message>
        <location filename="ui/mainui.ui" line="295"/>
        <location filename="src/mainui.cpp" line="916"/>
        <source>兼容版本</source>
        <translation>兼容版本</translation>
    </message>
    <message>
        <location filename="ui/mainui.ui" line="242"/>
        <source>（csv模式不支持多表单）</source>
        <translation>（csv模式不支持多表单）</translation>
    </message>
    <message>
        <location filename="ui/mainui.ui" line="278"/>
        <source>按线路表清空后导入</source>
        <translation>按线路表清空后导入</translation>
    </message>
    <message>
        <location filename="ui/mainui.ui" line="324"/>
        <source>周期（原存档：未读取）</source>
        <translation>周期（原存档：未读取）</translation>
    </message>
    <message>
        <location filename="ui/mainui.ui" line="341"/>
        <source>1 小时</source>
        <translation>1 小时</translation>
    </message>
    <message>
        <location filename="ui/mainui.ui" line="346"/>
        <source>2 小时</source>
        <translation>2 小时</translation>
    </message>
    <message>
        <location filename="ui/mainui.ui" line="351"/>
        <source>3 小时</source>
        <translation>3 小时</translation>
    </message>
    <message>
        <location filename="ui/mainui.ui" line="356"/>
        <source>6 小时</source>
        <translation>6 小时</translation>
    </message>
    <message>
        <location filename="ui/mainui.ui" line="361"/>
        <source>12 小时</source>
        <translation>12 小时</translation>
    </message>
    <message>
        <location filename="ui/mainui.ui" line="366"/>
        <source>24 小时</source>
        <translation>24 小时</translation>
    </message>
    <message>
        <location filename="ui/mainui.ui" line="462"/>
        <source>导入时刻表</source>
        <translation>导入时刻表</translation>
    </message>
    <message>
        <location filename="ui/mainui.ui" line="488"/>
        <source>忽略最后一行数据</source>
        <translation>忽略最后一行数据</translation>
    </message>
    <message>
        <location filename="ui/mainui.ui" line="308"/>
        <source>时刻表&amp;运行图、时刻表1.2</source>
        <translation>时刻表&amp;运行图、时刻表1.2</translation>
    </message>
    <message>
        <location filename="ui/mainui.ui" line="313"/>
        <source>时刻表1.3-1.5</source>
        <translation>时刻表1.3-1.5</translation>
    </message>
    <message>
        <location filename="src/mainui.cpp" line="1586"/>
        <source>警告</source>
        <translation>警告</translation>
    </message>
    <message>
        <location filename="src/mainui.cpp" line="1588"/>
        <source>该操作会清空 %1_line.xlsx 里的线路对应到存档内的时刻表数据，是否确认？</source>
        <translation>该操作会清空 %1_line.xlsx 里的线路对应到存档内的时刻表数据，是否确认？</translation>
    </message>
    <message>
        <location filename="src/mainui.cpp" line="560"/>
        <location filename="src/mainui.cpp" line="1594"/>
        <source>下次不再提示</source>
        <translation>下次不再提示</translation>
    </message>
    <message>
        <location filename="src/mainui.cpp" line="65"/>
        <source> V2.0</source>
        <translation> V2.0</translation>
    </message>
    <message>
        <location filename="src/mainui.cpp" line="1041"/>
        <source>列表模式下，文件 %1 不是 %2 格式，请检查文件格式选项或修改列表文件</source>
        <translation>列表模式下，文件 %1 不是 %2 格式，请检查文件格式选项或修改列表文件</translation>
    </message>
</context>
</TS>
