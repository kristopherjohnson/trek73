#ident "@(#) TREK73 $Header: /home/ncvs/trek73/src/cmds3.c,v 1.3 1999/10/26 20:15:20 dillon Exp $"
/*
 * $Source: /home/ncvs/trek73/src/cmds3.c,v $
 *
 * $Header: /home/ncvs/trek73/src/cmds3.c,v 1.3 1999/10/26 20:15:20 dillon Exp $
 *
 * $Log: cmds3.c,v $
 * Revision 1.3  1999/10/26 20:15:20  dillon
 *
 *     The survivors report function must return a value
 *
 * Revision 1.2  1999/10/26 20:12:58  dillon
 *
 *     Ansize the code.  Convert everything to prototypes
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
 * Revision 1.1  87/10/09  11:02:23  11:02:23  okamoto (Jeff Okamoto)
 * Initial revision
 * 
 */
/*
 * TREK73: cmds3.c
 *
 * User Commands
 *
 * jettison_engineering, detonate_engineering, phaser_status, tube_status,
 * survivors, alter_power
 *
 * (do_jettison)
 *
 */

#include "defs.h"

Prototype int jettison_engineering(struct ship *sp);
Prototype int detonate_engineering(struct ship *sp);
Prototype int phaser_status(struct ship *sp);
Prototype int tube_status(struct ship *sp);
Prototype int survivors(struct ship *sp);
Prototype int alter_power(struct ship *dummy);

void do_jettison(struct ship *sp);

int
jettison_engineering(struct ship *sp)
{

	printf("   Mr. %s, jettison our engineering section!\n", engineer);
	if (is_dead(sp, S_ENG)) {
		printf("%s:  But Captain, it's already jettisonned.\n", engineer);
		return 0;
	}
	do_jettison(sp);
	printf("%s:  Aye, %s.  Jettisoning engineering.\n", 
	    engineer, title);
	return 1;
}

void
do_jettison(struct ship *sp)
{
	struct list *lp;
	struct torpedo *tp;

	lp = newitem(I_ENG);
	tp = lp->data.tp = MKNODE(struct torpedo, *, 1);
	if (tp == (struct torpedo *)NULL) {
		fprintf(stderr, "do_jettison: malloc failed\n");
		exit(2);
	}
	tp->target = NULL;
	tp->speed = sp->warp;
	tp->newspeed = 0.0;
	tp->x = sp->x;
	tp->y = sp->y;
	tp->course = sp->course;
	/* This is correct */
	tp->fuel = sp->energy;
	tp->timedelay = 10.;
	tp->prox = 0;
	tp->from = sp;
	tp->id = new_slot();
	tp->type = TP_ENGINEERING;
	sp->energy = sp->pods = 0.;
	sp->newwarp = (sp->warp < 0.0) ? -0.99 : 0.99;
	sp->regen = 0.0;
	sp->status[S_ENG] = 100;	/* Set these as destroyed */
	sp->status[S_WARP] = 100;
	sp->max_speed = 1.0;
	sp->cloaking = C_NONE;
	sp->t_blind_left = sp->t_blind_right =
	    sp->p_blind_left = sp->p_blind_right = 180;
}

int
detonate_engineering(struct ship *sp)
{
	struct list *lp;
	struct torpedo *tp;
	int found;
	char buf[10];

	found = 0;
	printf("   Mr. %s, detonate engineering!\n", engineer);
	if (!is_dead(sp, S_ENG)) {
		printf("%s: But %s, it's still attached.\n",engineer,title);
		printf("   Detonate anyway? ");
		printf("%s:  [yes or no] ", captain);
		(void) Gets(buf, sizeof(buf));
		if ((buf[0] == NULL)
		    || ((buf[0] != 'y') && (buf[0] != 'Y')))
			return 0;
		else 
			do_jettison(sp);
	}
	for (lp = &head; lp != tail; lp = lp->fwd) {
		if (lp->type != I_ENG)
			continue;
		tp = lp->data.tp;
		if (tp->from != sp)
			continue;
		found++;
		tp->timedelay = 0.;
		break;
	}
	if (found)
		printf("%s:  Aye, %s.\n",engineer, title);
	else
		printf("%s:  Ours has already detonated.\n", engineer);
	return 1;
}


