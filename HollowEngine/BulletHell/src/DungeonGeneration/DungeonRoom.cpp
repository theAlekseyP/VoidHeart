#include <Hollow.h>
#include "DungeonRoom.h"

#include "Components/Attack.h"

#include "Hollow/Managers/ResourceManager.h"

#include "Hollow/Components/Body.h"
#include "Hollow/Components/Transform.h"
#include "Hollow/Components/Collider.h"
#include "Hollow/Components/Material.h"
#include "Hollow/Components/Script.h"
#include "GameMetaData/GameEventType.h"
#include "Hollow/Events/GameEvent.h"
#include "Hollow/Managers/EventManager.h"
#include "Hollow/Managers/AudioManager.h"

namespace BulletHell
{
    float DungeonRoom::mWallLength = 0;
    float DungeonRoom::mWallHeight = 0;
    float DungeonRoom::mRoomSize = 0;
    float DungeonRoom::mWallThickness = 0;
    float DungeonRoom::mDoorWidth = 0;
    float DungeonRoom::mDoorHeight = 0;
    float DungeonRoom::mDoorThickness = 0;
	
    DungeonRoom::DungeonRoom()
        : mRoomType(DungeonRoomType::EMPTY)
        , mRoomID(0)
        , mDoors(DoorDirrection::NONE)
        , mFloorNum(0)
        , mColumn(0)
        , mRow(0)
        , mDistFromEntrance(0)
		, mDoorGOs{ nullptr }
		, mIsUnlocked(false)
        , mIsLockedFirstTime(false)
		, mLightOffsetFromDoor(3.5f)
    {
    }

    DungeonRoom::DungeonRoom(DungeonRoomType roomType, int roomID, int doors
        , int floorNum, int xCoord, int yCoord)
        : mRoomType(roomType)
        , mRoomID(roomID)
        , mDoors(doors)
        , mFloorNum(floorNum)
        , mColumn(xCoord)
        , mRow(yCoord)
        , mDistFromEntrance(0)
        , mDoorGOs{ nullptr }
        , mIsUnlocked(false)
		, mLightOffsetFromDoor(3.5f)
    {
    }

    void DungeonRoom::Set(DungeonRoomType roomType, int roomID, int doors
        , int floorNum, int xCoord, int yCoord)
    {
        mRoomType = roomType;
        mRoomID = roomID;
        mDoors = doors;
        mFloorNum = floorNum;
        mColumn = xCoord;
        mRow = yCoord;
    }

    DungeonRoom& DungeonRoom::operator=(const DungeonRoom& room)
    {
        mRoomType = room.mRoomType;
        mRoomID = room.mRoomID;
        mDoors = room.mDoors;
        mFloorNum = room.mFloorNum;
        mColumn = room.mColumn;
        mRow = room.mRow;
        mDistFromEntrance = room.mDistFromEntrance;
        return *this;
    }

	glm::ivec2 DungeonRoom::GetCoords() const
    {
        return glm::ivec2(mColumn, mRow);
    }

    int DungeonRoom::TotalDoors() const
    {
        // have to cast to boolean because DOWN==2 and LEFT==4
        // so when I add the results of & operator it might be more then 4 doors
        bool a = mDoors & DoorDirrection::UP;
        bool b = mDoors & DoorDirrection::RIGHT;
        bool c = mDoors & DoorDirrection::DOWN;
        bool d = mDoors & DoorDirrection::LEFT;

        return a + b + c + d; // adding booleans and casting to int implicitly
    }

