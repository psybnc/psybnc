Frequently Asked Questions - up to version 2.3.2
--------------------------

Q: I tried to add a User for getting Op from psybnc with /ADDOP.
   He tried /msg bouncer op password, but it did not work ! What is
   wrong ?
A: A question to psyBNC for op has always to include the channel.
   So if a user asks for op, he has to do:
   /msg bouncer op password #channel
   to get op.

Q: I added a friend to my bouncer but he can't connect. The bouncer says 
   "WRONG PASSWORD" although i did check the password and it's correct !
A: Bouncer login is completed from two parts. One is LOGIN NAME other is PASSWORD.
   If you do a /ADDUSER, then the first parameter of ADDUSER isn't the nick, but
   the ident field.
   Both must be correct in order to succesfully LOG-IN ! In mIRC you must set the 
   IDENT to the name that you are added in bouncer. But be carefull ! Sometimes in 
   IRC the ident doesn't work right. So you must also set E-MAIL ADDRESS to that 
   name !

Q: I use bouncer but i can't DCC CHAT or SEND. What is wrong ?
A: In mIRC you should change the settings under LOCAL INFO -> LOOKUP METHOD to NORMAL. 
   You should also make sure that you have ON CONNECT ALWAYS GET: Local host & IP 
   checked !

Q: I connected my bouncer to BounceNet. But then i had to reinstall it. Now it won't 
   connect again even if i added the /LINKTO correctly.
A: The link must be added again on BOTH sides. Bouncer uses a verification which was 
   deleted when you reinstalled bouncer. Just make sure the ADMIN on the other side 
   adds your bouncer again.

Q: I have added a BOT DCC to my bouncer. It worked great. Now it's gone. What sould i 
   do ?
A: Problems like this or simmilar always use REHASH. Warning ! This will close your 
   connection !

Q: My bouncer is active but not on the IRC NETWORK. When i do /bconnect it says: 
   You are not marked as quitted. 
   What to do ?
A: Check the server that you have added. Check that the IRC server in ServerList 
   allows connection from that host. If both is verified then the reason is possibly 
   this. Your bouncer got disconnected from IRC by some error. Use /BQUIT to mark 
   it as quitted then use /BCONNECT again 

Q: I set bouncer so that it asks OP from a bot. I am added in bot and i checked my 
   password. But nothing happens. What is wrong ?
A: Usually eggdrops have a general command used to gain OPs . It's 
   /MSG BotName OP password. In your case the bot might have different command to 
   ask OPs from. Bouncer can only ask OPs the normal way.

Q: I would like to suggest a Bugfix/Change to psybnc. Whom should i ask ?
A: Mail to psychoid@gmx.net or query psychoid on ircnet

Q: I want psyBNC to keep my Nick ! Why doesnt it do ?
A: psyBNC tries to keep the nick you last set using your client. So if you login 
   using the nick 'user', it would try to regain the nick 'user' all 10 seconds.
   
Q: I want to have an autovoice/autonotice/automessage command in psyBNC
   Will you build it in ?
A: Please try to use scripting for your special purposes (e.g. msging 
   a channel- or nickserv on networks with services)
   Autovoice will be solved for Version 2.4, when autoop will be 
   changed to automode.

Q: I found psyBNC on a hacked Host ! Why do you code such an evil tool
   which is being used on hacked Hosts ?
A: psyBNC is used legally by the majority of its users on their own hosts or
   shells. psyBNC and its author is not responsible for an intrusion, its just
   a tool someone used unauthorized on your host. Dont mix up a used tool
   with an incident.
