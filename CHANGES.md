$Id: CHANGES,v 1.7 2005/06/04 18:13:37 hisi Exp $
CHANGES are always downwards compatible to earlier versions
(comment see EOF)
-------------------------------------------------------------------

Version 2.3.2:
-------------------------------------------------------------------
06-04-06   - Yeah, forgot a closing bracket, shame on me :)
06-04-05   - And back again, fixing the network separator, thanks for the freaking patch, Beeth.
             Why are finnish special chars allowed in nicks, but others not? A bit inconsequently, eh?
03-31-05   - Reverted to pre network separator patch, ~ won't be allowed
             in nicks in ircd 2.11.1
	   - Renamed log to p_log so compilation triggers no warnings.
03-25-05   - Finalized network separator patch.
03-21-05   - Changed network separator to be customizable in p_global.h
             Thank the .pl ircnet people to introduce ~ in nicknames :P
	     Also set default for DCCENABLE to 0 by public request.
02-22-05   - Fixed bug in p_sysmsg.c that caused an error with
             gcc 3.4, because sysparty() was used before being
	     declared. Fix provided by ksa (and got rid of DOS
	     linebreaks)
02-02-05   - Added QUIT logging support found and patched by mmalik
02-01-05   - Fixed compilation error in convconf.c with gcc 3.4.x
06-24-04   - A bug was found while terminating an ssl
             connection from explicitly disallowed ips
	     could lead to a denial of service
	     found by b-l-u-b
09-26-03   - segfaulted, when the user got kicked (only in 2.3.2-3)
             found by ICU
09-20-03   - multiple language file access bugs fixed
	     found by warlord
09-20-03   - menuconfig did react on space by redrawing the screen
             this could lead to 99% processor usage on differently
	     bandwithed client->server connections due to a massive
	     amount of data to be sent. linuxconf got this bug as well.
	     found by FireCode
07-26-03   - some clients need a nickchange, if psybnc cant regain
             a nick on reconnect. version 2.3.2 will always show 
	     an extra nickchange on reconnet, when the nick
	     from servermsg 001 doesnt fit the last used
	     bouncer nick.
	     found by ZZyZZ
07-25-03   - logging off from a multiconnection always logged the last ip
	     from where the client logged in last, even if an older client
	     quitted.
	     found by Jens
07-25-03   - connection delay raised to 5 seconds
	     needed to let oidentd time to answer correctly 
	     found by real-riot
07-25-03   - autoop refused to work after some time, so did banning and kicking. 
	     after 10 kicks/ops/bans, psyBNC refused to autoop/kick/ban before the next
	     serverping.
	     found by many users
07-24-03   - trafficlogging and ignores got loaded to same lists, 
	     could result in a segfault
	     found by ZZyZZ
07-24-03   - segfaulted in killoldlistener, when PSYBNC.SYSTEM.HOSTn was set to '*'
	     system dependent, also dependent, if IPV6 was enabled.
	     found by many (about 20 mails *g*)
07-23-03   - scripting on networks didnt work 
	     be sure to incorporate tokens, if you are using channels in your scripts. 
	     /SRELOAD also reloads the script for the network now 
	     found by Jens
07-22-03   - removed spamfilter for urls, users should use /addignore with contents they want to ignore 
             suggested by ZZyZZ
07-10-03   - logging on networks was buggy
	     no token got added on trafficlogging
	     found by me
07-06-03   - when switchnet was used, the kick message for the network 
	     to be made main was wrong 
	     found by leeps
07-05-03   - output to the traffic log didnt get flushed at write. 
	     found by ten9
07-01-03   - activated server events in scripting did not output coming up mode 
	     changes/joins anymore 
	     found by tuvok and [dave]
06-22-03   - DCC Bug on file receive - receive of data stopped at 40k. 
	     found by scr00ge
06-10-03   - substring match of scripting contents as like *blah* did not get 
	     triggered, when it was at end or at start of a matching text 
	     e.g. "bleh blah bleh" works, "bleh blah" doesnt, so doesnt "blah bleh") 
	     found by [nop]
05-10-03   - if a client got kicked, the kicking client got logged twice, 
	     and no reason got logged 
	     found by Mob
