import ECSmodule
import random

ent = ECSmodule.EntityManager()
for i in range(ent.getNumOfEntities("entity1")):
    name = "king" + str(i)
    ent.addEntityKing(name)
for i in range(ent.getNumOfEntities("entity2")):
    name = "soldier" + str(i)
    ent.addEntitySoldier(name)
for i in range(ent.getNumOfEntities("entity3")):
    name = "spearman" + str(i)
    ent.addEntitySpearman(name)
ent.init()

for entity in ent.getEntities():
    entity.getComponent().move(random.randint(-8, 5), 0, random.randint(-8, 5))
    entity.getGraphicsComponent().update()
