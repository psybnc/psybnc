/* $Id: p_hash.c,v 1.3 2005/06/04 18:00:14 hisi Exp $ */
/************************************************************************
 *   psybnc2.3.2, src/p_hash.c
 *   Copyright (C) 2003 the most psychoid  and
 *                      the cool lam3rz IRC Group, IRCnet
 *			http://www.psychoid.lam3rz.de
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 1, or (at your option)
 *   any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef lint
static char rcsid[] = "@(#)$Id: p_hash.c,v 1.3 2005/06/04 18:00:14 hisi Exp $";
#endif

#define P_HASH

#include <p_global.h>

/* local types */

struct hasht {
    int commandmsgnum;
    int(*handler)(int);
    int commentmsgnum;
    int helpfilemsgnum;
    int proceed; /* 0 = dont proceed, 1 = proceed, 2 = return code of routine (0 or 1) */
    int adminonly; /* 0 = normal users, 1 = admins */
};


struct hasht inboundhash[]={
    {1009,	cmdprivmsg,	0,	0	,2,0},
    {1010,	cmdping,	0,	0	,0,0},
    {1011,	cmdquit,	0,	0	,0,0},
    {1012,	cmdwho,		0,	0	,1,0},
    {1013,	cmduser,	0,	0	,0,0},
#ifdef PARTYCHANNEL
    {1014,	cmdjoin,	0,	0	,2,0},	
    {1015,	cmdpart,	0,	0	,2,0},	
    {1016,	cmdtopic,	0,	0	,2,0},
#endif
    {1017,	cmdnick,	0,	0	,1,0},
    {1018,	cmdbwho,	1019,	1020	,0,0},
    {1021,	cmdpassword,	1022,	1023	,0,0},
    {1024,	cmdvhost,	1025,	1026	,0,0},
#ifdef PROXYS
    {1027,	cmdproxy,	1028,	1029	,0,0},
#endif
    {1030,	cmdsetusername,	1031,	1032	,0,0},
    {1033,	cmdsetaway,	1034,	1035	,0,0},
    {1036,	cmdsetleavemsg,	1037,	1038	,0,0},
    {1039,	cmdleavequit,	1040,	1041	,0,0},
    {1042,	cmdsetawaynick,	1043,	1044	,0,0},
    {1045,	cmdjump,	1046,	1047	,0,0},
#ifndef DYNAMIC
    {1048,	cmdbquit,	1049,	1050	,0,0},
    {1051,	cmdbconnect,	1052,	1053	,0,0},
#endif
    {1054,	cmdaidle,	1055,	1056	,0,0},
    {1057,	cmdautorejoin,	1058,	1059	,0,0},
    {1060,	cmdaddserver,	1061,	1062	,0,0},
    {1063,	cmddelserver,	1064,	1065	,0,0},
    {1066,	cmdlistservers,	1067,	1068	,0,0},
#ifdef NETWORK
    {1069,	cmdaddnetwork,	1070,	1071	,0,0},
    {1072,	cmddelnetwork,	1073,	1074	,0,0},
    {1075,	cmdswitchnet,	1076,	1077	,0,0},
#endif
    {1078,	cmdaddop,	1079,	1080	,0,0},
    {1081,	cmddelop,	1082,	1083	,0,0},
    {1084,	cmdlistops,	1085,	1086	,0,0},
    {1087,	cmdaddautoop,	1088,	1089	,0,0},
    {1090,	cmddelautoop,	1091,	1092	,0,0},
    {1093,	cmdlistautoops,	1094,	1095	,0,0},
    {1096,	cmdaddban,	1097,	1098	,0,0},
    {1099,	cmddelban,	1100,	1101	,0,0},
    {1102,	cmdlistbans,	1103,	1104	,0,0},
    {1105,	cmdaddask,	1106,	1107	,0,0},
    {1108,	cmddelask,	1109,	1110	,0,0},
    {1111,	cmdlistask,	1112,	1113	,0,0},
    {1322,	cmdaddignore,	1323,	1324	,0,0},
    {1325,	cmddelignore,	1326,	1327	,0,0},
    {1328,	cmdlistignores,	1329,	1330	,0,0},
#ifdef DCCCHAT
    {1114,	cmdadddcc,	1115,	1116	,0,0},
    {1117,	cmdlistdcc,	1118,	1119	,0,0},
    {1120,	cmddeldcc,	1121,	1122	,0,0},
    {1123,	cmddccchat,	1124,	1125	,0,0},
    {1126,	cmddccanswer,	1127,	1128	,0,0},
#define DCCALLOW 1
    {1129,	cmddccenable,	1130,	1131	,0,0},
#endif
#ifdef DCCFILES
    {1132,	cmddccsend,	1133,	1134	,0,0},
    {1135,	cmddccget,	1136,	1137	,0,0},
    {1138,	cmddccsendme,	1139,	1140	,0,0},
    {1141,	cmdautogetdcc,	1142,	1143	,0,0},
#ifndef DCCALLOW
    {1144,	cmddccenable,	1145,	1146	,0,0},
#endif
#endif
#ifdef DCCANCEL
    {1147,	cmddcccancel,	1148,	1149	,0,0},
#endif
#ifndef DYNAMIC
    {1150,	playprivatelog,	1151,	1152	,0,0},
    {1153,	eraseprivatelog,1154,	1155	,0,0},
#endif
#ifdef TRAFFICLOG
#ifndef DYNAMIC
    {1156,	cmdaddlog,	1157,	1158	,0,0},
    {1159,	cmddellog,	1160,	1161	,0,0},
    {1162,	cmdlistlogs,	1163,	1164	,0,0},
    {1165,	playtrafficlog,	1166,	1167	,0,0},
    {1168,	erasetrafficlog,1169,	1170	,0,0},
#endif
#endif
#ifdef CRYPT
    {1171,	cmdencrypt,	1172,	1173	,0,0},
    {1174,	cmddelencrypt,	1175,	1176	,0,0},
    {1177,	cmdlistencrypt,	1178,	1179	,0,0},
#endif
#ifdef TRANSLATE
    {1180,	cmdtranslate,	1181,	1182	,0,0},
    {1183,	cmddeltranslate,1184,	1185	,0,0},
    {1186,	cmdlisttranslate,1187,	1188	,0,0},
#endif
    {1189,	cmdrehash,	1190,	1191	,0,1},
#ifdef MULTIUSER
    {1192,	cmdadmin,	1193,	1194	,0,1},
    {1195,	cmdunadmin,	1196,	1197	,0,1},
    {1198,	cmdbkill,	1199,	1200	,0,1},
#endif
    {1201,	cmdsockstat,	1202,	1203	,0,1},
#ifdef MULTIUSER
    {1204,	cmdadduser,	1205,	1206	,0,1},
    {1207, 	cmddeluser,	1208,	1209	,0,1},
#endif
#ifdef LINKAGE
    {1210,	cmdname,	1211,	1212	,0,1},
    {1213,	cmdlinkto,	1214,	1215	,0,1},
    {1216,	cmdlinkfrom,	1217,	1218	,0,1},
    {1219,	cmdrelaylink,	1220,	1221	,0,1},
    {1222,	cmddellink,	1223,	1224	,0,1},
    {1225,	cmdlistlinks,	1226,	1227	,0,0},
    {1228,	cmdrelink,	1229,	1230	,0,1},
#endif
#ifndef DYNAMIC
    {1231,	playmainlog,	1232,	1233	,0,1},
    {1234,	erasemainlog,	1235,	1236	,0,1},
#endif
    {1237,	cmdaddallow,	1238,	1239	,0,1},
    {1240,	cmddelallow,	1241,	1242	,0,1},
    {1243,	cmdlistallow,	1244,	1245	,0,1},
#ifdef SCRIPTING
    {1246,	cmdreloadscript,1247,	1248	,0,0},
    {1249,	cmdlisttasks,	1250,	1251	,0,0},
#endif
    {1342,	cmdsetlang,	1343,	1344	,0,1},
    {1252,	printhelp,	1253,	1254	,0,0},
    {0,		NULL,		0,	0	,0,0}
};