04-28-03   - fixed a bug in setting environment variables for a 
	     server event
	     found by [dave]
04-14-03   - fixed a bug in /unadmin and /admin
	     found by d.manglani

Version 2.3.1:
----------------------------------------------------------------------
08-14-02   - linking to dynamic ip dialup hosts caused
             sockets to stay open until the os detects the
	     inavailability of the interface.
	     found by fighter
07-24-02   - on a big sized conf-file psybnc could eat up to
             95% of cpu-usage.
	     found by starlein
07-16-02   - on startup, processor usage could become too heavy
             found by nomercy
07-15-02   - changed comparison of nicks to incasesensitive
             comparison. had the disadvantage, that a log
	     entry wasnt made, if the wrong cased nick was
	     adressed.
	     found by different users
07-12-02   - on a link connecting and a user disconnecting
             at the same moment a null pointer bug occured
	     in p_intnet.c
	     found by inFECT
	   - login penalty kept functional, even if the
	     password was changed. if the password gets
	     changed, the penalty gets reset with this version.
	     found by inFECT
07-08-02   - encryption with actions did not work.
             found by different users
           - link relay did not work. The link host was given
             instead of the irc server, also the connect was
	     done to an empty hostname.
             found by PartyAnml
06-16-02   - in case of no activated ssl-support a dcc send or
             a dcc file receive did not work, because the nick
	     wasnt set correctly.
	     found by ocr|bj
06-08-02   - fixed a bug in parsing networks from
             client input. The nick-command could
	     carry a network in the content of the
	     message.
	     found by arakis

Version 2.3.1-8:
----------------------------------------------------------------------
06-02-02   - fixed a bug in networking, giving channelmodes
             no parameters, if in first two segments of
	     CHANMODES reply.
	     found by me
	   - added a kind of opserv to intnet
	     if a user had op on an intnet channel,
	     and the channel is not cleared completely,
	     he will get op back on reconnect/rejoin.
	     deop and kick remove the autoflag.
	     found usefull by me
06-01-02   - fixed a bug in handling who in networks.
             channelresponses did not get prefixed.
	     found by BSB
	   - added ison and userhost on networks and intnet
	     found usefull by me
05-31-02   - fixed a bug while first connect. motd got displayed,
	     and a bad nick was given on connect during the first
	     server connect.
	     found by arakis


Version 2.3.1-7:
----------------------------------------------------------------------
05-26-02   - fixed a bug in CHANMODE-handling of servermsg 005
	     using wrong parameter-entries.
             thanks to perry
	   - own multiclient-messages on networks
	     where not prefixed.
	     found by dominance
	   - channel-topic maximal length set to
	     600
	     suggested by dominance
	   - listener on S=* did not work.
	     found by dominance and others
	   - fixed a bug in 005-Prefix-handling.
	     saved channels on rejoin did not get
	     the active users Prefix-modes, but the
	     standard @+ modes. Result: wrong mode-handling.
	     found by scr00ge
	   - stripping off space delimited PSYBNC.SYSTEM.ME-entries.
	     led to funny bugs, if the name consisted spaces.
	     found by cynapses
	   - changed the first msg asking the password back to a  
	     notice.
	     suggested by different users.

