/*********************************************************
** RegionDataParser.h 
** Primary author: Landon K
** Description: Parses regiondata file from scenario editor.
*********************************************************/
#include "xmlParser.h"
#include "SimpleScanner.h"
//#include "BoundingBox.h"
#include "vector.h"
#include "entity.h"
#include "region.h"
#include "fileexists.h"
#include "entityfactory.h"
#include <iostream>

#include "BB.h"

#include <vector>
#include <cmath>


//returns agent position based on grid-position in a region
//NOTE: Must be run through CEntity::AnchorToPlane() 
CVector gridToPos(int row, int col, const BoundingBox& BB)
{
	float startx = BB.minx + (Entity::LARGEST_ENTITY_RADIUS);
    float starty = BB.miny + (Entity::LARGEST_ENTITY_RADIUS);
    CVector pos(startx + ((float)col * (Entity::LARGEST_ENTITY_RADIUS * 2.3)), starty + ((float)row * Entity::LARGEST_ENTITY_RADIUS * 2.3), 0);
    return pos;
}	
	
	

//Parses Region Data File
//
//Which as of this version entails: 
//1. Loading Agent Positions
//
//In the future, this functions will also:
//1. Load Region Pathing Types
//2. Load Profiles (if necessary)
bool loadRegionData(std::string path, std::vector<CRegion*>* regionList = NULL)
{
     if(!textFileExists(path) || !regionList) 
          return false;
	 
     XMLNode rootNode = XMLNode::openFileHelper(path.c_str(), "Regiondata");
      
     if(rootNode.isEmpty())
         return false;
     
     XMLNode childNode;	 
	 
	 /*If we need to support profiles
	      Profile Parsing goes here*/
	 
	 childNode = rootNode.getChildNode("Regions");
     
     if(!childNode.isEmpty())
     {
		int regionCount = std::min(childNode.nChildNode("region"), int(regionList->size()));
		
        for(int i = 0; i < regionCount; i++)
		{
			XMLNode regionNode = childNode.getChildNode("region", i);
			
			if(regionNode.isEmpty() || !regionNode.isAttributeSet("id"))
				continue;
				
			std::string buffer = regionNode.getAttribute("id");
	
			if(buffer.length() <= 0)
				continue;
			
			int regionId = std::atoi(buffer.c_str());
    			
            
			if((regionId < 0) || (regionId > regionList->size()))
				continue;
			
			CRegion* region = (*regionList)[regionId];

			if(!region || !(region->Polygon))
				continue;
			
			BoundingBox BB = region->Polygon->GetBoundingBox();
				
			int rowCount = regionNode.nChildNode("row");
			
			for(int row = 0; (row < rowCount); row++)
			{
				XMLNode rowNode = regionNode.getChildNode("row", row);
				
				if(rowNode.isEmpty())
					continue;
				
                if(rowNode.nText() <= 0)
                    continue;
                
                std::string buffer = rowNode.getText();
                
                if(buffer.length() <= 0)
                    continue;				

				SimpleScanner scan;
				
				scan.compile(buffer, " ");
				
				int col = 0;
           
				while(scan.hasNextInt())
				{
                    int n;
					
					if(!scan.protectedNextInt(n))                                    
						continue;
					
					
					if(n)
                    {
                        CVector pos = gridToPos(row, col, BB);
                        if(ENTITYFACTORY)
                          ENTITYFACTORY->SpawnEntityAt(pos, regionId, SPECTATOR);
                    }
					col++;
				}
			}
		}
	}
			
}




