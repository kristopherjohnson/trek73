#ident "@(#) TREK73 $Header: /home/ncvs/trek73/src/endgame.c,v 1.2 1999/10/26 20:12:59 dillon Exp $"
/*
 * $Source: /home/ncvs/trek73/src/endgame.c,v $
 *
 * $Header: /home/ncvs/trek73/src/endgame.c,v 1.2 1999/10/26 20:12:59 dillon Exp $
 *
 * $Log: endgame.c,v $
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
 * Revision 1.1  87/10/09  11:05:18  11:05:18  okamoto (Jeff Okamoto)
 * Initial revision
 * 
 */
/*
 * TREK73: endgame.c
 *
 * prints end-of-game messages and warnings
 *
 * leftovers, final, warn
 *
 */

#include "defs.h"

Prototype int leftovers(void);
Prototype void final(int mesg);
Prototype int warn(int mesg);
Prototype void starfleet(void);

int
leftovers(void)
{
	struct list *lp;

	for (lp = &head; lp != tail; lp = lp->fwd) {
		if (lp->type == 0)
			continue;
		if (lp->type != I_SHIP)
			return 1;
	}
	return 0;
}

void
final(int mesg)
{
	int i;
	int j;
	struct	ship *sp;
	struct	ship *ep;
	char buf[80];


	sp = shiplist[0];
	/* If we're getting that message again, ignore it. */
	if ((mesg == 2) && (reengaged))
		return;
	switch (mesg) {
	case FIN_F_LOSE:
		starfleet();
		printf("We have recieved confirmation that the U.S.S. %s,\n",
			sp->name);
		printf("   captained by %s, was destroyed by %s%s\n",
		    captain, shipnum==1 ?"a ":"", foerace);
		printf("   %s%s.  May future Federation officers\n",
		    foestype, plural(shipnum));
		printf("   perform better in their duties.\n\n");
		break;
	case FIN_E_LOSE:
		starfleet();
		printf("We commend Captain %s and the crew of the %s on their\n",
		    captain, shipname);
		printf("   fine performance against the %ss.  They are\n",
		    foerace);
		puts("   an inspiration to all Starfleet personnel.\n");
		break;
	case FIN_TACTICAL:
		/*
		 * Give him a chance to re-engage if he wants to.  If he does,
		 * he has to get within a range of 3500 before he can again
		 * try to dis-engage
		 */
		if (!reengaged) {
			printf("%s:  %s, we are in a position to either disengage from the\n",
			    science, title);
			printf("   %ss, or re-engage them in combat.\n",
			    foerace);
			printf("   Do you wish to re-engage?\n");
			printf("%s: [y or n] ", captain);
			(void) Gets(buf, sizeof(buf));
			if ((*buf == NULL) || (*buf == 'y') || (*buf == 'Y')) {
				reengaged = 1;
				return;
			}
		}
		starfleet();
		printf("Captain %s of the starship %s has\n",captain,sp->name);
		printf("   out-maneuvered %s aggressors.  We commend\n",foerace);
		printf("   his tactical ability.\n");
		break;
	case FIN_F_SURRENDER:
		starfleet();
		printf("Captain %s has surrendered the U.S.S. %s \n",
		    captain, sp->name);
		printf("   to the %ss.  May Captain Donsell be remembered.\n",
		    foerace);
		break;
	case FIN_E_SURRENDER:
		starfleet();
		printf("We have recieved word from the %s that the\n",sp->name);
		printf("   %ss have surrendered.\n",foerace);
		break;
	case FIN_COMPLETE:
		starfleet();
		puts("One of our scout vessels has encountered the wreckage of");
		printf("   the %s and %d other %s vessel%s.\n", sp->name,
			shipnum, foerace, plural(shipnum));
		break;
	case QUIT:
		starfleet();
		printf("We have received word that Captain %s of the\n",
		    captain);
		printf("  starship %s has sold out to the %ss.\n",sp->name,
		    foerace);
		printf("  May he soon be court-martialled.\n");
		break;
	default:
		printf("How did we get here? final(%d)\n", mesg);
		break;
	}
	puts("\n\n");
	j = 0;
	for (i=0; i<=shipnum; i++) {
		ep = shiplist[i];
		if (is_dead(ep, S_DEAD))
			continue;
		if (!j)
			puts("Survivors Reported:\n");
		j++;
	}
	if (j) {
		for (i=0; i<=shipnum; i++) {
			ep = shiplist[i];
			if ((is_dead(ep, S_DEAD)) || (ep->complement <= 0))
				printf("   %s -- destroyed\n",
				    ep->name);
			else
				printf("   %s -- %d\n",
				    ep->name, ep->complement);
		}
	} else
		puts("*** No survivors reported ***\n");
	exit (1);
}

int
warn(int mesg)
{
	static	int beenhere[5] = {0, 0, 0, 0, 0};
	struct	ship *sp;

	if ((reengaged) && (mesg == 2)) {
		return 0;
	}
	if (beenhere[mesg])
		return 0;
	sp = shiplist[0];
	switch (mesg) {
	case FIN_F_LOSE:
		printf("Message to the Federation:  This is Commander\n");
		printf("   %s of the %s %s.  We have defeated\n",
		    foename, foerace, empire);
		printf("   the %s and are departing the quadrant.\n", sp->name);
		break;
	case FIN_E_LOSE:
		printf("%s: All %s vessels have been either\n",
		    science, foerace);
		printf("   destroyed or crippled.  We still, however, have\n");
		printf("   antimatter devices to avoid.\n");
		break;
	case FIN_TACTICAL:
		printf("%s: The %ss are falling behind and seem to\n",
		    helmsman, foerace);
		printf("   be breaking off their attack.\n");
		break;
	case FIN_F_SURRENDER:
		printf("%s: I'm informing Starfleet Command of our \n", com);
		printf("   disposition.\n");
		break;
	case FIN_E_SURRENDER:
		printf("%s: Although the %ss have surrendered,\n",
		    science, foerace);
		printf("   there are still antimatter devices floating\n");
		printf("   around us.\n");
		break;
	default:
		printf("How did we get here? final(%d)\n", mesg);
		break;
	}
	beenhere[mesg]++;
	return 0;
}

void
starfleet(void)
{
	puts("\n\nStarfleet Command: \n");
	(void) sleep(3);
	putchar('\n');
}

