#ident "@(#) TREK73 $Header: /home/ncvs/trek73/src/shipyard.c,v 1.3 1999/10/26 20:13:01 dillon Exp $"
/*
 * $Source: /home/ncvs/trek73/src/shipyard.c,v $
 *
 * $Header: /home/ncvs/trek73/src/shipyard.c,v 1.3 1999/10/26 20:13:01 dillon Exp $
 *
 * $Log: shipyard.c,v $
 * Revision 1.3  1999/10/26 20:13:01  dillon
 *
 *     Ansize the code.  Convert everything to prototypes
 *
 * Revision 1.2  1999/10/26 18:01:11  dillon
 *
 *     Initial changes to make trek compile under FreeBSD.  Remove some externs
 *     for library functions, fixup command.l to operate with a modern lexer.
 *
 * Revision 1.1.1.1  1999/10/26 17:07:14  dillon
 *
 * Nov 1988 import of trek73 from comp.sources.usenet.  This is the trek73
 * that was originally ported from basic to C and then further modified by
 * Jeff, Peter, Matt, and a few others in 1985.  Jeff later made additional
 * changes which led to the 1988 posting.
 *
 *
 *
 * Revision 1.2  87/11/30  09:36:35  09:36:35  okamoto (Jeff Okamoto)
 * Fixed some warnings from cc on advice of Bill Randle.
 * 
 * Revision 1.1  87/10/09  11:11:13  11:11:13  okamoto (Jeff Okamoto)
 * Initial revision
 * 
 */
/*
 * TREK73: shipyard.c
 *
 * Design your own ship
 *
 */

#include "defs.h"

void save_design(void);

static char buf[20];
static char class[3];
static char cloak;
static double bpv;

static struct {
	char description[30];
	char race[30];
	char empire[30];
} stuff;

static struct ship_stat design;

