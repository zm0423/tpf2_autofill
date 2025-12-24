<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="zh_CN">
<context>
    <name>MarkdownLanguageManager</name>
    <message>
        <location filename="src/MarkdownLanguageManager.cpp" line="108"/>
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
        <location filename="src/mainui.cpp" line="1059"/>
        <source>作者：今天学高代了吗&lt;br/&gt;b站视频教程：&lt;a href=&quot;https://www.bilibili.com/video/BV1yj2ABwE9v/?spm_id_from=333.1387.homepage.video_card.click&amp;vd_source=3fd42c24215ba0da48b95a40864f298c&quot;&gt;https://www.bilibili.com/video/BV1yj2ABwE9v&lt;/a&gt; &lt;br/&gt;github：&lt;a href=&quot;https://github.com/zm0423/tpf2_autofill&quot;&gt; https://github.com/zm0423/tpf2_autofill&lt;/a&gt; &lt;br/&gt;邮箱：15800733391@163.com &lt;br/&gt;2025.12.14</source>
        <translation>作者：今天学高代了吗&lt;br/&gt;b站视频教程：&lt;a href=&quot;https://www.bilibili.com/video/BV1yj2ABwE9v/?spm_id_from=333.1387.homepage.video_card.click&amp;vd_source=3fd42c24215ba0da48b95a40864f298c&quot;&gt;https://www.bilibili.com/video/BV1yj2ABwE9v&lt;/a&gt; &lt;br/&gt;github：&lt;a href=&quot;https://github.com/zm0423/tpf2_autofill&quot;&gt; https://github.com/zm0423/tpf2_autofill&lt;/a&gt; &lt;br/&gt;邮箱：15800733391@163.com &lt;br/&gt;2025.12.14</translation>
    </message>
    <message>
        <location filename="src/util.cpp" line="69"/>
        <location filename="src/util.cpp" line="98"/>
        <location filename="src/util.cpp" line="139"/>
        <location filename="src/util.cpp" line="204"/>
        <location filename="src/util.cpp" line="853"/>
        <source>错误</source>
        <translation>错误</translation>
    </message>
    <message>
        <location filename="src/util.cpp" line="69"/>
        <location filename="src/util.cpp" line="98"/>
        <source>站点或线路文件第二列含有非数字，请修改后重新打开软件</source>
        <translation>站点或线路文件第二列含有非数字，请修改后重新打开软件</translation>
    </message>
    <message>
        <location filename="src/util.cpp" line="113"/>
        <source>宋体</source>
        <translation>宋体</translation>
    </message>
    <message>
        <location filename="src/util.cpp" line="139"/>
        <source>文件存储失败，请检查文件夹权限、是否被其他应用打开、或磁盘空间</source>
        <translation>文件存储失败，请检查文件夹权限、是否被其他应用打开、或磁盘空间</translation>
    </message>
    <message>
        <location filename="src/util.cpp" line="204"/>
        <source>站点或线路输入信息的某一组内的第二行含有非数字</source>
        <translation>站点或线路输入信息的某一组内的第二行含有非数字</translation>
    </message>
    <message>
        <location filename="src/util.cpp" line="227"/>
        <location filename="src/util.cpp" line="761"/>
        <source>确定</source>
        <translation>确定</translation>
    </message>
    <message>
        <location filename="src/util.cpp" line="349"/>
        <source>成功</source>
        <translation>成功</translation>
    </message>
    <message>
        <location filename="src/util.cpp" line="349"/>
        <source>时刻表已成功导入，已生成备份文件 %1</source>
        <translation>时刻表已成功导入，已生成备份文件 %1</translation>
    </message>
    <message>
        <location filename="src/util.cpp" line="743"/>
        <source>确认</source>
        <translation>确认</translation>
    </message>
    <message>
        <location filename="src/util.cpp" line="748"/>
        <source>时刻表表单数据如下：</source>
        <translation>时刻表表单数据如下：</translation>
    </message>
    <message>
        <location filename="src/util.cpp" line="762"/>
        <source>取消</source>
        <translation>取消</translation>
    </message>
    <message>
        <location filename="src/util.cpp" line="789"/>
        <source>未发现线路id数据</source>
        <translation>未发现线路id数据</translation>
    </message>
    <message>
        <location filename="src/util.cpp" line="793"/>
        <source>未发现站点id数据</source>
        <translation>未发现站点id数据</translation>
    </message>
    <message>
        <location filename="src/util.cpp" line="797"/>
        <source>列表模式未发现列表</source>
        <translation>列表模式未发现列表</translation>
    </message>
    <message>
        <location filename="src/util.cpp" line="801"/>
        <source>列表模式下，线路%1不存在</source>
        <translation>列表模式下，线路%1不存在</translation>
    </message>
    <message>
        <location filename="src/util.cpp" line="805"/>
        <source>列表模式下，文件%1不存在</source>
        <translation>列表模式下，文件%1不存在</translation>
    </message>
    <message>
        <location filename="src/util.cpp" line="809"/>
        <source>列表模式下，%1表单不存在</source>
        <translation>列表模式下，%1表单不存在</translation>
    </message>
    <message>
        <location filename="src/util.cpp" line="813"/>
        <source>列表模式下无时刻表或简单模式未检测到任何时刻表</source>
        <translation>列表模式下无时刻表或简单模式未检测到任何时刻表</translation>
    </message>
    <message>
        <location filename="src/util.cpp" line="817"/>
        <source>站点%1不存在</source>
        <oldsource>站点%1对应id不存在</oldsource>
        <translation>站点%1不存在</translation>
    </message>
    <message>
        <location filename="src/util.cpp" line="821"/>
        <source>表单%1内无数据</source>
        <translation>表单%1内无数据</translation>
    </message>
    <message>
        <location filename="src/util.cpp" line="825"/>
        <source>线路%1内存在多个时刻表的站点不对应</source>
        <translation>线路%1内存在多个时刻表的站点不对应</translation>
    </message>
    <message>
        <location filename="src/util.cpp" line="829"/>
        <source>文件%1内含有无效时间数据</source>
        <translation>文件%1内含有无效时间数据</translation>
    </message>
    <message>
        <location filename="src/util.cpp" line="833"/>
        <source>文件%1内无表单</source>
        <translation>文件%1内无表单</translation>
    </message>
    <message>
        <location filename="src/util.cpp" line="837"/>
        <source>存档文件无法打开，请检查权限等</source>
        <translation>存档文件无法打开，请检查权限等</translation>
    </message>
    <message>
        <location filename="src/util.cpp" line="841"/>
        <source>存档文件无法保存，请检查权限或者是否在别的应用打开等</source>
        <translation>存档文件无法保存，请检查权限或者是否在别的应用打开等</translation>
    </message>
    <message>
        <location filename="src/util.cpp" line="845"/>
        <source>未安装时刻表mod</source>
        <translation>未安装时刻表mod</translation>
    </message>
    <message>
        <location filename="src/util.cpp" line="849"/>
        <source>其他</source>
        <translation>其他</translation>
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
        <source>游戏内获取的数据输入区域：</source>
        <translation>游戏内获取的数据输入区域：</translation>
    </message>
    <message>
        <location filename="ui/data_add.ui" line="81"/>
        <source>车站数据代码：</source>
        <translation>车站数据代码：</translation>
    </message>
    <message>
        <location filename="ui/data_add.ui" line="93"/>
        <location filename="ui/data_add.ui" line="130"/>
        <source>复制</source>
        <translation>复制</translation>
    </message>
    <message>
        <location filename="ui/data_add.ui" line="118"/>
        <source>线路数据代码：</source>
        <translation>线路数据代码：</translation>
    </message>
    <message>
        <location filename="ui/data_add.ui" line="155"/>
        <source>车站数据
