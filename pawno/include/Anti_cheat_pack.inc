#include <rAgc>//Anti gun cheats -Rogue
#include <Opba>//Anti airbreak\teleport - Rogue
#include <rAct>//Anti car trolls - Rogue
#include <AntiJC>//Anti jetpack cheats- Rogue
#include <rAGM>//Anti godmode -Rogue
#include <rAcs>//Anti car spam - Rogue

#if !defined FILTERSCRIPT 

#if defined RogueAC_OnGameModeInit
	forward RogueAC_OnGameModeInit();
#endif

public OnGameModeInit()
{
	print("	 [[--|| Rogue-AC V7.0 ||--]]	 ");
	print("	 [[--|| Rogue Anti cheat version 7.0 ||--]]	  ");
	print("	 [[--|| Successfully Loaded! ||--]]	  ");
	#if defined RogueAC_OnGameModeInit
		return RogueAC_OnGameModeInit();
	#else
		return 1;
	#endif
}

#if defined _ALS_OnGameModeInit
  #undef OnGameModeInit
#else
	#define _ALS_OnGameModeInit
#endif

#define OnGameModeInit RogueAC_OnGameModeInit

#endif
