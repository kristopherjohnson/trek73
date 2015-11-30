#ident "@(#) TREK73 $Header: /home/ncvs/trek73/src/firing.c,v 1.3 1999/10/26 20:17:44 dillon Exp $"
/*
 * $Source: /home/ncvs/trek73/src/firing.c,v $
 *
 * $Header: /home/ncvs/trek73/src/firing.c,v 1.3 1999/10/26 20:17:44 dillon Exp $
 *
 * $Log: firing.c,v $
 * Revision 1.3  1999/10/26 20:17:44  dillon
 *
 *     Remove bogus argument from fprintf
 *
 * Revision 1.2  1999/10/26 20:12:59  dillon
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
 * Revision 1.1  87/10/09  11:06:25  11:06:25  okamoto (Jeff Okamoto)
 * Initial revision
 * 
 */
/*
 * TREK73: firing.c
 *
 * Take care of firing phasers and torpedos for both enemy
 * and fed ships.
 *
 * phaser_firing, torpedo_firing, ship_detonate, torp_detonate
 *
 */

#include "defs.h"

Prototype int phaser_firing(struct ship *sp);
Prototype int torpedo_firing(struct ship *sp);
Prototype void ship_detonate(struct ship *sp, struct list *lp);
Prototype void torp_detonate(struct torpedo *tp, struct list *lp);

int
phaser_firing(struct ship *sp)
{
	int i;
	float j;
	int	hit;
	struct	ship *ep;
	struct	torpedo *tp;
	int	s;
	int	x, y;
	struct	ship *target;
	struct	list *lp;
	float	bear;
	struct 	ship *fed;


	fed = shiplist[0];
	for (i=0; i<sp->num_phasers; i++) {
		if (sp->phasers[i].status & P_FIRING)
			break;
	}
	if (i == sp->num_phasers)
		return 0;
	sp->phasers[i].status &= ~P_FIRING;
	target = sp->phasers[i].target;
	/*
	 * Put in j the relative bearing of the phasers relative to the ship
	 * Put in bear the absolute direction the phasers are pointing
	 */
	if (target == NULL) {
		bear = sp->phasers[i].bearing + sp->course;
		j = rectify(sp->phasers[i].bearing);
	} else {
		bear = bearing(sp->x,
		    (cantsee(target)) ? target->position.x : target->x,
		    sp->y,
		    (cantsee(target)) ? target->position.y : target->y);
		j = rectify(bear - sp->course);
	}
	if (betw(j, sp->p_blind_left, sp->p_blind_right)
	    && !is_dead(sp, S_ENG))
		return 0;
	if (target != NULL && is_dead(target, S_DEAD)) {
		if ((sp = fed) && (!shutup[PHASERS+i])
		    && !(is_dead(sp, S_DEAD))) {
			printf("%s phaser %d unlocking\n",
			    sp->name, i+1);
			shutup[PHASERS+i]++;
		}
		sp->phasers[i].target = NULL;
		return 0;
	}
	if (cantsee(sp))
		(void) e_cloak_off(sp, fed);
	printf(" <%s frng phasers>\n", sp->name);
	for (lp = &head; lp != tail; lp = lp->fwd) {
		if (lp->type == 0)
			continue;
		ep = NULL;
		tp = NULL;
		if (lp->type == I_SHIP) {
			ep = lp->data.sp;
			if (ep == sp)
				continue;
			x = ep->x;
			y = ep->y;
		} else {
			tp = lp->data.tp;
			x = tp->x;
			y = tp->y;
		}
		hit = phaser_hit(sp, x, y, &sp->phasers[i], bear);
		if (hit <= 0)
			continue;
		if (tp) {
			if (tp->timedelay > segment) {
				switch (lp->type) {
				case I_TORPEDO:
					printf("hit on torpedo %d\n",
						tp->id);
					break;
				case I_ENG:
					printf("hit on %s's engineering\n",
						tp->from->name);
					break;
				case I_PROBE:
					printf("hit on probe %d\n", 
						tp->id);
					break;
				default:
					fprintf(stderr, "hit on lp->type\n");
					break;
				}
				tp->timedelay = 0.;
			}
			tp->fuel -= hit/2;
			if (tp->fuel < 0)
				tp->fuel = 0;
			continue;
		}
		/*
		 * Determine which shield was hit
		 */
		j = rectify(bearing(x, sp->x, y, sp->y) - ep->course);
		if (j > 315.0 || j < 45.0)
			s = 1;
		else if (j < 135.0)
			s = 2;
		else if (j < 225.0)
			s = 3;
		else
			s = 4;
		(void) damage(hit, ep, s, &p_damage, D_PHASER);
	}
	/*
	 * Reduce the load by the firing percentage
	 */
	sp->phasers[i].load *= 1.0 - (float) sp->p_percent / 100;
	return 0;
}

