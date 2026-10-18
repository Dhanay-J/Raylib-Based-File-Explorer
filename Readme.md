# RayGUI test

## Raylib setup

```bash
mkdir deps 
cd deps 
wget https://github.com/raysan5/raylib/releases/download/6.0/raylib-6.0_linux_amd64.tar.gz 
cd raylib/include 
wget https://raw.githubusercontent.com/raysan5/raygui/master/src/raygui.h
```
## Font generation example
```bash
xxd -i Roboto-Regular.ttf > roboto_font.h
```