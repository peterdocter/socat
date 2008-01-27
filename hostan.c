/* $Id: hostan.c,v 1.2 2007/03/06 21:02:01 gerhard Exp $ */
/* Copyright Gerhard Rieger 2006-2007 */
/* Published under the GNU General Public License V.2, see file COPYING */

/* the subroutine hostan makes a "HOST ANalysis". It gathers information
   about the host environment it is running in without modifying its state
   (almost).
 */

#include "xiosysincludes.h"
#include "mytypes.h"
#include "compat.h"
#include "error.h"
#include "sycls.h"
#include "sysutils.h"
#include "filan.h"

#include "hostan.h"

static int iffan(FILE *outfile);

int hostan(FILE *outfile) {
#if WITH_SOCKET
   iffan(outfile);
#endif
   return 0;
}

#if WITH_SOCKET
static int iffan(FILE *outfile) {
   /* Linux: man 7 netdevice */
   /* FreeBSD: man 4 networking */
   /* Solaris: man 7 if_tcp */

/* currently we support Linux and a little FreeBSD */
#ifdef SIOCGIFCONF	/* not Solaris */
#ifdef SIOCGIFINDEX	/* not OpenBSD */

#define IFBUFSIZ 32*sizeof(struct ifreq) /*1024*/
   int s;
   unsigned char buff[IFBUFSIZ];
   struct ifconf ic;
   int i;

   if ((s = Socket(PF_INET, SOCK_DGRAM, IPPROTO_IP)) < 0) {
      Error1("socket(PF_INET, SOCK_DGRAM, IPPROTO_IP): %s", strerror(errno));
      return -1;
   }

   for (i=0; i < IFBUFSIZ; ++i) {
      buff[i] = 255;
   }
   ic.ifc_len = sizeof(buff);
   ic.ifc_ifcu.ifcu_buf = (caddr_t)buff;
   if (Ioctl(s, SIOCGIFCONF, &ic) < 0) {
      Error3("ioctl(%d, SIOCGIFCONF, %p): %s", s, &ic, strerror(errno));
      return -1;
   }

   for (i = 0; i < ic.ifc_len; i += sizeof(struct ifreq)) {
      struct ifreq *ifp = (struct ifreq *)((caddr_t)ic.ifc_req + i);
      struct ifreq ifr;

      strcpy(ifr.ifr_name, ifp->ifr_name);
      if (Ioctl(s, SIOCGIFINDEX, &ifr) < 0) {
	 Error3("ioctl(%d, SIOCGIFINDEX, {\"%s\"}): %s",
		s, &ifr.ifr_name, strerror(errno));
	 return 1;
      }
      /*fprintf(outfile, "%2d: %s\n", ifr.ifr_ifindex, ifp->ifr_ifrn.ifrn_name);*/
#if HAVE_STRUCT_IFREQ_IFR_INDEX
      fprintf(outfile, "%2d: %s\n", ifr.ifr_index, ifp->ifr_name);
#elif HAVE_STRUCT_IFREQ_IFR_IFINDEX
      fprintf(outfile, "%2d: %s\n", ifr.ifr_ifindex, ifp->ifr_name);
#endif /* HAVE_STRUCT_IFREQ_IFR_INDEX */
   }
   Close(s);
#endif /* defined(SIOCGIFCONF) */
#endif /* defined(SIOCGIFINDEX) */
   return 0;
}
#endif /* WITH_SOCKET */