struct hasht outboundhash[]={
    {1255,	msgERROR,	0,0,0,0},
    {1256,	msg001,		0,0,1,0},
    {1257,	msg002to005,	0,0,1,0},
    {1258,	msg002to005,	0,0,1,0},
    {1259,	msg002to005,	0,0,1,0},
    {1260,	msg002to005,	0,0,1,0},
    {1261,	msg251to255265to266,	0,0,1,0},
    {1262,	msg251to255265to266,	0,0,1,0},
    {1263,	msg251to255265to266,	0,0,1,0},
    {1264,	msg251to255265to266,	0,0,1,0},
    {1265,	msg251to255265to266,	0,0,1,0},
    {1349,	msg251to255265to266,	0,0,1,0},
    {1350,	msg251to255265to266,	0,0,1,0},
    {1266,	msg372to375,	0,0,2,0},
    {1267,	msg372to375,	0,0,2,0},
    {1268,	msg376,		0,0,0,0},
    {1269,	msg352,		0,0,2,0},
    {1270,	msg353,		0,0,1,0},
    {1271,	msg315,		0,0,2,0},
    {1272,	msg324,		0,0,1,0},
    {1273,	msg332,		0,0,1,0},
    {1274,	msg366,		0,0,1,0},
    {1275,	msg432to437,	0,0,2,0},
    {1276,	msg432to437,	0,0,2,0},
    {1277,	msg432to437,	0,0,2,0},
    {1278,	pong,		0,0,0,0},
    {1279,	gotnick,	0,0,1,0},
    {1280,	gotjoin,	0,0,1,0},
    {1281,	gotpart,	0,0,1,0},
    {1282,	gotquit,	0,0,1,0},
    {1283,	gotkick,	0,0,1,0},
    {1284,	gotmode,	0,0,1,0},
    {1285,	gottopic,	0,0,1,0},
    {1286, 	msgprivmsg,	0,0,2,0},        
    {1287, 	msgprivmsg,	0,0,2,0},        
    {0,		NULL,		0,0,0,0}
};

