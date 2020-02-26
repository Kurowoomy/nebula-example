#pragma once

#include "stdneb.h"
#include "core/refcounted.h"
#include "timing/timer.h"
#include "io/console.h"
#include "visibility/visibilitycontext.h"
#include "models/streammodelpool.h"
#include "models/modelcontext.h"
#include "input/keyboard.h"
#include "input/mouse.h"
#include "math/vector.h"
#include "math/point.h"
#include "dynui/imguicontext.h"
#include "lighting/lightcontext.h"
#include "characters/charactercontext.h"
#include "imgui.h"
#include "dynui/im3d/im3dcontext.h"
#include "dynui/im3d/im3d.h"
#include "graphics/environmentcontext.h"
#include "clustering/clustercontext.h"
#include "io/debug/iopagehandler.h"
#include "io/logfileconsolehandler.h"
#include "io/debug/consolepagehandler.h"
#include "core/debug/corepagehandler.h"
#include "threading/debug/threadpagehandler.h"
#include "memory/debug/memorypagehandler.h"
#include "io/debug/iopagehandler.h"
#include "io/fswrapper.h"
#include "system/nebulasettings.h"

class BaseComponent;
class BaseEntity;
class EntityManager;
struct Union;

//union types for component variables
enum Type {
	Matrix44,
	GraphicsEntityID,
	ResourceName
};
//message types, no functionality at the moment
enum message_type {
	msg_test, 
	msg_move
};

//for getComponent method
inline std::size_t getComponentID();
template<typename T> inline std::size_t getComponentID();



class Message {
public:
	message_type msg;
	BaseEntity* msgReciever;

	Message();
	virtual ~Message(); //for allowing dynamic casting
};
class MoveMessage : public Message
{
public:
	float x, y, z;

	MoveMessage();
	MoveMessage(message_type msg, float x, float y, float z);
	MoveMessage(message_type msg, float x, float y, float z, BaseEntity* msgReciever);

	
};
//TODO: create move message that inherits from Message, add specific variables in it. 
//move : Message*
//varför pekare? kanske inte behövs.

class BaseEntity : public Core::RefCounted
{
	__DeclareClass(BaseEntity)
	std::map<Util::StringAtom, Union> variables; //register variables for the components here
public:
	std::vector<std::unique_ptr<BaseComponent>> unique_components; //refcount and looping
	std::array<BaseComponent*, 32> componentArray; //accessing a component
	std::string name; //finding a specific entity
	EntityManager* entityManager; //accessing the other entities
	Message messager;

	virtual void init();
	virtual void update();
	virtual void shutdown();

	template<typename T> T& addComponent(); //just specify entity class
	template<typename T> 
	T& getComponent() //unresolved external symbol if declared in cpp
	{
		auto ptr(componentArray[getComponentID<T>()]);
		return *static_cast<T*>(ptr);
	}

	virtual void handleMessage(Message* msg); //sends msg to all its components
	virtual void sendMsg(Message* msg);

	void registerVariable(Util::StringAtom varName, Union value, Type type); //same as setVariable but checks duplicates
	void setVariable(Util::StringAtom varName, Union value, Type type);
	Union* getVariable(Util::StringAtom varName);
};

class EntityManager
{
public:
	std::vector<std::unique_ptr<BaseEntity>> entities;

	void init();
	void update();
	void shutdown();

	template<typename T> 
	void addEntity(std::string name) //unresolved external symbol if declared in cpp
	{
		T* newEntity = T::Create();
		newEntity->AddRef();
		newEntity->name = name;
		newEntity->entityManager = this;
		entities.emplace_back(newEntity);
	}

	template<typename T>
	T* getEntity(std::string name) //unresolved external symbol if declared in cpp
	{
		for (auto& e : entities) {
			if (e.get()->name == name) {
				return (T*)e.get();
			}
		}
		return nullptr;
	}
};

class BaseComponent : public Core::RefCounted
{
	__DeclareClass(BaseComponent)

public:
	BaseEntity* entity; //reference to owner of this component

	virtual void init() {}
	virtual void update() {}
	virtual void shutdown() {}

	virtual void handleMessage(Message* msg) {}
};

class TransformComponent : public BaseComponent
{
	__DeclareClass(TransformComponent)
public:
	void init();
	void update();
	void shutdown();

	void setPos(float x, float y, float z); //translation
	void move(float x, float y, float z); //translate
	void handleMessage(Message* msg);
};

struct Union {
	Type type;
	union {
		Math::matrix44* matrix44;
		Graphics::GraphicsEntityId grEntID;
		Resources::ResourceName rscName;
	};

	Union();
	Union(const Union& u);
	Union(const Math::matrix44& m);
	Union(const Graphics::GraphicsEntityId& g);
	Union(const char* r); //for Resources::ResourceName
	~Union();

	void operator=(const Union& val);
	void operator=(const Math::matrix44& val);

};

class Miner : public BaseEntity
{
	__DeclareClass(Miner)
public:
	void init();
	void update();
	void shutdown();

	//void moveAwayFromMe(); //message example
};



class GraphicsComponent : public BaseComponent //depends on TransformComponent
{
	__DeclareClass(GraphicsComponent)

public:
	void init();
	void update();
	void shutdown();

	void handleMessage(Message* msg);
};