int
main(int ac, char **av)
{
	double regen, efficiency;
	int crew, phasers, torps, pods, max_speed, turn, p_div, t_div;
	int done;

tryagain:
	done = 0;
	while (!done) {
		printf("Regeneration               :");
		(void) gets2(buf);
		regen = atof(buf);
		if (regen >= 0)
			done = 1;
		else
			printf(">>> Be reasonable.\n");
	}
	done = 0;
	while (!done) {
		printf("Pods                       :");
		(void) gets2(buf);
		pods = atof(buf);
		if (pods >= 0)
			done = 1;
		else
			printf(">>> Be reasonable.\n");
	}
	done = 0;
	while (!done) {
		printf("Number of phasers          :");
		(void) gets2(buf);
		phasers = atoi(buf);
		if ((phasers >= 0) && (phasers < MAXWEAPONS))
			done = 1;
		else
			if (phasers < 0)
				printf(">>> Be reasonable.\n");
			else
				printf(">>> Can't have more than %d.\n",
				    MAXWEAPONS-1);
	}
	done = 0;
	while (!done) {
		printf("Number of tubes            :");
		(void) gets2(buf);
		torps = atoi(buf);
		if ((torps >= 0) && (torps < MAXWEAPONS))
			done = 1;
		else
			if (torps < 0)
				printf(">>> Be reasonable.\n");
			else
				printf(">>> Can't have more than %d.\n",
				    MAXWEAPONS-1);
	}
	done = 0;
	while (!done) {
		printf("Shield divisor for phasers :");
		(void) gets2(buf);
		p_div = atof(buf);
		if (p_div > 0)
			done = 1;
		else
			printf(">>> Be reasonable.\n");
	}
	done = 0;
	while (!done) {
		printf("Shield divisor for torps   :");
		(void) gets2(buf);
		t_div = atof(buf);
		if (t_div > 0)
			done = 1;
		else
			printf(">>> Be reasonable.\n");
	}
	done = 0;
	while (!done) {
		printf("Crew                       :");
		(void) gets2(buf);
		crew = atoi(buf);
		if (crew > 0)
			done = 1;
		else
			printf(">>> Be reasonable.\n");
	}
	printf("Can the ship cloak         ?");
	(void) gets2(buf);
	if (buf != NULL && (buf[0] == 'y' || buf[0] == 'Y'))
		cloak = 1;
	else
		cloak = 0;

	calculate(regen, (float)pods, (float)p_div, (float)t_div,
	    phasers + torps, crew,
	    &bpv, &efficiency, &turn, &max_speed);

	printf("BPV = %.2f\n", bpv);
	printf("Efficiency = %.2f\n", efficiency);
	printf("Turn = %d\n", turn);
	printf("Max speed = %d\n", max_speed);

	printf("\nKeep this ship? ");
	(void) gets2(buf);
	if (buf != NULL && (buf[0] == 'n' || buf[0] == 'N'))
		goto tryagain;

	printf("Class identifier           :");
	(void) gets2(class);
	class[2] = '\0';
	printf("Class description          :");
	(void) gets2(stuff.description);
	stuff.description[29] = '\0';
	printf("Race name                  :");
	(void) gets2(stuff.race);
	stuff.race[29] = '\0';
	printf("Empire name                :");
	(void) gets2(stuff.empire);
	stuff.empire[29] = '\0';

	strcpy(design.abbr, class);
	design.num_phaser = phasers;
	design.num_torp = torps;
	/*
	 * The maximum warp speed for an "enemy" version of this class
	 * is 2 greater than the "good-guys" version, just as the
	 * standard class ships are
	 */
	design.o_warpmax = max_speed;
	design.e_warpmax = max_speed + 2;
	/*
	 * "Enemy" efficiency is harder to gauge.  It should never
	 * be less than zero, but should be less than the player's
	 * version, otherwise the player could run away forever.
	 * Until a good method is devised, we'll just leave it equal
	 * to the player's own
	 */
	design.o_eff = efficiency;
	design.e_eff = efficiency;
	design.regen = regen;
	/*
	 * The starting energy is arbitrarily put at 75% of maximum
	 */
	design.energy = pods * 3 / 4;
	design.pods = pods;
	/*
	 * The enemy crew is numbered at 25% greater than your own
	 */
	design.o_crew = crew;
	design.e_crew = crew * 5 / 4;
	design.ph_shield = p_div;
	design.tp_shield = t_div;
	design.turn_rate = turn;
	/*
	 * The cloaking energy is arbitrary
	 */
	design.cloaking_energy = 4;
	/*
	 * The blind arcs and firing delays after voiding a cloak
	 * are arbitrarily set as equal to those of the heavy cruiser.
	 * A better algorithm should be devised.
	 */
	design.t_blind_left = 135;
	design.t_blind_right = 225;
	design.p_blind_left = 125;
	design.p_blind_right = 235;
	design.p_firing_delay = CLOAK_DELAY;
	design.t_firing_delay = CLOAK_DELAY;

	save_design();
	return(0);
}

void
save_design(void)
{
	int fd, bytes;
	char path[BUFSIZ];
	char *home;

	if ((home = getenv("HOME")) != NULL)
		strcpy(path, home);
	else
		strcpy(path, ".");
	
	strcat(path, "/.trek");
	strcat(path, design.abbr);
	printf("Saving to file %s\n", path);
	
	if ((fd = open(path, O_WRONLY|O_CREAT, 0644)) < 0) {
		perror("open");
		exit(1);
	}
	bytes = write(fd, (char *)&design, sizeof(struct ship_stat));
	if (bytes != sizeof(struct ship_stat)) {
		fprintf(stderr, "Wrote only %d, not %d bytes\n", bytes,
		    sizeof(struct ship_stat));
		unlink(path);
		exit(1);
	}
	bytes = write(fd, &stuff, sizeof(stuff));
	bytes = write(fd, &cloak, 1);
	bytes = write(fd, (char *)&bpv, sizeof(int));
	close(fd);
}
