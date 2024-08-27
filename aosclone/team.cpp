#include "team.h"

#include "general.h"
#include "world.h"

aos_ubyte aos_gamemode = -1;

aos_ubyte aos_ctf_cap_limit = -1;

void TeamType::setName(char *name)
{
	aos_strcpycut(this->name, name, 10);
}

void aos_tc_update_team_scores()
{
	if (aos_gamemode != ID_GAMEMODE_TC) return;
	
	aos_team1_team.score = 0;
	aos_team2_team.score = 0;
	
	for (int i = 0; i < aos_tc_territory_count; i++) {
		TC_Territory *territory = &aos_tc_territories[i];
		switch (territory->team) {
		case ID_TEAM1:
			aos_team1_team.score++;
			break;
		case ID_TEAM2:
			aos_team2_team.score++;
			break;
		}
	}
}