The psyBNC Scripting Engine
---------------------------

Due to the fact, noone really seems to understand the way psyBNC offers
Scripting to users, It should be defined a little bit more detailed in
this file.

CONTENTS

I. General
I.1. The general Idea
I.2. Event-handled Scripting
I.3. The DEFAULT Script
I.4. Special User Scripts
II. Variables
II.1 Global Variables for all events
II.2 Event-specific Variable
III. Events
III.1 The server-Event
III.2 The dialogue-Event
III.3 The dccchat-Event
III.4 The dccsend-Event
III.5 The alias-Event
III.6 The ctcp-Event
IV. Last words

I. General

psyBNC offers a different way of scripting. While e.g. eggdrop offers tcl as 
scripting-language, psyBNC offers its own mechanism of creating user-defined
scripts.

I.1 The General Idea

IRC sends Messages to the user, and the User sends Messages to IRC. If no
data is received from the server, and no data is received from the User,
no Action has to be taken normally.

The scripting in psyBNC is based on events, which happen only, if data
is being received from the user or from the server.

If an event matches to the syntax of the script, then the action being given
in the script is taken.

A script can only be injected to psybnc by accessing the shell, and by putting
the scripts into the scripts/-Directory.

If a script was injected, while the bouncer was already running, it wont auto-
matically reload the script. You would have to trigger the psyBNC-Command
/SRELOAD to load changes on your script.

I.2 Event-handled Scripting

The syntax of every Line of a psyBNC-Script is as following:

event	function	fromuserhost/name	to	content	script

The event describes 6 possible event-types of the scripting Engine.
Those are:

- dccchat (If the user receives a dcc chat offer from an irc user)
- dccsend (If the user receives a dcc file offfer from an irc user)
- server (Any event from the server)
- dialogue (A permanent open dialogue which is handled by the script)
- ctcp (if a CTCP-request is received from an irc user)
- alias (a user defined Command for psyBNC)

All other fields depend in its functionality on the event:

The function-field:

- In dccchat, dccsend and dialogue it has no meaning at all
- in server, it is the name or Number of the message coming from the server
- in ctcp, it is the name of the CTCP command (on CTCP PING it would be PING)
- in aliases, its the name of the command, which should be user defined

The fromuserhost/name-Field:

- In dialogues it has no meaning at all
- In server, ctcp, dccchat and dccsend its a wildcard-hostmask (*!*a@*.b.com)
  If the Hostmask matches, then the script is called. On server events also
  content has to fit.
- In aliases, its used as description of the Command.

The to-Field:

