psyBNC 2.3.2
------------

This program is useful for people who cannot be on irc all the time. Its used to 
keep a connection to irc and your irc client connected, or also allows to act as 
a normal bouncer by disconnecting from the irc server when the client disconnects.

Being installed on a shell with a permanently connected machine you stay connected 
as long you want or until the program crashes *g*.

Due to the fact this programm allows multiple Users at the same time (just change 
MAXUSER in the config.h file to get less or more possible Users) you can also trade 
kinds of connections with other people running only one backgroundtask at all. 
Thats very interesting by using shells which prohibit more than one backgroundtask.

Remember, sometimes admins also care about traffic, traffic by one client connected 
to irc is approx. 170 MBytes of traffic per month.

If you would have 10 clients connected and it gets 1.7GB of traffic it also may
become serious to your shell :)

psyBNC2.0 has been completely rewritten in memory Management. Until Version 1.2
the user structures had been held constant available. From Version 2.0
every structure will be created on demand. Also files will be only read once
to improve performance.

psyBNC2.1 has been completely rewritten in socket management. Up to Version 2.0.2
every socket was single selected for socket action. From 2.1 on, a select appears
once, then all sockets will be processed. This enhances performance on a big
amount of connections rapidly and lets them being processed without any lag.
Also, psyBNC2.1 has been structurized. psyBNC1.0 was meant as pipe only, and
it grew in the meantime.

psyBNC2.2 has been completely rewritten in string and configuration files handling. 
From 2.2, psybnc only uses one config file at all, which even can be given at the 
first argument at start of psybnc. Also channel handling was rewritten for 2.2, 
every names-list and hostlist of users and channels will be kept in psybnc-memory, 
so that a connect would return all channelmodes and users as fast as possible
without creating traffic to the server.

psyBNC2.3 has not been completely rewritten. But ssl was added, and all strings where 
moved to Parameter-Files to allow different languages. Also, a nice SCRIPTING manual 
was added. And some bugs fixed.

The OVERVIEW:
-------------

a.) Compiling
b.) setting up

What is psyBNC capable of ?

1.) It stays connected when you disconnect
a.) The DYNAMIC flag disables this
2.) It allows Full Online Administration, the only thing you will have
    to set up before starting is your port where the demon should
    listen (standard is set to 31337. arent we cool ?).
    Also, psyBNC 2.2 offers a GUI-Interface for setting up the
    major settings for psyBNC. just insert
    
    make menuconfig
    
    at your shell prompt. Be sure your terminal program allows
    the handling of curses and the shell host has curses installed.
    
3.) It allows VHOSTS. It also allows the usage of VHOSTS on linked
    bouncers, if the admin declared the link to your bouncer as
    relaylink.
4.) It allows you to set bans on the client
5.) It allows you to set Users who could get op from psyBNC matched
    by password and hostname.
6.) It allows you to set Hosts and Passwords to clients, where psyBNC
    could get ops from, if it once get disconnected or you
    just want to automize that process even when you are connected.
    Also, it allows the automatic connection build to eggdrop and
    getting ops from those (see bot-dcc).
7.) Its absolutely floodproof due to the fact it has no responses at
    all. From 2.2, it got a reliable flood protection, so you 
    even could post full articles to channels without getting
    flooded.
8.) Its hack proof (as far as i can say *g*)
    It has no backdoors.
9.) It allows a partyline on psyBNC (a channel named &partyline)
    or querying specified Users on psyBNC (prefixed User-Login with $),
    also on linked bouncers.
    From 2.2 it allows a internal IRCD mode, where psybnc emulates
    a shared full compatible IRCD using the network token "int".
    All Channels and users will be shared over linked bouncers, also
    modes and kicks are possible, you can get op on channels and give
    this to others also.
10.)It supports full logging of Messages and traffic (channel traffic
    included into Version 2.1). From 2.2 it stores those logs in 
    a different directory.
11.)It supports linking to other psyBNCs. This allows a shared partyline
    of those, who are using psyBNC and from 2.2 a shared irc network
    on psyBNC itself, the so called IntNet, which will be adressed
    using the token int~ (e. g. /join int~#mychannel).
12.)It allows multiple connections of one Client to different irc-servers
    and networks. So you just can be connected to ircnet and efnet with
    only one client programm running on your home box.
