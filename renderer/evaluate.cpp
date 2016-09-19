#include "main.h"

#include <vector>
#include <algorithm> //reverse a vector

#include <string>

#include "stringutil.h"

#include "camsutil.h"

#include "convert.h"

#include "datetime.h"

#include "script.h"

#include "fileexists.h"

#include "frustrum.h"

#include "textbuffer.h"

#include "stringbuffer.h"

#include "map.h"

#include "model.h"

#include "RegionDataParser.h"

#include "sound.h"


using namespace std;

bool wascapturing = false;

 void EvaluatePrompt()
{
    textbuffer.InsertLine(stringbuffer.getData(), COMMAND);
    
    vector <string> tokens = tokenize(stringbuffer.getData(), " ");
    int n = tokens.size();
    if (n < 1) return;
    stringbuffer.clear();

    const string objprefix ("..\\obj\\");

    string cmd (lowercase(tokens.at(0) ));

    {
        if (cmd == "hold")
        {
         g_HOLD = true;
         return;
        }

        ////////////HELP SECTION /////////////////////////////////

        if (cmd == "help")
        {
         if (tokens.size() > 1 && tokens[1] == "at")
         {
          textbuffer.InsertLine("> at <percent> <command>");          
          textbuffer.InsertLine(">> Executes <command> after <percent> of agents have made egress. ");          
         }
         else if (tokens.size() > 1 && tokens[1] == "load" )
         {
          textbuffer.InsertLine("> load <filename> <spawnrate>");          
          textbuffer.InsertLine(">> Loads a pathing map and looks for a corresponding detail map. ");
          textbuffer.InsertLine(">> <filename> is an obj file containing a pathing map in the obj subfolder. ");
          textbuffer.InsertLine(">> <spawnrate> is an optional parameter that will spawn per all regions. ");
         }
         else if (tokens.size() > 1 && tokens[1] == "model")
         {
          textbuffer.InsertLine("> model <filename>");          
          textbuffer.InsertLine(">> Loads a detail map alone. ");
          textbuffer.InsertLine(">> <filename> is an obj file containing a pathing map in the obj subfolder. ");
         }
         else if (tokens.size() > 1 && tokens[1] == "script")
         {
          textbuffer.InsertLine("> script <filename>");          
          textbuffer.InsertLine(">> Loads a pathing map and looks for a corresponding detail map. ");
          textbuffer.InsertLine(">> <filename> is an obj file containing a pathing map in the script subfolder. ");
         }
         else if (tokens.size() > 1 && tokens[1] == "highlight")
         {
          textbuffer.InsertLine("> highlight <region>");          
          textbuffer.InsertLine(">> Highlights a particular region for easy viewing. ");
         }
          else
          {
            textbuffer.InsertLine(">> HINT: use 'help <command>' for extended help on that command");
            textbuffer.InsertLine("> at <percent> <command>");
            textbuffer.InsertLine("> demo");
            textbuffer.InsertLine("> label [regionids | components]");
            textbuffer.InsertLine("> load <filename> <spawnrate>");
            textbuffer.InsertLine("> loaddetail <filename>");
            textbuffer.InsertLine("> makevid");
            textbuffer.InsertLine("> res <resolution>");
            textbuffer.InsertLine("> script <scriptfile>");
            textbuffer.InsertLine("> sim");
            textbuffer.InsertLine("> spawn <spawnrate> <regionID>");
            textbuffer.InsertLine("> quit");
          } 
            return;
        }

        ////////////END HELP SECTION /////////////////////////////////
        
        if(cmd == "hl")
        {
         if (n == 2)
         {
          if(!g_Map) return;
          if (tokens[1] == "off")        g_Map->HighlightsOff();
         }
         else if (n < 3)
         {
          stringbuffer.setstr("help hl");
          EvaluatePrompt();
          return;
         }

            if(!g_Map)
            {
             textbuffer.InsertLine(">> Map is not loaded <<");
             return;
            } 

            if (tokens[1] == "shortest")        g_Map->HighlightOptimal(toInt(tokens[2]));
            else if (tokens[1] == "next")       g_Map->HighlightExitPath(toInt(tokens[2]));
            //else if (tokens[1] == "vis")        g_Map->HighlightVisSet(toInt(tokens[2]));
            else if (tokens[1] == "vicinity")   g_Map->HighlightVicinity(toInt(tokens[2]));
            else if (tokens[1] == "region")     g_Map->HighlightRegion(toInt(tokens[2]));
            else if (tokens[1] == "path")
            {
              if (n < 3)
              {
               stringbuffer.setstr("help highlightpath");
               EvaluatePrompt();
               return;
              }
  
              if(!g_Map)
              {
               textbuffer.InsertLine(">> Map is not loaded. <<");
               return;
              } 
  
              int Index1 = toInt(tokens[1]);
              int Index2 = toInt(tokens[2]);
  
              if ( Index1 < 0 || Index1 >= g_Map->NumRegions())
              {
               textbuffer.InsertLine("> Invalid Region ID: Argument1 <");
               return;
              }
  
              if ( Index2 < 0 || Index2 >= g_Map->NumRegions())
              {
               textbuffer.InsertLine("> Invalid Region ID: Argument2 <");
               return;
              }
  
              if (g_Map) g_Map->HighlightPathInclusive(Index1,Index2);
              return;
            }

            else
            {
             stringbuffer.setstr("help hl");
             EvaluatePrompt();
            }
            return;
        }

        if(cmd == "color")
        {
         if (n < 2)
         {
          stringbuffer.setstr("help color");
          EvaluatePrompt();
          return;
         }

            if(!g_Map) return;
            if (tokens[1] == "regions")    g_Map->SetColorMode(COLOR_REGIONS);
            else if (tokens[1] == "components") g_Map->SetColorMode(COLOR_COMPONENTS);          
            else if (tokens[1] == "errors")     g_Map->SetColorMode(COLOR_ERRORS);
            else 
            {
             stringbuffer.setstr("help color");
             EvaluatePrompt();
            }
            return;
        }

        if ( cmd == "makefloor" )
        {
          if (n != 2 || !g_Map) return;
           g_Map->floorRegion(toInt(tokens[1]));
          return;
        }

        if ( cmd == "fps")
        {
          if (n > 1 && tokens[1] == "on")       drawFPS = true;
          else if (n > 1 && tokens[1] == "off") drawFPS = false;
          else drawFPS = !drawFPS;
          return;
        }
        
        if( cmd == "avoiddead")
        {
          if(g_Map) g_Map->PathAroundDead();
          return; 
        }
        
        if( cmd == "barrierdead")
        {
          if(g_Map) g_Map->BarrierDead();
          return;
        }

        if( cmd == "barrierbomb")
        {
          if(g_Map) g_Map->BarrierType(BOMB_TECH);
          return;
        }
        
        if (cmd == "loadomap")
        {
            if (n != 3)
            {
                stringbuffer.setstr("help loadomap");
                EvaluatePrompt();
                return;
            }

            string temp = "> " + datetime();
            temp += " Loading occlusion map " + tokens[1] + " ...";
            textbuffer.InsertLine(temp);
            RenderMenu();

            if (g_Omap)
            {
                textbuffer.InsertLine("> Deleting Previous Occlusion Map ...");
                delete g_Omap;
                g_Omap = 0;
                textbuffer.tag("done <");
                RenderMenu();
            }

            string filepath = objprefix + tokens[1];

            if (!textFileExists( filepath ))
            {
                textbuffer.InsertLine("> No Occlusion map found. <");
                return;
            }
            else
            {
                int Leafsize = toInt(tokens[2]);
                g_Omap = new CBSPtree( filepath.c_str(),Leafsize );
                g_Draw_Occlusion_Map = false;
                textbuffer.InsertLine("> " + datetime() + " Compiled Occlusion Map. <");
            }
            
            if((!g_Map) && (!g_Model))
            {
              CVector CamPos = g_Omap->GetCenter();
              g_camera.SetPosition(CamPos.x,CamPos.y,CamPos.z);
            }
            g_MAIN_STATE = CAMERAMODE;
            
            if(g_Map) g_Map->RegisterOMap(g_Omap);

            return;

        }//end if loadomap
        
        if (cmd == "model")
        {
         if (n != 2)
         {
          stringbuffer.setstr("help model");
          EvaluatePrompt();
          return;
         }

         string temp;
         temp += "> Loading detail map " + tokens[1] + " ...";
         textbuffer.InsertLine(temp);
         RenderMenu();

         if (g_Model)
         {
          textbuffer.InsertLine("> Deleting Detail Map ...");
          delete g_Model;
          g_Model = 0;
          textbuffer.tag("done <");
          RenderMenu();  
         }
  
         string filepath = objprefix + tokens[1];

         if (!textFileExists( filepath ))
                {
                    textbuffer.InsertLine("> No detail map found. <");
                    return;
                }
         else
                {
                     g_Model = new Model( filepath.c_str() );
                     g_Draw_Detail_Map = true;
                }

                g_SimTime = 0;
                WallStart = SDL_GetTicks();            
                
                Vec3 CamPos = g_Model->boundingBox().center();
                g_camera.SetPosition(CamPos.x,CamPos.y,CamPos.z);
                g_MAIN_STATE = CAMERAMODE;

                return;

        }//end if loaddetail


//        if(strcmp(token, "obj2nav") == 0)
//        {
//            token1 = strtok(NULL," ");
//            
//            char prefix2[10] = "..\\obj\\";
//            strcat(prefix2, token1);
//            token1 = prefix2;
//
//            if (token1)
//            {
//                if (!textFileExists(token1))
//                {
//                    strcpy(temp, "> cannot open obj: ");
//                    strcat(temp, token1);
//                    strcat(temp, " <");             
//                    textbuffer.InsertLine(temp);
//                    return;
//                } 
//            }
//            else
//            {
//                textbuffer.InsertLine("obj2nav <objfilename> <navfilename>");
//                return;           
//            }
//
//            token2 = strtok(NULL," ");
//            
//            char prefix[10] = "..\\obj\\";
//            strcat(prefix, token2);
//            token2 = prefix;
//
//            if(!token2)
//            {
//                textbuffer.InsertLine("obj2nav <objfilename> <navfilename>");
//                return;           
//            }
//
//            strcpy(temp, "> compiling NAV file ");
//            strcat(temp, token2);
//            strcat(temp, " ... <");
//            textbuffer.InsertLine(temp);
//            RenderMenu();
//
//            if(!OBJtoNAV(token1,token2))
//            {
//                textbuffer.tag("failed. <");
//            }
//            else
//                textbuffer.tag("completed. <");
//
//            return;
//        }//end if obj2nav


        if (cmd == "load" || cmd == "loadsim" )
        {
         if (n < 2)
         {
          stringbuffer.setstr("help load");
          EvaluatePrompt();
          return;
         }

         string filepath = objprefix + tokens[1];

             if ( !textFileExists( filepath ))
             {
              string errornsf = ">> No such file: " + tokens[1] + " <<";
              textbuffer.InsertLine(errornsf);           
              return;
             }
                        
             RenderMenu();
             g_MAIN_STATE = CAMERAMODE;

            if (g_Map)
            {
                textbuffer.InsertLine("> Deleting Pathing Map ... "); 
                RenderMenu();   

                delete g_Map;
                g_Map = 0;

                textbuffer.tag("done <"); 
                RenderMenu();   
            }

     
            if (g_Model)
            {
                textbuffer.InsertLine("> Deleting Detail Map ... "); 
                RenderMenu();  

                delete g_Model;
                g_Model = 0;

                textbuffer.tag("done <"); 
                RenderMenu();  
            }

            textbuffer.InsertLine("> Loading Pathing Map " + filepath + " ... "); 
            RenderMenu();  

            if (n == 3) g_Map = new CMap(filepath.c_str(),toDouble(tokens[2]));
            else
            if (n == 2) g_Map = new CMap(filepath.c_str());
            
            textbuffer.tag("done <"); 
            RenderMenu();  

            g_SimTime = 0;
            WallStart = SDL_GetTicks();            
            
            if (g_Map) g_camera.SetPosition(g_Map->GetCenter().x, g_Map->GetCenter().y, g_Map->GetCenter().z);
            
            g_bdrawcursor = true;
            if (g_Map) g_Map->Update(SimStepSize);
            return;

        }//end load

        if (cmd == "loadregiondata")
        {
         if (n < 2)
         {
          stringbuffer.setstr("help loadregiondata");
          EvaluatePrompt();
          return;
         }

         string filepath = tokens[1];

             if ( !textFileExists( filepath ))
             {
              string errornsf = ">> No such file: " + tokens[1] + " <<";
              textbuffer.InsertLine(errornsf);           
              return;
             }
                        
             RenderMenu();

            if (g_Map)
            {
             loadRegionData(filepath, g_Map->getRegionPList() );
            }

            return;

        }//end loadregiondata



        if (cmd == "loadpath" )
        {
         if (n < 2)
         {
          stringbuffer.setstr("help loadpath");
          EvaluatePrompt();
          return;
         }

         string filepath;

         if (tokens[1].find("\"") == 0)
         {
          filepath = tokens[1].c_str();
          filepath += " ";
          //start building a path
          for (int i = 2; i < n; i++)
          {
           filepath += tokens[i] + " ";
           if (tokens[i].find("\"") != -1) break;
          }
           textbuffer.InsertLine(filepath.c_str());          
         }


			int offset;
	
			while ((offset = filepath.find("\"" )) != -1)
		   {
          filepath.erase( filepath.begin() + offset);
  
			}
	
     
             if ( !textFileExists( filepath ))
             {
              string errornsf = ">> No such file: " + filepath + " <<";
              textbuffer.InsertLine(errornsf);           
              return;
             }
                        
             RenderMenu();
             g_MAIN_STATE = CAMERAMODE;

            if (g_Map)
            {
                textbuffer.InsertLine("> Deleting Pathing Map ... "); 
                RenderMenu();   

                delete g_Map;
                g_Map = 0;

                textbuffer.tag("done <"); 
                RenderMenu();   
            }

     
            if (g_Model)
            {
                textbuffer.InsertLine("> Deleting Detail Map ... "); 
                RenderMenu();  

                delete g_Model;
                g_Model = 0;

                textbuffer.tag("done <"); 
                RenderMenu();  
            }

            textbuffer.InsertLine("> Loading Pathing Map " + filepath + " ... "); 
            RenderMenu();  

            if (n == 3) g_Map = new CMap(filepath.c_str(),toDouble(tokens[2]));
            else
            if (n == 2) g_Map = new CMap(filepath.c_str());
            
            textbuffer.tag("done <"); 
            RenderMenu();  

            g_SimTime = 0;
            WallStart = SDL_GetTicks();            
            
            if (g_Map) g_camera.SetPosition(g_Map->GetCenter().x, g_Map->GetCenter().y, g_Map->GetCenter().z);
            
            g_bdrawcursor = true;

            return;

        }//end loadpath


        if(cmd == "label")
        {
         if (n < 2)
         {
          stringbuffer.setstr("help label");
          EvaluatePrompt();
          return;
         }

            if(!g_Map) return;
            if (tokens[1] == "regions")    g_Map->LabelRegionIDs();
            if (tokens[1] == "components") g_Map->LabelComponents();
            if (tokens[1] == "distances")  g_Map->LabelRegionDistances();
            if (tokens[1] == "off")        g_Map->ClearLabels();
            return;
        }

        if (cmd == "cap")
        {
          if (n > 1 && tokens[1] == "on")
          {
           g_bImageCapture = true;
          } 
          else if (n > 1 && tokens[1] == "off")
          {
           g_bImageCapture = false;
          } 

          //else g_bImageCapture = !g_bImageCapture;
          textbuffer.InsertLine(">> Image capture is ");
          if (g_bImageCapture) textbuffer.tag("enabled. <<");
          else                 textbuffer.tag("disabled. <<");
            return;
        }

        if (cmd == "camspeed")
        {
          if (n != 2)
          {
           stringbuffer.setstr("help camspeed");
           EvaluatePrompt();
           return;
          }
          float speed = toDouble(tokens[1]);
          g_camera.SetSpeed(speed);
          g_camera.SetSensitivity(speed);
          return;
        }


        if (cmd == "play")
        {
          if (n == 2)
          {
           Sound * s = new Sound(tokens[1]);
           s->play();
           s->waitForFinish();
           delete s;
          }
            return;
        }


        if (cmd == "normals")
        {
          if (n > 1 && tokens[1] == "on" && !(DrawFlags & DRAW_NORMAL))
          DrawFlags |= DRAW_NORMAL;
          else if (n > 1 && tokens[1] == "off" && (DrawFlags & DRAW_NORMAL))
          DrawFlags -= DRAW_NORMAL;
            return;
        }

        if (cmd == "bounds")
        {
          if (n > 1 && tokens[1] == "on" && !(DrawFlags & DRAW_BB))
          DrawFlags |= DRAW_BB;
          else if (n > 1 && tokens[1] == "off" && (DrawFlags & DRAW_BB))
          DrawFlags -= DRAW_BB;
            return;
        }

        if (cmd == "lighting")
        {
          if (n > 1 && tokens[1] == "on" && !(DrawFlags & DRAW_LIT))
          DrawFlags |= DRAW_LIT;
          else if (n > 1 && tokens[1] == "off" && (DrawFlags & DRAW_LIT))
          DrawFlags -= DRAW_LIT;
            return;
        }

        if (cmd == "floors")
        {
          if (n > 1 && tokens[1] == "on" && !(DrawFlags & DRAW_FLOOR))
          DrawFlags |= DRAW_FLOOR;
          else if (n > 1 && tokens[1] == "off" && (DrawFlags & DRAW_FLOOR))
          DrawFlags -= DRAW_FLOOR;
            return;
        }

        if (cmd == "surfaces")
        {
          if (n > 1 && tokens[1] == "on" && !(DrawFlags & DRAW_SURFACE))
          DrawFlags |= DRAW_SURFACE;
          else if (n > 1 && tokens[1] == "off" && (DrawFlags & DRAW_SURFACE))
          DrawFlags -= DRAW_SURFACE;
            return;
        }

        if (cmd == "outline")
        {
          if (n > 1 && tokens[1] == "on" && !(DrawFlags & DRAW_OUTLINE))
          DrawFlags |= DRAW_OUTLINE;
          else if (n > 1 && tokens[1] == "off" && (DrawFlags & DRAW_OUTLINE))
          DrawFlags -= DRAW_OUTLINE;
            return;
        }

        if (cmd == "walls")
        {
          if (n > 1 && tokens[1] == "on" && !(DrawFlags & DRAW_WALL))
          DrawFlags |= DRAW_WALL;
          else if (n > 1 && tokens[1] == "off" && (DrawFlags & DRAW_WALL))
          DrawFlags -= DRAW_WALL;
            return;
        }

        if (cmd == "entities")
        {
          if (n > 1 && tokens[1] == "on" && !(DrawFlags & DRAW_ENTITIES))
          DrawFlags |= DRAW_ENTITIES;
          else if (n > 1 && tokens[1] == "off" && (DrawFlags & DRAW_ENTITIES))
          DrawFlags -= DRAW_ENTITIES;
            return;
        }

        if (cmd == "portals")
        {
          if (n > 1 && tokens[1] == "on" && !(DrawFlags & DRAW_PORTAL))
          DrawFlags |= DRAW_PORTAL;
          else if (n > 1 && tokens[1] == "off" && (DrawFlags & DRAW_PORTAL))
          DrawFlags -= DRAW_PORTAL;
            return;
        }


        if (cmd == "physics")
        {
          if (n > 1 && tokens[1] == "on")       g_PHYS_MODE = PHYSICSON;
          else if (n > 1 && tokens[1] == "off") g_PHYS_MODE = PHYSICSOFF;
          else g_PHYS_MODE = (g_PHYS_MODE == PHYSICSON ? PHYSICSOFF : PHYSICSON);
          textbuffer.InsertLine(">> Physical collisions are ");
          if (g_PHYS_MODE == PHYSICSON) textbuffer.tag("enabled. <<");
          else                 textbuffer.tag("disabled. <<");
            return;
        }

        if ( cmd == ">>" || cmd == "rem" )
        {
         //this part intentionally left blank
         // rem is a remark or comment in the script
            return;
        }

        if ( cmd == "pause")
        {
 
			  if (g_MAIN_STATE == SIMULATION)
			  {
               if(g_bImageCapture) wascapturing = true;
               else wascapturing = false;
               
               g_bImageCapture = false;
               g_MAIN_STATE = CAMERAMODE;
               textprompt = false;
			  }
              else if (g_MAIN_STATE == CAMERAMODE)
			  {
               if(wascapturing) g_bImageCapture = true;
               g_MAIN_STATE = SIMULATION;
               textprompt = false;
			  }

            //g_SimTime = 0;
            //WallStart = (double)SDL_GetTicks();
            return;
        }

        if ( cmd == "sim")
        {
            if (!g_Map)
            {
             textbuffer.InsertLine(">> No map is loaded. Use load mapfile.obj <<");
             return;
            } 

            g_MAIN_STATE = SIMULATION;
            textprompt = false;

            g_SimTime = 0;
            WallStart = (double)SDL_GetTicks();

            return;
        }

        if (cmd == "dmap")
        {
            g_Draw_Pathing_Map = false;
            g_Draw_Detail_Map = true;
            g_Draw_Occlusion_Map = false;
            return;
        }
        if (cmd == "pmap")
        {
            g_Draw_Pathing_Map = true;
            g_Draw_Detail_Map = false;
            g_Draw_Occlusion_Map = false;
            return;
        }
        
        if (cmd == "omap")
        {
            g_Draw_Pathing_Map = false;
            g_Draw_Detail_Map = false;
            g_Draw_Occlusion_Map = true;
            return;
        }

        ///////////////////////////////////
//        if (strcmp(token,"setspeed") == 0)
//        {
//         float minspeed, maxspeed;
//
//         char * token2 = strtok(NULL," "); //min speed parameter
//         if (token2) minspeed = atof(token2);
//
//         char * token3 = strtok(NULL," "); //max speed parameter
//         if (token3) maxspeed = atof(token3);         
//
//         else
//         {
//          textbuffer.InsertLine("setspeed <min> <max>");
//          return;
//         }
//
//         if (g_Map) g_Map->SetEntitySpeeds(minspeed, maxspeed);
//         else textbuffer.InsertLine("> Simulation mode only <");
//
//         return;
//        }
//

        if (cmd == "fontface")
        {
         if (n != 2)
         {
          stringbuffer.setstr("help fontface");
          EvaluatePrompt();
          return;
         }

         char * familydescr = strtok(NULL, " ");
         if (tokens[1] == "serif")
         font = serif;
         else if (tokens[1] == "sans")
         font = sans;
         else if (tokens[1] == "mono")
         font = monospace;
         return;
        }

        if (cmd == "fontsize")
        {  
         if (n != 2)
         {
          stringbuffer.setstr("help fontsize");
          EvaluatePrompt();
          return;
         }
         int fontsizel = toInt(tokens[1]);

         if (fontsizel < 24 && fontsizel > 4)
         {
          fontsize = fontsizel;
          sans = TTF_OpenFont( "fonts/sans.ttf", fontsize);
          monospace = TTF_OpenFont( "fonts/mono.ttf", fontsize );
          serif = TTF_OpenFont( "fonts/serif.ttf", fontsize );
          //font = monospace;
         }
         return;
        }

        if (cmd == "easteregg")
        {
         //font = monospace;                                      

         textbuffer.InsertLine("     ,^~~~-.         .-~~~\\\"-. ");                  
         textbuffer.InsertLine("    :  .--. \\       /  .--.  \\");                 
         textbuffer.InsertLine("    : (    .-`<^~~~-: :    )  :");            
         textbuffer.InsertLine("    `. `-,~            ^- '  .'");         
         textbuffer.InsertLine("      `-:                 ,.-~");      
         textbuffer.InsertLine("       .'                  `.");                 
         textbuffer.InsertLine("      ,'   @   @            |");                  
         textbuffer.InsertLine("      :    __               ;");                  
         textbuffer.InsertLine("   ...{   (__)          ,----.");                 
         textbuffer.InsertLine("  /   `.              ,' ,--. `.");               
         textbuffer.InsertLine(" |      `.,___   ,      :    : :");               
         textbuffer.InsertLine(" |     .'    ~~~~       \\    / :");               
         textbuffer.InsertLine("  \\.. /               `. `--' .'");               
         textbuffer.InsertLine(" code: Mike Johnson, Matthew Bennett");
         Sound * snd = new Sound("./sounds/grizzlybear.wav");
         snd->play();
         return;
        }

        ///////////////////////////////////////
			if (cmd == "regions")
			{
			 if (g_Map)
				{
 				int num = g_Map->getRegionPList()->size();
				string out = "Number of regions in map:  " + stringify(num);
				textbuffer.InsertLine(out, COMMENT);
				}
			 return;
			}	        
        

        if (cmd == "spawn")
        {
            if (n != 3)
            {
             stringbuffer.setstr("help spawn");
             EvaluatePrompt();
             return;
            }        
            if (!g_Map)
            {
                textbuffer.InsertLine(">> Error: Map must first be loaded to spawn <<");
                return;
            }

            int region = toInt(tokens[1]);
            float count = toDouble(tokens[2]);

            g_Map->SpawnInRegion(region, count);

            return;
        }
        
        if (cmd == "spawnbleachers")
        {
            if (n != 2)
            {
             stringbuffer.setstr("help spawn");
             EvaluatePrompt();
             return;
            }        
            if (!g_Map)
            {
                textbuffer.InsertLine(">> Error: Map must first be loaded to spawn <<");
                return;
            }

            float count = toInt(tokens[1]);


            g_Map->SpawnInBleachers(count);

            return;
        }

        if (cmd == "bombsquad")
        {
            if (n != 4)
            {
             stringbuffer.setstr("help bombsquad");
             EvaluatePrompt();
             return;
            }        
            if (!g_Map)
            {
                textbuffer.InsertLine(">> Error: Map must first be loaded to spawn <<");
                return;
            }

            int srcregion = toInt(tokens[1]);
            int count = toInt(tokens[2]);
				int dstregion = toInt(tokens[3]);

            g_Map->DeployBombTech(srcregion, count, dstregion);

            return;
        }
        
        if (cmd == "bombersquad")
        {
            if (n != 4)
            {
             stringbuffer.setstr("help bombsquad");
             EvaluatePrompt();
             return;
            }        
            if (!g_Map)
            {
                textbuffer.InsertLine(">> Error: Map must first be loaded to spawn <<");
                return;
            }

            int srcregion = toInt(tokens[1]);
            int count = toInt(tokens[2]);
				int dstregion = toInt(tokens[3]);

            g_Map->DeployBomber(srcregion, count, dstregion);

            return;
        }

        if (cmd == "spectatorsquad")
        {
            if (n != 4)
            {
             stringbuffer.setstr("help bombsquad");
             EvaluatePrompt();
             return;
            }        
            if (!g_Map)
            {
                textbuffer.InsertLine(">> Error: Map must first be loaded to spawn <<");
                return;
            }

            int srcregion = toInt(tokens[1]);
            int count = toInt(tokens[2]);
				int dstregion = toInt(tokens[3]);

            g_Map->DeploySpectator(srcregion, count, dstregion);

            return;
        }

        if (cmd == "explode")
        {
            if (n != 3)
            {
             stringbuffer.setstr("help explode");
             EvaluatePrompt();
             return;
            }        
            if (!g_Map)
            {
                textbuffer.InsertLine(">> Error: Map must first be loaded to explode <<");
                return;
            }

            int region = toInt(tokens[1]);
            float power = toDouble(tokens[2]);

            g_Map->ExplodeRegion(region, power);

            return;
        }

        if (cmd == "logpos")
        {
         if (n != 2)
         {
          stringbuffer.setstr("help logpos");
          EvaluatePrompt();
          return;
         }
         if (!g_Map)
         {
          textbuffer.InsertLine(">> Map must be loaded for logpos <<");
          return;
         }

         textbuffer.InsertLine(datetime() + " started logpos");
         //g_Map->logEntities(tokens[1].c_str() );
         textbuffer.InsertLine(datetime() + " finished logpos");
            return;

        }		  

        if (cmd == "savecams")
        {
         if (n != 2)
         {
          stringbuffer.setstr("help savecams");
          EvaluatePrompt();
          return;
         }

         storcams(tokens[1]);
         return;
        }

        if (cmd == "loadcams")
        {
         if (n != 2)
         {
          stringbuffer.setstr("help loadcams");
          EvaluatePrompt();
          return;
         }

            if (!textFileExists(tokens[1]))
            {
             textbuffer.InsertLine("> File not found. <");  
             return;
            }

            rtrvcams(tokens[1]);
         return;
        }

        if (cmd == "cam")
        {
         if (n != 2)
         {
           stringbuffer.setstr("help cam");
           EvaluatePrompt();
           return;
         }
         int cnum = toInt(tokens[1]);
         switchcam(cnum - 1);
         return;
        }

        if (cmd == "clrcams")
        {
           for (int i = 0; i < maxcams; i++) dirtyCams[i] = 0;
           g_Camera_list.clear();
           return;
        }
        
        if (cmd == "fullscr")
        {
         if (SDL_WM_ToggleFullScreen(MainWindow) == 0) textbuffer.InsertLine("> Fullscreen mode failed. <");
         return;
        }

        if (cmd == "res")
        {
            if (n != 2)
            {
             stringbuffer.setstr("help res");
             EvaluatePrompt();
             return;
            }

          int wide = toInt(tokens[1]);
          if (wide > 0)
          {
           MainWindow = SDL_SetVideoMode( wide, (3*wide/4), SCREEN_DEPTH, VideoFlags );
           Reshape(wide, (3*wide/4));   // now resize the OpenGL viewport
          }
           return;
        } 

        if (cmd == "script")
        {
            if (n != 2)
            {
             stringbuffer.setstr("help script");
             EvaluatePrompt();
             return;
            }

           string scriptpath = "..\\scripts\\" + tokens[1];

           if (textFileExists(scriptpath)) loadScript(scriptpath);
           else textbuffer.InsertLine("> No such file: " + scriptpath + " <");
           return;
         }

        if (cmd == "at")
        {
            if (n < 3)
            {
             stringbuffer.setstr("help at");
             EvaluatePrompt();
             return;
            }

            //accumulate all following tokens into one
            string commandtoQ;
            for (int i = 3; i < tokens.size(); i++)
            {
             commandtoQ += tokens[i] + " ";
            }

         if (tokens[2] == "percentsafe")
         {
           AtPercentCommands.push_back( pair<int, std::string> (toInt(tokens[1]), commandtoQ ) );           
           textbuffer.InsertLine(">> " + commandtoQ + " set to execute at " + tokens[1] + " percent egress << ");
         }
         if (tokens[2] == "percentunsafe")
         {
           AtPercentCommands.push_back( pair<int, std::string> (100-toInt(tokens[1]), commandtoQ ) );           
           textbuffer.InsertLine(">> " + commandtoQ + " set to execute at " + tokens[1] + " percent unsafe << ");
         }

         if (tokens[2] == "simhours")
         {
           AtSimTimeCommands.push_back( pair<int, std::string> (toInt(tokens[1])*3600, commandtoQ ) );           
           textbuffer.InsertLine(">> " + commandtoQ + " set to execute at " + tokens[1] + " hours simtime <<");
         }
         if (tokens[2] == "simminutes")
         {
           AtSimTimeCommands.push_back( pair<int, std::string> (toInt(tokens[1])*60, commandtoQ ) );           
           textbuffer.InsertLine(">> " + commandtoQ + " set to execute at " + tokens[1] + " minutes simtime <<");
         }
         if (tokens[2] == "simseconds")
         {
           AtSimTimeCommands.push_back( pair<int, std::string> (toInt(tokens[1]), commandtoQ ) );           
           textbuffer.InsertLine(">> " + commandtoQ + " set to execute at " + tokens[1] + " seconds simtime <<");
         }  

         if (tokens[2] == "wallhours")
         {
           AtWallTimeCommands.push_back( pair<int, std::string> (toInt(tokens[1])*3600, commandtoQ ) );           
           textbuffer.InsertLine(">> " + commandtoQ + " set to execute at " + tokens[1] + " hours walltime <<");
         }
         if (tokens[2] == "wallminutes")
         {
           AtWallTimeCommands.push_back( pair<int, std::string> (toInt(tokens[1])*60, commandtoQ ) );           
           textbuffer.InsertLine(">> " + commandtoQ + " set to execute at " + tokens[1] + " minutes walltime <<");
         }



          return;
        } 


        if (cmd == "time")
        {

         textbuffer.InsertLine( ">> Current system time: " + datetime() + " <<");	   

         return;
        }

        if (cmd == "acc") //accuracy
        {
            if (n != 2 || toInt(tokens[1]) < 1 || toInt(tokens[1]) > 10)
            {
             stringbuffer.setstr("help acc");
             EvaluatePrompt();
             return;
            }

    			 SimStepSize = 1.0/(10.0*toInt(tokens[1]));

           return;
        } 

        if (cmd == "makevid")
        {
            if (g_MAIN_STATE == SIMULATION || g_MAIN_STATE == CAMERAMODE)
            {
                if (g_Map)
                {
                    delete g_Map;
                    g_Map = 0;
                }

                if(g_Model)
                {
                    delete g_Model;
                    g_Model = 0;
                }

                g_MAIN_STATE = MENU;
      					 RenderMenu();
                g_Camera_list.clear();
            }			  

	        textbuffer.InsertLine("> Creating the videos... ");
              string cmdline = "call makeallvideos \"" + datetime_() + "\"";

              if (n > 1)
              {
               //this means there is a name for this run      
               cmdline += " \"" + tokens[1] + " \"";
              }
          
			  system(cmdline.c_str());
	        textbuffer.tag("done. <");
           g_HOLD = false;

           return;
        }

        if (cmd == "makeallcombos")
        {
			  if (g_MAIN_STATE == SIMULATION)
			  {
               if(g_bImageCapture) wascapturing = true;
               else wascapturing = false;
               
               g_bImageCapture = false;
               g_MAIN_STATE = CAMERAMODE;
               textprompt = false;
			  }
              else if (g_MAIN_STATE == CAMERAMODE)
			  {
               if(wascapturing) g_bImageCapture = true;
               g_MAIN_STATE = SIMULATION;
               textprompt = false;
			  }	  

        for (int i = 0; i < 9; i++)
        {
	        textbuffer.InsertLine("> Creating combo videos... ");
          
              string cmdline = "call combovid cam" + stringify((i)%9+1) + " cam"+ stringify((i+1)%9+1) + " cam"+ stringify((i+2)%9+1) + " cam"+ stringify((i+3)%9+1);
             
              //cmdline += " \"" + datetime_(DTHOUR | DTMIN | DTSEC) + "\"";
       
  			  system(cmdline.c_str());
	        textbuffer.tag("done. <" );
         }
           return;
        }

        if (cmd == "combovid")
        {

        if (n < 5)
        {
         stringbuffer.setstr("help combovid");
         EvaluatePrompt();
        }
      
			  if (g_MAIN_STATE == SIMULATION)
			  {
               if(g_bImageCapture) wascapturing = true;
               else wascapturing = false;
               
               g_bImageCapture = false;
               g_MAIN_STATE = CAMERAMODE;
               textprompt = false;
			  }
              else if (g_MAIN_STATE == CAMERAMODE)
			  {
               if(wascapturing) g_bImageCapture = true;
               g_MAIN_STATE = SIMULATION;
               textprompt = false;
			  }
		  

	        textbuffer.InsertLine("> Creating combo videos... ");
          
              string cmdline = "call combovid " + tokens[1] + " " + tokens[2] + " " + tokens[3] + " " + tokens[4];
             
              //cmdline += " \"" + datetime_(DTHOUR | DTMIN | DTSEC) + "\"";
              
              if (n > 5) cmdline += " " + tokens[5];  

        
  			  system(cmdline.c_str());
	        textbuffer.tag("done. <");

           return;
        }

        if (cmd == "quit" || cmd == "exit")
        {
         Quit(0);
        }

    }

    textbuffer.InsertLine("> Invalid Command. Type help for a list of commands <");

}//end EvaluatePrompt()

