TARGET = all

all: libs
	- cd cpu && $(MAKE) $(TARGET) 1>/dev/null
	- cd memory && $(MAKE) $(TARGET) 1>/dev/null
	- cd scheduler && $(MAKE) $(TARGET) 1>/dev/null
	- cd swap && $(MAKE) $(TARGET) 1>/dev/null

libs:
	- cd includes && $(MAKE) $(TARGET) 1>/dev/null

debug: TARGET = debug
debug: all

clean: TARGET = clean
clean: all