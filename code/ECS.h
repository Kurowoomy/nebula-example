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

//my own additions-----
#include "io/jsonreader.h"
#include "io/filestream.h"
#include <type_traits>  //for when registering variables from json in addEntity()


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

class BaseEntity : public Core::RefCounted
{
	__DeclareClass(BaseEntity)
	std::map<Util::StringAtom, Union> variables; //register variables for the components here
public:
	std::vector<BaseComponent*> unique_components; //refcount and looping
	std::array<BaseComponent*, 32> componentArray; //accessing a component
	std::string name; //finding a specific entity
	EntityManager* entityManager; //accessing the other entities
	Message messager;

	std::map<Util::String, Util::String> jsonVariables;

	virtual void init();
	virtual void update();
	virtual void shutdown();

	template<typename T> void addComponent(); //just specify entity type
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

//singleton
class EntityManager
{
private:
	EntityManager() {
		jsonr = IO::JsonReader::Create();
		jsonr->AddRef();
		Ptr<IO::FileStream> stream = IO::FileStream::Create();
		stream->SetURI("C:/Users/emmeli-8-local/Documents/S0016D_Spelmotorarkitektur/nebula-env/nebula-example/code/variables.json");
		jsonr->SetStream(stream);
		jsonr->GetStream()->AddRef();

		jsonr->Open();
		for (auto& node : jsonr->GetAttrs()) {
			jsonr->SetToRoot();
			jsonr->SetToNode(node);
			entNumbers.emplace(node, jsonr->GetInt("num"));
		}
	}

public:
	std::vector<BaseEntity*> entities;
	std::map<Util::String, int> entNumbers;
	IO::JsonReader* jsonr;

	static EntityManager& Get() {
		static EntityManager instance;
		return instance;
	}
	std::vector<BaseEntity*> getEntities() {
		return EntityManager::Get().entities;
	}
	int getNumOfEntities(std::string s) {
		Util::String str = s.c_str();
		return EntityManager::Get().entNumbers[str];
	}

	static void init();
	static void update();
	static void shutdown();

	template<typename T> 
	static void addEntity(std::string name) //unresolved external symbol if declared in cpp
	{
		Get().entities.emplace_back(T::Create());
		Get().entities.back()->AddRef();
		Get().entities.back()->name = name;
		Get().entities.back()->entityManager = &Get();

		//json
		//Problem: Can't open json :(
		//Solution: Don't open json :)
		//set jsonr to the right node, add variables to entity's dictionary
		Get().jsonr->SetToRoot();
		if (std::is_same_v<T, King>) {
			Get().jsonr->SetToNode("entity1");
			Get().jsonr->SetToNode("graphicsComponents");
			for (auto& g : Get().jsonr->GetAttrs()) {
				const char* c = g.AsCharPtr();
				Get().entities.back()->jsonVariables.emplace(g, Get().jsonr->GetString(c));
			}
		}
		else if(std::is_same_v<T, Soldier>) {
			Get().jsonr->SetToNode("entity2");
			Get().jsonr->SetToNode("graphicsComponents");
			for (auto& g : Get().jsonr->GetAttrs()) {
				const char* c = g.AsCharPtr();
				Get().entities.back()->jsonVariables.emplace(g, Get().jsonr->GetString(c));
			}
		}
		else if(std::is_same_v<T, Spearman>) {
			Get().jsonr->SetToNode("entity3");
			Get().jsonr->SetToNode("graphicsComponents");
			for (auto& g : Get().jsonr->GetAttrs()) {
				const char* c = g.AsCharPtr();
				Get().entities.back()->jsonVariables.emplace(g, Get().jsonr->GetString(c));
			}
		}
	}

	template<typename T>
	static T* getEntity(std::string name) //unresolved external symbol if declared in cpp
	{
		for (auto& e : Get().entities) {
			if (e->name == name) {
				return (T*)e;
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
	Union(const Util::String);
	~Union();

	void operator=(const Union& val);
	void operator=(const Math::matrix44& val);

};

class King : public BaseEntity
{
	__DeclareClass(King)
public:
	void init();
	void update();
	void shutdown();

	//void moveAwayFromMe(); //message example
};
class Soldier : public BaseEntity
{
	__DeclareClass(Soldier)
public:
	void init();
	void update();
	void shutdown();

	//void moveAwayFromMe(); //message example
};
class Spearman : public BaseEntity
{
	__DeclareClass(Spearman)
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