#ifdef INTNET

struct inthasht {
    int commandmsgnum;
    int(*handler)(int,int);
    int commentmsgnum;
    int helpfilemsgnum;
    int proceed; /* 0 = dont proceed, 1 = proceed, 2 = return code of routine (0 or 1) */
    int adminonly; /* 0 = normal users, 1 = admins */
};
    
struct inthasht inthash[]={
    {1288,	cmdintprivmsg,	0,0,0,0},
    {1289,	cmdintnotice,	0,0,0,0},
    {1290,	cmdintjoin,	0,0,0,0},	
    {1291,	cmdintmode,	0,0,0,0},
    {1292,	cmdintpart,	0,0,0,0},	
    {1293,	cmdintkick,	0,0,0,0},
    {1294,	cmdintnick,	0,0,0,0},
    {1295,	cmdintnames,	0,0,0,0},
    {1296,	cmdintquit,	0,0,0,0},
    {1297,	cmdintwho,	0,0,0,0},
    {1298,	cmdintwhois,	0,0,0,0},
    {1299,	cmdintuser,	0,0,0,0},
    {1300,	cmdinttopic,	0,0,0,0},
    {1301,	cmdintinvite,	0,0,0,0},
    {1358,	cmdintuserhost,	0,0,0,0},
    {1359,	cmdintison,	0,0,0,0},
    {0,		NULL,		0,0,0,0}
};

/* the internal network inbound hash processing */