导入</source>
        <translation>车站数据
导入</translation>
    </message>
    <message>
        <location filename="ui/data_add.ui" line="181"/>
        <source>线路数据
导入</source>
        <translation>线路数据
导入</translation>
    </message>
    <message>
        <location filename="ui/data_add.ui" line="189"/>
        <source>截断&quot;/&quot;以后内容</source>
        <translation>截断&quot;/&quot;以后内容</translation>
    </message>
    <message>
        <location filename="ui/data_add.ui" line="196"/>
        <source>自定义截断字符</source>
        <translation>自定义截断字符</translation>
    </message>
    <message>
        <location filename="ui/data_add.ui" line="216"/>
        <location filename="src/data_add.cpp" line="249"/>
        <source>说明</source>
        <translation>说明</translation>
    </message>
    <message>
        <location filename="src/data_add.cpp" line="33"/>
        <location filename="src/data_add.cpp" line="242"/>
        <source>截断&quot;%1&quot;以后内容</source>
        <translation>截断&quot;%1&quot;以后内容</translation>
    </message>
    <message>
        <location filename="src/data_add.cpp" line="34"/>
        <source>站点、线路数据导入</source>
        <translation>站点、线路数据导入</translation>
    </message>
    <message>
        <location filename="src/data_add.cpp" line="50"/>
        <location filename="src/data_add.cpp" line="57"/>
        <location filename="src/data_add.cpp" line="96"/>
        <location filename="src/data_add.cpp" line="147"/>
        <source>提示</source>
        <translation>提示</translation>
    </message>
    <message>
        <location filename="src/data_add.cpp" line="50"/>
        <source>站点复制代码已复制至粘贴板！</source>
        <translation>站点复制代码已复制至粘贴板！</translation>
    </message>
    <message>
        <location filename="src/data_add.cpp" line="57"/>
        <source>线路复制代码已复制至粘贴板！</source>
        <translation>线路复制代码已复制至粘贴板！</translation>
    </message>
    <message>
        <location filename="src/data_add.cpp" line="75"/>
        <location filename="src/data_add.cpp" line="114"/>
        <location filename="src/data_add.cpp" line="192"/>
        <location filename="src/data_add.cpp" line="204"/>
        <location filename="src/data_add.cpp" line="233"/>
        <source>错误</source>
        <translation>错误</translation>
    </message>
    <message>
        <location filename="src/data_add.cpp" line="75"/>
        <location filename="src/data_add.cpp" line="114"/>
        <source>数据过少</source>
        <translation>数据过少</translation>
    </message>
    <message>
        <location filename="src/data_add.cpp" line="96"/>
        <source>站点数据导入成功，若需更改请查看 存档名_station.xlsx</source>
        <translation>站点数据导入成功，若需更改请查看 存档名_station.xlsx</translation>
    </message>
    <message>
        <location filename="src/data_add.cpp" line="147"/>
        <source>线路数据导入成功，若需更改请查看 存档名_line.xlsx</source>
        <translation>线路数据导入成功，若需更改请查看 存档名_line.xlsx</translation>
    </message>
    <message>
        <location filename="src/data_add.cpp" line="170"/>
        <source>截断</source>
        <translation>截断</translation>
    </message>
    <message>
        <location filename="src/data_add.cpp" line="171"/>
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
        <location filename="src/data_add.cpp" line="193"/>
        <source>输入不能为空！</source>
        <translation>输入不能为空！</translation>
    </message>
    <message>
        <location filename="src/data_add.cpp" line="193"/>
        <source>只能输入一个字符！</source>
        <translation>只能输入一个字符！</translation>
    </message>
    <message>
        <location filename="src/data_add.cpp" line="205"/>
        <source>&apos;%1&apos; 不是ASCII字符！