13.)It has inbuild password-encryption. Changed to blowfish on version 2.1.
14.)It allows the automatic linkage of a user to eggdrop bots using
    a dcc session.
    The traffic to/from the bot will be managed by querying to the 
    added botname with a leading ')'.
15.)It has connection and talk encryption. 

    After the keys where set, a block encrypted bouncer connection is build.
    Symetric encryption is done using the blowfish and IDEA algorithm.
    Talk encryption is only done by blockciphers, you have to set your
    encryption password local, also your partner(s) on irc who want
    to talk encrypted. Keys have to match for right decryption. You can
    set different keys also to users or channels.

    Connection Encryption in psyBNC2.3 was moved to SSL. If you want
    to encrypt your Bouncerlinks or your Connections from Users, you
    would have to install the OpenSSL package available on www.openssl.org.
    If a Client wants to install to an SSL-Listener, he would have to 
    setup stunnel on his home machine.

16.)It has a translation module.
    Did you ever want to talk to someone from italy without using
    a dictionary ? Well, psybnc has (from Version 2.1) a translator
    for talk inbuild. Currently supported are:
    
	- english to italian
	- english to german
	- english to portugese
	- english to french
	- french to english
	- german to english
	- portugese to english
	- french to english
	
    You just need to set a language to a channel or person, then you will
    receive messages both in the native language and in the language you
    selected to translate to. Same vice versa, the text you send to him
    will be translated to his native language (and also shown to you how
    it was translated).
    Keep in mind: You need to talk CLEAR to get it translated.
    Example:
    
        Hello i am happy to meet you
	
    would have a nice chance to get translated.
    
        Yo Ey eye em h4pee tew m33t yaw
	
    wont have any chance.
    
    How does it work ? Well, simple. It simulates a browser request to
    http://babelfish.altavista.com. And then processes the result. :)
    See p_translate.c for more details :)
    
    (Someone got a polish or slovenian dictonary ? *g*)
    
17.)psyBNC2.2 is modular. If you want to disable modules, just edit the
    config.h to disable features as like CRYPT, TRANSLATE or PARTYCHANNEL.
18.)psyBNC allows Hostrestrictions
    A file named psbnc.hosts keeps all Hosts which may connect to your
    bouncer. Remember, that also linkhosts need to be added.
    Moved to the psybnc.conf file in version 2.2.
19. psyBNC has IPv6 support, and will be automatically incompiled,
    if the automatic detection on start senses an existing IPv6 API.
20. psyBNC allows Scripting, you can setup a general Script for all
    users or a script for every single User in the scripts directory
    of psyBNC2.2.
21. psyBNC allows the usage of DCC Chat and file sending/receiving
    from and to psyBNC.
22. psyBNC supports oidentd, an ident demon, which allows to
    change the given back ident to the server by feeding a file
    named ".ispoof" in your home directory, so every of your
    users will get their logins as ident.
23. psyBNC2.2 allows a "providerconfiguration", where the admin
    can define the maximum functionality of psybnc. Just move 
    a config.h file to /psybnc/config.h (from root of your machine)
    and make it readable for all users.
24. psyBNC2.3 offers SSL-Encryption to servers, which support it,
    and for Users, which are able to use stunnel or other clients
    to connect to psyBNC. 
25. psyBNC2.3 is multi lingual. On default, english and german
    are supported.

c.) Contact me for suggestions
d.) Credits

(a) Ths Program was only tested on Linux, freebsd and SunOs (5.6), also
    solaris 7 and 8. 
    I only support usage of this code on linux, but maybe with enough 
    feedback for other platforms.
    Unpack it with tar -xzvf psyBNC2.3.1.tar.gz

    Edit the config.h File to fit your needs of functionality or
    do
    
    make menuconfig

    then just enter
    
    make
    
    to start the compilation. psybnc should find out itself, which
    environment is currently in use and compile to this.

    If you want to delete compiled objects and the compiled binary,
    do 
	make clean

