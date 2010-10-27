#ifndef _GAME_H
#define _GAME_H

class Game
{
public:
	Game();
	~Game();

	//The game class contains five functions
	bool Initialise();

	bool LoadContent();

	void HandleInput(float deltaTime);

	void Update(float deltaTime);

	void Draw(float deltaTime);
};

#endif