请输入0-127范围内的字符。</source>
        <translation>&apos;%1&apos; 不是ASCII字符！
请输入0-127范围内的字符。</translation>
    </message>
    <message>
        <location filename="src/data_add.cpp" line="226"/>
        <source>不允许输入制表符(Tab)！</source>
        <translation>不允许输入制表符(Tab)！</translation>
    </message>
    <message>
        <location filename="src/data_add.cpp" line="228"/>
        <source>不允许输入换行符！</source>
        <translation>不允许输入换行符！</translation>
    </message>
    <message>
        <location filename="src/data_add.cpp" line="230"/>
        <source>字符 0x%1 是不可见的控制字符！</source>
        <translation>字符 0x%1 是不可见的控制字符！</translation>
    </message>
</context>
<context>
    <name>mainui</name>
    <message>
        <location filename="ui/mainui.ui" line="19"/>
        <source>mainui</source>
        <translation>狂热运输2 时刻表mod自动录入</translation>
    </message>
    <message>
        <location filename="ui/mainui.ui" line="31"/>
        <source>站点、线路数据录入</source>
        <translation>站点、线路数据录入</translation>
    </message>
    <message>
        <location filename="ui/mainui.ui" line="44"/>
        <source>全部导入（覆盖）</source>
        <translation>全部导入（覆盖）</translation>
    </message>
    <message>
        <location filename="ui/mainui.ui" line="60"/>
        <source>存在站点数据：</source>
        <translation>存在站点数据：</translation>
    </message>
    <message>
        <location filename="ui/mainui.ui" line="67"/>
        <location filename="ui/mainui.ui" line="81"/>
        <location filename="src/mainui.cpp" line="356"/>
        <location filename="src/mainui.cpp" line="357"/>
        <source>否</source>
        <translation>否</translation>
    </message>
    <message>
        <location filename="ui/mainui.ui" line="74"/>
        <source>存在线路数据：</source>
        <translation>存在线路数据：</translation>
    </message>
    <message>
        <location filename="ui/mainui.ui" line="110"/>
        <source>根目录文件夹：</source>
        <translation>根目录文件夹：</translation>
    </message>
    <message>
        <location filename="ui/mainui.ui" line="140"/>
        <source>更改根目录</source>
        <translation>更改根目录</translation>
    </message>
    <message>
        <location filename="ui/mainui.ui" line="164"/>
        <source>存档名称：</source>
        <translation>存档名称：</translation>
    </message>
    <message>
        <location filename="ui/mainui.ui" line="194"/>
        <source>更改存档</source>
        <translation>更改存档</translation>
    </message>
    <message>
        <location filename="ui/mainui.ui" line="217"/>
        <source>狂热运输2 时刻表mod自动录入</source>
        <translation>狂热运输2 时刻表mod自动录入</translation>
    </message>
    <message>
        <location filename="ui/mainui.ui" line="233"/>
        <source>简单模式</source>
        <translation>简单模式</translation>
    </message>
    <message>
        <location filename="ui/mainui.ui" line="246"/>
        <source>列表匹配模式</source>
        <translation>列表匹配模式</translation>
    </message>
    <message>
        <location filename="ui/mainui.ui" line="259"/>
        <source>XLSX</source>
        <translation>XLSX</translation>
    </message>
    <message>
        <location filename="ui/mainui.ui" line="272"/>
        <source>CSV-UTF8</source>
        <translation>CSV-UTF8</translation>
    </message>
    <message>
        <location filename="ui/mainui.ui" line="290"/>
        <source>csv模式不支持多表单</source>
        <translation>csv模式不支持多表单</translation>
    </message>
    <message>
        <location filename="ui/mainui.ui" line="303"/>
        <source>忽略最后