Version 2.3.1-6:
----------------------------------------------------------------------
05-16-02   - added multiclient-support. You can use as many clients
             as you wish to login into psybnc, older connections wont
	     get overridden by a new inbound connection. you can
	     disable this by using
	     #define NOMULTICLIENT
	     in the config.h - file before compilation.
	     suggested by Snq and Getty
	   - moved ncurses back to 'make menuconfig'. If you are
             using the old curses-library, try make menuconfig-curses.
	     not compatible to old curses-implementations.
	   - Readded systemmessages for networks. The bouncer did not
	     give any systemnotice to the user about network-connects 
	     and disconnects.
	   - Moved all system-notices to Messages from the nick -psyBNC.
	     Also enabled to let the user directly send messages to
	     -psyBNC, which would be handled as like commands.
	     This is a kind of "psyBNC-Console".
	     found usefull by me
	   - ADDDCC without SSL support did not work. A cryptic error
	     was given when a user tried to connect using ADDDCC.
	     Found by Jens and others
	   - Added oidentd2-Support
	     thanks to Andi
	   - When a ssl-Connect was not successfull, a listener-
	     socket got closed. This resulted in an infinite Loop
	     found by warlock
	   - Added italian language to the lng-files.
	     Thanks to 'Ard\ZzZz
	   - Added asynchroneous resolving.
	     Currently this is disabled on default, because the
	     implementation is experimental.
	     Idea by Aaron-[FGW]
	   - Removed random nicks, if nick isnt available.
	     The nick will be finished by a number, if the nick
	     isnt available, e. g. psychoid would become _psychoid, if
	     that nick would not be available as well, psychoi22.
	     Requested by fighter, inFECT and p.
	   - Added additional stoned check every 120 seconds for 240 seconds
	     possible delay of server response. The delay to check a stoned
	     server can be defined by
	     #define STONECHECKDELAY nn
	     e.g.
	     #define STONECHECKDELAY 300
	     and the waiting delay for an answer from the server before
	     disconnect can be set by using
	     #define STONETIME nn
	     e.g.
	     #define STONETIME 240
	     Requested by fighter and inFECT
	   - Improved Nick regain. Tries to regain nick all 10 seconds.
	     Requested by fighter and inFECT
	   - Added login penalty. If 3 tries of login are answered with
	     a false password, futher logins in a time of 10 minutes
	     plus random seconds (max. 50) dont succeed. This is thought to
	     disallow bruteforce-cracking of psybnc-Passwords from known
	     logins.
	     Suggested by different users
	   - fixed a segfault in menuconfig
	     found by Andi 
	   - fixed dcc-handling without using the own VHOST for connects
	     found by sebi
	   - removed the reference to gettxt(), it had collisions with
	     solaris 5.8 using a standard-gettxt() implementation.
	     found by Cajoline
	   - changed VHOST to BVHOST. This was needed, because some
	     ircds offer an inbuild VHOST-Command.
	     suggested by different users.	     
	   - changed BVHOST to be usable by admins for other users.
	     just use BVHOST username :host if you are an admin.
	     suggested by different users.
	   - added network-deletion on userdeletion.
	     found by me     
           - added own messages to appear on
	     the other own clients, if multiclients
	     are connected
	     requested by Snq
	   - added prefix (servermsg. 005) handling.
	     suggested by yath (sorry, no Prefixnames needed)
	   - passwordchanges only changed the password for the
	     main network
	     found by getty
	   - askop/autoop also where triggered, when the user
	     had no op.
	     found by ZZyZZ and denniz
	   - fixed a segfault in user deletion
	     found by Polobeer
	   - fixed a bug in curses menuconf. Double usage
	     of variable and call in menubox.c, and a
	     reference to ncurses.h in dialog.h
	     found by Snq
	   - removed static compilation. Possibly its more safe
	     on ptrace sniffing to compile a static binary, but on
	     the other hand a static binary consumes too many resources
	     and is problematic to compile on different os/archs, and
	     it also wont compile with shared ssl-libraries.
	     found usefull by me
	         
Version 2.3:

date	   boog/enhancement
----------------------------------------------------------------------
01-22-02   - Bug in Channel-Encryption solved by appending a 
             key checksum inside of the encrypted text.
	     e.g <user> [B]text text text <1522>.
	     If checksum doesnt match the known value,
	     you can be pretty sure the given text is spoofed.
	     found by Brian Rhea
09-27-01   - crypted keys for channels
             suggested by warlock

Version 2.3BETA:

date	   boog/enhancement
----------------------------------------------------------------------
09-15-01   - fixed USER.LOG-Entries in the language-Files
	     found by tric
09-09-01   - Removed a crash on a missing motd/ - directory
09-09-01   - Moved the user-logs to log/
09-09-01   - Replacement of '%' to the cyrillic 'ja' (255)
             to character 127.
	     found by Litil Divil
09-09-01   - Changed ncurses to curses. 
	     Also an issue for OpenBSD and other os
	     found by Szymon Czyz
