
main : can_ids.h can_parse.h main.c 
	gcc  main.c -I./ -o main

can_ids.h : 
	wget https://raw.githubusercontent.com/ZeniteSolar/CAN_IDS/master/can_ids.h -O can_ids.h

can_parse.h :
	wget https://raw.githubusercontent.com/ZeniteSolar/CAN_IDS/master/can_parser.h -O can_parse.h