一行数据</source>
        <oldsource>数据最后一行
为无效数据</oldsource>
        <translation>忽略最后
一行数据</translation>
    </message>
    <message>
        <location filename="ui/mainui.ui" line="317"/>
        <location filename="src/mainui.cpp" line="1050"/>
        <source>说明</source>
        <translation>说明</translation>
    </message>
    <message>
        <location filename="ui/mainui.ui" line="330"/>
        <source>时刻表堆叠</source>
        <translation>时刻表堆叠</translation>
    </message>
    <message>
        <location filename="ui/mainui.ui" line="343"/>
        <source>仅覆盖</source>
        <translation>仅覆盖</translation>
    </message>
    <message>
        <location filename="ui/mainui.ui" line="356"/>
        <source>清空line并导入</source>
        <translation>清空line并导入</translation>
    </message>
    <message>
        <location filename="ui/mainui.ui" line="369"/>
        <source>全部清空后导入</source>
        <translation>全部清空后导入</translation>
    </message>
    <message>
        <location filename="ui/mainui.ui" line="382"/>
        <location filename="src/mainui.cpp" line="1069"/>
        <source>关于</source>
        <translation>关于</translation>
    </message>
    <message>
        <location filename="ui/mainui.ui" line="395"/>
        <source>English</source>
        <translation></translation>
    </message>
    <message>
        <location filename="src/mainui.cpp" line="57"/>
        <source>狂热运输2 时刻表自动输入</source>
        <translation>狂热运输2 时刻表自动输入</translation>
    </message>
    <message>
        <location filename="src/mainui.cpp" line="158"/>
        <location filename="src/mainui.cpp" line="310"/>
        <source>宋体</source>
        <translation>宋体</translation>
    </message>
    <message>
        <location filename="src/mainui.cpp" line="163"/>
        <location filename="src/mainui.cpp" line="315"/>
        <source>线路</source>
        <translation>线路</translation>
    </message>
    <message>
        <location filename="src/mainui.cpp" line="164"/>
        <location filename="src/mainui.cpp" line="316"/>
        <source>文件1</source>
        <translation>文件1</translation>
    </message>
    <message>
        <location filename="src/mainui.cpp" line="165"/>
        <location filename="src/mainui.cpp" line="167"/>
        <location filename="src/mainui.cpp" line="317"/>
        <location filename="src/mainui.cpp" line="319"/>
        <source>表单名称</source>
        <translation>表单名称</translation>
    </message>
    <message>
        <location filename="src/mainui.cpp" line="166"/>
        <location filename="src/mainui.cpp" line="318"/>
        <source>文件2</source>
        <translation>文件2</translation>
    </message>
    <message>
        <location filename="src/mainui.cpp" line="172"/>
        <location filename="src/mainui.cpp" line="324"/>
        <source>未检测到列表文件，已自动生成</source>
        <translation>未检测到列表文件，已自动生成</translation>
    </message>
    <message>
        <location filename="src/mainui.cpp" line="173"/>
        <location filename="src/mainui.cpp" line="325"/>
        <source>，如采用列表模式请编辑该文件
