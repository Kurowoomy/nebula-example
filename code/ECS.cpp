#pragma once

#include "ECS.h"
#include "io/jsonreader.h"
#include "io/filestream.h"

__ImplementClass(BaseEntity, 'BABC', Core::RefCounted);
__ImplementClass(King, 'BBCD', Core::RefCounted);
__ImplementClass(Soldier, 'BCDE', Core::RefCounted);
__ImplementClass(Spearman, 'BDEF', Core::RefCounted);
__ImplementClass(BaseComponent, 'AABC', Core::RefCounted);
__ImplementClass(TransformComponent, 'ABCD', Core::RefCounted);
__ImplementClass(GraphicsComponent, 'ACDE', Core::RefCounted);

inline std::size_t getComponentID()
{
	static std::size_t lastID = 0;
	return lastID++;
}
template<typename T>
inline std::size_t getComponentID() 
{
	static std::size_t typeID = getComponentID();
	return typeID;
}



Message::Message()
{
}
Message::~Message()
{
}
MoveMessage::MoveMessage()
{
}
MoveMessage::MoveMessage(message_type msg, float x, float y, float z)
{
	this->msg = msg;
	this->x = x;
	this->y = y;
	this->z = z;
	msgReciever = nullptr;
}

MoveMessage::MoveMessage(message_type msg, float x, float y, float z, BaseEntity* msgReciever)
{
	this->msg = msg;
	this->x = x;
	this->y = y;
	this->z = z;
	this->msgReciever = msgReciever;
}



void BaseEntity::init()
{
	for (auto& c : unique_components) c->init();
}
void BaseEntity::update()
{
	for (auto& c : unique_components) c->update();
}
void BaseEntity::shutdown()
{
	for (auto& c : unique_components) {
		c->shutdown();
	}
	unique_components.clear();
	this->Release();
}
template<typename T>
void BaseEntity::addComponent()
{
	unique_components.emplace_back(T::Create());
	unique_components.back()->AddRef();
	unique_components.back()->entity = this;
	componentArray[getComponentID<T>()] = unique_components.back();
}
void BaseEntity::handleMessage(Message* msg)
{
	for (auto& c : unique_components) c->handleMessage(msg);
}
void BaseEntity::sendMsg(Message* msg)
{
	if (msg->msgReciever)
	{
		msg->msgReciever->handleMessage(msg);
	}
	else
	{
		for (auto& e : entityManager->entities) e->handleMessage(msg);
	}
}
void BaseEntity::registerVariable(Util::StringAtom varName, Union value, Type type)
{
	if (variables.find(varName) == variables.end()) {
		switch (type) {
		case Type::Matrix44:
			variables[varName].matrix44 = value.matrix44;
			break;
		case Type::GraphicsEntityID:
			variables[varName].grEntID = value.grEntID;
			break;
		case Type::ResourceName:
			variables[varName].rscName = value.rscName;
			break;
		}
	}
}
void BaseEntity::setVariable(Util::StringAtom varName, Union value, Type type)
{
	switch (type) {
	case Type::Matrix44:
		variables[varName].matrix44 = value.matrix44;
		break;
	case Type::GraphicsEntityID:
		variables[varName].grEntID = value.grEntID;
		break;
	case Type::ResourceName:
		variables[varName].rscName = value.rscName;
		break;
	}
}
Union* BaseEntity::getVariable(Util::StringAtom varName)
{
	if (variables.find(varName) != variables.end()) {
		return &variables.find(varName)->second;
	}
	return nullptr;
}



void King::init()
{
	addComponent<TransformComponent>();
	addComponent<GraphicsComponent>();

	//register graphics components variables, if it has any
	Util::String compName;
	if (jsonVariables.find("modelName") != jsonVariables.end()) {
		compName = "mdl:" + jsonVariables["modelName"] + ".n3";
		registerVariable("modelName", compName, Type::ResourceName);
	}
	if (jsonVariables.find("skeletonName") != jsonVariables.end()) {
		compName = "ske:" + jsonVariables["skeletonName"] + ".nsk3";
		registerVariable("skeletonName", compName, Type::ResourceName);
	}
	if (jsonVariables.find("animationName") != jsonVariables.end()) {
		compName = "ani:" + jsonVariables["animationName"] + ".nax3";
		registerVariable("animationName", compName, Type::ResourceName);
	}

	for (auto& c : unique_components) c->init();
}
void King::update()
{
	for (auto& c : unique_components) c->update();
}
void King::shutdown()
{
	for (auto& c : unique_components) {
		c->shutdown();
	}
	unique_components.clear();
	this->Release();
}
//void Miner::moveAwayFromMe()
//{
//	messager.msg = msg_test;
//	for (auto& e : entityManager->entities)
//	{
//		if (e.get()->name == "miner2")
//		{
//			messager.sendMsg(e.get());
//		}
//	}
//}
void Soldier::init()
{
	addComponent<TransformComponent>();
	addComponent<GraphicsComponent>();

	Util::String compName;
	if (jsonVariables.find("modelName") != jsonVariables.end()) {
		compName = "mdl:" + jsonVariables["modelName"] + ".n3";
		registerVariable("modelName", compName, Type::ResourceName);
	}
	if (jsonVariables.find("skeletonName") != jsonVariables.end()) {
		compName = "ske:" + jsonVariables["skeletonName"] + ".nsk3";
		registerVariable("skeletonName", compName, Type::ResourceName);
	}
	if (jsonVariables.find("animationName") != jsonVariables.end()) {
		compName = "ani:" + jsonVariables["animationName"] + ".nax3";
		registerVariable("animationName", compName, Type::ResourceName);
	}

	for (auto& c : unique_components) c->init();
}
void Soldier::update()
{
	for (auto& c : unique_components) c->update();
}
void Soldier::shutdown()
{
	for (auto& c : unique_components) {
		c->shutdown();
	}
	unique_components.clear();
	this->Release();
}
void Spearman::init()
{
	addComponent<TransformComponent>();
	addComponent<GraphicsComponent>();

	Util::String compName;
	if (jsonVariables.find("modelName") != jsonVariables.end()) {
		compName = "mdl:" + jsonVariables["modelName"] + ".n3";
		registerVariable("modelName", compName, Type::ResourceName);
	}
	if (jsonVariables.find("skeletonName") != jsonVariables.end()) {
		compName = "ske:" + jsonVariables["skeletonName"] + ".nsk3";
		registerVariable("skeletonName", compName, Type::ResourceName);
	}
	if (jsonVariables.find("animationName") != jsonVariables.end()) {
		compName = "ani:" + jsonVariables["animationName"] + ".nax3";
		registerVariable("animationName", compName, Type::ResourceName);
	}

	for (auto& c : unique_components) c->init();
}
void Spearman::update()
{
	for (auto& c : unique_components) c->update();
}
void Spearman::shutdown()
{
	for (auto& c : unique_components) {
		c->shutdown();
	}
	unique_components.clear();
	this->Release();
}



