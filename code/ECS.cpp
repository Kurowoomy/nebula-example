#pragma once

#include "ECS.h"

__ImplementClass(BaseEntity, 'BABC', Core::RefCounted);
__ImplementClass(Miner, 'BBCD', Core::RefCounted);
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
Message::Message(message_type msg)
{
	this->msg = msg;
}
void Message::sendMsg(BaseEntity* msgReciever)
{
	msgReciever->handleMessage(*this);
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
		c.release();
	}
	unique_components.clear();
	this->Release();
}
template<typename T>
T& BaseEntity::addComponent()
{
	T* unique_component = T::Create();
	unique_component->AddRef();
	unique_component->entity = this;
	unique_components.emplace_back(unique_component);

	componentArray[getComponentID<T>()] = unique_component;
	return *unique_component;
}
void BaseEntity::handleMessage(Message msg)
{
	for (auto& c : unique_components) c->handleMessage(msg);
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



void Miner::init()
{
	addComponent<TransformComponent>();

	addComponent<GraphicsComponent>();
	//different model names for each class type
	registerVariable("modelName", "mdl:Units/unit_king.n3", Type::ResourceName);
	registerVariable("skeletonName", "ske:Units/unit_king.nsk3", Type::ResourceName);
	registerVariable("animationName", "ani:Units/unit_king.nax3", Type::ResourceName);

	for (auto& c : unique_components) c->init();
}
void Miner::update()
{
	for (auto& c : unique_components) c->update();
}
void Miner::shutdown()
{
	for (auto& c : unique_components) {
		c->shutdown();
		c.release();
	}
	unique_components.clear();
	this->Release();
}
void Miner::moveAwayFromMe()
{
	messager.msg = msg_test;
	for (auto& e : entityManager->entities)
	{
		if (e.get()->name == "miner2")
		{
			messager.sendMsg(e.get());
		}
	}
}



void TransformComponent::init()
{
	entity->registerVariable("matrix44", Math::matrix44(), Type::Matrix44);
}
void TransformComponent::update()
{
	move(0.01, 0, 0); // yay it works
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
void TransformComponent::handleMessage(Message msg)
{
	switch (msg.msg) {
	case message_type::msg_test:
		move(0, 0, 1);
		break;
	default:
		break;
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
	for (auto& e : entities) e->init();
}
void EntityManager::update()
{
	for (auto& e : entities) e->update();
}
void EntityManager::shutdown()
{
	for (auto& e : entities) {
		e->shutdown();
		e.release();
	}
	entities.clear();
}



void GraphicsComponent::init()
{
	Characters::CharacterContext::Create();
	Models::ModelContext::Create();
	Visibility::ObservableContext::Create();
	entity->registerVariable("graphicsEntityID", Graphics::GraphicsEntityId(), Type::GraphicsEntityID);

	//if entity has skeleton, register skeleton
	if (entity->getVariable("skeletonName")->rscName.Value() != nullptr) {
		Graphics::RegisterEntity<Models::ModelContext, Visibility::ObservableContext, Characters::CharacterContext>(entity->getVariable("graphicsEntityID")->grEntID);
	}
	else
		Graphics::RegisterEntity<Models::ModelContext, Visibility::ObservableContext>(entity->getVariable("graphicsEntityID")->grEntID);
	Models::ModelContext::Setup(entity->getVariable("graphicsEntityID")->grEntID, entity->getVariable("modelName")->rscName, "Examples");
	Models::ModelContext::SetTransform(entity->getVariable("graphicsEntityID")->grEntID, *entity->getVariable("matrix44")->matrix44);
	Visibility::ObservableContext::Setup(entity->getVariable("graphicsEntityID")->grEntID, Visibility::VisibilityEntityType::Model);
	//if entity has skeleton, add animation
	if (entity->getVariable("skeletonName")->rscName.Value() != nullptr) {
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
void GraphicsComponent::handleMessage(Message msg)
{
}
