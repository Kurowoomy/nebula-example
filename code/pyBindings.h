#pragma once

#include "pybind11/pybind11.h"
#include "pybind11/embed.h"
#include "pybind11/eval.h"
#include "ECS.h"


PYBIND11_MAKE_OPAQUE(std::vector<BaseEntity*>);

PYBIND11_EMBEDDED_MODULE(ECSmodule, m) {
	pybind11::class_<std::vector<BaseEntity*>>(m, "entityVector")
		.def(pybind11::init<>())
		.def("__iter__", [](std::vector<BaseEntity*>& v) {
		return pybind11::make_iterator(v.begin(), v.end());
			}, pybind11::keep_alive<0, 1>());

	pybind11::class_<TransformComponent>(m, "TransformComponent")
		.def(pybind11::init<>())
		.def("setPos", &TransformComponent::setPos, pybind11::return_value_policy::reference, "set position x, y, z")
		.def("move", &TransformComponent::move, pybind11::return_value_policy::reference, "translate x, y, z");
	pybind11::class_<GraphicsComponent>(m, "GraphicsComponent")
		.def(pybind11::init<>())
		.def("update", &GraphicsComponent::update, pybind11::return_value_policy::reference, "update graphics");

	pybind11::class_<BaseEntity>(m, "BaseEntity")
		.def(pybind11::init<>())
		.def("getComponent", &BaseEntity::getComponent<TransformComponent>, pybind11::return_value_policy::reference, "get transformComponent")
		.def("getGraphicsComponent", &BaseEntity::getComponent<GraphicsComponent>, pybind11::return_value_policy::reference, "get graphicsComponent");
	pybind11::class_<King, BaseEntity>(m, "King")
		.def(pybind11::init<>());
	pybind11::class_<Soldier, BaseEntity>(m, "Soldier")
		.def(pybind11::init<>());
	pybind11::class_<Spearman, BaseEntity>(m, "Archer")
		.def(pybind11::init<>());

	pybind11::class_<EntityManager>(m, "EntityManager")
		.def(pybind11::init([]() { return EntityManager::Get(); }))
		.def_static("addEntityKing", &EntityManager::addEntity<King>, pybind11::return_value_policy::reference, "add a king")
		.def_static("addEntitySoldier", &EntityManager::addEntity<Soldier>, pybind11::return_value_policy::reference, "add a soldier")
		.def_static("addEntitySpearman", &EntityManager::addEntity<Spearman>, pybind11::return_value_policy::reference, "add a spearman")
		.def("getEntities", &EntityManager::getEntities, pybind11::return_value_policy::reference, "getAllEntities")
		.def("getNumOfEntities", &EntityManager::getNumOfEntities, "get number of entities of type entity1, entity2, etc")
		.def_static("init", &EntityManager::init, "initiate all entities");
}