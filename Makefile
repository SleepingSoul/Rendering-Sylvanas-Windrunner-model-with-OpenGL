all:
	g++ -o compiled/render_sylvanas.exe main.cpp LightCaster.cpp glad.c -lglfw3dll -lopengl32 -lassimp -Wall -Wno-stringop-overflow -std=c++17