    void DungeonRoom::ConstructRoom()
    {
        //E.x. room at 3,2 in floor grid with doors at left and top
        int mRoomX = mColumn;
        int mRoomY = mRow;
        int mDoorBits = mDoors;

        float halfRoomSize = mRoomSize/2.0f;
        float halfWallLength = mWallLength / 2.0f;
        float halfWallHeight = mWallHeight / 2.0f;
        float halfDoorHeight = mDoorHeight / 2.0f;
    	
        Hollow::GameObject* pRoomFloor = 
            Hollow::ResourceManager::Instance().LoadScaledPrefabAtPosition("Floor", 
                glm::vec3(mRoomY*mRoomSize + halfRoomSize,
							0.0f,
							mRoomX * mRoomSize + halfRoomSize),
                glm::vec3(mRoomSize, 1.0f, mRoomSize));

    	// Basic Debug Like representation of Entrance and Boss
        Hollow::Material* pMat = pRoomFloor->GetComponent<Hollow::Material>();
        if(mRoomType == DungeonRoomType::ENTRANCE)
        {
        	// Green
            pMat->mDiffuseColor = glm::vec3(0.0f, 1.0f, 0.0f);
        }
    	else if (mRoomType == DungeonRoomType::BOSS)
        {
    		// Red
            pMat->mDiffuseColor = glm::vec3(1.0f, 0.0f, 0.0f);
        }
        else
        {
        	// Grey
            pMat->mDiffuseColor = glm::vec3(0.8f, 0.8f, 0.8f);
        }
    	
    	// Origin of room is in top left corner
    	if(mDoorBits & 1)
        {
    		// Don't Make Up Wall with Door
    	}
        else
        {
            // TOP WALL
            Hollow::ResourceManager::Instance().LoadScaledPrefabAtPosition("Wall",
                glm::vec3(mRoomY * mRoomSize + halfRoomSize, halfWallHeight, mRoomX * mRoomSize),
                glm::vec3(mRoomSize, mWallHeight, mWallThickness));

        }

        if (mDoorBits & 8)
        {
            // Don't make Left Wall with Door
        }
        else
        {
            // LEFT WALL
            Hollow::ResourceManager::Instance().LoadScaledPrefabAtPosition("Wall",
                glm::vec3(mRoomY * mRoomSize, halfWallHeight, mRoomX * mRoomSize + halfRoomSize),
                glm::vec3(mWallThickness, mWallHeight, mRoomSize));
        }

    	if(mDoorBits & 2)
    	{
            // RIGHT WALL (first Part)
            Hollow::ResourceManager::Instance().LoadScaledPrefabAtPosition("Wall",
                glm::vec3(mRoomY * mRoomSize + mRoomSize, halfWallHeight, mRoomX * mRoomSize + halfWallLength),
                glm::vec3(mWallThickness, mWallHeight, mWallLength));

    		// DOOR
			glm::vec3 doorPos = glm::vec3(mRoomY * mRoomSize + mRoomSize, halfDoorHeight, mRoomX * mRoomSize + halfRoomSize);
            Hollow::GameObject* door = Hollow::ResourceManager::Instance().LoadScaledPrefabAtPosition("Door",
                                            doorPos,
                                            glm::vec3(mDoorThickness, mDoorHeight, mDoorWidth));
            door->GetComponent<Hollow::Collider>()->SetCustomBounds(glm::vec3(-0.5f, -1.6f, -2.0f),
                glm::vec3(0.5f, 1.6f, 2.0f));
            mDoorGOs[2] = door;

			// LIGHTS ON BOTH SIDES OF THE DOOR
			Hollow::ResourceManager::Instance().LoadPrefabAtPosition("newLight", glm::vec3(doorPos.x + mWallThickness, doorPos.y, doorPos.z + mLightOffsetFromDoor));
			Hollow::ResourceManager::Instance().LoadPrefabAtPosition("newLight", glm::vec3(doorPos.x - mWallThickness, doorPos.y, doorPos.z + mLightOffsetFromDoor));
			Hollow::ResourceManager::Instance().LoadPrefabAtPosition("newLight", glm::vec3(doorPos.x + mWallThickness, doorPos.y, doorPos.z - mLightOffsetFromDoor));
			Hollow::ResourceManager::Instance().LoadPrefabAtPosition("newLight", glm::vec3(doorPos.x - mWallThickness, doorPos.y, doorPos.z - mLightOffsetFromDoor));

    		// RIGHT WALL (second Part)
            Hollow::ResourceManager::Instance().LoadScaledPrefabAtPosition("Wall",
                glm::vec3(mRoomY * mRoomSize + mRoomSize, halfWallHeight, mRoomX * mRoomSize + (mRoomSize- halfWallLength)),
                glm::vec3(mWallThickness, mWallHeight, mWallLength));
    	}
        else
        {
            // RIGHT WALL
            Hollow::ResourceManager::Instance().LoadScaledPrefabAtPosition("Wall",
                glm::vec3(mRoomY * mRoomSize + mRoomSize, halfWallHeight, mRoomX * mRoomSize + halfRoomSize),
                glm::vec3(mWallThickness, mWallHeight, mRoomSize));
        }

        if (mDoorBits & 4)
        {
            // BOTTOM WALL (first Part)
            Hollow::ResourceManager::Instance().LoadScaledPrefabAtPosition("Wall",
                glm::vec3(mRoomY * mRoomSize + halfWallLength, halfWallHeight, mRoomX * mRoomSize + mRoomSize),
                glm::vec3(mWallLength, mWallHeight, mWallThickness));

            // DOOR
			glm::vec3 doorPos = glm::vec3(mRoomY * mRoomSize + halfRoomSize, halfDoorHeight, mRoomX * mRoomSize + mRoomSize);
            Hollow::GameObject* door = Hollow::ResourceManager::Instance().LoadScaledPrefabAtPosition("Door",
                                        doorPos,
                                        glm::vec3(mDoorWidth, mDoorHeight, mDoorThickness));
            door->GetComponent<Hollow::Collider>()->SetCustomBounds(glm::vec3(-0.5f, -1.6f, -2.0f),
                glm::vec3(0.5f, 1.6f, 2.0f));

        	// Rotate door 90 degrees to face the right way (needs to rotate both components for static objects)
            door->GetComponent<Hollow::Body>()->Rotate(glm::vec3(0.0f, 90.0f, 0.0f));
            door->GetComponent<Hollow::Transform>()->Rotate(glm::vec3(0.0f, 90.0f, 0.0f));

        	mDoorGOs[4] = door;

			// LIGHTS ON BOTH SIDES OF THE DOOR
			Hollow::ResourceManager::Instance().LoadPrefabAtPosition("newLight", glm::vec3(doorPos.x + mLightOffsetFromDoor, doorPos.y, doorPos.z + mWallThickness));
			Hollow::ResourceManager::Instance().LoadPrefabAtPosition("newLight", glm::vec3(doorPos.x + mLightOffsetFromDoor, doorPos.y, doorPos.z - mWallThickness));
			Hollow::ResourceManager::Instance().LoadPrefabAtPosition("newLight", glm::vec3(doorPos.x - mLightOffsetFromDoor, doorPos.y, doorPos.z + mWallThickness));
			Hollow::ResourceManager::Instance().LoadPrefabAtPosition("newLight", glm::vec3(doorPos.x - mLightOffsetFromDoor, doorPos.y, doorPos.z - mWallThickness));

            // BOTTOM WALL (second Part)
            Hollow::ResourceManager::Instance().LoadScaledPrefabAtPosition("Wall",
                glm::vec3(mRoomY * mRoomSize + (mRoomSize - halfWallLength), halfWallHeight, mRoomX * mRoomSize + mRoomSize),
                glm::vec3(mWallLength, mWallHeight, mWallThickness));
        }
        else
        {
            // BOTTOM WALL
            Hollow::ResourceManager::Instance().LoadScaledPrefabAtPosition("Wall",
                glm::vec3(mRoomY * mRoomSize + halfRoomSize, halfWallHeight, mRoomX * mRoomSize + mRoomSize),
                glm::vec3(mRoomSize, mWallHeight, mWallThickness));
        }
        
    }
	bool DungeonRoom::IsCleared()
	{
		if (GetEnemyCount() == 0)
			return true;
    	
		return false;
	}
	void DungeonRoom::UnlockRoom()
	{
		// TODO Replace with a nice animation of doors opening
        if (!mIsUnlocked)
        {
            for (int i = 0; i < 4; ++i)
            {
                if (mDoorGOs[1 << i] != nullptr)
                {
                    Hollow::Body* pB = mDoorGOs[1 << i]->GetComponent<Hollow::Body>();
                    pB->mPosition.y = -1.25f;
                    HW_TRACE("Door Opened {0}", 1 << i);
                }
            }
            mIsUnlocked = true;
			Hollow::AudioManager::Instance().PlayEffect("Resources/Audio/SFX/DoorUnlock.wav");
        }
	}
	void DungeonRoom::LockDownRoom()
	{
		// TODO Replace with a nice animation of doors closing
        if (!mIsLockedFirstTime)
        {
            for (int i = 0; i < 4; ++i)
            {
                if (mDoorGOs[1 << i] != nullptr && mDoorGOs[1 << i]->mActive)
                {
                    Hollow::Body* pB = mDoorGOs[1 << i]->GetComponent<Hollow::Body>();
                    pB->mPosition.y = 2.5f;
                }
            }

            // Delayed event to activate enemies in rooms
            Hollow::GameEvent* lde = new Hollow::GameEvent((int)GameEventType::ROOM_LOCKDOWN_DELAYED);
            Hollow::EventManager::Instance().AddDelayedEvent(lde, 1.0f);

            mIsLockedFirstTime = true;

			Hollow::AudioManager::Instance().PlayEffect("Resources/Audio/SFX/DoorLock.wav");
        }
	}

    int DungeonRoom::GetID() const
    {
        return mRoomID;
    }

    DungeonRoom::~DungeonRoom()
    {
        for (int i = 0; i < 9; i++)
            mDoorGOs[i] = nullptr;
    }

}