int internalinbound(int iusern, int link)
{
    int rc,ca;
    int usern;
    int hashn=0;
    int ret,dummy;
    pcontext;    
    usern=iusern;
    if ((usern>0 && user(usern)->instate > STD_NOCON) || link>0) {
	 parse();
	 if(ifcommand(lngtxt(1302)) && usern>0) return 0x0; /* nick only changed on server event */
	 while(inthash[hashn].commandmsgnum!=0)
	 {
	    if (ifcommand(lngtxt(inthash[hashn].commandmsgnum)))
	    {
		ret=inthash[hashn].proceed;
		if(inthash[hashn].adminonly==1 && user(usern)->rights!=RI_ADMIN && link==0)
		{
		    ssnprintf(user(iusern)->insock,lngtxt(1303),me,user(usern)->nick);
		    return 0x0;
		}
		pcontext;    
		if(ret==2)
		{
		    if(inthash[hashn].handler!=NULL)
			ret=(*inthash[hashn].handler)(usern,link);
		}
		else
		{
		    if(inthash[hashn].handler!=NULL)
			dummy=(*inthash[hashn].handler)(usern,link);
		}
		pcontext;    
		if (ret==0) return 0x0;
		break;
	    }
	    hashn++;
	  }
	  if(user(usern)->instate==STD_CONN && link==0)
	  {
	      ssnprintf(user(usern)->instate,lngtxt(1304),me,user(usern)->nick,irccommand);
	  }    
    }
}

#endif

/* displays a hash help text list, or a file to a topic */

int userhelp(int usern, char *cmd)
{
    int hashn=0;
    static char bufx[200];
    char lc=' ';
    FILE *inp;
    pcontext;    
    if(cmd==NULL)
    {
	while(inboundhash[hashn].commandmsgnum!=0)
	{
	    if(inboundhash[hashn].adminonly==1)
		lc='*';
	    else
		lc=' ';    
	    if (inboundhash[hashn].commentmsgnum!=0)
	    {
		ssnprintf(user(usern)->insock,lngtxt(1305),user(usern)->nick,lc,lngtxt(inboundhash[hashn].commandmsgnum),lngtxt(inboundhash[hashn].commentmsgnum));
	    }
	    hashn++;
	}
	ssnprintf(user(usern)->insock,lngtxt(1306),user(usern)->nick);
#ifdef SCRIPTING
	aliasbhelp(usern);
#endif
    } else {
	strmncpy(irccommand,cmd,sizeof(irccommand));
	ucase(irccommand);
	ssnprintf(user(usern)->insock,lngtxt(1307),user(usern)->nick,irccommand);
	while(inboundhash[hashn].commandmsgnum!=0)
	{
	    if(ifcommand(lngtxt(inboundhash[hashn].commandmsgnum)) && inboundhash[hashn].helpfilemsgnum!=0)
	    {
		if ((inp=fopen(lngtxt(inboundhash[hashn].helpfilemsgnum),lngtxt(1308)))==NULL)
		{
		    ssnprintf(user(usern)->insock,lngtxt(1309),user(usern)->nick,lngtxt(inboundhash[hashn].helpfilemsgnum));
		    return 0x0;
		}
		while(fgets(bufx,sizeof(bufx),inp))
		{
		    ssnprintf(user(usern)->insock,lngtxt(1310),user(usern)->nick,bufx);
		}
		fclose(inp);
		break;
	    }
	    hashn++;
	}
        if(inboundhash[hashn].commandmsgnum==0)
	{
	    ssnprintf(user(usern)->insock,lngtxt(1311),user(usern)->nick,irccommand);
	}
    }
    return 0x0;
}

/* the userinbound hash processing */

