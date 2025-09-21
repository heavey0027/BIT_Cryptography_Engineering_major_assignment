# 密码工作大作业


## 课程代码实现

---


详细算法设计与实现逻辑请参考附带的 **密码工程大作业.pdf** 文件。

所有编写程序都在根目录，uBlock设计源代码在lib文件夹；

所有可执行文件都在output文件夹；

所有照片均在photo文件夹

---


## 复现实验

### 🛠️ 开发环境配置
**开发工具**: Visual Studio Code  
**核心配置文件调整**：


在**c_cpp_properties.json**，添加openssl/include路径,否则运行吞吐量实验可能有如下错误：

~~检测到 #include 错误。请更新 includePath。~~

~~无法打开 源 文件 \"openssl/evp.h\；~~

~~无法打开 源 文件 \"openssl/aes.h\；~~

在**launch.json**中，并无需要修改

在**settings.json**中，并无需要修改

在**task.json**中，args[]修改如下：

1. "-mconsole"---确保 MinGW 编译的控制台程序能正常显示终端窗口
2. "-O3"---增加性能，提升uBlock吞吐量
3. "-mssse3"---确保支持uBlock使用SSSE3指令集
4. "-march=native"---让 GCC/G++ 编译器针对当前CPU架构生成最优化的机器码
5. "-fdiagnostics-color=always"--- 强制 GCC 输出彩色错误信息
6. "${workspaceFolder}/lib/uBlock_128.cpp"---指向uBlock_128.cpp让密码工作模式实现文件找得到uBlock加密
7. "-I", "C:/Program Files/OpenSSL-Win64/include","C:/Program Files/OpenSSL-Win64/lib/VC/x64/MDd/libcrypto.lib",  
"C:/Program Files/OpenSSL-Win64/lib/VC/x64/MDd/libssl.lib" ---告诉编译器去哪里找 OpenSSL 头文件和加密功能的静态库

---

### 🔬 实验注意事项
在进行正确性验证实验uBlock_correction.cpp时，删除或注释掉task.json中"${workspaceFolder}/lib/uBlock_128.cpp"

我们在正确性验证时，我们修改了每轮加密流程，让其在每轮加密后均输出加密结果，若还保留指向uBlock_128.cpp的task，
则会出现重复定义的报错

在吞吐量实验中，若想进一步提升吞吐量，可继续简化运行task中不重要的部分。

---

