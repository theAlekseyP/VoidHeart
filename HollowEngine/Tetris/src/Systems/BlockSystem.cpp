#include <Hollow.h>
#include "BlockSystem.h"
#include "Hollow/Common.h"

#include "LayerSystem.h"
#include "Hollow/Managers/SystemManager.h"
#include "Hollow/Managers/InputManager.h"

BlockSystem BlockSystem::instance;

void BlockSystem::Init()
{
	// Init tetrominos
	Tetromino3 L;
	L.mData[2][1][0] = L.mData[1][1][0] = L.mData[0][1][0] = L.mData[0][1][1] = true;
	mShapes[0] = L;

	Tetromino3 S;
	S.mData[2][1][0] = S.mData[1][1][0] = S.mData[1][1][1] = S.mData[0][1][1] = true;
	mShapes[1] = S;

	Tetromino3 T;
	T.mData[1][1][2] = T.mData[1][1][0] = T.mData[1][1][1] = T.mData[0][1][1] = true;
	mShapes[2] = T;

	// Bar
	mBar.mData[0][2][1] = mBar.mData[1][2][1] = mBar.mData[2][2][1] = mBar.mData[3][2][1] = true;
	
	mSpawnBlock = true;
}

void BlockSystem::Update()
{
	if(mSpawnBlock)
	{
		LayerSystem* layer = Hollow::SystemManager::Instance().GetSystem<LayerSystem>();
		//CopyTetromino3(mShapes[2].mData, layer->mLayers);
		//CopyTetromino4(mBar.mData, layer->mLayers);
		CopyTetromino2(mCube.mData, layer->mLayers);
		mSpawnBlock = false;
	}
	if(Hollow::InputManager::Instance().IsKeyTriggered(SDL_SCANCODE_K))
	{
		//Tetromino3 L = RotateAroundZ(mShapes[2]);
		//RotateAroundZ(mBar);
		LayerSystem* layer = Hollow::SystemManager::Instance().GetSystem<LayerSystem>();
		//CopyTetromino3(mShapes[2].mData, layer->mLayers);
		//CopyTetromino4(mBar.mData, layer->mLayers);
		CopyTetromino2(mCube.mData, layer->mLayers);
	}
}

void BlockSystem::AddGameObject(Hollow::GameObject* object)
{
}

Tetromino3 BlockSystem::RotateAroundZ(Tetromino3& data)
{
	bool workingArray[3][3] = { false };

	for(int i = 0; i < 3; ++i)
	{
		for(int j = 0; j < 3; ++j)
		{
			bool res = false;
			for(int k = 0; k < 3; ++k)
			{
				res = res || data.mData[2 - i][k][j];
			}
			workingArray[i][j] = res;
		}
	}

	// Mirror working array
	for(int i = 0; i < 3; ++i)
	{
		std::swap(workingArray[0][i], workingArray[2][i]);
		for(int j = 0; j < 3; ++j)
		{
			std::swap(data.mData[2][j][i], data.mData[0][j][i]);
		}
	}

	// Transpose working array
	for(int i = 0; i < 3; ++i)
	{
		for(int j = i; j < 3; ++j)
		{
			if(i==j)
			{
				continue;
			}
			std::swap(workingArray[i][j], workingArray[j][i]);
			for(int k = 0; k < 3; ++k)
			{
				std::swap(data.mData[2 - i][k][j], data.mData[2-j][k][i]);
			}
		}
	}

	return data;
}

Tetromino4 BlockSystem::RotateAroundZ(Tetromino4& data)
{
	bool workingArray[4][4] = { false };

	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			bool res = false;
			for (int k = 0; k < 4; ++k)
			{
				res = res || data.mData[3 - i][k][j];
			}
			workingArray[i][j] = res;
		}
	}

	// Mirror working array
	for (int i = 0; i < 4; ++i)
	{
		std::swap(workingArray[0][i], workingArray[3][i]);
		std::swap(workingArray[1][i], workingArray[2][i]);
		for (int j = 0; j < 4; ++j)
		{
			std::swap(data.mData[3][j][i], data.mData[0][j][i]);
			std::swap(data.mData[2][j][i], data.mData[1][j][i]);
		}
	}

	// Transpose working array
	for (int i = 0; i < 4; ++i)
	{
		for (int j = i; j < 4; ++j)
		{
			if (i == j)
			{
				continue;
			}
			std::swap(workingArray[i][j], workingArray[j][i]);
			for (int k = 0; k < 4; ++k)
			{
				std::swap(data.mData[3 - i][k][j], data.mData[3 - j][k][i]);
			}
		}
	}

	return data;
}

void BlockSystem::CopyTetromino3(bool src[3][3][3], bool dest[][10][10])
{
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			for (int k = 0; k < 3; ++k)
			{
				dest[i][j][k] = src[i][j][k];
			}
		}
	}
}

void BlockSystem::CopyTetromino4(bool src[4][4][4], bool dest[][10][10])
{
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			for (int k = 0; k < 4; ++k)
			{
				dest[i][j][k] = src[i][j][k];
			}
		}
	}
}

void BlockSystem::CopyTetromino2(bool src[2][2][2], bool dest[][10][10])
{
	for (int i = 0; i < 2; ++i)
	{
		for (int j = 0; j < 2; ++j)
		{
			for (int k = 0; k < 2; ++k)
			{
				dest[i][j][k] = src[i][j][k];
			}
		}
	}
}