int userinbound(int iusern)
{
    int rc,ca=0;
    int usern,ausern;
    int hashn=0;
    int ret,dummy;
    pcontext;    
    ausern=iusern;
    if(user(iusern)->parent!=0) ausern=user(iusern)->parent;
    usern=ausern;
    usern=checknetwork(usern);
    while (checknetwork(ausern)!=ausern); /* filtering other network tokens */
    if(usern<=1000)
    {
#ifdef SCRIPTING
	if(SCRSOCK==0) /* ouch. dont inherit the socket from a scripting input */
#endif
	    user(usern)->insock=currentsocket->sock->syssock;
	parse();
#ifdef SCRIPTING
	ca=executealias(usern);
	if(ca==0)
	{
#endif
	    while(inboundhash[hashn].commandmsgnum!=0)
    	    {
		if (ifcommand(lngtxt(inboundhash[hashn].commandmsgnum)))
		{
		    ret=inboundhash[hashn].proceed;
		    if(inboundhash[hashn].adminonly==1 && user(usern)->rights!=RI_ADMIN)
		    {
			ssnprintf(user(ausern)->insock,lngtxt(1312),user(ausern)->nick);
			return 0x0;
		    }
		    pcontext;    
		    if(ret==2)
		    {
			if(inboundhash[hashn].handler!=NULL)
			    ret=(*inboundhash[hashn].handler)(usern);
		    }
		    else
		    {
			if(inboundhash[hashn].handler!=NULL)
			    dummy=(*inboundhash[hashn].handler)(usern);
		    }
		    pcontext;    
		    if (ret==0) return 0x0;
		    break;
		}
		hashn++;
	    }
#ifdef SCRIPTING
	}
#endif
    }
#ifdef INTNET
    if(usern>10000)
    	internalinbound(usern-10000,-1);
    else
#endif
    if (user(usern)->outstate > STD_NOCON) {
#ifdef SCRIPTING
        if(ca==0)
#endif
    	    writesock(user(usern)->outsock,ircbuf);
    }
}

/* the useroutbound hash processing */

int useroutbound(int usern)
{
    int rc,ret;
    char *po;
    int hashn=0;
    int dummy;
    pcontext;    
    if(usern<=10000) 
    {
	if (user(usern)->parent != 0) {
	    user(usern)->insock=user(user(usern)->parent)->insock;
	    user(usern)->instate=user(user(usern)->parent)->instate;
	}
	if (user(usern)->autopongreply ==0) {
	    ssnprintf(user(usern)->outsock,lngtxt(1313),user(usern)->server);
	    user(usern)->autopongreply=200;
	}
	user(usern)->autopongreply--;
	parse();
	while(outboundhash[hashn].commandmsgnum!=0)
	{
	    if (ifcommand(lngtxt(outboundhash[hashn].commandmsgnum)))
	    {
		ret=outboundhash[hashn].proceed;
		pcontext;    
		if(ret==2)
		{
		    if(outboundhash[hashn].handler!=NULL)
			ret=(*outboundhash[hashn].handler)(usern);
		}
		else
		{
		    if(outboundhash[hashn].handler!=NULL)
			dummy=(*outboundhash[hashn].handler)(usern);
		}
		pcontext;
		if (ret==0) return 0x0;
		break;
	    }
	    hashn++;
	}
    }
    pcontext;
    if (user(usern)->parent != 0 || usern>10000) {
        addtoken(usern); /* adding the network token */
    }	
    pcontext;
    if (user(usern)->instate == STD_CONN && getpsocketbysock(user(usern)->insock)!=NULL) 
    {
        pcontext;    
	if (*ircbuf) writesock(user(usern)->insock,ircbuf);
#ifdef TRAFFICLOG
    } else {
        pcontext;    
        if (*ircbuf) checklogging(usern);
#endif
    }
#ifdef SCRIPTING
    pcontext;
    if(usern<=10000 && *ircbuf)
    {
	parse();
	if(!ifcommand(lngtxt(1314))) /* modes get triggered from parsing */
	    serverevent(usern);
    }
#endif
    return 0x0;
}