09-09-01   - Fixed a bug on compilation on OpenBSD
	     occured due to problems in stdargs
	     found by Real-Riot

date	   boog/enhancement
----------------------------------------------------------------------
07-25-01    - Bugfix in dcc-Chats, psyBNC crashed when
              trying to establish a dccchat.
              found by coke
07-25-01    - Added SSLPATH to the config.h 
              and "SSL-Path" and "SSL-SecLevel" to
	      menuconfig
	      suggested by tric
07-25-01    - Fixed a segfault in menuconf when
              adding Users.
	      found by acidangel
07-25-01    - Fixed a bug in SSL-Constants, which only
	      got declared, when Scripting was enabled.
	      psyBNC could not be compiled, when scripting
	      was disabled.
	      found by warlock
07-17-01    - Bugfix in Translation, altavista once
              again changed their output format.
	      Also added korean, japanese, spanish
	      and russian to the translateable languages.
	      found by different users
07-13-01    - Added /ADDIGNORE,/LISTIGNORE, /DELIGNORE
              to allow psyBNC to ignore special contents
	      and/or Users.
	      suggested by warlock
07-13-01    - Added language Files
	      English or German can be choosen online
	      by setting /SETLANG german or /SETLANG english.
	      The language files are put into the
	      directory lang/.
	      If you want to translate psybnc to other
	      languages, please send the results to be
	      included into psybnc to psychoid@lam3rz.de
	      Also, new files in the subdir /help have to
	      be added.
	      menuconf, README, CHANGES, SCRIPTING and COPYING are only
	      available in english.
	      found usefull by me
05-27-01    - added SSL-Support. This needs openssl locally installed.
              Added cert/public key check as well.
              Get openssl at www.openssl.org.
	      psyBNC also allows to listen on encrypted ports.
	      Links between psyBNC can be setup to be encrypted
	      on ssl.
	      For clients you can use 'stunnel' to your
	      bouncer with version 2.3.
	      get stunnel at www.stunnel.org.
              suggested by different users.
05-27-01    - Removed the old psyPipe-stylish Link-Encryption.
              That has to be done using SSL Listeners.
	      consequence from adding SSL
05-13-01    - Moved accepting/Connecting fully to p_socket.c
              found usefull by me


Version 2.2.2:

date	   boog/enhancement
----------------------------------------------------------------------
04-14-01   - removed a bug in tokening networks
             found by {alex}
	   - removed all // - comments for old gcc-compilers
	     suggested by dor
	   - added checking for sys/time.h and time.h. did not
	     compile on newer glibc-hosts.
	     found by different users.

Version 2.2.2BETA:

date	   boog/enhancement
----------------------------------------------------------------------
03-24-01   - removed internal references to (v)snprintf,and referenced
             it to ap_(v)snprintf in all cases. removed parameters %p 
	     and %n from the list of usable formats.
             enhances security a bit.
           - usage of getipnodebyname instead of gethostbyname2 in the  
	     case of SunOS-IPV6-Extensions
             suggested by eldoc
	     patch by duckel
           - removed a sigsegv in resolve (null-pointer)
           - added command AUTOGETDCC
             only use this command if you are sure that you would
             like psybnc to get all files sent.
             suggested by steve_ (with default setting to OFF)
           - removed the annoying ACOLLIDE-Feature
             suggested by those, who where annoyed by nick-changes
           - fixed a bug in network-modes, where the network-name
             was put at the end of a mode-line
             found by real-riot
           - fixed a bug in writing the pid-file, changed from
             ending \r\n to \n.
             suggested by different users
           - set AUTOREJOIN by default to 0 (=off)
             if the bouncer should rejoin channels on kick/kill,
             set this to 1. (/AUTOREJOIN 1)
             suggested by those, who where annoyed by rejoining clients.
	   - fixed some bugs in menuconfig
	     found by different users
	   - removed some bugs in the english-phrases (confifuration, 
	     too much input..)
	     suggested by dor

Version 2.2.1:

date	   boog/enhancement
----------------------------------------------------------------------
10-28-00   - removed MIBs snprintf and replaced
             it to apaches snprintf.
	     should compile on Digital Unix/solaris older as well now
	     suggested by illusi0n
