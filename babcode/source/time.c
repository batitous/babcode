// Copyright (c) 2013, Baptiste Burles
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the
//   documentation and/or other materials provided with the distribution.
// * Neither the name of the University of California, Berkeley nor the
//   names of its contributors may be used to endorse or promote products
//   derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY Baptiste Burles AND CONTRIBUTORS ``AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL Baptiste Burles AND CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "../include/babcode.h"

#include <math.h>
#include <sys/timeb.h>
#include <time.h>
#include <stdlib.h>


/**********************************************
*  calcul du jour de la semaine               *
*  d'apres congruence de Zeller decrite dans: *
*  "Acta Mathematica #7", Stockholm, 1887     *
*                                             *
*  mois     : 1..12                           *
*  date     : 1..31                           *
*  annees   : 1582..4902                      *
*                                             *
*  returns : 0 (dimanche) ... 6 (samedi)      *
**********************************************/
static int daynumber(int date, int month, int year)
{
   int yearnum, cent,day;

   month = month-2;
   if(month<1)
   {
	 month += 12;
     year--;
   }

   yearnum = year%100;
   cent    = year/100;
   day = (int)floor((double)(2.6*month-0.2))+date+yearnum+yearnum/4+cent/4-2*cent;

   while(day<0)
	   day += 7;

   return day%7;
}


/********************************/
/* sortie = 1 = annee bisextile */
/********************************/
static char bissex(int annee)
{
	if(!(annee & 0x0003))                     /* si divisible / 4  */
	{
		if(annee%100==0)                        /* si divisible /100 */
		{
			if(annee%400==0)                      /* et si divisible /400 */
				return 1;                           /* anne bissextile */
		}
		else                                    /* si divible /4 et pas /100 */
			return 1;                             /* anne bissextile */
	}

	return 0;
}

/******************************/
/* Calcul du No de la semaine */
/******************************/
static char calc_week(int day_of_week, int day_of_year,int year, int isBissec)
{
int jref;                                     /* 1 janvier de l'annee en cours */
int jder;                                     /* 31 decembre de l'annee en cours */
int jactu;
int nweek;                                    /* no de semaine */

  jref = daynumber(1,1,year /*Sys_time.tm_year+1900*/);  /* calcul le jour de la semaine du 1er janvier */
  jder = daynumber(31,12,year /*Sys_time.tm_year+1900*/);/* calcul le jour de la semaine du 1er janvier */
  jactu = day_of_week /*Sys_time.tm_wday*/;
  if(!jref) jref = 7;                         /****************************************/
  if(!jder) jder = 7;                         /* transorme jour 0-6 en 1-7            */
  if(!jactu) jactu = 7;                       /****************************************/

  nweek = (day_of_year-1)/7; /*(Sys_time.tm_yday-1)/7;              calcul nombre de semaine */
  if(jref<=4) nweek++;                        /* si 1er janvier <= jeudi */
  if(jactu<jref)nweek++;                      /* si aujourd'hui< 1er janvier */
  if(nweek==53 && jder>=1 && jder<=3)         /* si derniere semaine et 31/12 un lundi mardi ou mercredi */
    nweek = 1;
  if(nweek==0)
  { if(jref==5 || (jref==6 && isBissec/*bissex(Sys_time.tm_year+1900-1)*/)) /* si 1ere semaine & si 01/01 vendredi ou */
      nweek = 53;                                               /* samedi et bissextile */
    else
      nweek = 52;
  }
  return((char)nweek);
}


void getTime( Time *t )
{
	time_t tps ;
	struct tm *tps1 ;
    
	tps = time(&tps);
	tps1 = localtime(&tps);

	t->year = tps1->tm_year + 1900;
	t->month = tps1->tm_mon + 1;
	t->day = tps1->tm_mday ;
	t->hour = tps1->tm_hour;
	t->minute = tps1->tm_min;
	t->second = tps1->tm_sec;

	t->dayOfWeek = tps1->tm_wday;
	t->dayOfYear = tps1->tm_yday ;

	t->isBissextile = bissex(t->year);
	t->weekOfYear = calc_week(t->dayOfWeek,t->dayOfYear,t->year,t->isBissextile);

}