格式见文档，每行一个线路，如有更多文件请向后加。对于文件中的某些表单，请以空格分隔。如果需要一个文件里的所有表单请空置“表单名称”栏目，第一行仅做说明，可随意更改。</source>
        <translation></translation>
    </message>
    <message>
        <location filename="src/mainui.cpp" line="176"/>
        <location filename="src/mainui.cpp" line="212"/>
        <location filename="src/mainui.cpp" line="217"/>
        <location filename="src/mainui.cpp" line="328"/>
        <source>提示</source>
        <translation>提示</translation>
    </message>
    <message>
        <location filename="src/mainui.cpp" line="212"/>
        <source>请先选取工作文件夹</source>
        <translation>请先选取工作文件夹</translation>
    </message>
    <message>
        <location filename="src/mainui.cpp" line="217"/>
        <source>请先选取存档</source>
        <translation>请先选取存档</translation>
    </message>
    <message>
        <location filename="src/mainui.cpp" line="259"/>
        <location filename="src/mainui.cpp" line="263"/>
        <source>选择目录</source>
        <translation>选择目录</translation>
    </message>
    <message>
        <location filename="src/mainui.cpp" line="259"/>
        <source>请选取所有数据文件的根目录，即保存所有时刻表文件的目录。随后的车站和线路编号信息也都会存放于此</source>
        <translation>请选取所有数据文件的根目录，即保存所有时刻表文件的目录。随后的车站和线路编号信息也都会存放于此</translation>
    </message>
    <message>
        <location filename="src/mainui.cpp" line="287"/>
        <source>选择存档lua文件</source>
        <translation>选择存档lua文件</translation>
    </message>
    <message>
        <location filename="src/mainui.cpp" line="287"/>
        <source>选取存档，默认应该为“C:\Program Files (x86)\Steam\userdata\XXXX\1066780\local\save\xxx.lua”，取决于steam安装位置</source>
        <translation>选取存档，默认应该为“C:\Program Files (x86)\Steam\userdata\XXXX\1066780\local\save\xxx.lua”，取决于steam安装位置</translation>
    </message>
    <message>
        <location filename="src/mainui.cpp" line="291"/>
        <location filename="src/mainui.cpp" line="583"/>
        <source>打开文件</source>
        <translation>打开文件</translation>
    </message>
    <message>
        <location filename="src/mainui.cpp" line="294"/>
        <source>lua文件 (*.lua);;所有文件 (*.*)</source>
        <translation>lua文件 (*.lua);;所有文件 (*.*)</translation>
    </message>
    <message>
        <location filename="src/mainui.cpp" line="354"/>
        <location filename="src/mainui.cpp" line="355"/>
        <source>无</source>
        <translation>无</translation>
    </message>
    <message>
        <location filename="src/mainui.cpp" line="356"/>
        <location filename="src/mainui.cpp" line="357"/>
        <source>是</source>
        <translation>是</translation>
    </message>
    <message>
        <location filename="src/mainui.cpp" line="584"/>
        <source>正在打开文件...</source>
        <translation>正在打开文件...</translation>
    </message>
    <message>
        <location filename="src/mainui.cpp" line="608"/>
        <location filename="src/mainui.cpp" line="664"/>
        <source>正在打开文件：%1</source>
        <translation>正在打开文件：%1</translation>
    </message>
    <message>
        <location filename="src/mainui.cpp" line="909"/>
        <source>%1 : %2组数据</source>
        <translation>%1 : %2组数据</translation>
    </message>
    <message>
        <location filename="src/mainui.cpp" line="913"/>
        <source>     包含相近或重复数据</source>
        <translation>     包含相近或重复数据</translation>
    </message>
    <message>
        <location filename="src/mainui.cpp" line="938"/>
        <source>当前为覆盖模式，即仅添加或更新现有时刻表