10-27-00   - fixed the channelsaving, so that channels
             would be saved on join
	     found by wudoo
	   - fixed DCC timout handling running into a null pointer
	     found by AndrewX
	   - fixed a bug in BHELP on aliases
	     found by AndrewX	     
	   - fixed a bug in trafficlogging
	     found by different users

Version 2.2.1(beta):

date	   boog/enhancement
----------------------------------------------------------------------
10-21-00   - fixed compilation of convconf.c, which
             wont compile on systems without snprintf
	   - added checking, if libbind is needed
	     found by bats
	   - added command AUTOREJOIN to disable 
	     rejoining channels after being kicked
	     requested by different users
	   - dcc handling wrote chat and send requests
	     to networks to the main network
	   - found by CAJOLiNE	   
10-14-00   - fixed whois on network, added networktokens
             found by arakis
	   - fixed invite on network, added networktoken
	     found by arakis
	   - fixed menuconfig-compile-bug
	     found by spy
	   - network-mode wasnt right parsed
	     found by arakis
	   - added DCCENABLE, to allow to set on the bouncer
	     to allow DCCs answered by the bouncer. Default
	     set to 1 (=enabled).
	     suggested by arakis
	   - added timeout period to wingate and socks-connect (3 seconds)
	     before sending USER and NICK.
	     suggested by arakis
	   - fixed some bugs in scripting
	     found by kode54
	   - added IPs only to hostallows
	     suggested by zycx
10-02-00   - Added Pn Parameters to scripting, which divides
             the ircinput to different Parameters
	     suggested by CAJOLiNE
10-02-00   - did not compile on solaris. Thanks to Phil Tyler
             for support :)
10-02-00   - on *BSD, the DCCANSWER command did not work.
             This happened because of a different handling
	     of numeric resolves under BSD.
	     found by CAJOLiNE
10-02-00   - If crypting was disabled, the bouncer could not
             compile because of variables still needed but
	     not defined.
	     found by ZZyZZ
10-02-00   - If a link in intnet was removed, all clients
	     quitted because of a wrong disconnecting-order
	     in the removelink-handling.
	     found by me
10-02-00   - crash on bogus DCC SEND and DCC CHAT requests
	     this bug was already fixed in a "fast-patch", called
	     psyBNC2.2p1.
	     found by cras
10-02-00   - if userdir wasnt writeable, the bouncer crashed.
             found by HERZ
10-02-00   - If linkage was turned off, the bouncer could not compile
             because it missed the "cmdrelink"-routine.
10-02-00   - In scripting, the content mask-compare did not work.
             found by AndrewB
10-02-00   - .ispoof was create rw User only, so a nobody-oidentd
             could not read it.
10-02-00   - the leavequit-command triggered the antiidle-flag.
10-02-00   - cleaning up structure
10-02-00   - A bug in names and networks set the userflag
	     behind the network token.
	     found by jerky
10-02-00   - Server notices and messages dont get logged anymore
             suggested by Tha-Mob

Version 2.2:

date	   boog/enhancement
----------------------------------------------------------------------
08-13-00   - Changed the mode handling for scripting, so
             every single mode will be handed over.
	   - Added timeouts to resolves
08-05-00   - Added SWITCHNET to switch from the main to another
             network.
	   - Added LEAVEQUIT to leave all channels when signing off
	   - Added LEAVEMSG $QUIT, if set to $QUIT, the leavemsg will
	     be posted from the text to the quitmessage
	   - made a cronny script
