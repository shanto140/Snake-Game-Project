all:
	g++ -I src/include -L src/lib -o main mainSnake.cpp -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image -lSDL2_mixer
	./main
	g++ -I src/include -L src/lib -o snake snake.cpp -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image -lSDL2_mixer
	g++ -I src/include -L src/lib -o s_p s_p.cpp -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image -lSDL2_mixer