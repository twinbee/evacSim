#ifndef ENTITYFACTORY_H
#define ENTITYFACTORY_H

#include <vector>

#include "vector.h"
#include "entity.h"

#define ENTITYFACTORY EntityFactory::getCurrentInstance()


class CRegion;

class EntityFactory
{
public:
    
    static EntityFactory* getCurrentInstance(){return EntityFactory::currentInstance;}   

    EntityFactory (std::vector <CRegion *> regions, std::vector <int> safe);

    void SpawnEntitiesSmart(double toSpawn);

    void EntityFactory::SpawnEntitiesByTotal(int Total);

    void EntityFactory::SpawnEntitiesByCount(int RegionIndex, int count, entity_type et);
    
    void EntityFactory::SpawnBleachers(int count);

    void EntityFactory::SpawnEntityAt(CVector pos, unsigned int RegionIndex,entity_type et);

    inline int GetNumSpawned(){
        return NumSpawned;
    }

private:
    
    static EntityFactory* currentInstance;
    
    std::vector <CRegion *> RegionVec;
    std::vector <int> SafeRegions;
    int NumSpawned, ceiling;
};


#endif