- In alias, dccsend and dccfile it has no meaning at all
- in server and ctcp, it can be a channel (as like #sausage), or the * wildcard.
- In dialogues, it is the "nick" of the dialogue, which talks to the 
  bouncer user in a query (in brackets).

The content-Field:

- In alias, dialogues, dccsend, dccfile and ctcp it has no meaning at all
- In the server event, it is a wildcarded "content" of a message,
  for example, if this field contains "*Hello*", the servermessage which
  calls the Script has to contain "Hello" in the IRC-Content as well.

The script-Field:

- On ctcp, dccfile and server it can be any shellscript or shell-
  based command(s), which will be called once and have to terminate right
  after execution.
- On dialogue, dccchat and alias, the called script can stay backgrounded, and 
  do its work as long its needed.

A script can be anything, which can be handled by the shell. You also can
call PHP/Perl/tcl-Scripts from the script-Entry.
  
I.3 The DEFAULT-Script

If a script with the filename "DEFAULT.SCRIPT" is placed into the
directory scripts/, all Users will run this script, until no special
user script is defined. This global script will also be run for all
new added users.

I.4 Special User-Scripts

If a special User script has to be created, its Filename must be
USERnn.SCRIPT, where nn is the Number of the user, corresponding to
the Number in the psybnc.conf, with no leading zeros (e. g. User1 has to use
USER1.SCRIPT). The file has to be put into scripts/.

II. Variables

If a script is called, psyBNC sets special Environment-Variables
to support data to the script.

Global Variables are always set, no matter, which event is called.
Some events got special variables, which will be described in the
subsections of this part.

A variable can be adressed by a shellscript as a normal Environment-
Variable is being adressed:

echo "$USER did start a script"

would echo the User-Variable from psyBNC to the current standard-output,
which would be handled by psyBNC (see III. Events).

II.1 Global Variables for all events

This will show a list of all variables, which are set on every
Script-Call.

USERNUMBER	- The number of the psyBNC-User
USERLOGIN	- The ident/login of the psyBNC-User
USERNICK	- The current nick of the psyBNC-User
USERHOST	- The connected client Host of the psyBNC-User
USERON		- If set to 1, the User is connected currently

If the event is a server-Event, a dccsend-Event, a dccfile-Event or
a ctcp-Event, the following variables are used. They are filtered from
the incoming IRC-Message.

If the message would be a User-Message or a Channel-Message, it would
look like as following:

                                          Variable
                                          --------
:joe!joe@host.com PRIVMSG #hello :hello
  |   |     |        |       |      |
  |   |     |        |       |      |____ CONTENT
  |   |     |        |       |
  |   |     |        |       |___________ TO
  |   |     |        |    
  |   |     |        |___________________ CMD
  |   |     |
  |   |     |____________________________ HOST    \
  |   |                                            \
  |   |__________________________________ IDENT     \ USER
  |                                                 /
  |______________________________________ NICK    /
  
If you would request the Variable CONTENT, you would get "hello".
CONTENT saves the whole text from the Channel- or Usermessage.
The Variable CMD would be PRIVMSG. Remember, IRC does not really
know commands as like "QUERY" or "MSG", all User- and Channel-
messages are handled using PRIVMSG.
The NICK field would be joe, the ident field in this case as well.
the HOST field would be host.com.
The USER field would be the irc-hostmask, in this case joe!joe@host.com.


If the message would be a server-message, if would look as like the
following:

                                          Variable
                                          --------
:irc.somenet.org 001 ircgeek :Welcome to irc.somenet.org, ircgeek.
        |        |       |      |
        |        |       |      |____ CONTENT
        |        |       |
        |        |       |___________ TO
        |        |    
        |        |___________________ CMD
        |
        |____________________________ HOST / USER

In this case HOST and USER carry the same value.

Some servermessages support more than one parameters in TO.
For that sake, its also possible to read another List of Variables:

:joe!joe@host.com PRIVMSG #hello :hello people, how are you ?
        |    	    |       |      |     |      |   |   |   |
        |           |       |      |     |      |   |   |   |
       P1          P2      P3     P4    P5     P6  P7  P8  P9

The Pnn-Variables contain an array of each element of the IRC-Line.
If you would do:

echo $P3

you would get

#hello

On Aliases the Input is sent in Command-Format. This Command-Format
would also be stored in the Variables:

MYPART #something :Byebye
   |       |         |
   |       |         |____ CONTENT
   |       |
   |       |______________ TO
   |
   |______________________ CMD

The Pnn-Variables are also available in Aliases.

II.2 Event-specific Variables

The dialogue Event offers a variable named MYNICK. It stores the nick of
the dialogue-query, which appears to the user.

The Serverevent sets channel-Variables, if the TO-Field is a channel.
Those Variables are:

CHANNEL		- Name of the Channel

If the psyBNC-User is on the Channel, he will be also supplied with the
following Variables:

CHANTOPIC	- Topic of the Channel
CHANMODE	- Modes of the channel (e.g. +tn)
CHANKEY		- Key of the channel, if a key is set. Otherwise not used.
CHANLIMIT	- The limit (+l Mode), if the mode is set.
CHANUSERnn	- A User of the Channel, numbered by NN
                  e.g. CHANUSER1. The Syntax of a chanuser-Field is
	          as follows:
		  nick!ident@host|mode
		  The User-Hostmask is seperated from the Channels Usermode
		  by a Pipe-character (|).
		  The Usermode is set to the modes the user has on the
		  Channel.
		  Example:
		  joe!joe@host.com|o
		  would show, that User joe got op on the channel.

The dccchat-Event supplies a Variable named IP, it keeps the Host
of the dccchat-offering User.

The dccsend-Event supplies, as like dcchat, the IP-Variable, and another
Variable named FILE. It keeps the Name of the File, which the DCC-Send-
offering User wants to transfer to you.

The ctcp-Event supplies a Variable named X01. This variable ist used
to easily create an answer on a simple shellscript-Echo-Line without
having the need to call an external Program to create the control-
character \x01.

III. Events

This section tries to describe the Events itself, how they are used and 
how to parameterize them.

The output and input on every called script is defined by the Event itself.
psyBNC switches the standard-input, -output and Error-Output for the script
to its own internal structures.

If a task, which is called as a script from psyBNC, writes to STDOUT 
(Standard-Output), it is directly injected into psyBNC. The way
psyBNC handles that Output is dependend from the event.

The following words will be used in the Event-Context:

STDIN  - Standard Input to a called task (Socket 0)
STDOUT - Standard output from a called task (Socket 1)
STDERR - Standard Error-Output from a called task (Socket 2)

The events also sometimes use STDIN for receiving Output from the task.
A normal shellscript would handle output to STDERR or STDIN as follows:

echo "Hello" >&0

Would send Hello to STDIN

echo "Error !" >&2

Would send Error ! to STDERR.

In all events STDERR gets to the psyBNC-Main-Log.

III.1 The server-Event

The server-Event reacts on messages from the Server. That can be any
special server-message (numeric), or channel- and User-Messages (as 
like PRIVMSG, JOIN, PART).

A script, which was called on a server-Event, would not stay in background,
but send a reply to the given event possibly. 

The server-Event has the following settings for Standard In- and Outputs:

STDOUT goes to the server, via the psyBNC command Interpreter.
You also can trigger psyBNC commands from the given script while
creating output to STDOUT.

Example:

server  JOIN 	 *!*@* 	#mychannel * echo "PRIVMSG $NICK :Welcome to #mychannel"

Anyone, who would join to the Channel #mychannel would receive a Message
"Welcome to #mychannel" from your Client.

If the Script sends an Output to STDIN, the output will be sent to
the connected client of psyBNC.

Example:

server PRIVMSG	*!*@*	#mychannel *damned*  echo ":irc.server.net NOTICE $USERNICK :someone said damned" >&0

If someone in the Channel would post a sentence including "damned", your Client
would get a notice from the Server irc.server.net, that someone on the Channel
said "damned".

If the Script sends an Output to STDERR, psyBNC will put that into the Main-Log
of psyBNC.

Example:

server PART	*!*@* 	#mychannel *	echo "User $NICK left the channel !" >&2

If someone parts the channel, your would get a logged Message, that this
User left the channel.

III.2 The dialogue-Event

The dialogue Event starts a backgrounded application, which interacts with
the User on psyBNC using an emulated query. 

You can use dialogues to run applications, which need interaction, but are
usefull in the psyBNC Context. Possibly you want to write a Newsticker,
which actually displays the news to the query. Or you want to interact
with a line driven mail-software via a query in psyBNC.

Example:

dialogue *	*	mail	*	mail

Would call the unix-mailreader, and if mail was available, would stay in the 
background, until the user would stop the application.

STDIN and STDOUT are redirected to the emulated Query, STDERR is the Main-Log
of psyBNC.

III.3 The dccchat-Event

The dccchat Event may also start a background-Application, which would 
replace a possible User-Interaction on a dccchat.

For example:

dccchat *	*!*joe@bla.com	*  *	echo "DCCANSWER $NICK"; /home/me/eliza

This line would automatically accept any DCC Chat from any User which
matches the Hostmask *!*joe@bla.com. After it acknowledged the Chat, the
Program /home/me/eliza would be startet, and could possibly react
directly on the User-Input.

The Bouncer-User would monitor the in- and outputs, which would be send
to and from the DCC-Query of psyBNC. Be sure, you have DCC enabled on
psyBNC compiling time.

An Output to STDIN would be sent to the Server. 
Any Output given to STDOUT would be sent to the DCC-Chat.
Any Output given on STDERR would be put into the psyBNC-Main-Log.

III.4 The dccfile-Event

A dccfile Event could be used to possibly create a fileserver, which
would possibly deny the receipt of already existent files, and answer
automatically on a DCC File-Send Request.

Example:

dccfile  *	*!*@*	*	*	/home/me/filecheck

Would call an own application named /home/me/filecheck, which possibly
could handle the for sending requested file, and which could possibly
answer a the DCCGET automatically.

STDIN output would be directed to the Server. Using STDIN, you could
send commands to the bouncer, to let it accept the file for example.
STDOUT would be directed to the Clients Input. You could send a notice
to the User, that the file had been autoaccepted for example.
STDERR is directed to the psyBNC-Main-Log.

III.5 The alias-Event

Aliases can be used to add own Commands to the psyBNC-Command-List.
The User would be able to use the alias as like he would use an IRC-
or psyBNC command. If an alias is existent, which got the same
name of a psyBNC-Command, the Alias would be used instead of the
psyBNC-Command.

Example:

alias  HELLO	"Say Hello"	*	*	echo "PRIVMSG $TO :Hello"

would send a Hello to the given "TO" argument.
Usage of that Alias would be:

/HELLO #bla

The text would be posted to #bla then.
You also could do /BHELP HELLO then, but if you want to support a help
for the added alias, you would have to put a HELLO.TXT into help/.
If you would trigger /BHELP without a parameter, the Text "Say Hello"
would be displayed as quick Command Help.

STDIN Outputs from the script would be sent to the Clients Input.
STDOUT Outputs would be sent to the Server.
STDERR is written into the psyBNC Main-Log.

III.6 The ctcp-Event

A ctcp-Event is triggered, when the User receives a CTCP-Message from
an IRC-User. 

Example:

ctcp FINGER 	*!*@*	*	*	echo "NOTICE $NICK :Dont finger me"

If anyone would send a CTCP FINGER to your bouncer, he would get a Notice
back saying "Dont finger me".

STDIN Output would be sent to the Client.
STDOUT Output would be sent to the Server.
STDERR is put into the psyBNC-Main-Log.

IV. Last words

I hope this Documentation is a little bit more usefull than the Text
i put into the README up to Version 2.2.2.

If you got questions about the scripting-engine itself, please contact
me on IRCNet.

Please dont ask me for working scripts, i just offer the Scripting-Engine
of psyBNC to be used for scripting.