07-20-00   - Added scripting (cgi-stylish)
             See the readme and the litte example script
	     in scripts/example.
	   - Added DCC Chat & Files
	     Allows you to dcc chat directly from the
	     bouncer and to receive/send files from and
	     to the shell.
	     DCCCHATS are to be enabled with
	     #define DCCCHAT
	     and DCC File send/receive with
	     #define DCCFILES
	   - IPv6 Support
	     psyBNC allows from version 2.2 the support
	     of IPv6 in every mean. This includes listening
	     and connecting to IPv6 hosts. Also implemented
	     is DCC CHAT6 and DCC SEND6, but this is
	     still experimental, and only hell knows, if
	     it will be solved the way psybnc uses it.
	   - Support of oIdentd added
	     psyBNC allows the method of ident spoofing
	     oIdentd supports. 
	     Use
	     #define OIDENTD
	     in the config.h File to enable it.
	   - Moved all single INI-Files to one config file
	     called psybnc.conf by default.
	   - Multiple Listeners
	     psyBNC can listen on as many ips and Ports
	     as which are defined in the psybnc.conf file.
	     This also allows to listen on an intranet ip
	     and not as it was in 2.1 only on ALL interfaces.
	   - Moved channels/names to internal 
	     This allows names and channelspecific informations
	     much faster to be sent to the user, and will
	     not create traffic on the server when the user
	     rejoins psyBNC.
	   - Reorganized string routines, reorganized socket
	     string builds.
	   - Invented an internal ircd into psybnc, which 
	     can be adressed by using int~ as token.
	     This will be shared thruout the linked
	     network of psybnc, and allows all modes
	     including kicks/bans/op/voice.
	     This was done to replace the old &partyline,
	     which still can be used by setting the 
	     compiling option.
	     The internal network can be enabled by entering
	     #define INTNET
	     and the old partyline channel by
	     #define PARTYCHANNEL
	     to the config.h File.
	   - Flood protection
	     The serverside socket will wait for a server
	     reply before sending additional strings to
	     the server. This allows sending many lines
	     of text without getting flooded. The maximum
	     same total number of bytes is set to 700.
	   - Singleusermode / Multiusermode
	     If Multiusermode is disabled, the login has
	     not to fit to the added user, only the
	     password is being checked. Also, the user
	     may not add any other users than himself.
	     Multiusermode gets defined with 
	     #define MULTIUSER
	     and singleusermode will be active, when
	     MULTIUSER is not defined.
	   - Anonymous Mode
	     This allows to open the proxy for anonymous
	     users, so every login would be successfull.
	     Will be enabled with #define ANONYMOUS.
	   - Dynamic Mode
	     This will kill the server connection after
	     a user signed off from the bouncer.
	     Will be enabled with #define DYNAMIC
	   - Provider Configuration
	     The shellprovider can define a global
	     psyBNC config.h by putting this into
	     /psybnc/config.h. On Compilation that
	     config will be used.
	   - Bugfixes in Networking
	     The user did not see modes or nickchanges
	     correctly on secondary networks.
	   - The possibility to switch networks off
	     by removing the NETWORK define from
	     the config.h, the user cannot create any
	     new Networks.
	   - Bugfixes in Parsing
	     Some Bugs in Parsing of IRC-Strings
	     could crash the bouncer.
	     This has been solved with 2.2.
	   - Wrote a menuconfiguration with GUI
	     for psyBNC.
	     Just use 
	     make menuconfig
	     to start it.
	   - Deleted the os specific makefiles and
	     replaced them with a selfdetecting 
	     configuration tool, which will sense
	     the abilities of the system.

date	   boog/enhancement
----------------------------------------------------------------------
(last changes for 2.1)
06-02-00   - Altavista decided to change their website at
             babelfish.altavista.com, so i had to rebuild the
	     translation part.
04-19-00   - added password for servers
             see the changed syntax in /quote bhelp addserver
	     requested by plato
	   - added allow to switch logging off
	     just add
	     #define NOLOG
	     into the config.h before compiling
	     requested by [DRACON]
01-17-00   - another Bug in p_idea.c fixed
             found by XanTheR
	   - partyline channel joining fixed
	     found by bla
	   - erasemainlog crash fixed
	     found by plato
	   - added spamcheck / "you have notes waiting" filter
	     for private logging
	     i was annoyed by this :)
	   - whois bug fixed, if a whois on another nick was set
	     before reconnect, his channels were shown as to be on.
	     found by herz
	   - talk encryption also encrypted DCC requests and other
	     none-Message PRIVMSG transmissions.
	     2.1.1 will handle this messages correctly.
	     Warning: The Change for ACTION is NOT downwards compatible.
	     found by ace24
	   - a typo bug in ADDALLOW and DELALLOW to ADDALOW and DELALOW
	     reported by different Users
	   - relaying was disfunctional
	     found by mac
	     