</source>
        <translation>当前为覆盖模式，即仅添加或更新现有时刻表
</translation>
    </message>
    <message>
        <location filename="src/mainui.cpp" line="941"/>
        <source>当前为列表清空，即仅清空_line文件内包含的列表
</source>
        <translation>当前为列表清空，即仅清空_line文件内包含的列表
</translation>
    </message>
    <message>
        <location filename="src/mainui.cpp" line="944"/>
        <source>当前为全部清空模式，即删除所有原时刻表
</source>
        <translation>当前为全部清空模式，即删除所有原时刻表
</translation>
    </message>
    <message>
        <location filename="src/mainui.cpp" line="951"/>
        <source>当前时刻表存在重复或者相近（两组或多组时刻表所有站点时刻&lt;5s），
按确认则随机保留一组并继续，按取消则返回</source>
        <translation>当前时刻表存在重复或者相近（两组或多组时刻表所有站点时刻&lt;5s），
按确认则随机保留一组并继续，按取消则返回</translation>
    </message>
    <message>
        <location filename="src/mainui.cpp" line="953"/>
        <source>请核对时刻表数量以及表单，按确认继续</source>
        <translation>请核对时刻表数量以及表单，按确认继续</translation>
    </message>
    <message>
        <location filename="src/mainui.cpp" line="1074"/>
        <source>确定</source>
        <translation>确定</translation>
    </message>
</context>
</TS>
