# imgui_dx11

- 一个萌新的imgui项目
- 已经配置鼠标穿透，渲染窗口透明全屏，并且不会影响窗体触摸以及其他绘制项目
（记得点个star）
- 整体的实现是用winapi实现渲染窗口透明，鼠标穿透，窗体优先级最高，然后把用windowclasss把窗体分离出来（其实一开始用了一个非常不优雅的方法分离。。。。。
  <br>
24 1 8：添加控件类widget

注：
请确保cmake版本>=3.25，并且编译器支持c++20
<h1>如何构建</h1>
<string>请确保当前环境可使用cmake</string>
<span>1:在项目根目录打开cmd</span>
<span>2:执行cmake命令</span>
<span>3:如果上一步无误，你会看到一个sln文件，使用vs打开即可</span>
<string>请确保已经安装vs2022</string>