Version 2.1:

date       boog/enhancement
----------------------------------------------------------------------
12-25-99   - crashed, when two partys got an encrypted talk
             established, and bogus data was sent using the
             IDEA prefix ([I]).
             found by gatecrash and by XanTheR
           - removed SIGDIE-action
             will now ignore any TERMINATE request.
           - bug fixed in linkage, crashed on uplink loss
             found by plato
           - did not compile on sunos-sparc (no static compiling)
             found by _enigma     

date	   boog/enhancement
----------------------------------------------------------------------
11-29-99   - added connection and talk encryption
	     see the readme or /bhelp ENCRYPT for further infos.
	     requested by me
	   - redevelopment of the connection handling
	     this demon can really handle 500 connections w/o lag now :)
	     requested by me
	   - full channellogging
	     see /bhelp PLAYTRAFFICLOG for more infos.
	     requested by different users
	   - added a "last on" field to /bwho
	     requested by mac
	   - redevelopment of the help function and
	     creating help texts for any topic
	     requested by those who dont understand everything :)
	   - the partyline has been moved to a partychannel..
	     but dont try to join it on irc, it's not really existent :)
	     the channel is &partyline by default, until you dont part it,
	     see p_global.h if you want to change it. still downwards
	     compatible. A user with old psybnc will still see $$ as
	     partyline, also in a new psybnc network.
	     you are automatically joined to it :)
	     mentioned by me, accepted by all tcl members :)
	   - sysmsg-command removed
	     well, just part the partyline channel if you are annoyed :)
	     consequence from doing a partychannel
	   - hostallows included
	     This allows you to set hosts which may connect the bouncer.
	     see /bhelp ADDALLOW or /bhelp DELALLOW.
	     requested by me.
	   - translator included
	     This allows you to translate queries in italian, german,
	     french or portugese to english and vice versa.
	     see /bhelp TRANSLATE for further infos.
	   - setaway splitted into /setaway, /setawaynick and
	     /setleavemsg.
	     requested by bla.
	   - changed the makefiles.
	     please read the readme file.
	   - added channel key support.
	     you can set a key for channel joins.
	     requested by XanTheR
	   - added autoop features
	     allows you to set users and their hostmasks to autoop.
	     be sure not to use this feature extensivly. Autoop is
	     extremeley insecure.
	     requested by keule and others.
	   - added external proxy support.
	     For Usage of sock4, WinGates and Webproxys which allow
	     the CONNECT request.
	     requested by different Users.
	   - made the 5000 lines of code modular.
	     it was really time to do that.
	     requested by wudoo (i hope you get it now *g*).
	   
Version 2.0.2:

date	   boog/enhancement
----------------------------------------------------------------------
08-16-99   - eggies newer than 1.3.2x needed to wait for input done
             before sending the user/password
	     found by nomercy
	   - allows you to switch off systemmessages in the
	     partyline using /SYSMSG 0 and to turn it off
	     using /SYSMSG 1
	     requested by multiple users
	   - BitchX needs names messages after JOIN message of each
	     channel. The bouncer will now send a fake answer and
	     then send the real answer on names gotten from the server
	     it should work now.
	     requested by multiple users
	   - Some clients (as like mIRC) retreive the local IP
	     from the 001 Message the server responds. 
	     This is fixed, the motd will contain the users real
	     ip now.
	     found by multiple users.
	   - When reconecting to the bounce, it randomly reasked
	     for op sometimes.
	     found by gatecrash    

Version 2.0.1:

date	   boog/enhancement
----------------------------------------------------------------------
07-29-99   - dead connections solved, when a client machine
             got offline without the bouncer noticing it
	     found by gatecrash
	   - on third /adddcc the bouncer got to a endless loop
	     found by coke
	    
Version 2.0:

date	   boog/enhancement
----------------------------------------------------------------------
07-18-99   - when /relaylink was set to 0, the message
             from the bouncer answered with "relay enabled", and
	     on setting to 1, with disabled. Just a wrong message *g*
	     found by coke.