int
phaser_status(struct ship *sp)
{
	int i;

	puts("Phasers\n");
	printf("Control: ");
	for (i=0; i<sp->num_phasers; i++) {
		if (sp->phasers[i].status & P_DAMAGED)
			printf("\tdamaged");
		else if (sp->phasers[i].target == NULL)
			printf("\tmanual");
		else
			printf("\t%.7s", sp->phasers[i].target->name);
	}
	printf("\n Turned: ");
	for (i=0; i<sp->num_phasers; i++)
		if (sp->phasers[i].status & P_DAMAGED)
			putchar('\t');
		else if (sp->phasers[i].target == NULL)
			printf("\t%3.0f", sp->phasers[i].bearing);
		else
			printf("\tLOCKED");
	printf("\n  Level: ");
	for (i=0; i<sp->num_phasers; i++) {
		if (sp->phasers[i].status & P_DAMAGED)
			putchar('\t');
		else
			printf("\t%d", sp->phasers[i].load);
	}
	printf("\n  Drain: ");
	for (i=0; i<sp->num_phasers; i++) {
		if (sp->phasers[i].status & P_DAMAGED)
			putchar('\t');
		else
			printf("\t%d", sp->phasers[i].drain);
	}
	printf("\n\nFiring percentage: %d\n",sp->p_percent);
	printf("\nFiring angles: ");
	if (is_dead(sp, S_ENG)) {
		printf("unrestricted.\n");
	} else {
		printf("0 - %d and %d - 360.\n",
		    sp->p_blind_left, sp->p_blind_right);
	}
	return 1;
}

int
tube_status(struct ship *sp)
{
	int i;

	puts("Torpedos\n");
	printf("Control: ");
	for (i=0; i<sp->num_tubes; i++) {
		if (sp->tubes[i].status & T_DAMAGED)
			printf("\tdamaged");
		else if (sp->tubes[i].target == NULL)
			printf("\tmanual");
		else
			printf("\t%.7s", sp->tubes[i].target->name);
	}
	printf("\n Turned: ");
	for (i=0; i<sp->num_tubes; i++)
		if (sp->tubes[i].status & T_DAMAGED)
			putchar('\t');
		else if (sp->tubes[i].target == NULL)
			printf("\t%.0f", sp->tubes[i].bearing);
		else
			printf("\tLOCKED");
	printf("\n  Level: ");
	for (i=0; i<sp->num_tubes; i++) {
		if (sp->tubes[i].status & T_DAMAGED)
			putchar('\t');
		else
			printf("\t%d", sp->tubes[i].load);
	}
	printf("\n\nLaunch speed: %d\n", sp->t_lspeed);
	printf("  Time delay: %d\n", sp->t_delay);
	printf("  Prox delay: %d\n", sp->t_prox);
	printf("\nFiring angles: ");
	if (is_dead(sp, S_ENG)) {
		printf("unrestricted.\n");
	} else {
		printf("0 - %d and %d - 360.\n",sp->t_blind_left, sp->t_blind_right);
	}
	return 1;
}

int
survivors(struct ship *sp)
{
	struct ship *ep;
	int i;

	printf("\nSurvivors reported:\n");
	for (i=0; i<=shipnum; i++) {
		ep = shiplist[i];
		if (sp->complement < 0)
			printf("   %s -- destructed", sp->name);
		else if (cantsee(ep))
			printf("   %s -- ???\n", ep->name);
		else
			printf("   %s -- %d\n", ep->name, ep->complement);
	}
	sp = sp;		/* LINT */
	return(0);
}

int
alter_power(struct ship *dummy)
{
	int i;
	float j;
	char buf1[20];

	printf("\n%s:  Regeneration rate is %5.2f.\n",engineer, shiplist[0]->regen);
	for (i=0; i<SHIELDS; i++) {
		printf("%s:  Shield %d drain is [0.0 to 1.0] ",
		    captain, i + 1);
		(void) Gets(buf1, sizeof(buf1));
		j = (float) atof(buf1);
		if (buf1[strlen(buf1) - 1] == '*') {
			if ((j < 0.0) || (j > 1.0))
				goto badparam;
			for (; i<SHIELDS; i++)
				shiplist[0]->shields[i].attemp_drain =j;
			break;
		} else if ((j < 0.0) || (j > 1.0))
			goto badparam;
		else
			shiplist[0]->shields[i].attemp_drain = j;
	}				
	printf("\n");
	for (i=0; i<shiplist[0]->num_phasers; i++) {
		printf("%s:  Phaser %d drain is [%.0f to %.0f] ",
		    captain, i + 1, MIN_PHASER_DRAIN, MAX_PHASER_DRAIN);
		(void) Gets(buf1, sizeof(buf1));
		j = (float) atof(buf1);
		if (buf1[strlen(buf1) - 1] == '*') {
			if ((j < MIN_PHASER_DRAIN)
			    || (j > MAX_PHASER_DRAIN))
				goto badparam;
			for (; i<shiplist[0]->num_phasers; i++)
				shiplist[0]->phasers[i].drain = (int) j;
			break;
		} else if ((j < MIN_PHASER_DRAIN) 
		    || (j > MAX_PHASER_DRAIN))
			goto badparam;
		else
			shiplist[0]->phasers[i].drain = (int) j;
	}
	return 1;
badparam:
	printf("%s:  Bad parameters, %s.\n", engineer, title);
	return 0;
}