void TransformComponent::init()
{
	entity->registerVariable("matrix44", Math::matrix44(), Type::Matrix44);
}
void TransformComponent::update()
{
	move(0, 0, 0.01); // yay it works
}
void TransformComponent::shutdown()
{
	this->Release();
}
void TransformComponent::setPos(float x, float y, float z)
{
	*entity->getVariable("matrix44")->matrix44 = entity->getVariable("matrix44")->matrix44->translation(x, y, z);
}
void TransformComponent::move(float x, float y, float z)
{
	entity->getVariable("matrix44")->matrix44->translate(Math::float4(x, y, z, 0));
}
void TransformComponent::handleMessage(Message* msg)
{
	MoveMessage* castedMessage = dynamic_cast<MoveMessage*>(msg);
	if (castedMessage) //responds only if it's a MoveMessage
	{
		switch (msg->msg) {
		case message_type::msg_move:
			move(castedMessage->x, castedMessage->y, castedMessage->z);
			break;
		default:
			break;
		}
	}
}



Union::Union()
{
}
Union::Union(const Union& u)
{
}
Union::Union(const Math::matrix44& m)
{
	this->matrix44 = new Math::matrix44(m);
}
Union::Union(const Graphics::GraphicsEntityId& g)
{
	this->grEntID = Graphics::CreateEntity();
}
Union::Union(const char* r)
{
	this->rscName = r;
}
Union::Union(const Util::String s)
{
	this->rscName = s;
}
Union::~Union()
{
}
void Union::operator=(const Union& val)
{
}
void Union::operator=(const Math::matrix44& val)
{
	this->matrix44 = new Math::matrix44(val);
}



void EntityManager::init()
{
	for (auto& e : Get().entities) e->init();
}
void EntityManager::update()
{
	for (auto& e : Get().entities) e->update();
}
void EntityManager::shutdown()
{
	for (auto& e : Get().entities) {
		e->shutdown();
	}
	Get().entities.clear();
	Get().jsonr->Close();
	Get().jsonr->GetStream()->Release();
	Get().jsonr->Release();
}



void GraphicsComponent::init()
{
	Characters::CharacterContext::Create();
	Models::ModelContext::Create();
	Visibility::ObservableContext::Create();
	entity->registerVariable("graphicsEntityID", Graphics::GraphicsEntityId(), Type::GraphicsEntityID);

	//if entity has skeleton, register skeleton
	if (entity->getVariable("skeletonName")) {
		Graphics::RegisterEntity<Models::ModelContext, Visibility::ObservableContext, Characters::CharacterContext>(entity->getVariable("graphicsEntityID")->grEntID);
	}
	else
		Graphics::RegisterEntity<Models::ModelContext, Visibility::ObservableContext>(entity->getVariable("graphicsEntityID")->grEntID);
	Models::ModelContext::Setup(entity->getVariable("graphicsEntityID")->grEntID, entity->getVariable("modelName")->rscName, "Examples");
	Models::ModelContext::SetTransform(entity->getVariable("graphicsEntityID")->grEntID, *entity->getVariable("matrix44")->matrix44);
	Visibility::ObservableContext::Setup(entity->getVariable("graphicsEntityID")->grEntID, Visibility::VisibilityEntityType::Model);
	//if entity has skeleton, add animation
	if (entity->getVariable("skeletonName")) {
		Characters::CharacterContext::Setup(entity->getVariable("graphicsEntityID")->grEntID, entity->getVariable("skeletonName")->rscName, entity->getVariable("animationName")->rscName, "Examples");
		Characters::CharacterContext::PlayClip(entity->getVariable("graphicsEntityID")->grEntID, nullptr, 0, 0, Characters::Append, 1.0f, 1, Math::n_rand() * 100.0f, 0.0f, 0.0f, Math::n_rand() * 100.0f);
	}
}
void GraphicsComponent::update()
{
	Models::ModelContext::SetTransform(entity->getVariable("graphicsEntityID")->grEntID, *entity->getVariable("matrix44")->matrix44); //update transform
}
void GraphicsComponent::shutdown()
{
	this->Release();
}
void GraphicsComponent::handleMessage(Message* msg)
{
}
