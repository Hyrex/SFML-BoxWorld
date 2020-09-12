# SFML-BoxWorld
This game project is inspired by [Neon Beats](https://store.steampowered.com/search/?snr=1_4_4__12&term=neon) and I wanted to make something similar.

Setup
---
1. Download [SFML](https://www.sfml-dev.org/) v2.5.1 Visual C++15 (2017) 64-bit Downloaded and place it the C:\. The path should be C:\SFML-2.5.1
- If you renamed the folder, make sure the include and bin folder path matches in the app properties.  Dll are in repo for easier linkage.
- It must be SFML 64-bit, because 32-bit doesn't work with Box2D.
- Add 2 Environment Path in your system settings.
  - SFML include location : C:\SFML-2.5.1\include
  - SFML lib location : C:\SFML-2.5.1\lib

2. Clone [Box2D](https://github.com/erincatto/Box2D) and place it under C:/box2d-master/. 
- If you renamed the folder, make sure the path matches in the app properties.
- I have included a Box2D.lib built along with the project. If you wish to rebuild, follow the Box2D github instruction and relink the lib in Solution Properties. 

