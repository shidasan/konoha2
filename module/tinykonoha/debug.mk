UNAME = tinykonoha
DEBUG_CC = gcc
DEBUG_CFLAGS = -g2 -I../../include -I.
d :
	$(DEBUG_CC) $(UNAME).c -o $(UNAME) $(DEBUG_CFLAGS) -DK_USING_TINYVM=1