int
torpedo_firing(struct ship *sp)
{
	int i;
	float j;
	int th;
	struct	torpedo *tp;
	struct	ship *target;
	struct	list *lp;
	float	bear;
	struct	ship *fed;

	fed = shiplist[0];
	for (i=0; i<sp->num_tubes; i++) {
		if (sp->tubes[i].status & T_FIRING)
			break;
	}
	if (i == sp->num_tubes)
		return 0;
	sp->tubes[i].status &= ~T_FIRING;
	th = sp->tubes[i].load;
	if (th == 0)
		return 0;
	target = sp->tubes[i].target;
	/*
	 * Put in j the relative bearing of the tubes to the ship
	 * Put in bear the absolute direction the tubes are pointing
	 */
	if (target == NULL) {
		bear = sp->phasers[i].bearing + sp->course;
		j = rectify(sp->phasers[i].bearing);
	} else {
		bear = bearing(sp->x,
		    (cantsee(target)) ? target->position.x : target->x,
		    sp->y,
		    (cantsee(target)) ? target->position.y : target->y);
		j = rectify(bear - sp->course);
	}
	if (betw(j, sp->t_blind_left, sp->t_blind_right) && !is_dead(sp, S_ENG))
		return 0;
	if (target != NULL && (is_dead(target, S_DEAD))) {
		if ((sp == fed) && !shutup[TUBES+i] && !is_dead(sp, S_DEAD)) {
			printf("   tube %d disengaging\n", i+1);
			shutup[TUBES+i]++;
		}
		sp->tubes[i].target = NULL;
		return 0;
	}
	sp->tubes[i].load = 0;
	lp = newitem(I_TORPEDO);
	lp->type = I_TORPEDO;
	lp->data.tp = MKNODE(struct torpedo, *, 1);
	if (lp->data.tp == (struct torpedo *)NULL) {
		fprintf(stderr, "torpedo_firing: malloc failed\n");
		exit(2);
	}
	tp = lp->data.tp;
	tp->from = sp;
	tp->x = sp->x;
	tp->y = sp->y;
	tp->target = NULL;
	tp->course = rectify(bear);
	tp->fuel = th;
	tp->speed = sp->t_lspeed + sp->warp;
	tp->newspeed = tp->speed;
	tp->timedelay = (float)sp->t_delay;
	tp->prox = sp->t_prox;
	tp->id = new_slot();
	tp->type = TP_TORPEDO;
	if (teletype)
		printf("\007");
	if (cantsee(sp))
		(void) e_cloak_off(sp, fed);
	printf(" <<%s frng torpedo %d>>", sp->name, tp->id);
	if (teletype)
		printf("\007");
	printf("\n");
	return 1;
}

void
ship_detonate(struct ship *sp, struct list *lp)
{
	int fuel;
	int i;

	fuel = 0;
	printf("++%s++ destruct.\n", sp->name);
	for (i=0; i<sp->num_phasers; i++)
		if (!(sp->phasers[i].status & P_DAMAGED))
			fuel += min(sp->phasers[i].load, MAX_PHASER_CHARGE);
	for (i=0; i<sp->num_tubes; i++)
		if (!(sp->tubes[i].status & T_DAMAGED))
			fuel += min(sp->tubes[i].load, MAX_TUBE_CHARGE);
	fuel += sp->pods;
	antimatter_hit((char *) sp, sp->x, sp->y, fuel);
	lp->type = 0;
	for (i=0; i< S_NUMSYSTEMS; i++)
		sp->status[i] = 100;	/* He's dead, Jim */
	sp->cloaking = C_NONE;
	sp->complement = -1;
}


void
torp_detonate(struct torpedo *tp, struct list *lp)
{

	switch (lp->type) {
		case I_TORPEDO:
			printf(":: torp %d ::\n", tp->id);
			break;
		case I_PROBE:
			printf("** probe %d **\n", tp->id);
			break;
		case I_ENG:
			printf("## %s engineering ##\n", tp->from->name);
			break;
		default:
			fprintf(stderr, "torp_detonate lp->type %d\n",lp->type);
			break;
	}
	antimatter_hit((char *) tp, tp->x, tp->y, tp->fuel);
	return_slot(tp->id);
	delitem(lp);
}