07-17-99   - usernames with 0-length did not appear
	     on the BWHO list. 
	     found by me
	   - added SETUSERNAME to set the username
	     to a user defined string
	     requested by denniz
	   - changed the email of the systemmessages.
	     requested by myself
	   - automatically setting MODE +i-s nick on connect
	     to prevent the filling of the logs with spam
	     requested by myself
07-09-99   - fixed a bug which occurs on removing 
	     a nonexistent dcc session, which resulted
	     in a deadlock and processor usage of up to
	     90 %. heavy one :)
	     found by birdy
07-07-99   - fixed IAM broadcasting in bouncer linkage
	     found by me
	   - fixed linksaving only saving 4 digits ports
	     found by dracon
	   - fixed message multiposting on multiple clients
	     found by plato
	   - fixed wrong nick adressing
	     found by me
	   - fixed posting the partylines query
	     to the current users partyline nick
	     found by me
06-29-99   - Crypting of Passwords
             requested by SpWn
	   - variable Memory - Management    
	     this improves the Performance on < 586 Processors.
	     also the memory used is drastically reduced.
	     requested by gatecrash and others
	   - Multinetworking
	     requested by myself
	     idea by scut
	   - Linking of Bouncers    
	     requested by myself
	   - No Nick Change on Nick Loss
	     found by Coke
	   - fixing the Setaway-Query
	     found by SpWn  
	   - Bot-DCC
	     requested by myself

Version 1.2:

date	   boog/enhancement
----------------------------------------------------------------------
06-19-99   - The Nick was changed until regained
             This Function was thought as anticollide due
	     to the fact a nick is only lost in being a
	     collide victim. Changed this to an option.
	     Just use /quote acollide 0|1 to remove or
	     set Nick cycling after loss.
	     found by plato.
	   - The Partyline query to $NICK was lost 
	     when the ircnick was changed compared
	     to the login. This is fixed.
	     found by plato and hotacid.
	   - AskOp could flood the bouncer, when too
	     much bots where added. This version always
	     only asks one bot/bounce for op for every channel.
	     found by dracon and julze.
	   - changed the command /quote ADMIN to /quote MADMIN
	     /admin is a standard irc command and therefore
	     conflicts with the bounce command.
	     found by myself.    
	   - changed server-Ping-reply to asked content
	     Thats needed on spoof-checking irc-servers.
	     found by myself.

Version 1.1:

date	   boog/enhancement
----------------------------------------------------------------------
06-01-99   - finally created a makefile *g*
05-31-99   - maximum Users fixed.. some maxuser entrys still
	     where constant :)
	     found by myself
05-25-99   - fixed a bug in connecting from blocking sockets
	     bugged transmission of data
             (some windoze sockets are blocking only)
	     found by myself
05-24-99   - fixed a timeout bug in readsock(), which could lead
	     to denial of service to the proxy - heavy one :)
	     found by myself
05-23-99   - fixed PASSWORD bug on displaying the currentusers pass
	     found by plato
	   - added unset of away when reconnecting to the bouncer
	     using setaway
	     requested by dracon
05-21-99   - fixed timeouts on connects
	     found by xanther
	   - fixed a bug in deleting first added server,
	     resulting in error message "no server added"
	     found by dracon
	   - fixed bjump, did not save actual channel stats
	     found by sister_j
	   - changed /password to let admins set passwords
	     of users
	     requested by dracon
	   - added /setaway to set a leaving message that will be
	     posted to the channels and set as away text when leaving
	     the bouncer.
	     based on a request by plato.    
	   - added nonblocking to build connections
	   - added channel flags to all maskcommands
	   - added a setaway function
	   
Version 1.0:

date	   boog/enhancement
----------------------------------------------------------------------

05-12-99   - fixed antiidle, rejoining and reasking for op
             on join
	   - replaced autopong to a server ping for normal
	     timing
	   - added ./make.sunos, i hope it works :)

------------------------------------------------------------------------
\x68\x48\x69\x2e\x0a\x89\xe1\x31\xc0\x31\xdb\xb0\x04\x89\xc2\xcd\x80\xb0\x01\xeb\xfa