(b) The only thing it needs is psybnc.conf in its application Directory.
    psyBNC2.2 has a default conf-file included, which will
    let psybnc listen on port 31337 on all IPs of your machine.
    From 2.2, you can add as many listeners as you like, on IPv4 or
    on IPv6 if supported by your machine.
    
    For every listener, enter the following:
    
    PSYBNC.SYSTEM.HOSTnn=ip
    PSYBNC.SYSTEM.PORTnn=port
    
    for example:
    
    PSYBNC.SYSTEM.HOST1=192.168.12.2
    PSYBNC.SYSTEM.PORT1=23989
    PSYBNC.SYSTEM.HOST2=192.168.12.1
    PSYBNC.SYSTEM.PORT2=12000
    
    for IPv6 just use an IPv6 ip in the HOST parameter to create
    the listener on that ip.    

    If you want to start setup an SSL-Listening-Connection, just
    put a S= in front of the host, for example:
    
    PSYBNC.SYSTEM.HOST1=S=192.168.12.2
    PSYBNC.SYSTEM.PORT1=23989

    This would create a SSL-encrypted Listening socket.
    
    Also you should add the name of psyBNC, if you plan to
    link it to other bouncers.
    
    PSYBNC.SYSTEM.ME=name
    
    for example:
    
    PSYBNC.SYSTEM.ME=mybouncer

(1) Once you added a server to connect to, psyBNC will try to stay connected
    until your server goes down or psyBNC shell server has problems. By
    defining DYNAMIC into the config.h file, psyBNC would disconnect
    at leaving.

(2) Commands for Online Administration are:
    /BHELP			-> The command overview
    /BWHO			-> lists all current Users of psyBNC
   */ADDUSER login :realname	-> adds a new User
   */DELUSER login		-> deletes the specified User
   */BKILL login                -> Kills a user on psyBNC (does not delete)    
    /PASSWORD [network~]newpass	-> changes your current Password
    /JUMP [network~]		-> Jumps to next Server in List
    /ADDSERVER [network~] [S=]irc.serv.er :port
				-> Adds a new IRC Server to your IRC-Server List
				   if S= is supported in front of the hostname,
				   the connection is being build as SSL-
				   Connection.
    /DELSERVER [network~] # (Number)       
				-> Removes the corresponding Server Number in
    /LISTSERVERS [network~]	-> Lists all Servers you added
    /ADDIGNORE [network~] content :hostmask
				-> Adds a new Ignore to your Ignore List
    /DELIGNORE [network~] # (Number)       
				-> Removes the corresponding Ignore Number in
    /LISTIGNORES [network~]	-> Lists all Ignores you added
   */BREHASH			-> rehashes the proxy. Kills all connections.
   */MADMIN			-> Declares a user as admin
   */UNADMIN			-> takes Admin rights from User
    /BQUIT [network~]		-> Quits the client from IRC, stays disconnected until
    /BCONNECT [network~]        -> removes Quit and tries to reconnect
    /SETAWAY [network~]text	-> sets away text (if whoised, shows the text)
    /SETAWAY [network~]		-> disables away text
    /SETAWAYNICK [network~]nick	-> sets away nick. Is changed to, when you leave
    /SETAWAYNICK [network~]	-> disables nickchange on leave
    /SETLEAVEMSG [network~]text	-> sets leave message. It will be posten when you leave.
				   If set to $QUIT , your Quit-Msg will be posted.
    /SETLEAVEMSG [network~]	-> disables leave message
    /SETUSERNAME [network~] :name
				-> sets the username (real name)
    /AIDLE [network~] 0|1	-> 0=disables Antiidle, 1=enables it (default)
    /LEAVEQUIT [network~] 0|1	-> 1=leave channels on quit, 1=keep on (default)
    /SOCKSTAT			-> Displays the list of open sockets, how
				   much bytes gone in and out, if encrypted, in
				   which state, since when open and other infos.
    
