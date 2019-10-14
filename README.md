# SFML-BoxWorld
This game project is inspired by [Neon Beats](https://store.steampowered.com/search/?snr=1_4_4__12&term=neon) and I wanted to make something similar.

Setup
---
1. Download [SFML](https://www.sfml-dev.org/) v2.5.1 Visual C++15 (2017) 64-bit Downloaded and place under C:/SFML-2.5.1-64-bit/. Make sure the include and bin folder path matches.  Dll are in repo for easier linkage.
- 32-bit doesn't work with Box2D.
- Your path location should looks like this : 
  - SFML include location : C:/SFML-2.5.1-64-bit/include
  - SFML lib location : C:/SFML-2.5.1-64-bit/lib

2. Clone [Box2D](https://github.com/erincatto/Box2D) and place it under C:/Box2D-master/. 
- I have included a Box2D.lib built along with the project. If you wish to rebuild, follow the Box2D github instruction and relink the lib in Solution Properties. 

