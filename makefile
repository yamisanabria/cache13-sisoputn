TARGET = all

all: libs
	- cd cpu && $(MAKE) $(TARGET)
	- cd memory && $(MAKE) $(TARGET)
	- cd scheduler && $(MAKE) $(TARGET)
	- cd swap && $(MAKE) $(TARGET)

libs:
	- cd includes && $(MAKE) $(TARGET)

debug: TARGET = debug
debug: all

clean: TARGET = clean
clean: all