(3) psyBNC allows VHOSTS, Commands used for that are:    
    /BVHOST [network~] [LINK #] :your.vhost.com       
				-> sets your desired VHost
    /BVHOST			-> erases the Vhost, standard ip would be used

(4) You can set Bans that will also be set if you are off
    /ADDBAN [network~][#chan] reason :host
				-> Adds a new ban with specified Hostmask
    /DELBAN [network~] # (Number)		
				-> Removes a corresponding Ban shown in
    /LISTBANS [network~]	-> Lists the Bans you set

(5) You can specify Users who could get op from psyBNC by msging them
    in the from eggdrop known way (op password #channel).
    /ADDOP [network~][#chan] pass :host	
				-> Adds a new User with given Password
    /DELOP [network~]# (Number)	-> Deletes a user corresponding to
    /LISTOPS [network~]		-> Lists all Users

    Also autoop was added in psyBNC2.1. Be sure to use this function with
    care, because spoofing is still possible on most of the ircnetworks.

    /ADDAUTOOP [network~][#chan] :host
				-> Adds a User to the autoop list
    /DELAUTOOP (Number)		-> Deletes an added Autoop from
    /LISTAUTOOPS		-> Lists the added autoop-Entrys				

(6) If psyBNC got disconnected, the client will rejoin the channels on reconnect.
    In this Case, it will perform ASKOP, msging "op password #channel" to an added
    client. If you added a bot-dcc connection, you also can request op from
    the bot. You can administrate the list using following commands:
    /ADDASK [network~][#chan] pass :host or -botname
				-> Adds a Hostmask to get Op from with password
    /DELASK [network~]# (number)-> removes a host to ask op from, number from
    /LISTASK [network~]		-> List of hostmasks and passwords to Ask Op.

    If you want to add an addask from a bot-dcc, there is no need to
    support a password in /ADDASK.

    Adding a bot op request for a dcc linked bot, would be like the
    following example:
    
    /ADDASK #blah :-botnick

    If you established a connection to a bot using the /ADDDCC command,
    there will be no password sent via irc. That way is recommended.

(7) As long your client isnt connected, psyBNC will NOT answer any ctcp Querys

(8) Well, as long you dont use too dumb passwords *g*
    Passwords get encrypted using a hash created on compiling time.
    The source makesalt.c creates a file called salt.h, which keeps
    the needed encryption salts. Be sure to move the salt.h File to
    a safe place after you compiled psyBNC. Authentification passwords
    which dont need to be enciphered, are stored as blowfish crypt-by-key
    passwords.

(9) In psyBNC2.2 you can still use the old &partyline, 

(10)Everything gets logged. Every connect and disconnect. If you arent connected
    to your bouncer, all queries will be logged to your private log.
    Commands for the Connection Log are:
   */PLAYMAINLOG		-> Displays the connection logs
   */ERASEMAINLOG		-> Erases the connection logs
    Commands for Messagelogs are:
    /PLAYPRIVATELOG [network~]	-> Displays all querys
    /ERASEPRIVATELOG [network~]	-> Erases your logged queries.
    Commands for the Trafficlog are:
    /ADDLOG [network~] #channel/person :filter
				-> Allows you an source and filter
				   to log from. Example:
				   /ADDLOG #blah :*
				   would log everything on #blah.
    /DELLOG [network~] (Number) -> Deletes a source and filter shown from
    /LISTLOG [network~]		-> Lists the logentrys
    /PLAYTRAFFICLOG source fromdate fromtime todate totime :filter
				-> plays the trafficlog recorded
				   you can also specify another filter as
				   in /addlog to filter more special entrys
    /ERASETRAFFICLOG 		-> erases the traffic log, starts new.
    
    Some notes about Traffic-Logging:
    - be sure to set filters. Your shell/machine would log every traffic
      on a filter *. Typical filters would be JOIN, PART or KICK, to
      monitor all joins, parts or Kicks.
    - You have to care about the fact, that you have got enough space
      free on your shell to traffic log. Be sure to erase your Trafficlog
      as often as possible.
    - Traffic logging raises processor usage. Be sure to check the usage
      if you are going to log any event. And buy a new hd, if you only
      got 500mbyte :)

(11)It is possible to link your bouncer to others and to link other bouncers
    to yours. That allows a shared off-irc partyline and sharing of resources
    as like vhosts. Commands for linking are:
   */NAMEBOUNCER name		-> set the name of your bouncer
                                   to identify it in the network.
				   This has to be done before
				   adding links.
   */LINKTO :[S=]host:port	-> adds a host and port of another
                                   psyBNC, where your bouncer should
				   link to. If S= is supported, the
				   Connection is handled as SSL-Connection.
   */LINKFROM name :host:port	-> adds a bouncer, its host and its
                                   port that could link to your bouncer.
    /LISTLINKS			-> lists all added links in the network.
				   Changed in 2.2 to show only the 
				   local links and a tree of the
				   currently linked network.
   */DELLINK number		-> deletes the link with the number
                                   corresponding to LISTLINKS.
   
   */RELAYLINK number		-> allows users from the added link
                                   to relay link to your bouncer using
				   associated vhosts of your machine.    

(12)psyBNC allows the using of multiple irc sessions for one
    client. Its possible to be on ircnet, efnet and other
    networks by using one client only. 
    
    Before using this feature, read the following restrictions, which
    result from the usage of this feature:
    
    - You will always see yourself having the nick of the main
      network, even, if psyBNC needed to change your
      nick on the secondary network. Check, which nick your
      client is using on the other network. Maybe your nick
      was used, so psyBNC had to change it.
    - Every channel and user will be tokened with the network
      handle. If you want to join a channel on the network,
      you would have to give a command as like
               /JOIN #EfNet~#Talk
      or you would need to use commands as like
               /QUERY EfNet~Joe
      instead of /QUERY joe. You got the clue ?
    
    Commands for multiple networking are:
    /ADDNETWORK name		-> adds a network to your client
    /DELNETWORK name		-> removes a network from your client
    /SWITCHNET newname :oldname -> switches main to "oldname" and names
                                   your old main as "newname" network.
				 
    If you want to administrate the network specifics, you will
    have to use the commands already described above including the
    network token. Examples:
    
    /ADDOP EfNet~#talk password :*!*joe@*.blah.net
    /ADDDCC EfNet~lamebot lamer lamepass :ip3.blah.bleh.net:31337
    /ADDSERVER EfNet~irc.lame.org :6667
    
    You need to add a server for your network. It wont connect until
    you did.    
    You need to establish own dcc connections for every network
    if you want to use this feature. Thats logical, bots
    cant be on multiple networks :).
    
    The multinetworking can make problems in rejoining and resolving
    channel names. This is done asynchron, and its possible the
    client gets NAMES of both network for two different channels
    at the same time. Maybe mIRC acts funny :>.

(13)I added password encryption. Its a self made two-way
    password encryption and therefore crackable with some effort.
    Be sure to move the salt.h File created after compiling from
    your shell to your home box or another safe place you totally
    controll. If someone gets the salt.h File, he can decrypt
    all your passwords. If not, its much harder.
    From psybnc2.1 authentifications passwords are encrypted
    using blowfish.

(14)You can establish dcc connections to bots from psyBNC.
    This is for special usage with bots. It will send your
    login and password right after it connected to the bot
    for the login session. After your client joined the bots
    partyline successfully, it can ask for op from the bot.
    Commands for bot-dcc sessions are:
    
    /ADDDCC [network~]botname user password :[S=]host:port
				-> adds a new bot-dcc session
				   If S= is set before the host,
				   an SSL-Connection will be tried.
    /DELDCC [network~]number	-> deletes the previous
				   added dcc session, get the number
				   from
    /LISTDCC [network~]		-> lists all dcc connections

    The partyline of the bot will be queried to/from the botname
    prefixed by a ')'. Example:

    /QUERY )lamebot
    
    would set your query to the bots partyline.
    
    If you want to get op from the bot automatically, just do:
    
    /ADDASK #channel :)botname
    
    psyBNC would send the op query to the bots partyline then
    instead of querying to the irc client matching its hostname.
    This is recommended. Querying to other eggdrops is insecure
    because of possible spoofed clients who got op by takeover.
        
(15)Talk encryption
    Talk encryption was being introduced to annoy all those
    sniffing kiddies and to protect your privacy on irc.
    
    You need to set a password for a channel or person you are
    querying with. That is done via:
    
    /ENCRYPT [network~] password :channel/user
    
    Example:
    
    /ENCRYPT #leet :nothingbutapassword
    
    Your counterpart would have to do it vice versa. If you adress
    a channel, every person on the channel has to have the key.
    
    /DELENCRYPT (Number) 	-> deletes an entry for encryption from
    /LISTENCRYPT		-> Lists all encryption adresses and passwords.
    
    Also normal irc clients can request an encrypted connection. Clients
    for this purpose are ready on www.psychoid.lam3rz.de. Specifications
    can be requested from me (mail to psychoid@lam3rz.de).

    Bouncer- and Userconnectionencryption from Version 2.2 had been removed 
    and was replaced by SSL. If you want to use SSL in psyBNC, you would have
    to install OpenSSL on your machine. You should run stunnel on your home
    Machine to connect to psyBNC, if you want to encrypt Listeners as well.

    * This functionality needs CRYPT defined before compiling.
      It's set enabled in standard, so just remove it from config.h
      if you dont use it.

(16)Translating
    psyBNC allows you to set a user or channel as translation source/dest.
    So, everything you type, will be automatically translated to the
    language your counterpart talks. Same is with the text they talk,
    it also will be translated.
    
    Commands for this purpose are:
    
    /ADDTRANSLATE [network~]#channel/user :language-from language-to
    
    language can be:
	    en_de
	    en_it
	    en_fr
	    en_pt
	    de_en
	    it_en
	    fr_en
	    pt_en
    
    Examples:
    
    /TRANSLATE #bayern :de_en en_de
    
    Result would be, you would get both the german text spoken on #bayern 
    as also the english text.
    
    In return, everything you would type in english to #bayern, would be 
    posted in german to the channel.
    
    You can remove a Translation by:
    
    /DELTRANSLATE (Number)
    
    from the list displayed by
    
    /LISTTRANSLATE.

    * This functionality needs -DTRANSLATE defined before compiling.
      It's set enabled in standard, so just remove it from config.h
      if you dont use it.
    
(17)Modular structure
    Up to 2.1, psybnc was a single source code. I decided to part it into
    different sections. That allows you to remove a functionality from
    the config.h, if you dont need it (improves performance and decreases
    memory usage).
    
    This has to be done before Compilation.
    The following defines mean the follow functionality:
    
    CRYPT		-> allows encrypted connections, talk encryption
     BLOWFISH		-> chooses blowfish as symetric cipher
     DIDEA		-> chooses idea as symetric cipher
     
     You can only set BLOWFISH OR IDEA. Not both at the same time.
     psyBNC also will understand the other crypting, but it will
     use the selected encryption on sending data.
     
    PARTYCHANNEL	-> Enables the partychannel instead of the old $$
    TRANSLATE		-> Enables the translator.
    PROXY		-> Allows you to use external proxys (socks, wingate etc.)
    TRAFFICLOG		-> Allows you to log traffic.
    INTNET		-> enables the internal Network
    DCCCHAT		-> allows internal DCC Chats
    DCCFILES		-> allows to receive and to send files from psyBNC
    MULTIUSER		-> defines the proxy as multiuser-bouncer
    MAXUSER nn		-> Number of maximum Users
    MAXCONN nn		-> maximum Number of Link-Connections
    NETWORK		-> allow the User to create multiple Sessions
    SCRIPTING		-> allow Usage of Scripting
    PROXYS		-> allow Proxy-Usage
    LINKAGE		-> Allow to link psyBNCs
    OIDENTD		-> support the spoofing of idents using oidentd
    CTCPVERSION	"text"	-> the text to answer on a ctcp version query    
    ANONYMOUS		-> turns psyBNC into a "anonymous" bouncer, so
                           everyone can connect and login
    DYNAMIC		-> Disconnects a user, if he quits
    LOGLEVEL		-> 0=Errors, Warnings, Infos, 1 = Errors, Warnings
			   2=Errors

    Possibly you are not interested in privacy, so disable Crypting.
    Or you dont want a translation module, so just disable it.
    
(18)Host allows
    With psyBNC2.1 you will get the File psbnc.hosts. Those allows you
    to change Hosts which are allowed to connect. Standard is set to *, every
    host may connect.
    
    You can change that settings by:
    
    /ADDALLOW (Host)	-> Adds a host which is allows to connect
    /DELALLOW (Number)	-> Deletes an entry from
    /LISTALLOW		-> Lists the allowed Hosts
    
    A Host may be entered with matchcodes.
    Examples:
    
    /ADDALLOW dialup*.bleh.net
    
    or
    
    /ADDALLOW *.somewhere.org
    
    Also psyBNCs which want to link have to be added.

  (*) preceded Commands may be only used by admins

(19) psyBNC2.2 offers IPv6 support
    
    If your host allows the use of IPv6 Hostnames or IPs, psyBNC
    will be compiled with support of this protocoll. When your
    host offers devices which use the IPv6 Protocoll as well, you
    can use this hosts by setting them as vhosts, and by listening
    on them if you also connect from outside to this host via IPv6.
    
    Also dccchat and dccfiles is being supported with IPv6 
    (experimental), but it is not sure, if the DCC protocol will
    be realised for IPv6 the way it was done in psybnc.    

(20) psyBNC2.2 offers the use of scripting in a cgi-stylish way.

    Read the File SCRIPTING, which is included in every psyBNC-Package.
    
    A final Comment about scripting:
    psyBNC scripting processes fork from the main task to deny blocking
    of other users. This could not be solved in another way.
    Providerowners possibly would like to disable this function.
    
(21) psyBNC allows the Usage of DCC Chats and DCC Send/Receive of
     files on the shell and from the shell.
     
     /DCCCHAT [S=]nick			- offers the nick a DCC Chat
                                          session. If S= is given
					  in front of the Nick, an
					  SSL-Connection is being build.
     /DCCANSWER [S=]nick		- answers a received DCC Chat
                                          request. If S= is given in
					  front of the Nick, an SSL-
					  Connection is being build.
					  
     If an SSL-Connection has to be build, the counterpart would
     also have to setup SSL and would also need to use psyBNC-DCCCHAT
     for a working SSL-Link.

     An established DCC Chat gets into a query from (nick.
     DCC Chats from ports lower than 1024 will be denied for security reasons.
     
     /DCCSEND nick :file		- offers a send to the given nick
     /DCCGET nick :file			- accepts a file a user wants to send
     
     Accepted files will be stored in downloads/USER%n/, path arguments
     given on the filename will be ignored. File sends from /etc and /dev
     will be denied by rule from psybnc. '..' and '/' characters get
     automatically stripped on receive.
     
     /DCCSENDME file			- sends the user on psyBNC 
                                          the given file from the shell
     /DCCCANCEL nick
     or
     /DCCCANCEL nick :file		- will cancel an active DCC Chat
                                          or file receive/send.
     /AUTOGETDCC 0|1			- switches auto-getting of offered
                                          files off/on. Default is off.
					  
(22) psyBNC supports oIdentd, an ident demon which allows you to change
     the ident being given to the irc server. Read the 'INSTALL' file
     of oidentd for further informations.
(23) psyBNC allows a "providerconfiguration", a host-dependent config.h
     file, which will be used if existent. It needs to be located in
     /psybnc, from the root. its Name is also config.h, and it should
     be readable by all users.
     
     If present, psyBNC denies the change of userspecific settings.
(24) psyBNC allows the ability of using SSL-Connections, if OpenSSL is 
     installed on your Host.
     
     To use SSL on linking, you have have to setup Listeners, which
     listen on a Port supporting SSL. You may connect to an SSL
     Port with a psyBNC-Link or with a SSL-supporting Client.
     
     Add a listening Port to you Config which offers SSL:
     
     PSYBNC.SYSTEM.HOST1=S=192.168.12.2
     PSYBNC.SYSTEM.PORT1=23989
     
     If you want to link to this Port on another Bouncer, you will
     have to add the link using the LINKTO command on the other
     bouncer as follows:
     
     /LINKTO psybnc :S=192.168.12.2:23989
     
     On the bouncer which offers the SSL-Connect, you have to add:
     
     /LINKFROM mybnc :192.168.12.3:12345
     
     if the other bouncer is put on host 192.168.12.3 with the
     first listending port on 12345. 
     
     Warning: In a Linkfrom-Entry you always have to add the FIRST
     listening Port of the Linkto-Bouncer.
     
     If you want to connect to the SSL-Port, use an SSL-capable Client,
     or use stunnel, available at www.stunnel.org
     
     To use stunnel properly, use the following syntax:
     
     stunnel -r psybnchost:psybncport -c -d localhost:localport
     
     If you want to start an encrypted DCC or want to add an ircserver
     which offers SSL, just use S= in front of the hostname, e.g.:
     
     /ADDSERVER S=irc.encrypted.org:6667
     
     or
     
     /ADDDCC botname mynick maypass :S=ssl.bot.org:1234
     
     If you want to use pending DCCS encrypted via SSL, use:
     
     /DCCCHAT S=nick
     
     The counterpart would have to answer using
     
     /DCCANSWER S=mynick
(25) psyBNC from version 2.3 allows multi-language-capabilities.
     All phrases used by psyBNC are put into one special formatted
     language file in the directory lang/.
     
      The standard psyBNC Package includes the languages English
      and German.
      
      SETLANG language		- Sets the language of psyBNC
      
      Current arguments only can be "german" or "english".
      
      The setting takes place, after the command was issued.
      
      If you want to create a language File for you country.
      take a look on lang/english.lng.
      
      The format of the File is easy:
      
      msgnnnn=Text
      
      where nnnn is a zero-leading Number of the Message. Every
      number has a special context, every Format-Parameter must
      match to the english Pattern.

About Channels:

      #chan entrys are optional, if no chan is entered, the used
      command will be taken to all channels.

      Do not use [#chan] but #chan. The brackets only show its optional.

About Networks:

      network~ entrys are optional. If no network is given,
      your standard server connection will be adressed.
      
      Do not use [network~] but network~, the brackets only show
      also here that its just optional.

About the Partyline:

      In Version 2.1, the old $$-partyline was changed to an
      internal bouncer channel. This channel is not existent on irc.
      You cannot kick, ban, op or set any modes there. But you can
      set the topic, which is also broadcasted to the restnetwork.
      
      The channel is defined by the name &partyline.

      In Version 2.2 an "internal network" was introduced. This
      can be used by using the network int~ for adressing the
      channels on the linked bouncers. You can join channels,
      part them, add modes, kick, ban, set topics, as like
      on original IRC.

    - intnet needs #define INTNET in the config.h file.
      Partyline needs #define PARTYCHANNEL in the config.h file.

About the nick -psyBNC:

    From version 2.3.1 this nick accepts commands for psybnc, also
    posts all system-messages and errors from this nick as message
    to the user. If you /query -psyBNC, all sent text to this 
    nick will be given as entered command.

The Statement Hostmask may be a already from bots and irc known Hostmask
(for example *!*bleh@*.fuck.net).. be sure always using Hostnames, and never
Nicks only (dumb would be blah!*@*). The statement Host has to be
a fully valid hostname. psyBNC needs to get its ip, it has to
connect that.

(c) Contact me: psychoid@ircnet - on many channels. i love irc.	
		psychoid@lam3rz.de
		psychoid@gmx.net

		http://www.psychoid.lam3rz.de

		I prefer IRC for contact. Possibly a mail will
		be ignored, but a query wont.

		If you want to contact me, please try on ircnet first.

		Oh yes, i am NOT on efnet. If you met someone by the
		nick 'psychoid' on efnet, he surely was a fake. Especially
		the guy having "www.psychoid.lam3rz.de" in his real name.
		Very funny. Not.
		
(d) Credits

    match.c	by Chris Fuller
    curses-GUI	by Savio Lam
    Blowfish	by Bruce Schneier
    IDEA	by ascom ltd, switzerland
    bsd-setenv  by anonymous

    ap_snprintf by Apache Software
    -- apache notice --
     "This product includes software developed by the Apache Group
     for use in the Apache HTTP server project (http://www.apache.org/)."
    -- eof ------------    
     
    Helptexts	by [DRACON]

    Webpage 	by coke
    
    Hosting	by www.rewtbox.de

    Special thanks to: andi (basics of ipv6-coding, patches),
		       Phil Tyler (solaris suggestions for 2.2.1)
		       duckel (sunos IPv6, built into 2.2.2)
	
    Special greeting to: trinoo ;-) thanks for the funny night in "the yard"
    		       
    Thanks to all those who report bugs. A tool lives from its users.

There is given absolutely NO WARRANTY of functionality. I am not responsible
if anything gets screwed. Read the GNU-Public license which is included.


have fun,
psychoid / tCl
inFECT / eMPiRe